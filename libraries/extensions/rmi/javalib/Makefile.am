#
# RMI extension library component.
#
# Copyright (c) 1999
#      Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.
#
# Class libraries are not currently part of the integrate Make cycle.
#

CLASSFILE = rmi.jar
SRCDIRS = java kaffe

class_DATA = $(CLASSFILE)

rmi_jar_SRCS = \
	$(java_rmi_SRCS) \
	$(java_rmi_activation_SRCS) \
	$(java_rmi_dgc_SRCS) \
	$(java_rmi_registry_SRCS) \
	$(java_rmi_server_SRCS) \
	$(kaffe_rmi_dgc_SRCS) \
	$(kaffe_rmi_registry_SRCS) \
	$(kaffe_rmi_rmic_SRCS) \
	$(kaffe_rmi_server_SRCS)
java_rmi_SRCS = \
	java/rmi/AccessException.java \
	java/rmi/AlreadyBoundException.java \
	java/rmi/ConnectException.java \
	java/rmi/ConnectIOException.java \
	java/rmi/MarshalException.java \
	java/rmi/MarshalledObject.java \
	java/rmi/Naming.java \
	java/rmi/NoSuchObjectException.java \
	java/rmi/NotBoundException.java \
	java/rmi/RMISecurityException.java \
	java/rmi/RMISecurityManager.java \
	java/rmi/Remote.java \
	java/rmi/RemoteException.java \
	java/rmi/ServerError.java \
	java/rmi/ServerException.java \
	java/rmi/ServerRuntimeException.java \
	java/rmi/StubNotFoundException.java \
	java/rmi/UnexpectedException.java \
	java/rmi/UnknownHostException.java \
	java/rmi/UnmarshalException.java
java_rmi_activation_SRCS = \
	java/rmi/activation/Activatable.java \
	java/rmi/activation/ActivateFailedException.java \
	java/rmi/activation/ActivationDesc.java \
	java/rmi/activation/ActivationException.java \
	java/rmi/activation/ActivationGroup.java \
	java/rmi/activation/ActivationGroupDesc.java \
	java/rmi/activation/ActivationGroupID.java \
	java/rmi/activation/ActivationID.java \
	java/rmi/activation/ActivationInstantiator.java \
	java/rmi/activation/ActivationMonitor.java \
	java/rmi/activation/ActivationSystem.java \
	java/rmi/activation/Activator.java \
	java/rmi/activation/UnknownGroupException.java \
	java/rmi/activation/UnknownObjectException.java
java_rmi_dgc_SRCS = \
	java/rmi/dgc/DGC.java \
	java/rmi/dgc/Lease.java \
	java/rmi/dgc/VMID.java
java_rmi_registry_SRCS = \
	java/rmi/registry/LocateRegistry.java \
	java/rmi/registry/Registry.java \
	java/rmi/registry/RegistryHandler.java
java_rmi_server_SRCS = \
	java/rmi/server/ExportException.java \
	java/rmi/server/LoaderHandler.java \
	java/rmi/server/LogStream.java \
	java/rmi/server/ObjID.java \
	java/rmi/server/Operation.java \
	java/rmi/server/RMIClassLoader.java \
	java/rmi/server/RMIClientSocketFactory.java \
	java/rmi/server/RMIFailureHandler.java \
	java/rmi/server/RMIServerSocketFactory.java \
	java/rmi/server/RMISocketFactory.java \
	java/rmi/server/RemoteCall.java \
	java/rmi/server/RemoteObject.java \
	java/rmi/server/RemoteRef.java \
	java/rmi/server/RemoteServer.java \
	java/rmi/server/RemoteStub.java \
	java/rmi/server/ServerCloneException.java \
	java/rmi/server/ServerNotActiveException.java \
	java/rmi/server/ServerRef.java \
	java/rmi/server/Skeleton.java \
	java/rmi/server/SkeletonMismatchException.java \
	java/rmi/server/SkeletonNotFoundException.java \
	java/rmi/server/SocketSecurityException.java \
	java/rmi/server/UID.java \
	java/rmi/server/UnicastRemoteObject.java \
	java/rmi/server/Unreferenced.java
kaffe_rmi_dgc_SRCS = \
	kaffe/rmi/dgc/DGCImpl.java \
	kaffe/rmi/dgc/DGCImpl_Skel.java \
	kaffe/rmi/dgc/DGCImpl_Stub.java
kaffe_rmi_registry_SRCS = \
	kaffe/rmi/registry/RegistryImpl.java \
	kaffe/rmi/registry/RegistryImpl_Skel.java \
	kaffe/rmi/registry/RegistryImpl_Stub.java
kaffe_rmi_rmic_SRCS = \
	kaffe/rmi/rmic/RMIC.java \
	kaffe/rmi/rmic/TabbedWriter.java
kaffe_rmi_server_SRCS = \
	kaffe/rmi/server/ProtocolConstants.java \
	kaffe/rmi/server/RMIDefaultSocketFactory.java \
	kaffe/rmi/server/RMIHashes.java \
	kaffe/rmi/server/RMIObjectInputStream.java \
	kaffe/rmi/server/RMIObjectOutputStream.java \
	kaffe/rmi/server/UnicastConnection.java \
	kaffe/rmi/server/UnicastConnectionManager.java \
	kaffe/rmi/server/UnicastRef.java \
	kaffe/rmi/server/UnicastRemoteCall.java \
	kaffe/rmi/server/UnicastRemoteStub.java \
	kaffe/rmi/server/UnicastServer.java \
	kaffe/rmi/server/UnicastServerRef.java

## Do not delete the previous blank line; update-class-list depends on it

EXTRA_DIST = $(class_DATA) $(rmi_jar_SRCS)

LIBDIR = lib

clean-hook:
	rm -rf $(LIBDIR)

.PHONY: new-classes
new-classes: $(top_srcdir)/developers/update-class-list
	cd $(srcdir) && $(SHELL) $(top_srcdir)/developers/update-class-list

rebuildLib = $(top_builddir)/libraries/javalib/rebuildLib
Klasses_jar = $(top_srcdir)/libraries/javalib/Klasses.jar

.PHONY: compile-classes classes
## classes is provided for backward compatibility; use compile-classes
compile-classes $(LIBDIR)/stamp classes: $(rebuildLib) $(rmi_jar_SRCS)
	@echo $(SHELL) $(rebuildLib)
	@CPATH=$(Klasses_jar) $(SHELL) $(rebuildLib) $(rmi_jar_SRCS)
	@echo timestamp > $(LIBDIR)/stamp

.PHONY: jar-classes
jar-classes $(LIBDIR)/rmi.jar: $(LIBDIR)/stamp
	rm -f $(LIBDIR)/$(CLASSFILE)
	(cd $(LIBDIR) && $(ZIP) -r $(CLASSFILE) $(SRCDIRS))

.PHONY: build-classes Klasses
build-classes Klasses: $(LIBDIR)/$(CLASSFILE)
	rm -f $(srcdir)/$(CLASSFILE)
	cp $(LIBDIR)/$(CLASSFILE) $(srcdir)/$(CLASSFILE)
