// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>
#include <string.h>
#include <node_buffer.h>
#include "domain.h"

namespace NodeLibvirt {
	Persistent<FunctionTemplate> Domain::constructor_template;

	static Persistent<String> state_symbol;
	static Persistent<String> max_memory_symbol;
	static Persistent<String> memory_symbol;
	static Persistent<String> vcpus_number_symbol;
	static Persistent<String> cpu_time_symbol;
	static Persistent<String> number_symbol;
	static Persistent<String> cpu_symbol;
	static Persistent<String> affinity_symbol;
	static Persistent<String> usable_symbol;
	static Persistent<String> migration_uri_symbol;
	static Persistent<String> migration_name_symbol;
	static Persistent<String> migration_bandwidth_symbol;
	static Persistent<String> migration_flags_symbol;
	static Persistent<String> migration_hypervisor_symbol;

	//Jobinfo symbols
	static Persistent<String> type_symbol;
	static Persistent<String> time_symbol;
	static Persistent<String> data_symbol;
	static Persistent<String> file_symbol;
	static Persistent<String> elapsed_symbol;
	static Persistent<String> remaining_symbol;
	static Persistent<String> total_symbol;
	static Persistent<String> processed_symbol;

	//SecurityLabel symbols
	static Persistent<String> label_symbol;
	static Persistent<String> enforcing_symbol;

	//memory stat symbols
	static Persistent<String> memory_stat_swap_in_symbol;
	static Persistent<String> memory_stat_swap_out_symbol;
	static Persistent<String> memory_stat_major_fault_symbol;
	static Persistent<String> memory_stat_minor_fault_symbol;
	static Persistent<String> memory_stat_unused_symbol;
	static Persistent<String> memory_stat_available_symbol;

	//block stat symbols
	static Persistent<String> block_stat_rd_req_symbol;
	static Persistent<String> block_stat_rd_bytes_symbol;
	static Persistent<String> block_stat_wr_req_symbol;
	static Persistent<String> block_stat_wr_bytes_symbol;
	static Persistent<String> block_stat_errs_symbol;

	//block info symbols
	static Persistent<String> block_info_capacity_symbol;
	static Persistent<String> block_info_allocation_symbol;
	static Persistent<String> block_info_physical_symbol;

	//block job info symbols
	static Persistent<String> block_job_info_type_symbol;
	static Persistent<String> block_job_info_bandwidth_symbol;
	static Persistent<String> block_job_info_cur_symbol;
	static Persistent<String> block_job_info_end_symbol;

	//domain network interface statistics symbols
	static Persistent<String> nwiface_stat_rx_bytes_symbol;
	static Persistent<String> nwiface_stat_rx_packets_symbol;
	static Persistent<String> nwiface_stat_rx_errors_symbol;
	static Persistent<String> nwiface_stat_rx_drop_symbol;
	static Persistent<String> nwiface_stat_tx_bytes_symbol;
	static Persistent<String> nwiface_stat_tx_packets_symbol;
	static Persistent<String> nwiface_stat_tx_errors_symbol;
	static Persistent<String> nwiface_stat_tx_drop_symbol;

	struct BatonBase {
		v8::Persistent<v8::Function> callback;
		std::string error;
		Domain* domain;

		virtual ~BatonBase() {
			callback.Dispose();
		}
	};

	struct AttachDeviceBaton : BatonBase {
		const char* xml;
		unsigned int flags;
	};

	struct BlockPullBaton : BatonBase {
		const char* disk;
		unsigned int bandwidth;
		unsigned int flags;
	};

	struct CreateDomainBaton : BatonBase {
		const char* xml;
		unsigned int flags;
		Hypervisor* hypervisor;
  };

	struct DetachDeviceBaton : BatonBase {
		const char* xml;
		unsigned int flags;
	};

	struct GetBlockJobInfoBaton : BatonBase {
		const char* disk;
		unsigned int flags;
		virDomainBlockJobInfo info;
	};

	struct GetInfoBaton : BatonBase {
		virDomainInfo res;
	};

  struct GetSaveImageXmlBaton : BatonBase {
		Hypervisor* hypervisor;
		const char* path;
		unsigned int flags;
		char* xml;
  };

	struct DeleteSnapshotBaton : BatonBase {
		const char* name;
		unsigned int flags;
	};

	struct DestroyBaton : BatonBase {
		int res;
	};

	struct LookupDomainByIdBaton : BatonBase {
		int id;
		Hypervisor* hypervisor;
	};

	struct LookupDomainByNameBaton : BatonBase {
		const char* name;
		Hypervisor* hypervisor;
  };

	struct RestoreBaton : BatonBase {
    int res;
		const char* path;
		Hypervisor* hypervisor;
  };

	struct ResumeBaton : BatonBase {
		int res;
	};

	struct RevertToSnapshotBaton : BatonBase {
		const char* name;
		unsigned int lookupflags;
		unsigned int revertflags;
	};

  struct SetTimeBaton : BatonBase {
    int ret;
    long long seconds;
    unsigned int nseconds;
    unsigned int flags;
  };

	struct SaveBaton : BatonBase {
		int res;
		const char* path;
	};

	struct SuspendBaton : BatonBase {
		int res;
	};

	struct TakeSnapshotBaton : BatonBase {
		const char* xml;
		unsigned int flags;
	};

	struct ToXmlBaton : BatonBase {
		unsigned int flags;
		char* xml;
	};

  struct UpdateSaveImageXmlBaton : BatonBase {
		Hypervisor* hypervisor;
		const char* path;
		const char* xml;
		unsigned int flags;
		int res;
  };

	void Domain::Initialize() {
		Local<FunctionTemplate> t = FunctionTemplate::New();

		//        t->Inherit(EventEmitter::constructor_template);
		t->InstanceTemplate()->SetInternalFieldCount(1);

		NODE_SET_PROTOTYPE_METHOD(t, "getId",
				Domain::GetId);
		NODE_SET_PROTOTYPE_METHOD(t, "toXml",
				Domain::ToXml);
		NODE_SET_PROTOTYPE_METHOD(t, "getJobInfo",
				Domain::GetJobInfo);
		NODE_SET_PROTOTYPE_METHOD(t, "abortCurrentJob",
				Domain::AbortCurrentJob);
		NODE_SET_PROTOTYPE_METHOD(t, "getMaxMemory",
				Domain::GetMaxMemory);
		NODE_SET_PROTOTYPE_METHOD(t, "setMaxMemory",
				Domain::SetMaxMemory);
		NODE_SET_PROTOTYPE_METHOD(t, "setMemory",
				Domain::SetMemory);
		NODE_SET_PROTOTYPE_METHOD(t, "getMaxVcpus",
				Domain::GetMaxVcpus);
		NODE_SET_PROTOTYPE_METHOD(t, "getAutostart",
				Domain::GetAutostart);
		NODE_SET_PROTOTYPE_METHOD(t, "setAutostart",
				Domain::SetAutostart);
		NODE_SET_PROTOTYPE_METHOD(t, "getName",
				Domain::GetName);
		NODE_SET_PROTOTYPE_METHOD(t, "getOsType",
				Domain::GetOsType);
		NODE_SET_PROTOTYPE_METHOD(t, "getInfo",
				Domain::GetInfo);
		NODE_SET_PROTOTYPE_METHOD(t, "getSchedParams",
				Domain::GetSchedParams);
		NODE_SET_PROTOTYPE_METHOD(t, "setSchedParams",
				Domain::SetSchedParams);
		/*NODE_SET_PROTOTYPE_METHOD(t, "getSchedType",
			Domain::GetSchedType); */ //It's necessary this function?
		NODE_SET_PROTOTYPE_METHOD(t, "getSecurityLabel",
				Domain::GetSecurityLabel);
		NODE_SET_PROTOTYPE_METHOD(t, "hasManagedImage",
				Domain::HasManagedImage);
		NODE_SET_PROTOTYPE_METHOD(t, "saveManagedImage",
				Domain::SaveManagedImage);
		NODE_SET_PROTOTYPE_METHOD(t, "removeManagedImage",
				Domain::RemoveManagedImage);
		NODE_SET_PROTOTYPE_METHOD(t, "memoryPeek",
				Domain::MemoryPeek);
		NODE_SET_PROTOTYPE_METHOD(t, "getMemoryStats",
				Domain::GetMemoryStats);
		NODE_SET_PROTOTYPE_METHOD(t, "blockPeek",
				Domain::BlockPeek);
		NODE_SET_PROTOTYPE_METHOD(t, "blockPull",
				Domain::BlockPull);
		NODE_SET_PROTOTYPE_METHOD(t, "blockRebase",
				Domain::BlockRebase);
		NODE_SET_PROTOTYPE_METHOD(t, "getBlockStats",
				Domain::GetBlockStats);
		NODE_SET_PROTOTYPE_METHOD(t, "getBlockInfo",
				Domain::GetBlockInfo);
		NODE_SET_PROTOTYPE_METHOD(t, "getBlockJobInfo",
				Domain::GetBlockJobInfo);
		NODE_SET_PROTOTYPE_METHOD(t, "abortBlockJob",
				Domain::AbortBlockJob);
		NODE_SET_PROTOTYPE_METHOD(t, "getUUID",
				Domain::GetUUID);
		NODE_SET_PROTOTYPE_METHOD(t, "getVcpus",
				Domain::GetVcpus);
		NODE_SET_PROTOTYPE_METHOD(t, "setVcpus",
				Domain::SetVcpus);
		NODE_SET_PROTOTYPE_METHOD(t, "isActive",
				Domain::IsActive);
		NODE_SET_PROTOTYPE_METHOD(t, "isPersistent",
				Domain::IsPersistent);
		NODE_SET_PROTOTYPE_METHOD(t, "isUpdated",
				Domain::IsUpdated);
		NODE_SET_PROTOTYPE_METHOD(t, "getInterfaceStats",
				Domain::GetInterfaceStats);
		NODE_SET_PROTOTYPE_METHOD(t, "coreDump",
				Domain::CoreDump);
		NODE_SET_PROTOTYPE_METHOD(t, "migrate",
				Domain::Migrate);
		NODE_SET_PROTOTYPE_METHOD(t, "setMigrationMaxDowntime",
				Domain::SetMigrationMaxDowntime);
		NODE_SET_PROTOTYPE_METHOD(t, "pinVcpu",
				Domain::PinVcpu);
		NODE_SET_PROTOTYPE_METHOD(t, "reboot",
				Domain::Reboot);
		NODE_SET_PROTOTYPE_METHOD(t, "reset",
				Domain::Reset);
		NODE_SET_PROTOTYPE_METHOD(t, "resume",
				Domain::Resume);
		NODE_SET_PROTOTYPE_METHOD(t, "save",
				Domain::Save);
    NODE_SET_PROTOTYPE_METHOD(t, "setTime",
        Domain::SetTime);
		NODE_SET_PROTOTYPE_METHOD(t, "shutdown",
				Domain::Shutdown);
		NODE_SET_PROTOTYPE_METHOD(t, "start",
				Domain::Start);
		NODE_SET_PROTOTYPE_METHOD(t, "suspend",
				Domain::Suspend);
		NODE_SET_PROTOTYPE_METHOD(t, "sendKey",
				Domain::SendKey);
		NODE_SET_PROTOTYPE_METHOD(t, "attachDevice",
				Domain::AttachDevice);
		NODE_SET_PROTOTYPE_METHOD(t, "detachDevice",
				Domain::DetachDevice);
		NODE_SET_PROTOTYPE_METHOD(t, "updateDevice",
				Domain::UpdateDevice);
		NODE_SET_PROTOTYPE_METHOD(t, "destroy",
				Domain::Destroy);
		NODE_SET_PROTOTYPE_METHOD(t, "undefine",
				Domain::Undefine);
		NODE_SET_PROTOTYPE_METHOD(t, "hasCurrentSnapshot",
				Domain::HasCurrentSnapshot);
		NODE_SET_PROTOTYPE_METHOD(t, "revertToSnapshot",
				Domain::RevertToSnapshot);
		NODE_SET_PROTOTYPE_METHOD(t, "takeSnapshot",
				Domain::TakeSnapshot);
		NODE_SET_PROTOTYPE_METHOD(t, "getCurrentSnapshot",
				Domain::GetCurrentSnapshot);
		NODE_SET_PROTOTYPE_METHOD(t, "deleteSnapshot",
				Domain::DeleteSnapshot);
		NODE_SET_PROTOTYPE_METHOD(t, "lookupSnapshotByName",
				Domain::LookupSnapshotByName);
		NODE_SET_PROTOTYPE_METHOD(t, "getSnapshots",
				Domain::GetSnapshots);

		constructor_template = Persistent<FunctionTemplate>::New(t);
		constructor_template->SetClassName(String::NewSymbol("Domain"));

		Local<ObjectTemplate> object_tmpl = t->InstanceTemplate();

		//Constants initialization
		//virDomainState
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_NOSTATE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_RUNNING);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_BLOCKED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_PAUSED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SHUTDOWN);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SHUTOFF);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_CRASHED);

