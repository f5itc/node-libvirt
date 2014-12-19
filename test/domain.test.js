var should  = require('should');


var SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

var sys = require('sys');
var libvirt = require('../build/Release/libvirt');
var fixture = require('./lib/helper').fixture;

var Hypervisor = libvirt.Hypervisor;
var hypervisor = new Hypervisor('test:///default');;
var domain;

// Hypervisor-object functions relating to and defined with the domain.
describe('hypervisor', function() {
	describe('#createDomain(xml, flags, callback)', function() {
		var flags = [];
		it('should create a persistent Domain from its JSON Description', function(done) {
			var xml = fixture('domain.xml');

			hypervisor.createDomain(xml, flags, function(err, res) {
				if (err) done(err);

				res.should.be.ok;
				res.getName().should.equal('nodejs-test');
				done();
			});
		}),

		it('should throw an error if no arguments are passed', function() {
			try {
				hypervisor.createDomain();
				should.fail();
			} catch(e) {
				e.message.should.equal('You must specify a string as first argument');
			}
		}),

		it('should throw an error if the first value is not a string', function(done) {
			try {
				hypervisor.createDomain(flags, function(err, res) {
					should.fail();
					done();
				});
			} catch(e) {
				e.message.should.equal('You must specify a string as first argument');
				done();
			}
		}),

		it('should throw an error if no flags are passed', function(done) {
			try {
				hypervisor.createDomain(xml, function(err, res) {
					should.fail();
					done();
				});
			} catch(e) {
				e.message.should.equal("xml is not defined");
				done();
				}
			})
		})

	describe('Domain Lookups', function() {
		describe('#lookupDomainById(id, callback)', function() {
			it('should lookup a domain by id', function(done) {
				hypervisor.lookupDomainById(1, function(err, res) {
					if (err) done(err);
					res.should.be.an.instanceOf(Object);
					done();
				});
			}),
			it('should throw an error if the first argument is not an integer', function(done) {
				try {
					hypervisor.lookupDomainById('1', function(err, res) {
						done();
					});
				} catch(e) {
					e.should.be.instanceOf(TypeError);
					done();
				}
			}),
			it('should return an error if the domain does not exist', function(done) {
				hypervisor.lookupDomainById(99999999, function(err, res) {
					if (err) {
						err.message.should.equal("Domain not found");
						return done();
					}
					should.fail();
					done();
				});
			})
		})

		describe('#lookupDomainByName(name, callback)', function() {
			it('should lookup a domain by name', function(done) {
				hypervisor.lookupDomainByName('nodejs-test', function(err, res) {
					if (err) {
						should.fail();
						return done(err);
					}
					res.should.be.instanceOf(Object);
					return done();
				});
			}),
			it('should throw an error if the first argument is not a string', function(done) {
				try {
					hypervisor.lookupDomainByName(1, function(err, res) {
						should.fail();
						return done();
					});
					should.fail();
					done();
				} catch (e) {
					done();
				}
			}),
			it('should return an error if the domain does not exist', function(done) {
				hypervisor.lookupDomainByName('wat', function(err, res) {
					if (err) {
						err.message.should.equal("Domain not found");
						return done();
					}
					should.fail();
					done();
				});
			})
		})
	})
})

