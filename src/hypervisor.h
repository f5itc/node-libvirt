// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_HYPERVISOR_H_
#define SRC_HYPERVISOR_H_

#include "node_libvirt.h"
#include "event_impl.h"
#include "domain.h"
#include "node_device.h"
#include "network.h"
#include "network_filter.h"
#include "interface.h"
#include "secret.h"
#include "storage_pool.h"
#include "storage_volume.h"
#include "error.h"
#include "worker.h"
#include <string>

namespace NodeLibvirt {

    class Hypervisor : public ObjectWrap {
        friend class ConnectWorker;

        public:
            static void Initialize(Handle<Object> target);
            static inline bool HasInstance(v8::Handle<v8::Value> value) {
                if (!value->IsObject()) {
                    return false;
                }
                return true; // XXX hack
                //v8::Local<v8::Object> object = value->ToObject();
                //return constructor_template->HasInstance(object);
            }
            virConnectPtr connection() const;

        protected:
            static Handle<Value> New(const Arguments& args);

            static NAN_METHOD(Connect);
            static NAN_METHOD(GetCapabilities);
            static Handle<Value> GetHostname(const Arguments& args);
            static Handle<Value> GetSysinfo(const Arguments& args);
            static Handle<Value> GetType(const Arguments& args);
            static Handle<Value> GetConnectionUri(const Arguments& args);
            static Handle<Value> GetVersion(const Arguments& args);
            static Handle<Value> GetLibVirtVersion(const Arguments& args);
            static Handle<Value> GetMaxVcpus(const Arguments& args);
            static Handle<Value> GetBaselineCPU(const Arguments& args);
            static Handle<Value> CompareCPU(const Arguments& args);
            static Handle<Value> IsConnectionEncrypted(const Arguments& args);
            static Handle<Value> IsConnectionSecure(const Arguments& args);
            static Handle<Value> IsConnectionAlive(const Arguments& args);
            static Handle<Value> CloseConnection(const Arguments& args);
            static Handle<Value> SetKeepAlive(const Arguments& args);

            //virConnectList functions
            static Handle<Value> GetDefinedDomains(const Arguments& args);
            static Handle<Value> GetDefinedInterfaces(const Arguments& args);
            static Handle<Value> GetDefinedNetworks(const Arguments& args);
            static Handle<Value> GetDefinedStoragePools(const Arguments& args);
            static Handle<Value> GetDomainCount(const Arguments& args);
            static Handle<Value> GetActiveDomainNames(const Arguments& args);
            static Handle<Value> GetActiveDomains(const Arguments& args);
            static Handle<Value> GetActiveInterfaces(const Arguments& args);
            static Handle<Value> GetNetworkFilters(const Arguments& args);
            static Handle<Value> GetActiveNetworks(const Arguments& args);
            static Handle<Value> GetSecrets(const Arguments& args);
            static Handle<Value> GetActiveStoragePools(const Arguments& args);

            //virConnectNumOf functions
            static Handle<Value> GetNumberOfDefinedDomains(const Arguments& args);
            static Handle<Value> GetNumberOfDefinedInterfaces(const Arguments& args);
            static Handle<Value> GetNumberOfDefinedNetworks(const Arguments& args);
            static Handle<Value> GetNumberOfDefinedStoragePools(const Arguments& args);
            static Handle<Value> GetNumberOfActiveDomains(const Arguments& args);
            static Handle<Value> GetNumberOfActiveInterfaces(const Arguments& args);
            static Handle<Value> GetNumberOfActiveNetworks(const Arguments& args);
            static Handle<Value> GetNumberOfNetworkFilters(const Arguments& args);
            static Handle<Value> GetNumberOfSecrets(const Arguments& args);
            static Handle<Value> GetNumberOfActiveStoragePools(const Arguments& args);

            //Node functions
            static Handle<Value> GetNodeFreeMemoryInNumaCells(const Arguments& args);
            static Handle<Value> GetNodeFreeMemory(const Arguments& args);
            static Handle<Value> GetNodeInfo(const Arguments& args);
            static Handle<Value> GetNodeDevicesNames(const Arguments& args);
            static Handle<Value> GetNodeSecurityModel(const Arguments& args);

            //Event functions
            static Handle<Value> RegisterDomainEvent(const Arguments& args);
            static Handle<Value> UnregisterDomainEvent(const Arguments& args);

            //Misc functions
            static Handle<Value> FindStoragePoolSources(const Arguments& args);

            //Async functions
            friend void GetDomainCountAsync(uv_work_t* req);
            friend void GetDomainCountAsyncAfter(uv_work_t* req);
            friend void GetNodeFreeMemoryAsync(uv_work_t* req);
            friend void GetNodeFreeMemoryAsyncAfter(uv_work_t* req);
            friend void GetNodeInfoAsync(uv_work_t* req);
            friend void GetNodeInfoAsyncAfter(uv_work_t* req);

            Hypervisor( char* uri,
                        char* user,
                        char* pass,
                        bool readOnly);


        protected:
            virConnectPtr conn_;
            char* uri_;
            char* username_;
            char* password_;
            bool readOnly_;

        private:

            static void domain_event_free(void *opaque);
            static int domain_event_lifecycle_callback( virConnectPtr conn,
                                                        virDomainPtr domain,
                                                        int event,
                                                        int detail,
                                                        void *opaque);
            static int domain_event_generic_callback(   virConnectPtr conn,
                                                        virDomainPtr domain,
                                                        void *opaque);
            static int domain_event_rtcchange_callback( virConnectPtr conn,
                                                        virDomainPtr domain,
                                                        long long utcoffset,
                                                        void *opaque);
            static int domain_event_watchdog_callback(  virConnectPtr conn,
                                                        virDomainPtr domain,
                                                        int action,
                                                        void *opaque);
            static int domain_event_io_error_callback(  virConnectPtr conn,
                                                        virDomainPtr domain,
                                                        const char *src_path,
                                                        const char *dev_alias,
                                                        int action,
                                                        void *opaque);
            static int domain_event_io_error_reason_callback(virConnectPtr conn,
                                                             virDomainPtr domain,
                                                             const char *src_path,
                                                             const char *dev_alias,
                                                             int action,
                                                             const char *reason,
                                                             void *opaque);
            static int domain_event_graphics_callback(  virConnectPtr conn,
                                                        virDomainPtr domain,
                                                        int phase,
                                                        virDomainEventGraphicsAddressPtr local,
                                                        virDomainEventGraphicsAddressPtr remote,
                                                        const char *authScheme,
                                                        virDomainEventGraphicsSubjectPtr subject,
                                                        void *opaque);
            static int auth_callback(   virConnectCredentialPtr cred,
                                        unsigned int ncred,
                                        void *data);
    };

    class ConnectWorker : public LibvirtWorker {
        public:
            ConnectWorker(NanCallback *callback, Hypervisor *hypervisor)
                : LibvirtWorker(callback, NULL), hypervisor_(hypervisor) {}
            void Execute();
            static int auth_callback(virConnectCredentialPtr cred, unsigned int ncred, void *data);
        private:
            Hypervisor *hypervisor_;
    };

    class GetCapabilitiesWorker : public StringReturnWorker<LibvirtWorker, virConnectPtr> {
        public:
            GetCapabilitiesWorker(NanCallback *callback, virConnectPtr conn)
                : StringReturnWorker(callback, conn) {}
            void Execute();
    };

}  // namespace NodeLibvirt

#endif  // SRC_HYPERVISOR_H_