#ifdef VIR_DOMAIN_PMSUSPENDED
		// If its available in libvirt.h, then make it available in node
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_PMSUSPENDED);
#endif

		//virDomainSnapshotCreateFlags
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_CREATE_REDEFINE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_CREATE_CURRENT);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_CREATE_NO_METADATA);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_CREATE_HALT);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_CREATE_DISK_ONLY);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_CREATE_REUSE_EXT);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_CREATE_QUIESCE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_CREATE_ATOMIC);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_CREATE_LIVE);

		//virDomainSnapshotRevertFlags
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_REVERT_RUNNING);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_REVERT_PAUSED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_REVERT_FORCE);

		//virDomainSnapshotDeleteFlags
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_DELETE_CHILDREN);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_DELETE_METADATA_ONLY);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SNAPSHOT_DELETE_CHILDREN_ONLY);

		//virDomainDeviceModifyFlags
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_DEVICE_MODIFY_CURRENT);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_DEVICE_MODIFY_LIVE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_DEVICE_MODIFY_CONFIG);

		//virDomainBlockCommitFlags
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_BLOCK_COMMIT_SHALLOW);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_BLOCK_COMMIT_DELETE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_BLOCK_COMMIT_ACTIVE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_BLOCK_COMMIT_RELATIVE);

		//virDomainBlockRebaseFlags
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_BLOCK_REBASE_SHALLOW);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_BLOCK_REBASE_REUSE_EXT);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_BLOCK_REBASE_COPY_RAW);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_BLOCK_REBASE_COPY);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_BLOCK_REBASE_RELATIVE);

		//virDomainMigrateFlags
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_LIVE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_PEER2PEER);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_TUNNELLED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_PERSIST_DEST);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_UNDEFINE_SOURCE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_PAUSED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_NON_SHARED_DISK);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_NON_SHARED_INC);

		//virDomainXMLFlags
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_XML_SECURE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_XML_INACTIVE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_XML_UPDATE_CPU);

		//virDomainJobType
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_NONE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_BOUNDED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_UNBOUNDED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_COMPLETED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_FAILED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_CANCELLED);

		//virDomainMemoryFlags
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_MEMORY_VIRTUAL);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_MEMORY_PHYSICAL);

		//virDomainEventType
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_DEFINED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_UNDEFINED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_RESUMED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED);

		//virDomainEventIOErrorAction
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_IO_ERROR_NONE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_IO_ERROR_PAUSE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_IO_ERROR_REPORT);

		//virDomainEventResumedDetailType
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_RESUMED_UNPAUSED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_RESUMED_MIGRATED);

		//virDomainEventStartedDetailType
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED_BOOTED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED_MIGRATED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED_RESTORED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED_FROM_SNAPSHOT);

		//virDomainEventStoppedDetailType
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_SHUTDOWN);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_DESTROYED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_CRASHED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_MIGRATED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_SAVED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_FAILED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_FROM_SNAPSHOT);

		//virDomainEventSuspendedDetailType
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED_PAUSED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED_MIGRATED);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED_IOERROR);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED_WATCHDOG);

		//virDomainEventUndefinedDetailType
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_UNDEFINED_REMOVED);

		//virDomainEventWatchdogAction
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_NONE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_PAUSE);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_RESET);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_POWEROFF);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_SHUTDOWN);
		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_DEBUG);


		NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SEND_KEY_MAX_KEYS);

		state_symbol        = NODE_PSYMBOL("state");
		max_memory_symbol   = NODE_PSYMBOL("max_memory");
		memory_symbol       = NODE_PSYMBOL("memory");
		vcpus_number_symbol = NODE_PSYMBOL("vcpus_number");
		cpu_time_symbol     = NODE_PSYMBOL("cpu_time");
		number_symbol       = NODE_PSYMBOL("number");
		cpu_symbol          = NODE_PSYMBOL("cpu");
		affinity_symbol     = NODE_PSYMBOL("affinity");
		usable_symbol       = NODE_PSYMBOL("usable");
		migration_uri_symbol            = NODE_PSYMBOL("dest_uri");
		migration_name_symbol           = NODE_PSYMBOL("dest_name");
		migration_bandwidth_symbol      = NODE_PSYMBOL("bandwidth");
		migration_flags_symbol          = NODE_PSYMBOL("flags");
		migration_hypervisor_symbol     = NODE_PSYMBOL("dest_hypervisor");

		type_symbol         = NODE_PSYMBOL("type");
		time_symbol         = NODE_PSYMBOL("time");
		data_symbol         = NODE_PSYMBOL("data");
		file_symbol         = NODE_PSYMBOL("file");
		elapsed_symbol      = NODE_PSYMBOL("elapsed");
		remaining_symbol    = NODE_PSYMBOL("remaining");
		total_symbol        = NODE_PSYMBOL("total");
		processed_symbol    = NODE_PSYMBOL("processed");

		label_symbol = NODE_PSYMBOL("label");
		enforcing_symbol = NODE_PSYMBOL("enforcing");

		memory_stat_swap_in_symbol = NODE_PSYMBOL("swap_in");
		memory_stat_swap_out_symbol = NODE_PSYMBOL("swap_out");
		memory_stat_major_fault_symbol = NODE_PSYMBOL("major_fault");
		memory_stat_minor_fault_symbol = NODE_PSYMBOL("minor_fault");
		memory_stat_unused_symbol = NODE_PSYMBOL("unused");
		memory_stat_available_symbol = NODE_PSYMBOL("available");

		block_stat_rd_req_symbol = NODE_PSYMBOL("read_requests");
		block_stat_rd_bytes_symbol = NODE_PSYMBOL("read_bytes");
		block_stat_wr_req_symbol = NODE_PSYMBOL("write_requests");
		block_stat_wr_bytes_symbol = NODE_PSYMBOL("write_bytes");
		block_stat_errs_symbol = NODE_PSYMBOL("errors");

		block_info_capacity_symbol = NODE_PSYMBOL("capacity");
		block_info_allocation_symbol = NODE_PSYMBOL("allocation");
		block_info_physical_symbol = NODE_PSYMBOL("physical");

		block_job_info_type_symbol = NODE_PSYMBOL("type");
		block_job_info_bandwidth_symbol = NODE_PSYMBOL("bandwidth");
		block_job_info_cur_symbol = NODE_PSYMBOL("cur");
		block_job_info_end_symbol = NODE_PSYMBOL("end");

		nwiface_stat_rx_bytes_symbol = NODE_PSYMBOL("rx_bytes");
		nwiface_stat_rx_packets_symbol = NODE_PSYMBOL("rx_packets");
		nwiface_stat_rx_errors_symbol = NODE_PSYMBOL("rx_errors");
		nwiface_stat_rx_drop_symbol = NODE_PSYMBOL("rx_drop");
		nwiface_stat_tx_bytes_symbol = NODE_PSYMBOL("tx_bytes");
		nwiface_stat_tx_packets_symbol = NODE_PSYMBOL("tx_packets");
		nwiface_stat_tx_errors_symbol = NODE_PSYMBOL("tx_errors");
		nwiface_stat_tx_drop_symbol = NODE_PSYMBOL("tx_drop");
	}

	virDomainPtr Domain::domain() const {
		return domain_;
	}

	void CreateDomainAsync(uv_work_t* req) {

		CreateDomainBaton* baton = static_cast<CreateDomainBaton*>(req->data);

		Hypervisor *hypervisor = baton->hypervisor;
		const char *xml = baton->xml;
		unsigned int flags = baton->flags;

		virErrorPtr err;
		Domain *domain = new Domain();

		domain->domain_ = virDomainCreateXML(hypervisor->connection(), xml, flags);

		if(domain->domain_ == NULL) {
			err = virGetLastError();
			baton->error = err->message;
		}

		else {
			baton->domain = domain;
		}
	}

	void CreateDomainAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		CreateDomainBaton* baton = static_cast<CreateDomainBaton*>(req->data);
		delete req;

		Handle<Value> argv[2];

		if (!baton->error.empty()) {
			argv[0] = Exception::Error(String::New(baton->error.c_str()));
			argv[1] = Undefined();
		}

		else {
			Domain *domain = baton->domain;
			Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
			domain->Wrap(domain_obj);

			argv[0] = Undefined();
			argv[1] = scope.Close(domain_obj);
		}

		TryCatch try_catch;

		if (try_catch.HasCaught())
			FatalException(try_catch);

		baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
		delete baton;
	}

	Handle<Value> Domain::Create(const Arguments& args) {

		HandleScope scope;
		unsigned int flags = 0;

		if(!args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a string as first argument")));
		}

		if(!args[1]->IsArray()) {
			return ThrowException(Exception::TypeError(
						String::New("Second argument, if specified, must be an array")));
		}

		// Flags
		Local<Array> flags_ = Local<Array>::Cast(args[1]);
		unsigned int length = flags_->Length();

		for (unsigned int i = 0; i < length; i++) {
			flags |= flags_->Get(Integer::New(i))->Int32Value();
		}

		Local<Object> hyp_obj = args.This();

		if(!Hypervisor::HasInstance(hyp_obj)) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a Hypervisor object instance")));
		}

		Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

		const char *xml = parseString(args[0]);

		// Callback
		Local<Function> callback = Local<Function>::Cast(args[2]);

		// Create baton; add callback, hypervisor, flags, and xml
		CreateDomainBaton* baton = new CreateDomainBaton();

		baton->callback = Persistent<Function>::New(callback);
		baton->hypervisor = hypervisor;
		baton->flags = flags;
		baton->xml = xml;

		// Compose req
		uv_work_t* req = new uv_work_t();
		req->data = baton;

		// Dispatch work
		uv_queue_work(
				uv_default_loop(),
				req,
				CreateDomainAsync,
				(uv_after_work_cb)CreateDomainAsyncAfter
				);

		return scope.Close(Undefined());
	}

	Handle<Value> Domain::Define(const Arguments& args) {
		HandleScope scope;

		int argsl = args.Length();

		if(argsl == 0) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify at least one argument")));
		}

		if(!args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a string as first argument")));
		}

		Local<Object> hyp_obj = args.This();

		if(!Hypervisor::HasInstance(hyp_obj)) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a Hypervisor instance")));
		}

		String::Utf8Value xml(args[0]->ToString());

		Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

		Domain *domain = new Domain();
		domain->domain_ = virDomainDefineXML(hypervisor->connection(), (const char *) *xml);

		if(domain->domain_ == NULL) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();

		domain->Wrap(domain_obj);

		return scope.Close(domain_obj);
	}

	Handle<Value> Domain::Undefine(const Arguments& args) {
		HandleScope scope;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
		ret = virDomainUndefine(domain->domain_);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	void LookupDomainByIdAsync(uv_work_t* req) {
		LookupDomainByIdBaton* baton = static_cast<LookupDomainByIdBaton*>(req->data);

		int id = baton->id;
		Hypervisor *hypervisor = baton->hypervisor;
		virErrorPtr err;

		Domain *domain = new Domain();
		domain->domain_ = virDomainLookupByID(hypervisor->connection(), id);

		if (domain->domain_ == NULL) {
			err = virGetLastError();
			baton->error = err->message;
		}

		else {
			baton->domain = domain;
		}
	}

	void LookupDomainByIdAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		LookupDomainByIdBaton* baton = static_cast<LookupDomainByIdBaton*>(req->data);
		delete req;
		Handle<Value> argv[2];

		if (!baton->error.empty()) {
			argv[0] = Exception::Error(String::New(baton->error.c_str()));
			argv[1] = Undefined();
		}
		else {
			Domain *domain = baton->domain;
			Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
			domain->Wrap(domain_obj);

			argv[0] = Undefined();
			argv[1] = scope.Close(domain_obj);
		}

		TryCatch try_catch;

		if (try_catch.HasCaught())
			FatalException(try_catch);

		baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
		delete baton;
	}

	Handle<Value> Domain::LookupById(const Arguments& args) {
		HandleScope scope;
		int id = -1;

		// Create baton
		LookupDomainByIdBaton* baton = new LookupDomainByIdBaton();

		if(args.Length() == 0 || !args[0]->IsInt32()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a domain id as an int")));
		}

		Local<Object> hyp_obj = args.This();

		if(!Hypervisor::HasInstance(hyp_obj)) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a Hypervisor instance")));
		}

		id = args[0]->Int32Value();
		// Unwrap hypervisor
		Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

		// Callback
		Local<Function> callback = Local<Function>::Cast(args[1]);

		// Add callback, name, and hypervisor
		baton->callback = Persistent<Function>::New(callback);
		baton->id = id;
		baton->hypervisor = hypervisor;

		// Compose req
		uv_work_t* req = new uv_work_t;
		req->data = baton;

		// Dispatch work
		uv_queue_work(
			uv_default_loop(),
			req,
			LookupDomainByIdAsync,
			(uv_after_work_cb)LookupDomainByIdAsyncAfter
		);

		return scope.Close(Undefined());
	}

	void LookupDomainByNameAsync(uv_work_t* req) {
		LookupDomainByNameBaton* baton = static_cast<LookupDomainByNameBaton*>(req->data);

		const char* name = baton->name;
		Hypervisor *hypervisor = baton->hypervisor;
		virErrorPtr err;

		Domain *domain = new Domain();
		domain->domain_ = virDomainLookupByName(hypervisor->connection(), name);

		if(domain->domain_ == NULL) {
			err = virGetLastError();
			baton->error = err->message;
		}

		else {
			baton->domain = domain;
		}
	}

	void LookupDomainByNameAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		LookupDomainByNameBaton* baton = static_cast<LookupDomainByNameBaton*>(req->data);
		delete req;
		Handle<Value> argv[2];

		if (!baton->error.empty()) {
			argv[0] = Exception::Error(String::New(baton->error.c_str()));
			argv[1] = Undefined();
		}

		else {
			Domain *domain = baton->domain;
			Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
			domain->Wrap(domain_obj);

			argv[0] = Undefined();
			argv[1] = scope.Close(domain_obj);
		}

		TryCatch try_catch;

		if (try_catch.HasCaught())
			FatalException(try_catch);

		baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
		delete baton;
	}

	Handle<Value> Domain::LookupByName(const Arguments& args) {
		HandleScope scope;

		if(args.Length() == 0 || !args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a domain name as a string")));
		}

		Local<Object> hyp_obj = args.This();

		if(!Hypervisor::HasInstance(hyp_obj)) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a Hypervisor instance")));
		}

		const char* name = parseString(args[0]);

		// Unwrap hypervisor
		Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

		// Callback
		Local<Function> callback = Local<Function>::Cast(args[1]);

		// Create baton
		LookupDomainByNameBaton* baton = new LookupDomainByNameBaton();

		// Add callback, name, and hypervisor
		baton->callback = Persistent<Function>::New(callback);
		baton->name = name;
		baton->hypervisor = hypervisor;

		// Compose req
		uv_work_t* req = new uv_work_t;
		req->data = baton;

		// Dispatch work
		uv_queue_work(
				uv_default_loop(),
				req,
				LookupDomainByNameAsync,
				(uv_after_work_cb)LookupDomainByNameAsyncAfter
				);

		return scope.Close(Undefined());
	}

	Handle<Value> Domain::LookupByUUID(const Arguments& args) {
		HandleScope scope;

		if(args.Length() == 0 || !args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a UUID string.")));
		}

		Local<Object> hyp_obj = args.This();

		if(!Hypervisor::HasInstance(hyp_obj)) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a Hypervisor instance")));
		}

		String::Utf8Value uuid(args[0]->ToString());

		Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

		Domain *domain = new Domain();
		domain->domain_ = virDomainLookupByUUIDString(hypervisor->connection(), (const char *) *uuid);

		if(domain->domain_ == NULL) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();

		domain->Wrap(domain_obj);

		return scope.Close(domain_obj);
	}

	Handle<Value> Domain::GetId(const Arguments& args) {
		HandleScope scope;
		unsigned int id = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		id = virDomainGetID(domain->domain_);

		if(id == -1u) {
			if (virGetLastError() != NULL) {
				ThrowException(Error::New(virGetLastError()));
			}

			return Null();
		}

		return scope.Close(Integer::NewFromUnsigned(id));
	}

	void GetInfoAsync(uv_work_t* req) {
		GetInfoBaton* baton = static_cast<GetInfoBaton*>(req->data);

		Domain *domain = baton->domain;

		virDomainInfo info;
		virErrorPtr err;

		int ret = -1;

		ret = virDomainGetInfo(domain->domain_, &info);

		if(ret == -1) {
			err = virGetLastError();
			baton->error = err->message;
		}

		else {
			baton->res = info;
		}
	}

	void GetInfoAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		GetInfoBaton* baton = static_cast<GetInfoBaton*>(req->data);
		delete req;

		Handle<Value> argv[2];

		virDomainInfo res = baton->res;

		if (!baton->error.empty()) {
			argv[0] = Exception::Error(String::New(baton->error.c_str()));
			argv[1] = Undefined();
		}

		else {
			Local<Object> object = Object::New();

			object->Set(state_symbol, Integer::New(res.state)); //virDomainState
			object->Set(max_memory_symbol, Number::New(res.maxMem)); //KBytes
			object->Set(memory_symbol, Number::New(res.memory)); //KBytes
			object->Set(vcpus_number_symbol, Integer::New(res.nrVirtCpu));
			object->Set(cpu_time_symbol, Number::New(res.cpuTime)); //nanoseconds

			argv[0] = Undefined();
			argv[1] = scope.Close(object);
		}

		TryCatch try_catch;

		if (try_catch.HasCaught())
			FatalException(try_catch);

		baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
		delete baton;
	}

	Handle<Value> Domain::GetInfo(const Arguments& args) {
		HandleScope scope;

		// Domain context
		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		// Callback
		Local<Function> callback = Local<Function>::Cast(args[0]);

		// Create baton
		GetInfoBaton* baton = new GetInfoBaton();

		// Add callback and domain
		baton->callback = Persistent<Function>::New(callback);
		baton->domain = domain;

		// Compose req
		uv_work_t* req = new uv_work_t;
		req->data = baton;

		// Dispatch work
		uv_queue_work(
				uv_default_loop(),
				req,
				GetInfoAsync,
				(uv_after_work_cb)GetInfoAsyncAfter
				);

		return Undefined();
	}

	Handle<Value> Domain::GetName(const Arguments& args) {
		HandleScope scope;
		const char *name = NULL;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		name = virDomainGetName(domain->domain_);

		if(name == NULL) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		return scope.Close(String::New(name));
	}

	Handle<Value> Domain::GetUUID(const Arguments& args) {
		HandleScope scope;
		int ret = -1;
		char *uuid = new char[VIR_UUID_STRING_BUFLEN];

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainGetUUIDString(domain->domain_, uuid);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			delete[] uuid;
			return Null();
		}

		Local<String> uuid_str = String::New(uuid);

		delete[] uuid;

		return scope.Close(uuid_str);
	}

	Handle<Value> Domain::GetAutostart(const Arguments& args) {
		HandleScope scope;
		int ret = -1;
		int autostart_;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainGetAutostart(domain->domain_, &autostart_);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		bool autostart = autostart_ == 0 ? true : false;

		return scope.Close(Boolean::New(autostart));
	}

	Handle<Value> Domain::SetAutostart(const Arguments& args) {
		HandleScope scope;
		int ret = -1;

		if(args.Length() == 0 || !args[0]->IsBoolean()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a boolean argument")));
		}

		bool autostart = args[0]->IsTrue();

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainSetAutostart(domain->domain_, autostart ? 0 : 1);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	Handle<Value> Domain::GetMaxMemory(const Arguments& args) {
		HandleScope scope;
		unsigned long memory = 0;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		memory = virDomainGetMaxMemory(domain->domain_);

		if(memory == 0) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		return scope.Close(Number::New(memory));
	}

	Handle<Value> Domain::SetMaxMemory(const Arguments& args) {
		HandleScope scope;
		unsigned long memory = 0;
		int ret = -1;

		if(args.Length() == 0 || !args[0]->IsInt32()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a valid amount of memory")));
		}

		memory = args[0]->Int32Value();

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainSetMaxMemory(domain->domain_, memory);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	Handle<Value> Domain::SetMemory(const Arguments& args) {
		HandleScope scope;
		unsigned long memory = 0;
		int ret = -1;

		if(args.Length() == 0 || !args[0]->IsInt32()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a valid amount of memory")));
		}

		memory = args[0]->Int32Value();

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainSetMemory(domain->domain_, memory);
		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	Handle<Value> Domain::GetOsType(const Arguments& args) {
		HandleScope scope;
		char *os_type = NULL;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		os_type = virDomainGetOSType(domain->domain_);

		if(os_type == NULL) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		return scope.Close(String::New(os_type));
	}

	Handle<Value> Domain::GetMaxVcpus(const Arguments& args) {
		HandleScope scope;
		int vcpus = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		vcpus = virDomainGetMaxVcpus(domain->domain_);

		if(vcpus == -1) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		return scope.Close(Integer::New(vcpus));
	}

	Handle<Value> Domain::IsActive(const Arguments& args) {
		HandleScope scope;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainIsActive(domain->domain_);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}
		bool is_active = ret == 1 ? true : false;

		return scope.Close(Boolean::New(is_active));
	}

	Handle<Value> Domain::IsPersistent(const Arguments& args) {
		HandleScope scope;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainIsPersistent(domain->domain_);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}
		bool is_persistent = ret == 1 ? true : false;

		return scope.Close(Boolean::New(is_persistent));
	}

	Handle<Value> Domain::IsUpdated(const Arguments& args) {
		HandleScope scope;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		int ret = virDomainIsUpdated(domain->domain_);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		return scope.Close(Boolean::New(ret == 1));
	}

	Handle<Value> Domain::Reboot(const Arguments& args) {
		HandleScope scope;
		unsigned int flags = 0;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainReboot(domain->domain_, flags);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	Handle<Value> Domain::Reset(const Arguments& args) {
		HandleScope scope;
		unsigned int flags = 0;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainReset(domain->domain_, flags);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	void SaveAsync(uv_work_t* req) {
		SaveBaton* baton = static_cast<SaveBaton*>(req->data);
		Domain *domain = baton->domain;
		const char *path = baton->path;
		virErrorPtr err;

		int ret = -1;
		ret = virDomainSave(domain->domain_, path);

		if(ret == -1) {
			err = virGetLastError();
			baton->error = err->message;
		}

		baton->res = ret;
	}

	void SaveAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		SaveBaton* baton = static_cast<SaveBaton*>(req->data);
		delete req;

		Handle<Value> argv[2];

		if (!baton->error.empty()) {
			argv[0] = Exception::Error(String::New(baton->error.c_str()));
			argv[1] = Undefined();
		}

		else {
			argv[0] = Undefined();
			argv[1] = scope.Close(True());
		}

		TryCatch try_catch;

		if (try_catch.HasCaught())
			FatalException(try_catch);

		baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
		delete baton;
	}

	Handle<Value> Domain::Save(const Arguments& args) {
		HandleScope scope;

		if(args.Length() == 0 || !args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify the destination path string as the first argument")));
		}

		if(args.Length() == 1 || !args[1]->IsFunction()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a callback function as the second argument")));
		}

    // Path string
		const char *path = parseString(args[0]);

    // Domain context
    Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

    // Create baton
    SaveBaton* baton = new SaveBaton();

    // Callback
    Local<Function> callback = Local<Function>::Cast(args[1]);
    baton->callback = Persistent<Function>::New(callback);

    // Add data
    baton->domain = domain;
    baton->path   = path;

    // Compose req
    uv_work_t* req = new uv_work_t;
    req->data = baton;

    uv_queue_work(
        uv_default_loop(),
        req,
        SaveAsync,
        (uv_after_work_cb)SaveAsyncAfter
    );

    return scope.Close(Undefined());
	}

  void RestoreAsync(uv_work_t* req) {
    RestoreBaton* baton = static_cast<RestoreBaton*>(req->data);
    Hypervisor *hypervisor = baton->hypervisor;
    const char *path = baton->path;
    virErrorPtr err;

    int ret = -1;
    ret = virDomainRestore(hypervisor->connection(), path);

    if(ret == -1) {
      err = virGetLastError();
      baton->error = err->message;
    }

    baton->res = ret;
  }

  void RestoreAsyncAfter(uv_work_t* req) {
    HandleScope scope;

    RestoreBaton* baton = static_cast<RestoreBaton*>(req->data);
    delete req;

    Handle<Value> argv[2];

    if (!baton->error.empty()) {
      argv[0] = Exception::Error(String::New(baton->error.c_str()));
      argv[1] = Undefined();
    }

    else {
      argv[0] = Undefined();
      argv[1] = scope.Close(True());
    }

    TryCatch try_catch;

    if (try_catch.HasCaught())
      FatalException(try_catch);

    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    delete baton;
  }


	Handle<Value> Domain::Restore(const Arguments& args) {
		HandleScope scope;

		if(args.Length() == 0 || !args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a path string as the first argument")));
		}

		Local<Object> hyp_obj = args.This();

		if(!Hypervisor::HasInstance(hyp_obj)) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a Hypervisor object instance")));
		}

		if(args.Length() == 1 || !args[1]->IsFunction()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a callback function as the second argument")));
		}

    // Path string
    const char *path = parseString(args[0]);

    // Hypervisor context
		Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

    // Create baton
    RestoreBaton* baton = new RestoreBaton();

    // Callback
    Local<Function> callback = Local<Function>::Cast(args[1]);
    baton->callback = Persistent<Function>::New(callback);

    // Add data
    baton->hypervisor = hypervisor;
    baton->path = path;

    // Compose req
    uv_work_t* req = new uv_work_t;
    req->data = baton;

    uv_queue_work(
        uv_default_loop(),
        req,
        RestoreAsync,
        (uv_after_work_cb)RestoreAsyncAfter
    );

    return scope.Close(Undefined());
	}

	void SuspendAsync(uv_work_t* req) {
		SuspendBaton* baton = static_cast<SuspendBaton*>(req->data);
		Domain *domain = baton->domain;
		virErrorPtr err;

		int ret = -1;
		ret = virDomainSuspend(domain->domain_);

		if(ret == -1) {
			err = virGetLastError();
			baton->error = err->message;
		}
		baton->res = ret;
	}

	void SuspendAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		SuspendBaton* baton = static_cast<SuspendBaton*>(req->data);
		delete req;

		Handle<Value> argv[2];

		if (!baton->error.empty()) {
			argv[0] = Exception::Error(String::New(baton->error.c_str()));
			argv[1] = Undefined();
		}

		else {
			argv[0] = Undefined();
			argv[1] = scope.Close(True());
		}

		TryCatch try_catch;

		if (try_catch.HasCaught())
			FatalException(try_catch);

		baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
		delete baton;
	}

	Handle<Value> Domain::Suspend(const Arguments& args) {
		HandleScope scope;

		// Domain context
		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		// Create baton
		SuspendBaton* baton = new SuspendBaton();

		// Callback
		Local<Function> callback = Local<Function>::Cast(args[0]);
		baton->callback = Persistent<Function>::New(callback);

		// Add data
		baton->domain = domain;

		// Compose req
		uv_work_t* req = new uv_work_t;
		req->data = baton;

		uv_queue_work(
				uv_default_loop(),
				req,
				SuspendAsync,
				(uv_after_work_cb)SuspendAsyncAfter
				);

		return scope.Close(Undefined());
	}

	void ResumeAsync(uv_work_t* req) {
		ResumeBaton* baton = static_cast<ResumeBaton*>(req->data);
		Domain *domain = baton->domain;
		virErrorPtr err;

		int ret = -1;
		ret = virDomainResume(domain->domain_);

		if(ret == -1) {
			err = virGetLastError();
			baton->error = err->message;
		}

		baton->res = ret;
	}

	void ResumeAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		ResumeBaton* baton = static_cast<ResumeBaton*>(req->data);
		delete req;

		Handle<Value> argv[2];

		if (!baton->error.empty()) {
			argv[0] = Exception::Error(String::New(baton->error.c_str()));
			argv[1] = Undefined();
		}

		else {
			argv[0] = Undefined();
			argv[1] = scope.Close(True());
		}

		TryCatch try_catch;

		if (try_catch.HasCaught())
			FatalException(try_catch);

		baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
		delete baton;
	}

	Handle<Value> Domain::Resume(const Arguments& args) {

		ResumeBaton* baton = new ResumeBaton();
		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		// Callback
		Local<Function> callback = Local<Function>::Cast(args[0]);
		baton->callback = Persistent<Function>::New(callback);

		// Add data
		baton->domain = domain;

		// Compose req
		uv_work_t* req = new uv_work_t;
		req->data = baton;

		// Dispatch work
		uv_queue_work(
				uv_default_loop(),
				req,
				ResumeAsync,
				(uv_after_work_cb)ResumeAsyncAfter
				);

		return Undefined();
	}

	Handle<Value> Domain::Shutdown(const Arguments& args) {
		HandleScope scope;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
		ret = virDomainShutdown(domain->domain_);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	Handle<Value> Domain::Start(const Arguments& args) {
		HandleScope scope;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainCreate(domain->domain_);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}
		return True();
	}

	Handle<Value> Domain::SendKey(const Arguments& args) {
		HandleScope scope;
		int ret = -1;

		if(args.Length() == 0) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify arguments to invoke this function")));
		}

		if(!args[0]->IsArray()) {
			return ThrowException(Exception::TypeError(
						String::New("Argument must be an array of objects")));
		}

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		unsigned int keycodes[VIR_DOMAIN_SEND_KEY_MAX_KEYS];

		Local<Array> keycodes_ = Local<Array>::Cast(args[0]);

		unsigned int length = keycodes_->Length();

		for(unsigned int i = 0; i < length; i++) {
			keycodes[i] = (unsigned int) keycodes_->Get(Integer::New(i))->Int32Value();
		}

		ret = virDomainSendKey(domain->domain_, 0, 150, keycodes, length, 0);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}
		return True();
	}

	Handle<Value> Domain::GetVcpus(const Arguments& args) {
		HandleScope scope;

		virDomainInfo info;
		virNodeInfo nodeinfo;
		virVcpuInfoPtr cpuinfo = NULL;
		unsigned char *cpumap = NULL;
		int ncpus;
		int cpumaplen;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainGetInfo(domain->domain_, &info);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		ret = virNodeGetInfo(virDomainGetConnect(domain->domain_), &nodeinfo);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		cpuinfo = (virVcpuInfoPtr) malloc(sizeof(*cpuinfo) * info.nrVirtCpu);
		if(cpuinfo == NULL) {
			LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
			return Null();
		}
		memset(cpuinfo, 0, sizeof(*cpuinfo) * info.nrVirtCpu);

		cpumaplen = VIR_CPU_MAPLEN(VIR_NODEINFO_MAXCPUS(nodeinfo));

		cpumap = (unsigned char*)malloc(info.nrVirtCpu * cpumaplen);
		if(cpumap == NULL) {
			free(cpuinfo);
			LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
			return Null();
		}
		memset(cpumap, 0, info.nrVirtCpu * cpumaplen);

		ncpus = virDomainGetVcpus(domain->domain_, cpuinfo, info.nrVirtCpu, cpumap, cpumaplen);

		if(ncpus < 0) {
			free(cpuinfo);
			free(cpumap);
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		Local<Array> vcpus = Array::New(info.nrVirtCpu);
		for(int i = 0; i < info.nrVirtCpu; i++) {
			Local<Object> obj = Object::New();
			obj->Set(number_symbol, Integer::New(cpuinfo[i].number));
			obj->Set(state_symbol, Integer::New(cpuinfo[i].state));
			obj->Set(cpu_time_symbol, Number::New(cpuinfo[i].cpuTime)); //nanoseconds
			obj->Set(cpu_symbol, Integer::New(cpuinfo[i].cpu));

			int maxcpus =  VIR_NODEINFO_MAXCPUS(nodeinfo);
			Local<Array> affinity = Array::New(maxcpus);
			for(int j = 0; j < maxcpus; j++) {
				Local<Object> cpu = Object::New();
				cpu->Set(cpu_symbol, Integer::New(j));
				cpu->Set(usable_symbol, Boolean::New(VIR_CPU_USABLE(cpumap, cpumaplen, i, j)));

				affinity->Set(Integer::New(j), cpu);
			}
			obj->Set(affinity_symbol, affinity);

			vcpus->Set(Integer::New(i), obj);
		}
		free(cpuinfo);
		free(cpumap);

		return scope.Close(vcpus);
	}

	Handle<Value> Domain::SetVcpus(const Arguments& args) {
		HandleScope scope;
		unsigned int vcpus = 0;
		int ret = -1;

		if(args.Length() == 0 || !args[0]->IsInt32()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a number")));
		}

		vcpus = args[0]->Int32Value();

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainSetVcpus(domain->domain_, vcpus);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

  void SetTimeAsync(uv_work_t* req) {
    SetTimeBaton* baton = static_cast<SetTimeBaton*>(req->data);
    Domain *domain = baton->domain;
    long long seconds = baton->seconds;
    unsigned int nseconds = baton->nseconds;
    unsigned int flags = baton->flags;
    virErrorPtr err;

    int ret = -1;
    ret = virDomainSetTime(domain->domain_, seconds, nseconds, flags);

    if (ret == -1) {
      err = virGetLastError();
      baton->error = err->message;
    }

    baton->ret = ret;
  }

  void SetTimeAsyncAfter(uv_work_t* req) {
    HandleScope scope;

    SetTimeBaton* baton = static_cast<SetTimeBaton*>(req->data);
    delete req;

    Handle<Value> argv[2];

    if (!baton->error.empty()) {
      argv[0] =Exception::Error(String::New(baton->error.c_str()));
      argv[1] = Undefined();
    }

    else {
      argv[0] = Undefined();
      argv[1] = scope.Close(True());
    }

    TryCatch try_catch;

    if (try_catch.HasCaught())
      FatalException(try_catch);

    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    delete baton;
  }

  Handle<Value> Domain::SetTime(const Arguments& args) {
    HandleScope scope;

    SetTimeBaton* baton = new SetTimeBaton();
    Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

    long long seconds = 0;
    unsigned int nseconds = 0;
    unsigned int flags = 0;
    int ret = -1;

    if (args.Length() == 0) {
      return ThrowException(Exception::TypeError(
            String::New("You must specify arguments to invoke this function")));
    }

    if(!args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32()) {
      return ThrowException(Exception::TypeError(
            String::New("You must specify the seconds, nanoseconds, and flags as numbers.")));
    }

    // Callback
    Local<Function> callback = Local<Function>::Cast(args[3]);

    // Add data
    baton->domain = domain;
    baton->callback = Persistent<Function>::New(callback);
    baton->seconds = seconds;
    baton->nseconds = nseconds;
    baton->flags = flags;
    baton->ret = ret;

    // Compose request
    uv_work_t* req = new uv_work_t;
    req->data = baton;

    // Dispatch work
    uv_queue_work(
        uv_default_loop(),
        req,
        SetTimeAsync,
        (uv_after_work_cb)SetTimeAsyncAfter
        );

    return scope.Close(Undefined());
  }

  Handle<Value> Domain::Migrate(const Arguments& args) {
		HandleScope scope;
		unsigned long flags = 0;
		unsigned long bandwidth = 0;
		//        const char* dest_name = NULL;
		int ret = -1;

		if(args.Length() == 0) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify arguments to invoke this function")));
		}

		if(!args[0]->IsObject()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify an object as first argument")));
		}

		Local<Object> args_ = args[0]->ToObject();

		if(!args_->Has(migration_uri_symbol)) {
			return ThrowException(Exception::TypeError(
						String::New("You must have set property dest_uri in the object")));
		}

		//dest_uri
		String::Utf8Value dest_uri(args_->Get(migration_uri_symbol));

		//dest_name
		//if(args_->Has(migration_name_symbol)) {
		String::Utf8Value dest_name(args_->Get(migration_name_symbol));
		//dest_name = ToCString(dest_name_);
		//}

		//flags
		if(args_->Has(migration_flags_symbol)){
			Local<Array> flags_ = Local<Array>::Cast(args_->Get(migration_flags_symbol));
			unsigned int length = flags_->Length();

			for (unsigned int i = 0; i < length; i++) {
				flags |= flags_->Get(Integer::New(i))->Int32Value();
			}
		}

		//bandwidth (Mbps)
		if(args_->Has(migration_bandwidth_symbol)) {
			bandwidth = args_->Get(migration_bandwidth_symbol)->Int32Value();
		}

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		if(args_->Has(migration_hypervisor_symbol)) {
			Local<Object> hyp_obj = args_->Get(migration_hypervisor_symbol)->ToObject();

			if(!Hypervisor::HasInstance(hyp_obj)) {
				return ThrowException(Exception::TypeError(
							String::New("You must specify a Hypervisor object instance")));
			}

			Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

			Domain *migrated_domain = new Domain();
			migrated_domain->domain_ = virDomainMigrate(domain->domain_,
					hypervisor->connection(),
					flags,
					(const char *) *dest_name,
					(const char *) *dest_uri,
					bandwidth);

			if(migrated_domain->domain_ == NULL) {
				ThrowException(Error::New(virGetLastError()));
				return False();
			}

			migrated_domain->Wrap(args.This());

			return scope.Close(migrated_domain->constructor_template->GetFunction()->NewInstance());
		} else {
			ret = virDomainMigrateToURI(domain->domain_,
					(const char *) *dest_uri,
					flags,
					(const char *) *dest_name,
					bandwidth);
		}

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	Handle<Value> Domain::SetMigrationMaxDowntime(const Arguments& args) {
		HandleScope scope;
		long long downtime = 0;
		unsigned int flags = 0;
		int ret = -1;

		if(args.Length() == 0) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify arguments to invoke this function")));
		}

		if(!args[0]->IsInt32()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a number as first argument")));
		}

		downtime = args[0]->Int32Value();

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainMigrateSetMaxDowntime(domain->domain_, downtime, flags);
		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	Handle<Value> Domain::PinVcpu(const Arguments& args) {
		HandleScope scope;
		virNodeInfo nodeinfo;
		unsigned char *cpumap = NULL;
		int cpumaplen;
		int vcpu;
		int ret = -1;

		if(args.Length() < 2) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify two arguments")));
		}

		if(!args[0]->IsInt32()) {
			return ThrowException(Exception::TypeError(
						String::New("The first argument must be an integer")));
		}

		if(!args[1]->IsArray()) {
			return ThrowException(Exception::TypeError(
						String::New("The second argument must be an array of objects")));
		}

		vcpu = args[0]->Int32Value();

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virNodeGetInfo(virDomainGetConnect(domain->domain_), &nodeinfo);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		int maxcpus = VIR_NODEINFO_MAXCPUS(nodeinfo);

		cpumaplen = VIR_CPU_MAPLEN(maxcpus);
		cpumap = (unsigned char*)malloc(cpumaplen);
		if(cpumap == NULL) {
			LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
			return False();
		}
		memset(cpumap, 0, cpumaplen);

		Local<Array> cpus = Local<Array>::Cast(args[1]);
		int ncpus = cpus->Length();

		for(int i = 0; i < ncpus; i++) {
			if(i > maxcpus) {
				break;
			}

			if(!cpus->Get(Integer::New(i))->IsObject()) {
				free(cpumap);
				return ThrowException(Exception::TypeError(
							String::New("The second argument must be an array of objects")));
			}

			Local<Object> cpu = cpus->Get(Integer::New(i))->ToObject();
			bool usable = cpu->Get(usable_symbol)->IsTrue();

			if(usable) {
				VIR_USE_CPU(cpumap, cpu->Get(cpu_symbol)->Int32Value());
			} else {
				VIR_UNUSE_CPU(cpumap, cpu->Get(cpu_symbol)->Int32Value());
			}
		}

		ret = virDomainPinVcpu(domain->domain_, vcpu, cpumap, cpumaplen);
		free(cpumap);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	void AttachDeviceAsync(uv_work_t* req) {
		AttachDeviceBaton* baton = static_cast<AttachDeviceBaton*>(req->data);

		Domain *domain = baton->domain;
		const char *xml = baton->xml;
		unsigned int flags = baton->flags;

		virErrorPtr err;
		int ret = -1;

		if(flags > 0) {
			ret = virDomainAttachDeviceFlags(domain->domain_, xml, flags);
		} else {
			ret = virDomainAttachDevice(domain->domain_, xml);
		}

		if(ret == -1) {
			err = virGetLastError();
			baton->error = err->message;
		}
	}

	void AttachDeviceAsyncAfter(uv_work_t* req) {
		AttachDeviceBaton* baton = static_cast<AttachDeviceBaton*>(req->data);
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

	Handle<Value> Domain::AttachDevice(const Arguments& args) {
		HandleScope scope;

		unsigned int flags = 0;
		int argsl = args.Length();

		if(argsl < 1 || !args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("First argument must be string of device XML")));
		}

		// XML
		const char *xml = parseString(args[0]);

		if(argsl < 2 || !args[1]->IsArray()) {
			return ThrowException(Exception::TypeError(
						String::New("Second argument must be array of flags")));
		}

		// Parse flags
		Local<Array> flags_ = Local<Array>::Cast(args[1]);
		unsigned int length = flags_->Length();

		for (unsigned int i = 0; i < length; i++) {
			flags |= flags_->Get(Integer::New(i))->Int32Value();
		}

		if(argsl < 3 || !args[2]->IsFunction()) {
			return ThrowException(Exception::TypeError(
						String::New("Third argument must be a callback function")));
		}

		// Callback
		Local<Function> callback = Local<Function>::Cast(args[2]);

		// Domain context
		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		// Create baton
		AttachDeviceBaton* baton = new AttachDeviceBaton();

		// Add data to baton
		baton->callback = Persistent<Function>::New(callback);
		baton->domain = domain;
		baton->flags = flags;
		baton->xml = xml;

		// Compose req
		uv_work_t* req = new uv_work_t;
		req->data = baton;

		// Dispatch work
		uv_queue_work(
				uv_default_loop(),
				req,
				AttachDeviceAsync,
				(uv_after_work_cb)AttachDeviceAsyncAfter
				);

		return Undefined();
	}

	void DetachDeviceAsync(uv_work_t* req) {
		DetachDeviceBaton* baton = static_cast<DetachDeviceBaton*>(req->data);

		Domain *domain = baton->domain;
		const char *xml = baton->xml;
		unsigned int flags = baton->flags;

		virErrorPtr err;
		int ret = -1;

		if(flags > 0) {
			ret = virDomainDetachDeviceFlags(domain->domain_, xml, flags);
		} else {
			ret = virDomainDetachDevice(domain->domain_, xml);
		}

		if(ret == -1) {
			err = virGetLastError();
			baton->error = err->message;
		}
	}

	void DetachDeviceAsyncAfter(uv_work_t* req) {
		DetachDeviceBaton* baton = static_cast<DetachDeviceBaton*>(req->data);
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

	Handle<Value> Domain::DetachDevice(const Arguments& args) {
		HandleScope scope;

		unsigned int flags = 0;
		int argsl = args.Length();

		if(argsl < 1 || !args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("First argument must be string of device XML")));
		}

		// XML
		const char *xml = parseString(args[0]);

		if(argsl < 2 || !args[1]->IsArray()) {
			return ThrowException(Exception::TypeError(
						String::New("Second argument must be array of flags")));
		}

		// Parse flags
		Local<Array> flags_ = Local<Array>::Cast(args[1]);
		unsigned int length = flags_->Length();

		for (unsigned int i = 0; i < length; i++) {
			flags |= flags_->Get(Integer::New(i))->Int32Value();
		}

		if(argsl < 3 || !args[2]->IsFunction()) {
			return ThrowException(Exception::TypeError(
						String::New("Third argument must be a callback function")));
		}

		// Callback
		Local<Function> callback = Local<Function>::Cast(args[2]);

		// Domain context
		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		// Create baton
		DetachDeviceBaton* baton = new DetachDeviceBaton();

		// Add data to baton
		baton->callback = Persistent<Function>::New(callback);
		baton->domain = domain;
		baton->flags = flags;
		baton->xml = xml;

		// Compose req
		uv_work_t* req = new uv_work_t;
		req->data = baton;

		// Dispatch work
		uv_queue_work(
				uv_default_loop(),
				req,
				DetachDeviceAsync,
				(uv_after_work_cb)DetachDeviceAsyncAfter
				);

		return Undefined();
	}

	Handle<Value> Domain::UpdateDevice(const Arguments& args) {
		HandleScope scope;
		unsigned int flags = 0;
		int ret = -1;

		if(args.Length() < 2) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify two arguments to invoke this function")));
		}

		if(!args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a string as first argument")));
		}

		if(!args[1]->IsArray()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify an array as second argument")));
		}

		String::Utf8Value xml(args[0]->ToString());

		//flags
		Local<Array> flags_ = Local<Array>::Cast(args[1]);
		unsigned int length = flags_->Length();

		for (unsigned int i = 0; i < length; i++) {
			flags |= flags_->Get(Integer::New(i))->Int32Value();
		}

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainUpdateDeviceFlags(domain->domain_, (const char *) *xml, flags);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	void DestroyAsync(uv_work_t* req) {
		DestroyBaton* baton = static_cast<DestroyBaton*>(req->data);

		Domain *domain = baton->domain;
		virErrorPtr err;

		int ret = -1;
		ret = virDomainDestroy(domain->domain_);

		if(ret == -1) {
			err = virGetLastError();
			baton->error = err->message;
		}
		baton->res = ret;
	}

	void DestroyAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		DestroyBaton* baton = static_cast<DestroyBaton*>(req->data);
		delete req;

		Handle<Value> argv[2];

		if (!baton->error.empty()) {
			argv[0] = Exception::Error(String::New(baton->error.c_str()));
			argv[1] = Undefined();
		}

		else {
			argv[0] = Undefined();
			argv[1] = scope.Close(True());
		}

		TryCatch try_catch;
		if (try_catch.HasCaught())
			FatalException(try_catch);

		baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
		delete baton;
	}

	Handle<Value> Domain::Destroy(const Arguments& args) {
		HandleScope scope;

		// Domain handle
		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		// Create baton
		DestroyBaton* baton = new DestroyBaton();

		// Callback
		Local<Function> callback = Local<Function>::Cast(args[0]);
		baton->callback = Persistent<Function>::New(callback);

		// Add data
		baton->domain = domain;

		// Compose req
		uv_work_t* req = new uv_work_t;
		req->data = baton;

		// Dispatch work
		uv_queue_work(
				uv_default_loop(),
				req,
				DestroyAsync,
				(uv_after_work_cb)DestroyAsyncAfter
				);

		return scope.Close(Undefined());
	}

	void ToXmlAsync(uv_work_t* req) {
		ToXmlBaton* baton = static_cast<ToXmlBaton*>(req->data);

		Domain *domain = baton->domain;
		unsigned int flags = baton->flags;

		virErrorPtr err;
		char* xml_ = NULL;

		xml_ = virDomainGetXMLDesc(domain->domain_, flags);

		if(xml_ == NULL) {
			err = virGetLastError();
			baton->error = err->message;
		}

		else {
			//Local<String> xml = String::New(xml_);
			baton->xml = xml_;
		}
		//free(xml_);
		//return scope.Close(xml);
	}

	void ToXmlAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		ToXmlBaton* baton = static_cast<ToXmlBaton*>(req->data);
		delete req;

		Handle<Value> argv[2];

		if (!baton->error.empty()) {
			argv[0] = Exception::Error(String::New(baton->error.c_str()));
			argv[1] = Undefined();
		}

		else {
			argv[0] = Undefined();
			argv[1] = String::New(baton->xml);
		}

		TryCatch try_catch;
		if (try_catch.HasCaught())
			FatalException(try_catch);
		baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
		delete baton;
	}

	Handle<Value> Domain::ToXml(const Arguments& args) {
		HandleScope scope;

		// Flags
		int flags = 0;
		if(args.Length() == 0 || !args[0]->IsArray()) {
			return ThrowException(Exception::TypeError(
						String::New("First argument must be an array of flags")));
		}
		Local<Array> flags_ = Local<Array>::Cast(args[0]);
		unsigned int length = flags_->Length();

		for (unsigned int i = 0; i < length; i++) {
			flags |= flags_->Get(Integer::New(i))->Int32Value();
		}

		if(args.Length() == 1 || !args[1]->IsFunction()) {
			return ThrowException(Exception::TypeError(
						String::New("Second argument must be a callback function")));
		}

		// Domain context
		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		// Create baton
		ToXmlBaton* baton = new ToXmlBaton();

		// Callback
		Local<Function> callback = Local<Function>::Cast(args[1]);
		baton->callback = Persistent<Function>::New(callback);

		// Add data to baton
		baton->domain = domain;
		baton->flags = flags;

		// Compose req
		uv_work_t* req = new uv_work_t;
		req->data = baton;

		// Dispatch work
    uv_queue_work(
      uv_default_loop(),
      req,
      ToXmlAsync,
      (uv_after_work_cb)ToXmlAsyncAfter
    );

		return Undefined();
	}

	Handle<Value> Domain::GetJobInfo(const Arguments& args) {
		HandleScope scope;
		virDomainJobInfo info_;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainGetJobInfo(domain->domain_, &info_);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		Local<Object> info = Object::New();
		info->Set(type_symbol, Integer::New(info_.type));

		//time
		Local<Object> time = Object::New();
		time->Set(elapsed_symbol, Number::New(info_.timeElapsed));
		time->Set(remaining_symbol, Number::New(info_.timeRemaining));

		//data
		Local<Object> data = Object::New();
		data->Set(total_symbol, Number::New(info_.dataTotal));
		data->Set(processed_symbol, Number::New(info_.dataProcessed));
		data->Set(remaining_symbol, Number::New(info_.dataRemaining));

		//memory
		Local<Object> memory = Object::New();
		memory->Set(total_symbol, Number::New(info_.memTotal));
		memory->Set(processed_symbol, Number::New(info_.memProcessed));
		memory->Set(remaining_symbol, Number::New(info_.memRemaining));

		//file
		Local<Object> file = Object::New();
		file->Set(total_symbol, Number::New(info_.fileTotal));
		file->Set(processed_symbol, Number::New(info_.fileProcessed));
		file->Set(remaining_symbol, Number::New(info_.fileRemaining));

		info->Set(time_symbol, time);
		info->Set(data_symbol, data);
		info->Set(memory_symbol, memory);
		info->Set(file_symbol, file);

		return scope.Close(info);
	}

	Handle<Value> Domain::AbortCurrentJob(const Arguments& args) {
		HandleScope scope;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
		ret = virDomainAbortJob(domain->domain_);
		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	Handle<Value> Domain::GetSchedType(const Arguments& args) {
		HandleScope scope;
		return Undefined();
	}

	Handle<Value> Domain::GetSchedParams(const Arguments& args) {
		HandleScope scope;
		int nparams = 0;
		int ret = -1;
		char *type = NULL;
		virSchedParameterPtr params_;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
		type = virDomainGetSchedulerType(domain->domain_, &nparams);

		if(type == NULL) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}
		free(type);

		params_ = (virSchedParameterPtr) malloc(sizeof(*params_) * nparams);

		if(params_ == NULL) {
			LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
			return Null();
		}
		memset(params_, 0, sizeof(*params_) * nparams);

		ret = virDomainGetSchedulerParameters(domain->domain_, params_, &nparams);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			free(params_);
			return Null();
		}

		Local<Object> params = Object::New();

		for(int i = 0; i < nparams; i++) {
			Local<Value> value = Local<Value>::New(Null());

			switch(params_[i].type) {
				case VIR_DOMAIN_SCHED_FIELD_INT:
					value = Integer::New(params_[i].value.i);
					break;
				case VIR_DOMAIN_SCHED_FIELD_UINT:
					value = Integer::New(params_[i].value.ui);
					break;
				case VIR_DOMAIN_SCHED_FIELD_LLONG:
					value = Number::New(params_[i].value.l);
					break;
				case VIR_DOMAIN_SCHED_FIELD_ULLONG:
					value = Number::New(params_[i].value.ul);
					break;
				case VIR_DOMAIN_SCHED_FIELD_DOUBLE:
					value = Number::New(params_[i].value.d);
					break;
				case VIR_DOMAIN_SCHED_FIELD_BOOLEAN:
					value = Integer::New(params_[i].value.b);
					break;
			}

			params->Set(String::New(params_[i].field), value);
		}
		free(params_);

		return scope.Close(params);
	}

	Handle<Value> Domain::SetSchedParams(const Arguments& args) {
		HandleScope scope;
		virSchedParameterPtr params = NULL;
		int nparams = 0;
		char *type = NULL;
		int ret = -1;

		if(args.Length() == 0 || !args[0]->IsObject()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify an object as argument to invoke this function")));
		}

		Local<Object> newparams = args[0]->ToObject();

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
		type = virDomainGetSchedulerType(domain->domain_, &nparams);

		if(type == NULL) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}
		free(type);

		params = (virSchedParameterPtr) malloc(sizeof(*params) * nparams);
		if(params == NULL) {
			LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
			return False();
		}

		memset(params, 0, sizeof(*params) * nparams);

		ret = virDomainGetSchedulerParameters(domain->domain_, params, &nparams);
		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			free(params);
			return False();
		}

		for(int i = 0; i < nparams; i++) {
			Local<String> field = String::New(params[i].field);
			if(!newparams->Has(field)) {
				continue;
			}

			Local<Value> value = newparams->Get(field);

			switch (params[i].type) {
				case VIR_DOMAIN_SCHED_FIELD_INT:
					params[i].value.i = value->Int32Value();
					break;
				case VIR_DOMAIN_SCHED_FIELD_UINT:
					params[i].value.ui = value->Uint32Value();
					break;
				case VIR_DOMAIN_SCHED_FIELD_LLONG:
					params[i].value.l = value->NumberValue();
					break;
				case VIR_DOMAIN_SCHED_FIELD_ULLONG:
					params[i].value.ul = value->NumberValue();
					break;
				case VIR_DOMAIN_SCHED_FIELD_DOUBLE:
					params[i].value.d = value->NumberValue();
					break;
				case VIR_DOMAIN_SCHED_FIELD_BOOLEAN:
					params[i].value.b = value->Uint32Value();
					break;
			}
		}
		ret = virDomainSetSchedulerParameters(domain->domain_, params, nparams);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			free(params);
			return False();
		}
		free(params);

		return True();
	}

	Handle<Value> Domain::GetSecurityLabel(const Arguments& args) {
		HandleScope scope;
		virSecurityLabel label_;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		memset(&label_, 0, sizeof label_);
		ret = virDomainGetSecurityLabel(domain->domain_, &label_);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		Local<Object> label = Object::New();
		label->Set(label_symbol, String::New(label_.label));
		label->Set(enforcing_symbol, Boolean::New(label_.enforcing));

		return scope.Close(label);
	}

	Handle<Value> Domain::SaveManagedImage(const Arguments& args) {
		HandleScope scope;
		unsigned int flags = 0;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainManagedSave(domain->domain_, flags);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	Handle<Value> Domain::RemoveManagedImage(const Arguments& args) {
		HandleScope scope;
		unsigned int flags = 0;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainManagedSaveRemove(domain->domain_, flags);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	Handle<Value> Domain::HasManagedImage(const Arguments& args) {
		HandleScope scope;
		unsigned int flags = 0;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainHasManagedSaveImage(domain->domain_, flags);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return scope.Close(Boolean::New(ret));
	}

	Handle<Value> Domain::MemoryPeek(const Arguments& args) {
		HandleScope scope;
		unsigned long long start = 0;
		size_t size = 0;
		char * buffer_ = NULL;
		unsigned int flags = 0;

		if(args.Length() < 3) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify three arguments to invoke this function")));
		}

		if(!args[0]->IsNumber() || !args[1]->IsNumber()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a number in the first and second argument")));
		}

		if(!args[2]->IsArray()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify an array in the third argument")));
		}

		start = args[0]->NumberValue();
		size = args[1]->NumberValue() * sizeof(char *);

		//flags
		Local<Array> flags_ = Local<Array>::Cast(args[2]);
		unsigned int length = flags_->Length();

		for (unsigned int i = 0; i < length; i++) {
			flags |= flags_->Get(Integer::New(i))->Int32Value();
		}

		buffer_ = (char*) malloc(size);

		if(buffer_ == NULL) {
			LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
			return Null();
		}

		memset(buffer_, 0, size);

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		int ret = virDomainMemoryPeek(domain->domain_, start, size, buffer_, flags);

		if(ret == -1) {
			free(buffer_);
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		Buffer *buffer = Buffer::New(size);
		memcpy(Buffer::Data(buffer), buffer_, size);
		free(buffer_);

		return scope.Close(buffer->handle_);
	}

	Handle<Value> Domain::GetMemoryStats(const Arguments& args) {
		HandleScope scope;
		unsigned int nr_stats = 0;
		unsigned int flags = 0;
		virDomainMemoryStatStruct stats_[VIR_DOMAIN_MEMORY_STAT_NR];

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		nr_stats = virDomainMemoryStats(domain->domain_, stats_,
				VIR_DOMAIN_MEMORY_STAT_NR, flags);

		if(nr_stats == -1u) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		Local<Object> stats = Object::New();

		for(unsigned int i = 0; i < nr_stats; i++) {
			switch (stats_[i].tag) {
				case VIR_DOMAIN_MEMORY_STAT_SWAP_IN:
					stats->Set(memory_stat_swap_in_symbol, Number::New(stats_[i].val));
					break;
				case VIR_DOMAIN_MEMORY_STAT_SWAP_OUT:
					stats->Set(memory_stat_swap_out_symbol, Number::New(stats_[i].val));
					break;
				case VIR_DOMAIN_MEMORY_STAT_MAJOR_FAULT:
					stats->Set(memory_stat_major_fault_symbol, Number::New(stats_[i].val));
					break;
				case VIR_DOMAIN_MEMORY_STAT_MINOR_FAULT:
					stats->Set(memory_stat_minor_fault_symbol, Number::New(stats_[i].val));
					break;
				case VIR_DOMAIN_MEMORY_STAT_UNUSED:
					stats->Set(memory_stat_unused_symbol, Number::New(stats_[i].val));
					break;
				case VIR_DOMAIN_MEMORY_STAT_AVAILABLE:
					stats->Set(memory_stat_available_symbol, Number::New(stats_[i].val));
					break;
			}
		}
		return scope.Close(stats);
	}

	Handle<Value> Domain::BlockPeek(const Arguments& args) {
		HandleScope scope;
		unsigned long long start = 0;
		size_t size = 0;
		char * buffer_ = NULL;
		unsigned int flags = 0;

		if(args.Length() < 3) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify three arguments to invoke this function")));
		}

		if(!args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a string in the first and second argument")));
		}

		if(!args[1]->IsNumber() || !args[2]->IsNumber()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify numbers in the first and second argument")));
		}

		String::Utf8Value path(args[0]->ToString());

		start = args[1]->NumberValue();
		size = args[2]->NumberValue() * sizeof(char *);

		buffer_ = (char*) malloc(size);

		if(buffer_ == NULL) {
			LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
			return Null();
		}

		memset(buffer_, 0, size);

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		int ret = virDomainBlockPeek(domain->domain_, (const char *) *path, start, size, buffer_, flags);

		if(ret == -1) {
			free(buffer_);
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		Buffer *buffer = Buffer::New(size);
		memcpy(Buffer::Data(buffer), buffer_, size);
		free(buffer_);

		return scope.Close(buffer->handle_);
	}

	void BlockPullAsync(uv_work_t* req) {
		BlockPullBaton* baton = static_cast<BlockPullBaton*>(req->data);

		Domain *domain         = baton->domain;
		const char *disk       = baton->disk;
		unsigned int flags     = baton->flags;
		unsigned int bandwidth = baton->bandwidth;

		virErrorPtr err;

		int ret = virDomainBlockPull(domain->domain_, disk, bandwidth, flags);

		if(ret == -1) {
			err = virGetLastError();
			baton->error = err->message;
		}
	}

	void BlockPullAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		BlockPullBaton* baton = static_cast<BlockPullBaton*>(req->data);
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

	Handle<Value> Domain::BlockPull(const Arguments& args) {
		HandleScope scope;

		Local<Object> options_;
		Local<Function> callback_;

		unsigned int bandwidth = 0;
		unsigned int flags = 0;
		const char* disk = "vda";

		// If only callback has been specified, use defaults
		if(args[0]->IsFunction()) {
			callback_ = Local<Function>::Cast(args[0]);
		}

		// If options have been specified
		else if(args[0]->IsObject()) {

			// If callback has not been specified
			if(!args[1]->IsFunction()) {
				ThrowException(Exception::TypeError(
							String::New("Second argument must be a function"))
						);
				return scope.Close(Undefined());
			}

			options_  = Local<Object>::Cast(args[0]);
			callback_ = Local<Function>::Cast(args[1]);

			// Supported options
			Handle<Value> disk_      = options_->Get(String::New("disk"));
			Handle<Value> flags_     = options_->Get(String::New("flags"));
			Handle<Value> bandwidth_ = options_->Get(String::New("bandwidth"));

			// When disk option is specified
			if (!disk_->IsUndefined()) {

				// If disk option is not a string
				if (!disk_->IsString()) {
					ThrowException(Exception::TypeError(
								String::New("Disk must be a string"))
							);
					return scope.Close(Undefined());
				}

				disk = parseString(disk_->ToString());
			}

			// When flags option is specified
			if (!flags_->IsUndefined()) {

				// If flags option is not an array
				if (!flags_->IsArray()) {
					ThrowException(Exception::TypeError(
								String::New("Flags, must be an array"))
							);
					return scope.Close(Undefined());
				}

				Local<Array> flagsArr_ = Local<Array>(Array::Cast(*flags_));
				unsigned int length = flagsArr_->Length();

				for (unsigned int i = 0; i < length; i++) {
					flags |= flagsArr_->Get(Integer::New(i))->Int32Value();
				}
			}

			// When bandwidth option is specified
			if (!bandwidth_->IsUndefined()) {

				if (!bandwidth_->IsNumber()) {
					ThrowException(Exception::TypeError(
								String::New("Bandwidth must be a number"))
							);
					return scope.Close(Undefined());
				}

				//bandwidth = bandwidth_->ToUint32();
				bandwidth = bandwidth_->NumberValue();
			}
		}

		// If first argument is neither an object nor a function
		else {
			return ThrowException(Exception::TypeError(
						String::New("First argument must be an object or function")));
		}

		// Domain context
		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		// Create baton
		BlockPullBaton* baton = new BlockPullBaton();

		// Add data
		baton->callback  = Persistent<Function>::New(callback_);
		baton->domain    = domain;
		baton->disk      = disk;
		baton->bandwidth = bandwidth;
		baton->flags     = flags;

		// Compose req
		uv_work_t* req = new uv_work_t;
		req->data = baton;

		// Dispatch work
		uv_queue_work(
				uv_default_loop(),
				req,
				BlockPullAsync,
				(uv_after_work_cb)BlockPullAsyncAfter
				);

		return scope.Close(Undefined());
	}

	Handle<Value> Domain::BlockRebase(const Arguments& args) {
		HandleScope scope;
		unsigned int bandwidth = 0;
		unsigned int flags = 0;

		if(args.Length() < 2) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify at least two arguments to invoke this function")));
		}

		if(!args[0]->IsString() || !args[1]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a string in the first and second arguments")));
		}

		String::Utf8Value disk(args[0]->ToString());
		String::Utf8Value base(args[1]->ToString());

		if(args.Length() > 2) {
			if(!args[2]->IsNumber()) {
				return ThrowException(Exception::TypeError(
							String::New("Third argument, if specified, must be a number")));
			}
			bandwidth = args[2]->NumberValue();
		}

		if(args.Length() > 3) {
			if(!args[3]->IsObject()) {
				return ThrowException(Exception::TypeError(
							String::New("Fourth argument, if specified, must be an object")));
			}

			Local<Array> flags_ = Local<Array>::Cast(args[3]);
			unsigned int length = flags_->Length();

			for (unsigned int i = 0; i < length; i++) {
				flags |= flags_->Get(Integer::New(i))->Int32Value();
			}
		}

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		int ret = virDomainBlockRebase(domain->domain_, (const char *) *disk, (const char *) *base, bandwidth, flags);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		return True();
	}

	Handle<Value> Domain::GetBlockStats(const Arguments& args) {
		HandleScope scope;
		int ret = -1;
		virDomainBlockStatsStruct stats_;

		if(args.Length() == 0 || !args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a string as argument to invoke this function")));
		}
		String::Utf8Value path(args[0]->ToString());

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainBlockStats(domain->domain_, (const char *) *path, &stats_, sizeof(stats_));

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		Local<Object> stats = Object::New();
		stats->Set(block_stat_rd_req_symbol, Number::New(stats_.rd_req));
		stats->Set(block_stat_rd_bytes_symbol, Number::New(stats_.rd_bytes));
		stats->Set(block_stat_wr_req_symbol, Number::New(stats_.wr_req));
		stats->Set(block_stat_wr_bytes_symbol, Number::New(stats_.wr_bytes));
		stats->Set(block_stat_errs_symbol, Number::New(stats_.errs));

		return scope.Close(stats);
	}

	Handle<Value> Domain::GetBlockInfo(const Arguments& args) {
		HandleScope scope;
		virDomainBlockInfo info_;
		unsigned int flags = 0;
		int ret = -1;

		if(args.Length() == 0 || !args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a string as argument to invoke this function")));
		}
		String::Utf8Value path(args[0]->ToString());

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainGetBlockInfo(domain->domain_, (const char *) *path, &info_, flags);
		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		Local<Object> info = Object::New();
		info->Set(block_info_capacity_symbol, Number::New(info_.capacity));
		info->Set(block_info_allocation_symbol, Number::New(info_.allocation));
		info->Set(block_info_physical_symbol, Number::New(info_.physical));

		return scope.Close(info);
	}

	Handle<Value> Domain::AbortBlockJob(const Arguments& args) {
		HandleScope scope;

		Local<Object> options;
		Local<Function> callback;

		unsigned int flags = 0;
		int ret = -1;
		const char* disk = "vda";

		// If only callback has been specified, use defaults
		if(args[0]->IsFunction()) {
			callback = Local<Function>::Cast(args[0]);
		}

		// If options have been specified
		else if(args[0]->IsObject()) {

			// If callback has not been specified
			if(!args[1]->IsFunction()) {
				ThrowException(Exception::TypeError(
							String::New("Second argument must be a function"))
						);
				return scope.Close(Undefined());
			}

			options  = Local<Object>::Cast(args[0]);
			callback = Local<Function>::Cast(args[1]);

			// Supported options
			Handle<Value> flags_ = options->Get(String::New("flags"));
			Handle<Value> disk_ = options->Get(String::New("disk"));

			// When disk option is specified
			if (!flags_->IsNull()) {

				// If disk option is not a string
				if (!disk_->IsString()) {
					ThrowException(Exception::TypeError(
								String::New("Disk, if specified, must be a string"))
							);
					return scope.Close(Undefined());
				}

				disk = parseString(disk_->ToString());
			}

			// When flags option is specified
			if (!flags_->IsUndefined()) {

				// If flags option is not an array
				if (!flags_->IsArray()) {
					ThrowException(Exception::TypeError(
								String::New("Flags, if specified, must be an array"))
							);
					return scope.Close(Undefined());
				}

				Local<Array> flagsArr_ = Local<Array>(Array::Cast(*flags_));
				unsigned int length = flagsArr_->Length();

				for (unsigned int i = 0; i < length; i++) {
					flags |= flagsArr_->Get(Integer::New(i))->Int32Value();
				}
			}
		}

		// If first argument is neither an object nor a function
		else {
			return ThrowException(Exception::TypeError(
						String::New("First argument must be an object or function")));
		}

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainBlockJobAbort(
				domain->domain_,
				disk,
				flags
				);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
		}

		return scope.Close(Undefined());
	}

	void GetBlockJobInfoAsync(uv_work_t* req) {
		GetBlockJobInfoBaton* baton = static_cast<GetBlockJobInfoBaton*>(req->data);

		Domain *domain = baton->domain;
		unsigned int flags = baton->flags;
		const char *disk = baton->disk;

		virDomainBlockJobInfo info;
		virErrorPtr err;

		int ret = -1;

		ret = virDomainGetBlockJobInfo(domain->domain_, disk, &info, flags);

		if(ret == -1) {
			err = virGetLastError();
			baton->error = err->message;
		}

		else {
			baton->info = info;
		}
	}

	void GetBlockJobInfoAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		GetBlockJobInfoBaton* baton = static_cast<GetBlockJobInfoBaton*>(req->data);
		virDomainBlockJobInfo info = baton->info;
		delete req;

		Handle<Value> argv[2];

		if(!baton->error.empty()) {
			argv[0] = Exception::Error(String::New(baton->error.c_str()));
			argv[1] = Undefined();
		}

		else {
			Local<Object> obj = Object::New();

			obj->Set(block_job_info_type_symbol,      Number::New(info.type));
			obj->Set(block_job_info_bandwidth_symbol, Number::New(info.bandwidth));
			obj->Set(block_job_info_cur_symbol,       Number::New(info.cur));
			obj->Set(block_job_info_end_symbol,       Number::New(info.end));

			argv[0] = Undefined();
			argv[1] = scope.Close(obj);
		}

		TryCatch try_catch;

		if (try_catch.HasCaught())
			FatalException(try_catch);

		baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
		delete baton;
	}

	Handle<Value> Domain::GetBlockJobInfo(const Arguments& args) {
		HandleScope scope;

		Local<Object> options;
		Local<Function> callback;

		unsigned int flags = 0;
		const char* disk = "vda";

		// If only callback has been specified, use defaults
		if(args[0]->IsFunction()) {
			callback = Local<Function>::Cast(args[0]);
		}

		// If options have been specified
		else if(args[0]->IsObject()) {

			// If callback has not been specified
			if(!args[1]->IsFunction()) {
				ThrowException(Exception::TypeError(
							String::New("Second argument must be a function"))
						);
				return scope.Close(Undefined());
			}

			options  = Local<Object>::Cast(args[0]);
			callback = Local<Function>::Cast(args[1]);

			// Supported options
			Handle<Value> flags_ = options->Get(String::New("flags"));
			Handle<Value> disk_ = options->Get(String::New("disk"));

			// When disk option is specified
			if (!flags_->IsNull()) {

				// If disk option is not a string
				if (!disk_->IsString()) {
					ThrowException(Exception::TypeError(
								String::New("Disk, if specified, must be a string"))
							);
					return scope.Close(Undefined());
				}

				disk = parseString(disk_->ToString());
			}

			// When flags option is specified
			if (!flags_->IsUndefined()) {

				// If flags option is not an array
				if (!flags_->IsArray()) {
					ThrowException(Exception::TypeError(
								String::New("Flags, if specified, must be an array"))
							);
					return scope.Close(Undefined());
				}

				Local<Array> flagsArr_ = Local<Array>(Array::Cast(*flags_));
				unsigned int length = flagsArr_->Length();

				for (unsigned int i = 0; i < length; i++) {
					flags |= flagsArr_->Get(Integer::New(i))->Int32Value();
				}
			}
		}

		// If first argument is neither an object nor a function
		else {
			return ThrowException(Exception::TypeError(
						String::New("First argument must be an object or function")));
		}

		// Domain context
		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		// Create baton
		GetBlockJobInfoBaton* baton = new GetBlockJobInfoBaton();

		// Add data to baton
		baton->callback = Persistent<Function>::New(callback);
		baton->domain   = domain;
		baton->disk     = disk;
		baton->flags    = flags;

		// Compose req
		uv_work_t* req = new uv_work_t;
		req->data = baton;

		// Dipatch work
		uv_queue_work(
				uv_default_loop(),
				req,
				GetBlockJobInfoAsync,
				(uv_after_work_cb)GetBlockJobInfoAsyncAfter
				);

		return scope.Close(Undefined());
	}

	Handle<Value> Domain::GetInterfaceStats(const Arguments& args) {
		HandleScope scope;
		struct _virDomainInterfaceStats stats_;
		int ret = -1;

		if(args.Length() == 0 || !args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a string as argument to invoke this function")));
		}
		String::Utf8Value device(args[0]->ToString());

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainInterfaceStats(domain->domain_, (const char *) *device, &stats_, sizeof(stats_));
		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		Local<Object> stats = Object::New();
		stats->Set(nwiface_stat_rx_bytes_symbol, Number::New(stats_.rx_bytes));
		stats->Set(nwiface_stat_rx_packets_symbol, Number::New(stats_.rx_packets));
		stats->Set(nwiface_stat_rx_errors_symbol, Number::New(stats_.rx_errs));
		stats->Set(nwiface_stat_rx_drop_symbol, Number::New(stats_.rx_drop));
		stats->Set(nwiface_stat_tx_bytes_symbol, Number::New(stats_.tx_bytes));
		stats->Set(nwiface_stat_tx_packets_symbol, Number::New(stats_.tx_packets));
		stats->Set(nwiface_stat_tx_errors_symbol, Number::New(stats_.tx_errs));
		stats->Set(nwiface_stat_tx_drop_symbol, Number::New(stats_.tx_drop));

		return scope.Close(stats);
	}

	Handle<Value> Domain::CoreDump(const Arguments& args) {
		HandleScope scope;
		int flags = 0;
		int ret = -1;

		if(args.Length() == 0 || !args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a string as argument to invoke this function")));
		}
		String::Utf8Value path(args[0]->ToString());

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainCoreDump(domain->domain_, (const char *) *path, flags);

		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return True();
	}

	Handle<Value> Domain::HasCurrentSnapshot(const Arguments& args) {
		HandleScope scope;
		unsigned int flags = 0;
		int ret = -1;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		ret = virDomainHasCurrentSnapshot(domain->domain_, flags);
		if(ret == -1) {
			ThrowException(Error::New(virGetLastError()));
			return False();
		}

		return ret == 1 ? True() : False();
	}

	void RevertToSnapshotAsync(uv_work_t* req) {

		RevertToSnapshotBaton* baton = static_cast<RevertToSnapshotBaton*>(req->data);

		Domain *domain = baton->domain;
		const char *name = baton->name;
		//unsigned int lookupflags = baton->lookupflags;
		unsigned int revertflags = baton->revertflags;

		virDomainSnapshotPtr snapshot = NULL;
		virErrorPtr err;

		int ret = -1;

		snapshot = virDomainSnapshotLookupByName(domain->domain_, name, baton->lookupflags);

		if(snapshot == NULL) {
			err = virGetLastError();
			baton->error = err->message;
		}

		else {
			ret = virDomainRevertToSnapshot(snapshot, revertflags);

			if(ret == -1) {
				err = virGetLastError();
				baton->error = err->message;
			}

			else {
				virDomainSnapshotFree(snapshot);
			}
		}
	}

	void RevertToSnapshotAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		RevertToSnapshotBaton* baton = static_cast<RevertToSnapshotBaton*>(req->data);
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

	Handle<Value> Domain::RevertToSnapshot(const Arguments& args) {
		HandleScope scope;

		// Domain context
		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		unsigned int lookupflags = 0;
		unsigned int revertflags = 0;

		if(args.Length() == 0 || !args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a string as argument to invoke this function")));
		}

		// Domain name
		const char *name = parseString(args[0]);

		if (args.Length() > 1) {
			if (!args[1]->IsObject()) {
				return ThrowException(Exception::TypeError(
							String::New("Second argument, if provided, must be an object to invoke this function")));
			}

			Local<Array> flags_ = Local<Array>::Cast(args[1]);
			unsigned int length = flags_->Length();

			for (unsigned int i = 0; i < length; i++) {
				revertflags |= flags_->Get(Integer::New(i))->Int32Value();
			}
		}

		// Create baton
		RevertToSnapshotBaton* baton = new RevertToSnapshotBaton();

		// Callback
		Local<Function> callback = Local<Function>::Cast(args[2]);
		baton->callback = Persistent<Function>::New(callback);

		// Add data to baton
		baton->domain = domain;
		baton->name = name;
		baton->lookupflags = lookupflags;
		baton->revertflags = revertflags;

		// Compose req
		uv_work_t* req = new uv_work_t;
		req->data = baton;

		// Dispatch work
		uv_queue_work(
				uv_default_loop(),
				req,
				RevertToSnapshotAsync,
				(uv_after_work_cb)RevertToSnapshotAsyncAfter
				);

		return Undefined();
	}

	void TakeSnapshotAsync(uv_work_t* req) {

		TakeSnapshotBaton* baton = static_cast<TakeSnapshotBaton*>(req->data);

		Domain *domain = baton->domain;
		const char *xml = baton->xml;
		unsigned int flags = baton->flags;

		virDomainSnapshotPtr snapshot = NULL;
		virErrorPtr err;

		snapshot = virDomainSnapshotCreateXML(domain->domain_, xml, flags);

		if(snapshot == NULL) {
			err = virGetLastError();
			baton->error = err->message;
		}

		else {
			virDomainSnapshotFree(snapshot);
		}
	}

	void TakeSnapshotAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		TakeSnapshotBaton* baton = static_cast<TakeSnapshotBaton*>(req->data);
		delete req;

		Handle<Value> argv[2];

		if (!baton->error.empty()) {
			argv[0] = Exception::Error(String::New(baton->error.c_str()));
			argv[1] = Undefined();
		}

		else {
			argv[0] = Undefined();
			argv[1] = scope.Close(True());
		}

		TryCatch try_catch;

		if (try_catch.HasCaught())
			FatalException(try_catch);

		baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
		delete baton;
	}

	Handle<Value> Domain::TakeSnapshot(const Arguments& args) {
		HandleScope scope;

		// Domain context
		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		// XML
		const char *xml = parseString(args[0]);

		// Flags
		unsigned int flags = 0;

		// Parse args
		if(args.Length() > 0) {
			if (!args[0]->IsString()) {
				return ThrowException(Exception::TypeError(
							String::New("First argument, if provided, must be a string")));
			}

			if (args.Length() > 1) {
				if (!args[1]->IsObject()) {
					return ThrowException(Exception::TypeError(
								String::New("Second argument, if provided, must be an object")));
				}

				Local<Array> flags_ = Local<Array>::Cast(args[1]);
				unsigned int length = flags_->Length();

				for (unsigned int i = 0; i < length; i++) {
					flags |= flags_->Get(Integer::New(i))->Int32Value();
				}
			}
		}

		// Create baton
		TakeSnapshotBaton* baton = new TakeSnapshotBaton();

		// Callback
		Local<Function> callback = Local<Function>::Cast(args[2]);
		baton->callback = Persistent<Function>::New(callback);

		// Add data to baton
		baton->domain = domain;
		baton->flags = flags;
		baton->xml = xml;

		// Compose req
		uv_work_t* req = new uv_work_t;
		req->data = baton;

		// Dispatch work
		uv_queue_work(
				uv_default_loop(),
				req,
				TakeSnapshotAsync,
				(uv_after_work_cb)TakeSnapshotAsyncAfter
				);

		return scope.Close(Undefined());
	}

	Handle<Value> Domain::GetCurrentSnapshot(const Arguments& args) {
		HandleScope scope;
		unsigned int flags = 0;
		virDomainSnapshotPtr snapshot = NULL;
		char* xml_ = NULL;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
		snapshot = virDomainSnapshotCurrent(domain->domain_, flags);

		if(snapshot == NULL) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		xml_ = virDomainSnapshotGetXMLDesc(snapshot, flags);
		if(xml_ == NULL) {
			virDomainSnapshotFree(snapshot);
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}
		virDomainSnapshotFree(snapshot);
		//TODO serialize to json

		Local<String> xml = String::New(xml_);
		free(xml_);

		return scope.Close(xml);
	}

	void DeleteSnapshotAsync(uv_work_t* req) {

		DeleteSnapshotBaton* baton = static_cast<DeleteSnapshotBaton*>(req->data);

		Domain *domain = baton->domain;
		const char *name = baton->name;
		unsigned int flags = baton->flags;

		virDomainSnapshotPtr snapshot = NULL;
		virErrorPtr err;

		snapshot = virDomainSnapshotLookupByName(domain->domain_, name, 0);

		if(snapshot == NULL) {
			err = virGetLastError();
			baton->error = err->message;
		}

		else {
			int ret = virDomainSnapshotDelete(snapshot, flags);
			if(ret == -1) {
				err = virGetLastError();
				baton->error = err->message;
			}

			else {
				virDomainSnapshotFree(snapshot);
			}
		}
	}

	void DeleteSnapshotAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		DeleteSnapshotBaton* baton = static_cast<DeleteSnapshotBaton*>(req->data);
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

	Handle<Value> Domain::DeleteSnapshot(const Arguments& args) {
		HandleScope scope;

		// Domain context
		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		// Flags
		unsigned int flags = 0;

		if(args.Length() == 0 || !args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("First argument must be a string")));
		}

		// Domain name
		const char *name = parseString(args[0]);

		if (args.Length() > 1) {
			if (!args[1]->IsObject()) {
				return ThrowException(Exception::TypeError(
							String::New("Second argument must be an object")));
			}

			Local<Array> flags_ = Local<Array>::Cast(args[1]);
			unsigned int length = flags_->Length();

			for (unsigned int i = 0; i < length; i++) {
				flags |= flags_->Get(Integer::New(i))->Int32Value();
			}
		}

		// Create baton
		DeleteSnapshotBaton* baton = new DeleteSnapshotBaton();

		// Callback
		Local<Function> callback = Local<Function>::Cast(args[2]);
		baton->callback = Persistent<Function>::New(callback);

		// Add data to baton
		baton->domain = domain;
		baton->name = name;
		baton->flags = flags;

		// Compose req
		uv_work_t* req = new uv_work_t;
		req->data = baton;

    uv_queue_work(
      uv_default_loop(),
      req,
      DeleteSnapshotAsync,
      (uv_after_work_cb)DeleteSnapshotAsyncAfter
    );

		return True();
	}

	Handle<Value> Domain::LookupSnapshotByName(const Arguments& args) {
		HandleScope scope;
		char* xml_ = NULL;
		unsigned int flags = 0;
		virDomainSnapshotPtr snapshot = NULL;

		if(args.Length() == 0 || !args[0]->IsString()) {
			return ThrowException(Exception::TypeError(
						String::New("You must specify a string as argument to invoke this function")));
		}
		String::Utf8Value name(args[0]->ToString());

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
		snapshot = virDomainSnapshotLookupByName(domain->domain_, (const char *) *name, flags);
		if(snapshot == NULL) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		xml_ = virDomainSnapshotGetXMLDesc(snapshot, flags);
		if(xml_ == NULL) {
			virDomainSnapshotFree(snapshot);
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}
		virDomainSnapshotFree(snapshot);
		//TODO serialize to json

		Local<String> xml = String::New(xml_);
		free(xml_);

		return scope.Close(xml);
	}

	Handle<Value> Domain::GetSnapshots(const Arguments& args) {
		HandleScope scope;
		char **snapshots_ = NULL;
		unsigned int flags = 0;
		int num_snapshots = 0;

		Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

		num_snapshots = virDomainSnapshotNum(domain->domain_, flags);

		if(num_snapshots == -1) {
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		snapshots_ = (char**) malloc(sizeof(*snapshots_) * num_snapshots);
		if(snapshots_ == NULL) {
			LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
			return Null();
		}

		num_snapshots = virDomainSnapshotListNames(domain->domain_, snapshots_, num_snapshots, flags);
		if(num_snapshots == -1) {
			free(snapshots_);
			ThrowException(Error::New(virGetLastError()));
			return Null();
		}

		Local<Array> snapshots = Array::New(num_snapshots);
		for (int i = 0; i < num_snapshots; i++) {
			virDomainSnapshotPtr snapshot = virDomainSnapshotLookupByName(domain->domain_, snapshots_[i], flags);
			char *xml = virDomainSnapshotGetXMLDesc(snapshot, flags);
			virDomainSnapshotFree(snapshot);

			snapshots->Set(Integer::New(i), String::New(xml));
			free(snapshots_[i]);
			free(xml);
		}
		free(snapshots_);

		return scope.Close(snapshots);
	}

	void GetSaveImageXmlAsync(uv_work_t* req) {
		GetSaveImageXmlBaton* baton = static_cast<GetSaveImageXmlBaton*>(req->data);

		Hypervisor *hypervisor = baton->hypervisor;
		unsigned int flags = baton->flags;
    const char *path = baton->path;

		virErrorPtr err;
		char* xml_ = NULL;

		xml_ = virDomainSaveImageGetXMLDesc(hypervisor->connection(), path, flags);

		if(xml_ == NULL) {
			err = virGetLastError();
			baton->error = err->message;
		}

		else {
			baton->xml = xml_;
		}

		free(xml_);
	}

	void GetSaveImageXmlAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		GetSaveImageXmlBaton* baton = static_cast<GetSaveImageXmlBaton*>(req->data);
		delete req;

		Handle<Value> argv[2];

		if (!baton->error.empty()) {
			argv[0] = Exception::Error(String::New(baton->error.c_str()));
			argv[1] = Undefined();
		}

		else {
			argv[0] = Undefined();
			argv[1] = String::New(baton->xml);
		}

		TryCatch try_catch;
		if (try_catch.HasCaught())
			FatalException(try_catch);

		baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
		delete baton;
	}

  // file, flags, cb
  Handle<Value> Domain::GetSaveImageXml(const Arguments& args) {
    HandleScope scope;
    unsigned int flags = 0;

    if(args.Length() == 0 || !args[0]->IsString()) {
      return ThrowException(Exception::TypeError(
            String::New("You must specify a path string as the first argument")));
    }

    Local<Object> hyp_obj = args.This();

    if(!Hypervisor::HasInstance(hyp_obj)) {
      return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor object instance")));
    }

    if(args.Length() == 1 || !args[1]->IsFunction()) {
      return ThrowException(Exception::TypeError(
            String::New("You must specify a callback function as the second argument")));
    }

    // Path string
    const char *path = parseString(args[0]);

    // Hypervisor context
    Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

    // Create baton
    GetSaveImageXmlBaton* baton = new GetSaveImageXmlBaton();

    // Callback
    Local<Function> callback = Local<Function>::Cast(args[1]);
    baton->callback = Persistent<Function>::New(callback);

    // Add data
    baton->hypervisor = hypervisor;
    baton->flags = flags;
    baton->path = path;

    // Compose req
    uv_work_t* req = new uv_work_t;
    req->data = baton;

    uv_queue_work(
        uv_default_loop(),
        req,
        GetSaveImageXmlAsync,
        (uv_after_work_cb)GetSaveImageXmlAsyncAfter
    );

    return scope.Close(Undefined());

  }

	void UpdateSaveImageXmlAsync(uv_work_t* req) {
		UpdateSaveImageXmlBaton* baton = static_cast<UpdateSaveImageXmlBaton*>(req->data);

		Hypervisor *hypervisor = baton->hypervisor;
    const char *path = baton->path;
    const char *xml = baton->xml;
		unsigned int flags = baton->flags;

		virErrorPtr err;
    int ret = -1;

		ret = virDomainSaveImageDefineXML(hypervisor->connection(), path, xml, flags);

    if(ret == -1) {
      err = virGetLastError();
      baton->error = err->message;
    }

    baton->res = ret;
	}

	void UpdateSaveImageXmlAsyncAfter(uv_work_t* req) {
		HandleScope scope;

		UpdateSaveImageXmlBaton* baton = static_cast<UpdateSaveImageXmlBaton*>(req->data);
		delete req;

		Handle<Value> argv[2];

		if (!baton->error.empty()) {
			argv[0] = Exception::Error(String::New(baton->error.c_str()));
			argv[1] = Undefined();
		}

		else {
			argv[0] = Undefined();
			argv[1] = scope.Close(True());
		}

		TryCatch try_catch;
		if (try_catch.HasCaught())
			FatalException(try_catch);

		baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
		delete baton;
	}

  Handle<Value> Domain::UpdateSaveImageXml(const Arguments& args) {
    HandleScope scope;

    if(args.Length() == 0 || !args[0]->IsString()) {
      return ThrowException(Exception::TypeError(
            String::New("You must specify a path string as the first argument")));
    }

    if(args.Length() == 1 || !args[1]->IsString()) {
      return ThrowException(Exception::TypeError(
            String::New("You must specify domain XML as the second argument")));
    }

    Local<Object> hyp_obj = args.This();

    if(!Hypervisor::HasInstance(hyp_obj)) {
      return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor object instance")));
    }

    if(args.Length() == 2 || !args[2]->IsFunction()) {
      return ThrowException(Exception::TypeError(
            String::New("You must specify a callback function as the third argument")));
    }

    unsigned int flags = 0;

    // Path and XML strings
    const char *path = parseString(args[0]);
    const char *xml  = parseString(args[1]);

    // Hypervisor context
    Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

    // Create baton
    UpdateSaveImageXmlBaton* baton = new UpdateSaveImageXmlBaton();

    // Callback
    Local<Function> callback = Local<Function>::Cast(args[1]);
    baton->callback = Persistent<Function>::New(callback);

    // Add data
    baton->hypervisor = hypervisor;
    baton->path = path;
    baton->xml = xml;
    baton->flags = flags;

    // Compose req
    uv_work_t* req = new uv_work_t;
    req->data = baton;

    uv_queue_work(
        uv_default_loop(),
        req,
        UpdateSaveImageXmlAsync,
        (uv_after_work_cb)UpdateSaveImageXmlAsyncAfter
    );

    return scope.Close(Undefined());
  }

} //namespace NodeLibvirt