describe('domain', function() {
	var domain;
	hypervisor.lookupDomainById(1, function(err, res) {
		domain = res;
	});

	describe('#getId()', function() {
		it('should return the id', function() {
			(domain.getId() !== null).should.be.true;
			(domain.getId()).should.be.exactly(1);
		})
	})

	describe('#getOsType()', function() {
		it('should return the operating system type', function() {
			(domain.getOsType() != null).should.be.true;
			domain.getOsType().should.equal('linux');
		})
	})

	describe('#getInfo(callback)', function() {
		it('should return all domain information', function() {
			domain.getInfo(function(err, res) {
				res.should.have.properties({
					'state': domain.VIR_DOMAIN_RUNNING,
					'max_memory': 8388608,
					'memory': 2097152,
					'vcpus_number': 2
				});
				res.cpu_time.should.be.ok;
				done();
			});
		})
	})

	describe('#getName()', function() {
		it('should return the name', function() {
			domain.getName().should.equal('test');
		})
	})

	describe('#getUUID()', function() {
		it('should return a uuid', function() {
			(domain.getUUID() === null).should.not.be.true;
			domain.getUUID().should.be.type("string");
		})
	})

	describe('Autostart', function() {
		describe('#getAutostart()', function() {
			it('should indicate the enabledness of autostart', function() {
				domain.getAutostart().should.be.true;
			})
		})

		describe('#setAutostart(enabled)', function() {
			it('should enable autostart', function() {
				domain.setAutostart(false).should.be.true;
				domain.getAutostart().should.be.false;
			}),

			it('should disable autostart', function() {
				domain.setAutostart(true).should.be.true;
				domain.getAutostart().should.be.true;
			})
		})
	})

	describe('Memory', function() {
		describe('#getMaxMemory()', function() {
			it('should return max amount of physical memory (kilobytes) allocated to a domain', function() {
				domain.getMaxMemory().should.equal(8388608);
			})
		})

		describe('#setMaxMemory(kilobytes)', function() {
			it('should change the max amount of physical memory allocated to a domain', function() {
				domain.setMaxMemory(512000).should.be.true;
				domain.getMaxMemory().should.equal(512000);
			})
		})

		describe('#setMemory(kilobytes)', function(done) {
			it('should dynamically change the runtime amount of memory allocated to a domain', function() {
				domain.setMemory(256000).should.be.true;
				domain.getInfo(function(err, res) {
					res.memory.should.equal(256000);
					done();
				});
			})
		})
	})

	describe('Power states', function() {
		var path;
		before(function() {
			path = '/tmp/' + domain.getName() + '-saved.img';
		})

		describe('#isActive()', function() {
			it('should indicate whether or not the domain is active', function() {
				domain.isActive().should.be.true;
			})
		})

		describe('#isPersistent()', function() {
			it('should indicate whether the domain is persistent', function() {
				domain.isPersistent().should.be.true;
			})
		})

		describe('#save(path), #restore(path)', function() {
			it('should save the domain', function() {
				domain.save(path).should.be.true;
			}),

			it('should restore the domain', function() {
				hypervisor.restoreDomain(path).should.be.true;
			})
		})

		describe('#reset()', function() {
			it('should reset the domain')
			// libvirt:  error : this function is not supported by the connection driver: virDomainReset
			//domain.reset().should.be.false;
		})

		describe('#reboot()', function() {
			it('should reboot the domain', function() {
				domain.reboot().should.be.true;
			})
		})

		describe('#suspend(), #resume()', function() {
			it('should suspend the domain', function() {
				domain.suspend(function(err, res) {
					if(err) done(err);
					res.should.be.true;
					done();
				});
			}),

			it('should resume the domain', function() {
				domain.resume(function(err, res) {
					if(err) done(err);
					res.should.be.true;
					done();
				})
			})
		})

		describe('#shutdown()', function() {
			it('should shutdown the domain', function() {
				domain.shutdown().should.be.true;
			})
		})
	})

	describe('vCPU getters and setters', function() {
		beforeEach(function() {
			if (!domain.isActive()){
				domain.start();
			}
		})

		describe('#getMaxVcpus()', function() {
			it('should return the maximum number of virtual CPUs supported for the guest VM', function() {
				domain.getMaxVcpus().should.equal(2);
			})
		})

		describe('#setVcpus(cpus)', function() {
			it('should dynamically change the number of virtual CPUs used by the domain', function() {
				domain.setVcpus(1).should.be.true;
			})
		})

		describe('#getVcpus()', function() {
			it('should get information about all vcpus', function() {
				var vcpus = domain.getVcpus();
				var affinity = vcpus[0].affinity;
				var real_cpu = 0; //pedagogical purpose

				vcpus.should.be.instanceOf(Array);
				(vcpus !== undefined).should.be.true;
				vcpus[0].should.have.properties(
					'number',
					'state',
					'cpu_time',
					'cpu',
					'affinity'
				);

				vcpus[0].affinity.should.be.instanceOf(Array);
				vcpus[0].affinity[real_cpu].should.have.property('usable');
			})
		})

		describe('#pinVcpus()', function() {
			it('should allow change of real CPUs, which can be allocated to a virtual CPU', function() {
				var vcpus = domain.getVcpus();
				var affinity = vcpus[0].affinity;
				affinity[0].usable = false;
				affinity[1].usable = false;

				domain.pinVcpu(vcpus[0].number, vcpus[0].affinity).should.be.true;

				var vcpus2 = domain.getVcpus();
				var affinity2 = vcpus2[0].affinity;

				affinity2[0].usable.should.be.false;
				affinity2[1].usable.should.be.false;
			}),

			it('should throw an error if no arguments are provided', function() {
				(function() { domain.pinVcpu() }).should.throw();
			}),

			it('should throw an error if only one argument is provided', function() {
				(function() { domain.pinVcpu.bind(null, vcpus[0].number) }).should.throw();
			}),

			it('should throw an error if the first argument is not an integer', function() {
				(function() { domain.pinVcpu.bind(null, 'test', affinity) }).should.throw();
			}),

			it('should throw an error if the second argument is not an array', function() {
				(function() { domain.pinVcpu.bind(null, vcpus[0].number, 2) }).should.throw();
			}),

			it('should throw an error if the second argument is not an array of objects', function() {
				(function() { domain.pinVcpu.bind(null, vcpus[0].number, ['']) }).should.throw();
			})
		})
	})

	describe('Devices', function() {
		var device = fixture('device.xml'),
		device_update = fixture('device_update.xml')
		flags = [];

// libvirt:  error : this function is not supported by the connection driver: virDomainAttachDevice
		describe('#attachDevice(device, flags, callback)', function() {
			it('should attach a device'),
//				domain.attachDevice(device, flags, function(err, res) {
//					console.log("Result: " + res);
//					res.should.be.true;
//					done(err);
//				});
//			}),

			it('should throw an error if there are no arguments', function() {
				(function() { domain.attachDevice() }).should.throw("First argument must be string of device XML");
			})
		})

// libvirt:  error : this function is not supported by the connection driver: virDomainDetachDevice
		describe('#detachDevice(device, flags, callback)', function() {
			it('should detach a device'),
//				domain.detachDevice(device, flags, function(err, res) {
//					if (err) done(err);
//					res.should.be.true;
//					done();
//				});
//			}),

			it('should throw an error if there are no arguments', function() {
				(function() { domain.detachDevice() }).should.throw();
			})
		})

// libvirt:  error : this function is not supported by the connection driver: virDomainUpdateDeviceFlags
		describe('#updateDevice(device, flags, callback)', function() {
			it('should update a device'),
			//	flags = [libvirt.VIR_DOMAIN_DEVICE_MODIFY_CONFIG];
			//	domain.updateDevice(device_update, flags).should.be.true;

			it('should throw an error if there are no arguments', function() {
				(function() { domain.updateDevice() }).should.throw();
			})
		})
	})

	describe('Migrating between hypervisors', function() {
		//		var flags;
		// libvirt:  error : this function is not supported by the connection driver: virDomainMigrateToURI
		describe('#migrateDomain(newDomain)', function() {
			it('should migrate a domain to another hypervisor through a hypervisor connection (bandwidth in Mbps)'),
			//			flags = [
			//					domain.VIR_MIGRATE_LIVE,
			//					domain.VIR_MIGRATE_PEER2PEER,
			//					domain.VIR_MIGRATE_PAUSED,
			//					domain.VIR_MIGRATE_PERSIST_DEST
			//				];
			//			var hypervisor2 = new Hypervisor('test:///default');
			//				domain.migrate({
			//					'dest_hypervisor': hypervisor2,
			//					'dest_name': 'test2',
			//					'dest_uri': '',
			//					'bandwidth': 100,
			//					'flags': flags
			//				}).should.be.true;
			//			}),

			it('should migrate a domain to another hypervisor through a URI')
			//				domain.migrate({
			//					'dest_uri': 'test:///default',
			//					'dest_name': 'test2',
			//					'bandwidth': 100,
			//					'flags': flags
			//				});
			//			})
		})

		// libvirt:  error : this function is not supported by the connection driver: virDomainMigrateSetMaxDowntime
		describe('#setMigrationMaxDowntime(milliseconds)', function() {
			it('should set a maximum tolerable time for which the domain is allowed to be paused at the end of live migration')
			//				domain.setMigrationMaxDowntime(100000).should.be.true;
			//			})
		})
	})

	describe('Domain Representations', function() {
		describe('#toXml(flags)', function() {
			it('should return domain XML representation', function() {
				var flags = [
					libvirt.VIR_DOMAIN_XML_SECURE,
					libvirt.VIR_DOMAIN_XML_INACTIVE
				];

				domain.toXml(flags, function(err, res) {
					if (err) done(err);
					res.should.match(/<name>test\/name/);
				});
			})
		})

		// libvirt:  Test Driver error : this function is not supported by the conenction driver: virDomainGetJobInfo
		describe('#getJobInfo()', function() {
			it('should return domain JSON representation')
			//			var info = domain.getJobInfo();
			//			info.should.have.properties(
			//				'type',
			//				'time',
			//				'data',
			//				'memory'
			//			);
			//			info.time.should.have.properties(
			//				'elapsed',
			//				'remaining'
			//			);
			//			info.data.should.have.properties(
			//			'total',
			//				'processed',
			//				'remaining'
			//			);
			//			info.memory.should.have.properties(
			//				'total',
			//				'processed',
			//				'remaining'
			//			);
			//			info.file.should.have.properties(
			//				'total',
			//				'processed',
			//				'remaining'
			//			);
			//		})
		})
	})

	describe('The Job Scheduler', function() {
		// libvirt:  Test Driver error : this function is not supported by the connection driver: virDomainAbortJob
		describe('#abortCurrentJob', function() {
			it('should abort the current background job on the domain')
			//				domain.abortCurrentJob().should.be.true;
			//			})
		})

		describe('#getSchedParams()', function() {
			it('should get the domain scheduler parameters', function() {
				var params = domain.getSchedParams();
				params.weight.should.equal(50);
			})
		})

		describe('#setSchedParams', function() {
			it('should set the domain scheduler parameters', function() {
				var params = domain.getSchedParams();
				params.weight = 30;

				domain.setSchedParams(params).should.be.true;
				params = domain.getSchedParams();
				params.weight.should.equal(50);
			})
		})
	})

	// libvirt:  Test Driver error : this function is not supported by the connection driver: virDomainGetSecurityLabel
	describe('#getSecurityLabel()', function() {
		it('should return the domain security labels')
		//			var info = domain.getSecurityLabel();
		//			info.should.have.properties(
		//				'label',
		//				'enforcing'
		//			);
		//		})
	})

	describe('Managed Images', function() {
		describe('#saveManagedImage', function() {
			it('should save a managed image of the domain', function() {
				domain.saveManagedImage().should.be.true;
				domain.start();
			})
		})

		describe('#hasManagedImage()', function() {
			it('should return true if there is a managed image', function() {
				domain.saveManagedImage().should.be.true;
				domain.hasManagedImage().should.be.true;
			}),

			it('should return false if there is no managed image', function() {
				domain.removeManagedImage();
				domain.hasManagedImage().should.be.false;
			})
		})

		describe('#removeManagedImage()', function() {
			it('should remove a managed image of the domain', function() {
				domain.removeManagedImage().should.be.true;
			}),

			it('should do something, but i don\'t know what', function() {
				if(domain.hasManagedImage())
					domain.removeManagedImage();
				domain.removeManagedImage().should.be.true;
			})
		})
	})

	describe('Memory Information', function() {
		//	libvirt:  error : this function is not supported by the connection driver: virDomainMemoryPeek
		describe('#memorypeek(start, end, type)', function() {
			//			var physical, virtual;
			//			before(function(){
			//				physical = [domain.VIR_MEMORY_PHYSICAL];
			//				virtual = [domain.VIR_MEMORY_VIRTUAL];
			//			})
			it('should allow read of the domain\'s physical memory content'),
			//				domain.memoryPeek(0, 1024, physical).should.be.instanceOf(Buffer);
			//			}),
			it('should allow read of the domain\'s virtual memory content')
			//				domain.emmoryPeek(0, 1024, virtual).should.be.instanceOf(Buffer);
			//			})
		})

		//	libvirt:  error : this function is not supported by the connection driver: virDomainMemoryStats
		describe('#getMemoryStats()', function() {
			//			var stats;
			//			before(function() {
			//				stats = domain.getMemoryStats();
			//			});
			//
			it('should return the domain\'s memory statistics (in kbs)')
			//				stats.should.have.properties(
			//					'swap_in',
			//					'swap_out',
			//					'major_fault',
			//					'minor_fault',
			//					'unused',
			//					'available'
			//				);
			//			})
		})

		describe('#coreDump(path)', function() {
			it('should dump the core of a domain on a given file for analysis', function() {
				var path = '/tmp/dumpcore-test.txt';
				domain.coreDump(path).should.be.ok;
			})
		})
	})

	describe('Device Information', function() {
		//	libvirt:  error : this function is not supported by the connection driver: virdomainblockpeek
		describe('#blockpeek(device, start, end)', function() {
			it('should allow read of the domain\'s block device')
			//	domain.blockpeek('/dev/sda', 0, 1024).should.be.instanceof(buffer);
			//})
		})

		//	libvirt:  error : this function is not supported by the connection driver: virdomaingetblockinfo
		describe('#getblockstats(device)', function() {
			it('should return block device stats for block devices attached to the domain')
			//	var stats = domain.getblockstats('/dev/sda');
			//	stats.should.have.properties(
			//		'read_requests',
			//		'read_bytes',
			//		'write_requests',
			//		'write_bytes'
			//	);
			//})
		})

		//	libvirt:  error : this function is not supported by the connection driver: virdomaingetblockinfo
		describe('#getblockstats(path)', function() {
			it('should return basic information about a domain\'s block device')
			//				var info = domain.getblockinfo('/path');
			//				info.should.have.properties(
			//					'capacity',
			//					'allocation',
			//					'physical'
			//				);
			//			})
		})
	})
	//
	// None of these network tests will work until domain.attachDevice has test driver supprt...
	describe('Network Interfaces', function() {
		//		before(function() {
		//			var ethDevice = fixture('interface.xml'),
		//			flags = [];
		//
		//			domain.attachDevice(ethDevice, flags, function(err, res) {
		//				return res;
		//			});
		//		})
		//
		describe('#getInterfaceStats', function() {
			it('should return network interface statistics of the domain')
			//				var stats = domain.getInterfaceStats('eth1');
			//				stats.should.have.properties(
			//					'rx_bytes',
			//					'rx_packets',
			//					'rx_errors',
			//					'rx_drop',
			//					'tx_bytes',
			//					'tx_packets',
			//					'tx_errors',
			//					'tx_drop'
			//				);
			//			})
		})
		//
		describe('#getInterfaces()', function() {
			//			//TODO: Figure out what this is actually supposed to return
			it('should get the network interfaces available')
			//				domain.getInterfaces().should.be.ok;
			//			})
		})
	})

	describe('Snapshots', function() {
		var xml = fixture('domainsnapshot.xml');
		var flags = [];
		describe('#takeSnapshot()', function() {
			it('should take a snapshot of the domain', function() {
				domain.takeSnapshot(xml, flags, function(err, res) {
					res.should.be.true;
					(err === null).should.be.true;
					done();
				});
			})
		})

		describe('#hasCurrentSnapshot()', function() {
			it('should return true if the domain does have a snapshot', function() {
				domain.hasCurrentSnapshot().should.be.true;
			})
		})

		describe('#getCurrentSnapshot()', function() {
			it('should get the current snapshot of the domain', function() {
				domain.getCurrentSnapshot().should.be.ok;
			})
		})

		describe('#lookupSnapshotByName(snapshot)', function() {
			it('should get a snapshot of a domain by its name', function() {
				domain.lookupSnapshotByName('test-snapshot').should.be.ok;
			})
		})

		describe('#getSnapshots()', function() {
			it('should get all snapshots of the domain', function() {
				domain.getSnapshots().should.be.instanceOf(Array);
			})
		})

		describe('#revertToSnapshot()',function() {
			it('should revert a domain to a snapshot', function() {
				domain.revertToSnapshot('test-snapshot', function(err, res) {
					res.should.be.ok;
					done();
				});
			})
		})

		describe('#deleteSnapshot()', function() {
			it('should delete a snapshot if one exists', function() {
				domain.deleteSnapshot('test-snapshot').should.be.true;
			})
		})
	})
})
