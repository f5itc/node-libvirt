// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>
#include <string.h>
#include "storage_volume.h"

namespace NodeLibvirt {
    Persistent<FunctionTemplate> StorageVolume::constructor_template;

    //storage volume info
    static Persistent<String> info_type_symbol;
    static Persistent<String> info_capacity_symbol;
    static Persistent<String> info_allocation_symbol;

    struct BatonBase {
        v8::Persistent<v8::Function> callback;
        std::string error;

        virtual ~BatonBase() {
            callback.Dispose();
        }
    };

    struct DeleteBaton : BatonBase {
        unsigned int flags;
        StorageVolume* volume;
        int ret;
    };

    struct GetInfoBaton : BatonBase {
        unsigned int flags;
        StorageVolume* volume;
        virStorageVolInfo res;
    };

    struct LookupByNameBaton : BatonBase {
        const char* name;
        StorageVolume* volume;
        StoragePool* pool;
    };

    struct LookupByPathBaton : BatonBase {
        Hypervisor* hypervisor;
        const char* path;
        StorageVolume* volume;
    };

    void StorageVolume::Initialize() {
        HandleScope scope;

        Local<FunctionTemplate> t = FunctionTemplate::New();

        //        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(t, "getInfo",
                StorageVolume::GetInfo);
        NODE_SET_PROTOTYPE_METHOD(t, "getKey",
                StorageVolume::GetKey);
        NODE_SET_PROTOTYPE_METHOD(t, "getName",
                StorageVolume::GetName);
        NODE_SET_PROTOTYPE_METHOD(t, "getPath",
                StorageVolume::GetPath);
        NODE_SET_PROTOTYPE_METHOD(t, "toXml",
                StorageVolume::ToXml);
        NODE_SET_PROTOTYPE_METHOD(t, "remove",
                StorageVolume::Delete);
        NODE_SET_PROTOTYPE_METHOD(t, "wipe",
                StorageVolume::Wipe);

        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->SetClassName(String::NewSymbol("StorageVolume"));

        Local<ObjectTemplate> object_tmpl = t->InstanceTemplate();

