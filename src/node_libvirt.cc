// Copyright 2013, Camilo Aguilar. Cloudescape, LLC.
#ifndef BUILDING_NODE_EXTENSION
    #define BUILDING_NODE_EXTENSION
#endif
#include <stdio.h>
#include <stdlib.h>
#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"

namespace NodeLibvirt {
    void InitializeLibvirt(Handle<Object> target) {
        HandleScope scope;
        virInitialize();

        Hypervisor::Initialize(target);
        EventImpl::Initialize(target);
        Domain::Initialize();
        NodeDevice::Initialize();
        Interface::Initialize();
        Network::Initialize();
        NetworkFilter::Initialize();
        Secret::Initialize();
        Error::Initialize();
        StoragePool::Initialize();
        StorageVolume::Initialize();
        //Stream::Initialize();
        //Event::Initialize();

        target->Set(String::NewSymbol("version"),
                    String::New(NODE_LIBVIRT_VERSION));

        target->Set(String::NewSymbol("libvirt_version"),
                    Number::New(LIBVIR_VERSION_NUMBER));

        Handle<ObjectTemplate> global = ObjectTemplate::New();
        Handle<Context> context = Context::New(NULL, global);
        Context::Scope context_scope(context);

        context->Global()->Set(String::NewSymbol("libvirt"), target);
    }

    // Extracts a C string from a V8 Utf8Value.
    const char* ToCString(const String::Utf8Value& value) {
        return *value ? *value : "<string conversion failed>";
    }

    extern "C" void init (Handle<Object> target) {
        HandleScope scope;
        InitializeLibvirt(target);
    }

    NODE_MODULE(libvirt, init)
} //namespace NodeLibvirt