        //Constants initialization
        //virStorageVolType
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_VOL_FILE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_VOL_BLOCK);

        info_type_symbol         = NODE_PSYMBOL("type");
        info_capacity_symbol     = NODE_PSYMBOL("capacity");
        info_allocation_symbol   = NODE_PSYMBOL("allocation");
    }

    Handle<Value> StorageVolume::Create(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;

        int argsl = args.Length();

        if(argsl == 0) {
            return ThrowException(Exception::TypeError(
                        String::New("You must specify at least one argument")));
        }

        if(!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
                        String::New("You must specify a string as first argument")));
        }

        Local<Object> pool_obj = args.This();

        if(!StoragePool::HasInstance(pool_obj)) {
            return ThrowException(Exception::TypeError(
                        String::New("You must specify a Hypervisor object instance")));
        }
        String::Utf8Value xml(args[0]->ToString());

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(pool_obj);

        StorageVolume *volume = new StorageVolume();
        volume->volume_ = virStorageVolCreateXML(pool->pool(), (const char *) *xml, flags);

        if(volume->volume_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> volume_obj = volume->constructor_template->GetFunction()->NewInstance();
        volume->Wrap(volume_obj);

        return scope.Close(volume_obj);
    }

    void DeleteStorageVolAsync(uv_work_t* req) {
        // re-baton the request data
        DeleteBaton* baton = static_cast<DeleteBaton*>(req->data);
        virErrorPtr err;

        baton->ret  = virStorageVolDelete(baton->volume->volume_, baton->flags);

        if(baton->ret == -1) {
            err = virGetLastError();
            baton->error = err->message;
        }

        if(baton->volume->volume_ != NULL) {
            virStorageVolFree(baton->volume->volume_);
        }
    }

    // err: virGetLastError
    // result: return value from virStorageVolDelete
    void DeleteStorageVolAsyncAfter(uv_work_t* req) {
        HandleScope scope;

        DeleteBaton* baton = static_cast<DeleteBaton*>(req->data);
        delete req;
        Handle<Value> argv[2];

        if (!baton->error.empty()) {
            argv[0] = Exception::Error(String::New(baton->error.c_str()));
            argv[1] = Undefined();
        }
        else {
            argv[0] = Undefined();
            argv[1] = True();
        }

        TryCatch try_catch;

        if (try_catch.HasCaught())
            FatalException(try_catch);

        baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
        delete baton;
    }

    Handle<Value> StorageVolume::Delete(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;

        DeleteBaton* baton = new DeleteBaton();

        Local<Function> callback = Local<Function>::Cast(args[0]);
        StorageVolume *volume = ObjectWrap::Unwrap<StorageVolume>(args.This());

        baton->callback = Persistent<Function>::New(callback);
        baton->flags = flags;
        baton->volume = volume;

        // Compose request
        uv_work_t* req = new uv_work_t;
        req->data = baton;

        // Dispatch work
        uv_queue_work(
                uv_default_loop(),
                req,
                DeleteStorageVolAsync,
                (uv_after_work_cb)DeleteStorageVolAsyncAfter
                );

        return scope.Close(Undefined());
    }

    Handle<Value> StorageVolume::Wipe(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        int ret = -1;

        StorageVolume *volume = ObjectWrap::Unwrap<StorageVolume>(args.This());

        ret = virStorageVolWipe(volume->volume_, flags);
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    void GetStorageVolumeInfoAsync(uv_work_t* req) {
        GetInfoBaton* baton = static_cast<GetInfoBaton*>(req->data);

        StorageVolume *volume = baton->volume;

        virStorageVolInfo info;
        virErrorPtr err;

        int ret = -1;
        ret = virStorageVolGetInfo(volume->volume_, &info);

        if(ret == -1) {
            err = virGetLastError();
            baton->error = err->message;
        }

        else {
            baton->res = info;
        }
    }

    void GetStorageVolumeInfoAsyncAfter(uv_work_t* req) {
        HandleScope scope;

        GetInfoBaton* baton = static_cast<GetInfoBaton*>(req->data);
        delete req;

        Handle<Value> argv[2];

        virStorageVolInfo res = baton->res;

        if (!baton->error.empty()) {
            argv[0] = Exception::Error(String::New(baton->error.c_str()));
            argv[1] = Undefined();
        }

        else {
            Local<Object> object = Object::New();

            // virStoragePoolState
            object->Set(info_type_symbol, Integer::New(res.type));

            // bytes
            object->Set(info_capacity_symbol, Number::New(res.capacity));

            // bytes
            object->Set(info_allocation_symbol, Number::New(res.allocation));

            argv[0] = Undefined();
            argv[1] = scope.Close(object);
        }

        TryCatch try_catch;

        if (try_catch.HasCaught())
            FatalException(try_catch);

        baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
        delete baton;
    }

    Handle<Value> StorageVolume::GetInfo(const Arguments& args) {
        HandleScope scope;

        // Storage volume context
        StorageVolume *volume = ObjectWrap::Unwrap<StorageVolume>(args.This());

        // Callback
        Local<Function> callback = Local<Function>::Cast(args[0]);

        // Create baton
        GetInfoBaton* baton = new GetInfoBaton();

        // Add callback and storage volume
        baton->callback = Persistent<Function>::New(callback);
        baton->volume = volume;

        // Compose req
        uv_work_t* req = new uv_work_t;
        req->data = baton;

        // Dispatch work
        uv_queue_work(
            uv_default_loop(),
            req,
            GetStorageVolumeInfoAsync,
            (uv_after_work_cb)GetStorageVolumeInfoAsyncAfter
        );

        return Undefined();
    }

    Handle<Value> StorageVolume::GetKey(const Arguments& args) {
        HandleScope scope;
        const char *key = NULL;

        StorageVolume *volume = ObjectWrap::Unwrap<StorageVolume>(args.This());

        key = virStorageVolGetKey(volume->volume_);

        if(key == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return scope.Close(String::New(key));
    }

    Handle<Value> StorageVolume::GetName(const Arguments& args) {
        HandleScope scope;
        const char *name = NULL;

        StorageVolume *volume = ObjectWrap::Unwrap<StorageVolume>(args.This());

        name = virStorageVolGetName(volume->volume_);

        if(name == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return scope.Close(String::New(name));
    }

    Handle<Value> StorageVolume::GetPath(const Arguments& args) {
        HandleScope scope;
        const char *path = NULL;

        StorageVolume *volume = ObjectWrap::Unwrap<StorageVolume>(args.This());

        path = virStorageVolGetPath(volume->volume_);

        if(path == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return scope.Close(String::New(path));
    }

    Handle<Value> StorageVolume::ToXml(const Arguments& args) {
        HandleScope scope;
        char* xml_ = NULL;
        unsigned int flags = 0;

        StorageVolume *volume = ObjectWrap::Unwrap<StorageVolume>(args.This());
        xml_ = virStorageVolGetXMLDesc(volume->volume_, flags);

        if(xml_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<String> xml = String::New(xml_);

        free(xml_);

        return scope.Close(xml);
    }

    void LookupByNameAsync(uv_work_t* req) {
        LookupByNameBaton* baton = static_cast<LookupByNameBaton*>(req->data);

        StoragePool *pool = baton->pool;
        const char *name = baton->name;

        StorageVolume *volume = new StorageVolume();
        virErrorPtr err;

        volume->volume_ = virStorageVolLookupByName(pool->pool(), name);

        if(volume->volume_ == NULL) {
            err = virGetLastError();
            baton->error = err->message;
        }

        else {
            baton->volume = volume;
        }
    }

    void LookupByNameAsyncAfter(uv_work_t* req) {
        HandleScope scope;

        LookupByNameBaton* baton = static_cast<LookupByNameBaton*>(req->data);
        delete req;

        Handle<Value> argv[2];

        if (!baton->error.empty()) {
            argv[0] = Exception::Error(String::New(baton->error.c_str()));
            argv[1] = Undefined();
        }

        else {
            StorageVolume *volume = baton->volume;
            Local<Object> volume_obj = volume->constructor_template->GetFunction()->NewInstance();
            volume->Wrap(volume_obj);

            argv[0] = Undefined();
            argv[1] = scope.Close(volume_obj);
        }

        TryCatch try_catch;

        if (try_catch.HasCaught())
            FatalException(try_catch);

        baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
        delete baton;
    }

    Handle<Value> StorageVolume::LookupByName(const Arguments& args) {
        HandleScope scope;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
                        String::New("You must specify a string to call this function")));
        }

        Local<Object> pool_obj = args.This();

        if(!StoragePool::HasInstance(pool_obj)) {
            return ThrowException(Exception::TypeError(
                        String::New("You must specify a StoragePool instance")));
        }

        const char *name = parseString(args[0]);

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(pool_obj);

        // Callback
        Local<Function> callback = Local<Function>::Cast(args[1]);

        // Create baton
        LookupByNameBaton* baton = new LookupByNameBaton();

        // Add callback and domain
        baton->callback = Persistent<Function>::New(callback);
        baton->pool = pool;
        baton->name = name;

        // Compose req
        uv_work_t* req = new uv_work_t;
        req->data = baton;

        // Dispatch work
        uv_queue_work(
                uv_default_loop(),
                req,
                LookupByNameAsync,
                (uv_after_work_cb)LookupByNameAsyncAfter
                );

        return scope.Close(Undefined());
    }

    Handle<Value> StorageVolume::LookupByKey(const Arguments& args) {
        HandleScope scope;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
                        String::New("You must specify a string to call this function")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
                        String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value key(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        StorageVolume *volume = new StorageVolume();
        volume->volume_ = virStorageVolLookupByKey(hypervisor->connection(), (const char *) *key);

        if(volume->volume_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> volume_obj = volume->constructor_template->GetFunction()->NewInstance();

        volume->Wrap(volume_obj);

        return scope.Close(volume_obj);
    }

    void LookupByPathAsync(uv_work_t* req) {

        LookupByPathBaton* baton = static_cast<LookupByPathBaton*>(req->data);

        Hypervisor *hypervisor = baton->hypervisor;
        const char *path = baton->path;

        StorageVolume *volume = new StorageVolume();
        virErrorPtr err;

        volume->volume_ = virStorageVolLookupByPath(hypervisor->connection(), path);

        if(volume->volume_ == NULL) {
            err = virGetLastError();
            baton->error = err->message;
        }

        else {
            baton->volume = volume;
        }
    }

    void LookupByPathAsyncAfter(uv_work_t* req) {
        HandleScope scope;

        LookupByPathBaton* baton = static_cast<LookupByPathBaton*>(req->data);
        delete req;

        Handle<Value> argv[2];

        if (!baton->error.empty()) {
            argv[0] = Exception::Error(String::New(baton->error.c_str()));
            argv[1] = Undefined();
        }

        else {
            StorageVolume *volume = baton->volume;
            Local<Object> volume_obj = volume->constructor_template->GetFunction()->NewInstance();
            volume->Wrap(volume_obj);

            argv[0] = Undefined();
            argv[1] = scope.Close(volume_obj);
        }

        TryCatch try_catch;

        if (try_catch.HasCaught())
            FatalException(try_catch);

        baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
        delete baton;
    }

    Handle<Value> StorageVolume::LookupByPath(const Arguments& args) {

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
                        String::New("You must specify a string to call this function")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
                        String::New("You must specify a Hypervisor instance")));
        }

        const char *path = parseString(args[0]);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        // Callback
        Local<Function> callback = Local<Function>::Cast(args[1]);

        // Create baton and add data
        LookupByPathBaton* baton = new LookupByPathBaton();
        baton->callback = Persistent<Function>::New(callback);
        baton->hypervisor = hypervisor;
        baton->path = path;

        // Compose req
        uv_work_t* req = new uv_work_t;
        req->data = baton;

        // Dispatch work
        uv_queue_work(
                uv_default_loop(),
                req,
                LookupByPathAsync,
                (uv_after_work_cb)LookupByPathAsyncAfter
                );

        return Undefined();
    }

    Handle<Value> StorageVolume::Clone(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;

        if(args.Length() < 2) {
            return ThrowException(Exception::TypeError(
                        String::New("You must specify two arguments to call this function")));
        }

        if(!StorageVolume::HasInstance(args[0])) {
            return ThrowException(Exception::TypeError(
                        String::New("You must specify a StorageVolume instance as first argument")));
        }

        if(!args[1]->IsString()) {
            return ThrowException(Exception::TypeError(
                        String::New("You must specify a string as second argument")));
        }

        Local<Object> pool_obj = args.This();

        if(!StoragePool::HasInstance(pool_obj)) {
            return ThrowException(Exception::TypeError(
                        String::New("You must specify a StoragePool instance")));
        }

        String::Utf8Value xml(args[1]->ToString());

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(pool_obj);
        StorageVolume *source_volume = ObjectWrap::Unwrap<StorageVolume>(args[0]->ToObject());

        StorageVolume *clone_volume = new StorageVolume();
        clone_volume->volume_ = virStorageVolCreateXMLFrom(pool->pool(),
                (const char *) *xml,
                source_volume->volume_,
                flags);

        if(clone_volume->volume_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> clone_vol_obj = clone_volume->constructor_template->GetFunction()->NewInstance();

        clone_volume->Wrap(clone_vol_obj);

        return scope.Close(clone_vol_obj);
    }

} //namespace NodeLibvirt

