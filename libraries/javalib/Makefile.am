# Only build the peerless AWT libraries if kaffe is configured to
# build the native libraries as well.
if COND_KAFFE_QT_AWT
MAYBE_PEERLESS_KAFFE_AWT=awt-implementations
else
if COND_KAFFE_X_AWT
MAYBE_PEERLESS_KAFFE_AWT=awt-implementations
else
if COND_KAFFE_NANOX_AWT
MAYBE_PEERLESS_KAFFE_AWT=awt-implementations
endif
endif
endif

# Only build gmp java.math libraries if kaffe is configured
# to build the native libraries as well.
if COND_PURE_JAVA_MATH
else
MAYBE_GMP_JAVA_MATH=gmp-math
endif

if COND_NATIVE_SOUND
  MAYBE_TRITONUS_SOUND=tritonus-sound
endif

if USE_PRECOMPILED_RT_JAR
stamp-glibj:
	@echo Importing glibj.zip from $(PATH_TO_RT_JAR)
	@cp $(PATH_TO_RT_JAR) external/classpath/lib/glibj.zip
	@$(LN_S) external/classpath/lib/glibj.zip glibj.zip
	@touch stamp-glibj

MAYBE_GLIBJ_ZIP= glibj.zip

glibj.zip: stamp-glibj
else
stamp-glibj:
	@touch stamp-glibj
endif

clean-local:
	rm -rf stamp-glibj glibj.zip

SUBDIRS= \
	. \
	external/classpath \
	$(MAYBE_PEERLESS_KAFFE_AWT) \
	$(MAYBE_GMP_JAVA_MATH) \
	$(MAYBE_TRITONUS_SOUND)

jrelib_DATA= \
	     logging.properties \
	     $(MAYBE_GLIBJ_ZIP)

export JAVA_COMPILER

install-data-local: installdirs
	if test -f $(DESTDIR)${jrelibdir}/glibj.zip ; then \
		if ! test -f $(DESTDIR)${jrelibdir}/rt.jar ; then \
			$(LN_S) $(DESTDIR)${jrelibdir}/glibj.zip $(DESTDIR)${jrelibdir}/rt.jar ; \
		fi ; \
	fi

uninstall-local: 
	$(RM) $(DESTDIR)${jrelibdir}/rt.jar

EXTRA_DIST= \
	    Makefile.am.in \
	    logging.properties \
	vmspecific/META-INF/MANIFEST.MF \
	vmspecific/gnu/bytecodecvssnap/Access.java \
	vmspecific/gnu/bytecodecvssnap/ArrayClassLoader.java \
	vmspecific/gnu/bytecodecvssnap/ArrayType.java \
	vmspecific/gnu/bytecodecvssnap/AttrContainer.java \
	vmspecific/gnu/bytecodecvssnap/Attribute.java \
	vmspecific/gnu/bytecodecvssnap/ClassFileInput.java \
	vmspecific/gnu/bytecodecvssnap/ClassType.java \
	vmspecific/gnu/bytecodecvssnap/ClassTypeWriter.java \
	vmspecific/gnu/bytecodecvssnap/CodeAttr.java \
	vmspecific/gnu/bytecodecvssnap/ConstantPool.java \
	vmspecific/gnu/bytecodecvssnap/ConstantValueAttr.java \
	vmspecific/gnu/bytecodecvssnap/CpoolClass.java \
	vmspecific/gnu/bytecodecvssnap/CpoolEntry.java \
	vmspecific/gnu/bytecodecvssnap/CpoolFloat.java \
	vmspecific/gnu/bytecodecvssnap/CpoolNameAndType.java \
	vmspecific/gnu/bytecodecvssnap/CpoolRef.java \
	vmspecific/gnu/bytecodecvssnap/CpoolString.java \
	vmspecific/gnu/bytecodecvssnap/CpoolUtf8.java \
	vmspecific/gnu/bytecodecvssnap/CpoolValue1.java \
	vmspecific/gnu/bytecodecvssnap/CpoolValue2.java \
	vmspecific/gnu/bytecodecvssnap/ExceptionsAttr.java \
	vmspecific/gnu/bytecodecvssnap/Field.java \
	vmspecific/gnu/bytecodecvssnap/Filter.java \
	vmspecific/gnu/bytecodecvssnap/IfState.java \
	vmspecific/gnu/bytecodecvssnap/InnerClassesAttr.java \
	vmspecific/gnu/bytecodecvssnap/Label.java \
	vmspecific/gnu/bytecodecvssnap/LineNumbersAttr.java \
	vmspecific/gnu/bytecodecvssnap/LocalVarsAttr.java \
	vmspecific/gnu/bytecodecvssnap/Location.java \
	vmspecific/gnu/bytecodecvssnap/Method.java \
	vmspecific/gnu/bytecodecvssnap/MiscAttr.java \
	vmspecific/gnu/bytecodecvssnap/ObjectType.java \
	vmspecific/gnu/bytecodecvssnap/PrimType.java \
	vmspecific/gnu/bytecodecvssnap/Scope.java \
	vmspecific/gnu/bytecodecvssnap/SourceDebugExtAttr.java \
	vmspecific/gnu/bytecodecvssnap/SourceFileAttr.java \
	vmspecific/gnu/bytecodecvssnap/SwitchState.java \
	vmspecific/gnu/bytecodecvssnap/TryState.java \
	vmspecific/gnu/bytecodecvssnap/Type.java \
	vmspecific/gnu/bytecodecvssnap/VarEnumerator.java \
	vmspecific/gnu/bytecodecvssnap/Variable.java \
	vmspecific/gnu/bytecodecvssnap/ZipArchive.java \
	vmspecific/gnu/bytecodecvssnap/ZipLoader.java \
	vmspecific/gnu/bytecodecvssnap/dump.java \
	vmspecific/gnu/bytecodecvssnap/package.html \
	vmspecific/gnu/classpath/Configuration.java.in \
	vmspecific/gnu/classpath/VMStackWalker.java \
	vmspecific/gnu/classpath/VMSystemProperties.java \
	vmspecific/gnu/classpath/jdwp/VMFrame.java \
	vmspecific/gnu/classpath/jdwp/VMIdManager.java \
	vmspecific/gnu/classpath/jdwp/VMMethod.java \
	vmspecific/gnu/classpath/jdwp/VMVirtualMachine.java \
	vmspecific/gnu/getopt/Getopt.java \
	vmspecific/gnu/getopt/LongOpt.java \
	vmspecific/gnu/getopt/MessagesBundle.properties \
	vmspecific/gnu/getopt/MessagesBundle_cs.properties \
	vmspecific/gnu/getopt/MessagesBundle_de.properties \
	vmspecific/gnu/getopt/MessagesBundle_fr.properties \
	vmspecific/gnu/getopt/MessagesBundle_hu.properties \
	vmspecific/gnu/getopt/MessagesBundle_ja.properties \
	vmspecific/gnu/getopt/MessagesBundle_nl.properties \
	vmspecific/gnu/getopt/MessagesBundle_no.properties \
	vmspecific/gnu/inet/comsat/ComsatClient.java \
	vmspecific/gnu/inet/comsat/ComsatInfo.java \
	vmspecific/gnu/inet/finger/FingerConnection.java \
	vmspecific/gnu/inet/finger/FingerURLConnection.java \
	vmspecific/gnu/inet/finger/Handler.java \
	vmspecific/gnu/inet/finger/package.html \
	vmspecific/gnu/inet/gopher/DirectoryEntry.java \
	vmspecific/gnu/inet/gopher/DirectoryListing.java \
	vmspecific/gnu/inet/gopher/GopherConnection.java \
	vmspecific/gnu/inet/gopher/GopherContentHandler.java \
	vmspecific/gnu/inet/gopher/GopherURLConnection.java \
	vmspecific/gnu/inet/gopher/Handler.java \
	vmspecific/gnu/inet/gopher/package.html \
	vmspecific/gnu/inet/imap/IMAPConnection.java \
	vmspecific/gnu/inet/imap/IMAPConstants.java \
	vmspecific/gnu/inet/imap/IMAPException.java \
	vmspecific/gnu/inet/imap/IMAPResponse.java \
	vmspecific/gnu/inet/imap/IMAPResponseTokenizer.java \
	vmspecific/gnu/inet/imap/ListEntry.java \
	vmspecific/gnu/inet/imap/MailboxStatus.java \
	vmspecific/gnu/inet/imap/MessageSetTokenizer.java \
	vmspecific/gnu/inet/imap/MessageStatus.java \
	vmspecific/gnu/inet/imap/Namespaces.java \
	vmspecific/gnu/inet/imap/Pair.java \
	vmspecific/gnu/inet/imap/Quota.java \
	vmspecific/gnu/inet/imap/UIDPlusHandler.java \
	vmspecific/gnu/inet/imap/UTF7imap.java \
	vmspecific/gnu/inet/imap/package.html \
	vmspecific/gnu/inet/ldap/AttributeValues.java \
	vmspecific/gnu/inet/ldap/BERConstants.java \
	vmspecific/gnu/inet/ldap/BERDecoder.java \
	vmspecific/gnu/inet/ldap/BEREncoder.java \
	vmspecific/gnu/inet/ldap/BERException.java \
	vmspecific/gnu/inet/ldap/LDAPConnection.java \
	vmspecific/gnu/inet/ldap/LDAPResult.java \
	vmspecific/gnu/inet/ldap/Modification.java \
	vmspecific/gnu/inet/ldap/ResultHandler.java \
	vmspecific/gnu/inet/nntp/ActiveTime.java \
	vmspecific/gnu/inet/nntp/ActiveTimesIterator.java \
	vmspecific/gnu/inet/nntp/ArticleNumberIterator.java \
	vmspecific/gnu/inet/nntp/ArticleResponse.java \
	vmspecific/gnu/inet/nntp/ArticleStream.java \
	vmspecific/gnu/inet/nntp/FileNewsrc.java \
	vmspecific/gnu/inet/nntp/Group.java \
	vmspecific/gnu/inet/nntp/GroupIterator.java \
	vmspecific/gnu/inet/nntp/GroupResponse.java \
	vmspecific/gnu/inet/nntp/HeaderEntry.java \
	vmspecific/gnu/inet/nntp/HeaderIterator.java \
	vmspecific/gnu/inet/nntp/LineIterator.java \
	vmspecific/gnu/inet/nntp/NNTPConnection.java \
	vmspecific/gnu/inet/nntp/NNTPConstants.java \
	vmspecific/gnu/inet/nntp/NNTPException.java \
	vmspecific/gnu/inet/nntp/Newsrc.java \
	vmspecific/gnu/inet/nntp/Overview.java \
	vmspecific/gnu/inet/nntp/OverviewIterator.java \
	vmspecific/gnu/inet/nntp/Pair.java \
	vmspecific/gnu/inet/nntp/PairIterator.java \
	vmspecific/gnu/inet/nntp/PendingData.java \
	vmspecific/gnu/inet/nntp/PostStream.java \
	vmspecific/gnu/inet/nntp/Range.java \
	vmspecific/gnu/inet/nntp/StatusResponse.java \
	vmspecific/gnu/inet/nntp/package.html \
	vmspecific/gnu/inet/pop3/POP3Connection.java \
	vmspecific/gnu/inet/pop3/package.html \
	vmspecific/gnu/inet/smtp/Parameter.java \
	vmspecific/gnu/inet/smtp/ParameterList.java \
	vmspecific/gnu/inet/smtp/SMTPConnection.java \
	vmspecific/gnu/inet/smtp/package.html \
	vmspecific/gnu/inet/util/BASE64.java \
	vmspecific/gnu/inet/util/CRLFInputStream.java \
	vmspecific/gnu/inet/util/CRLFOutputStream.java \
	vmspecific/gnu/inet/util/EmptyX509TrustManager.java \
	vmspecific/gnu/inet/util/GetLocalHostAction.java \
	vmspecific/gnu/inet/util/LineInputStream.java \
	vmspecific/gnu/inet/util/MessageInputStream.java \
	vmspecific/gnu/inet/util/MessageOutputStream.java \
	vmspecific/gnu/inet/util/SaslCallbackHandler.java \
	vmspecific/gnu/inet/util/SaslCramMD5.java \
	vmspecific/gnu/inet/util/SaslInputStream.java \
	vmspecific/gnu/inet/util/SaslLogin.java \
	vmspecific/gnu/inet/util/SaslOutputStream.java \
	vmspecific/gnu/inet/util/SaslPlain.java \
	vmspecific/gnu/inet/util/TraceLevel.java \
	vmspecific/gnu/java/lang/VMInstrumentationImpl.java \
	vmspecific/gnu/java/lang/management/VMClassLoadingMXBeanImpl.java \
	vmspecific/gnu/java/lang/management/VMCompilationMXBeanImpl.java \
	vmspecific/gnu/java/lang/management/VMGarbageCollectorMXBeanImpl.java \
	vmspecific/gnu/java/lang/management/VMMemoryMXBeanImpl.java \
	vmspecific/gnu/java/lang/management/VMMemoryManagerMXBeanImpl.java \
	vmspecific/gnu/java/lang/management/VMMemoryPoolMXBeanImpl.java \
	vmspecific/gnu/java/lang/management/VMRuntimeMXBeanImpl.java \
	vmspecific/gnu/java/lang/management/VMThreadMXBeanImpl.java \
	vmspecific/gnu/java/net/DNSJavaInetAddressImpl.java \
	vmspecific/gnu/java/net/InetAddressImpl.java \
	vmspecific/gnu/java/net/PlainDatagramSocketImpl.java \
	vmspecific/gnu/java/net/PlainSocketImpl.java \
	vmspecific/gnu/java/net/SysInetAddressImpl.java \
	vmspecific/gnu/java/nio/VMChannel.java \
	vmspecific/gnu/java/nio/VMPipe.java \
	vmspecific/gnu/java/nio/VMSelector.java \
	vmspecific/java/io/VMFile.java \
	vmspecific/java/io/VMObjectInputStream.java \
	vmspecific/java/io/VMObjectStreamClass.java \
	vmspecific/java/lang/Runtime.java \
	vmspecific/java/lang/VMClass.java \
	vmspecific/java/lang/VMClassLoader.java \
	vmspecific/java/lang/VMCompiler.java \
	vmspecific/java/lang/VMDouble.java \
	vmspecific/java/lang/VMFloat.java \
	vmspecific/java/lang/VMMath.java \
	vmspecific/java/lang/VMObject.java \
	vmspecific/java/lang/VMProcess.java \
	vmspecific/java/lang/VMRuntime.java \
	vmspecific/java/lang/VMString.java \
	vmspecific/java/lang/VMSystem.java \
	vmspecific/java/lang/VMThread.java \
	vmspecific/java/lang/VMThrowable.java \
	vmspecific/java/lang/management/VMManagementFactory.java \
	vmspecific/java/lang/ref/Reference.java \
	vmspecific/java/lang/reflect/Constructor.java \
	vmspecific/java/lang/reflect/Field.java \
	vmspecific/java/lang/reflect/Method.java \
	vmspecific/java/lang/reflect/VMArray.java \
	vmspecific/java/lang/reflect/VMProxy.java \
	vmspecific/java/net/InetAddress.java \
	vmspecific/java/net/VMInetAddress.java \
	vmspecific/java/net/VMNetworkInterface.java \
	vmspecific/java/net/VMURLConnection.java \
	vmspecific/java/nio/VMDirectByteBuffer.java \
	vmspecific/java/nio/channels/VMChannels.java \
	vmspecific/java/security/VMAccessController.java \
	vmspecific/java/security/VMSecureRandom.java \
	vmspecific/java/util/VMTimeZone.java \
	vmspecific/java/util/mime/MimeType.java \
	vmspecific/java/util/mime/MimeTypeParameterList.java \
	vmspecific/java/util/mime/MimeTypeParseException.java \
	vmspecific/java/util/zip/Adler32.java \
	vmspecific/java/util/zip/CRC32.java \
	vmspecific/java/util/zip/Deflater.java \
	vmspecific/java/util/zip/DeflaterOutputStream.java \
	vmspecific/java/util/zip/Inflater.java \
	vmspecific/java/util/zip/ZipEntry.java \
	vmspecific/java/util/zip/ZipFile.java \
	vmspecific/java/util/zip/ZipInputStream.java \
	vmspecific/java/util/zip/ZipOutputStream.java \
	vmspecific/org/kaffe/jar/ExecJar.java \
	vmspecific/org/kaffe/jar/ExecJarName.java \
	vmspecific/org/kaffe/lang/UNIXProcess.java \
	vmspecific/org/kaffe/management/Classpath.java \
	vmspecific/org/kaffe/management/Debug.java \
	vmspecific/org/kaffe/management/JIT.java \
	vmspecific/org/kaffe/management/XProfiler.java \
	vmspecific/org/kaffe/security/LameRandomness.java \
	vmspecific/org/kaffe/security/Randomness.java \
	vmspecific/org/kaffe/security/UnixRandomness.java \
	vmspecific/org/kaffe/security/provider/Kaffe.java \
	vmspecific/org/kaffe/security/provider/MD2.java \
	vmspecific/org/kaffe/security/provider/MD4.java \
	vmspecific/org/kaffe/security/provider/MD5.java \
	vmspecific/org/kaffe/security/provider/SHA.java \
	vmspecific/org/kaffe/security/provider/SHA1PRNG.java \
	vmspecific/org/kaffe/security/provider/UpdateDigest.java \
	vmspecific/org/kaffe/util/Assert.java \
	vmspecific/org/kaffe/util/NotImplemented.java \
	vmspecific/org/kaffe/util/Ptr.java \
	vmspecific/org/kaffe/util/SupportDisabled.java \
	vmspecific/org/kaffe/util/UTF8.java \
	vmspecific/org/kaffe/util/zip/SwitchInflater.java \
	vmspecific/org/objectweb/asm153/Attribute.java \
	vmspecific/org/objectweb/asm153/ByteVector.java \
	vmspecific/org/objectweb/asm153/ClassReader.java \
	vmspecific/org/objectweb/asm153/ClassVisitor.java \
	vmspecific/org/objectweb/asm153/ClassWriter.java \
	vmspecific/org/objectweb/asm153/CodeVisitor.java \
	vmspecific/org/objectweb/asm153/CodeWriter.java \
	vmspecific/org/objectweb/asm153/Constants.java \
	vmspecific/org/objectweb/asm153/Edge.java \
	vmspecific/org/objectweb/asm153/Item.java \
	vmspecific/org/objectweb/asm153/Label.java \
	vmspecific/org/objectweb/asm153/Type.java \
	vmspecific/org/xbill/DNS/A6Record.java \
	vmspecific/org/xbill/DNS/AAAARecord.java \
	vmspecific/org/xbill/DNS/AFSDBRecord.java \
	vmspecific/org/xbill/DNS/APLRecord.java \
	vmspecific/org/xbill/DNS/ARecord.java \
	vmspecific/org/xbill/DNS/Address.java \
	vmspecific/org/xbill/DNS/CERTRecord.java \
	vmspecific/org/xbill/DNS/CNAMERecord.java \
	vmspecific/org/xbill/DNS/Cache.java \
	vmspecific/org/xbill/DNS/Client.java \
	vmspecific/org/xbill/DNS/Compression.java \
	vmspecific/org/xbill/DNS/Credibility.java \
	vmspecific/org/xbill/DNS/DClass.java \
	vmspecific/org/xbill/DNS/DNAMERecord.java \
	vmspecific/org/xbill/DNS/DNSInput.java \
	vmspecific/org/xbill/DNS/DNSKEYRecord.java \
	vmspecific/org/xbill/DNS/DNSOutput.java \
	vmspecific/org/xbill/DNS/DNSSEC.java \
	vmspecific/org/xbill/DNS/DSRecord.java \
	vmspecific/org/xbill/DNS/EmptyRecord.java \
	vmspecific/org/xbill/DNS/ExtendedFlags.java \
	vmspecific/org/xbill/DNS/ExtendedResolver.java \
	vmspecific/org/xbill/DNS/Flags.java \
	vmspecific/org/xbill/DNS/FormattedTime.java \
	vmspecific/org/xbill/DNS/GPOSRecord.java \
	vmspecific/org/xbill/DNS/Generator.java \
	vmspecific/org/xbill/DNS/HINFORecord.java \
	vmspecific/org/xbill/DNS/Header.java \
	vmspecific/org/xbill/DNS/ISDNRecord.java \
	vmspecific/org/xbill/DNS/InvalidDClassException.java \
	vmspecific/org/xbill/DNS/InvalidTTLException.java \
	vmspecific/org/xbill/DNS/InvalidTypeException.java \
	vmspecific/org/xbill/DNS/KEYBase.java \
	vmspecific/org/xbill/DNS/KEYRecord.java \
	vmspecific/org/xbill/DNS/KXRecord.java \
	vmspecific/org/xbill/DNS/LOCRecord.java \
	vmspecific/org/xbill/DNS/Lookup.java \
	vmspecific/org/xbill/DNS/MBRecord.java \
	vmspecific/org/xbill/DNS/MDRecord.java \
	vmspecific/org/xbill/DNS/MFRecord.java \
	vmspecific/org/xbill/DNS/MGRecord.java \
	vmspecific/org/xbill/DNS/MINFORecord.java \
	vmspecific/org/xbill/DNS/MRRecord.java \
	vmspecific/org/xbill/DNS/MXRecord.java \
	vmspecific/org/xbill/DNS/Master.java \
	vmspecific/org/xbill/DNS/Message.java \
	vmspecific/org/xbill/DNS/Mnemonic.java \
	vmspecific/org/xbill/DNS/NAPTRRecord.java \
	vmspecific/org/xbill/DNS/NSAPRecord.java \
	vmspecific/org/xbill/DNS/NSAP_PTRRecord.java \
	vmspecific/org/xbill/DNS/NSECRecord.java \
	vmspecific/org/xbill/DNS/NSRecord.java \
	vmspecific/org/xbill/DNS/NULLRecord.java \
	vmspecific/org/xbill/DNS/NXTRecord.java \
	vmspecific/org/xbill/DNS/Name.java \
	vmspecific/org/xbill/DNS/NameTooLongException.java \
	vmspecific/org/xbill/DNS/OPTRecord.java \
	vmspecific/org/xbill/DNS/Opcode.java \
	vmspecific/org/xbill/DNS/Options.java \
	vmspecific/org/xbill/DNS/PTRRecord.java \
	vmspecific/org/xbill/DNS/PXRecord.java \
	vmspecific/org/xbill/DNS/RPRecord.java \
	vmspecific/org/xbill/DNS/RRSIGRecord.java \
	vmspecific/org/xbill/DNS/RRset.java \
	vmspecific/org/xbill/DNS/RTRecord.java \
	vmspecific/org/xbill/DNS/Rcode.java \
	vmspecific/org/xbill/DNS/Record.java \
	vmspecific/org/xbill/DNS/RelativeNameException.java \
	vmspecific/org/xbill/DNS/ResolveThread.java \
	vmspecific/org/xbill/DNS/Resolver.java \
	vmspecific/org/xbill/DNS/ResolverConfig.java \
	vmspecific/org/xbill/DNS/ResolverListener.java \
	vmspecific/org/xbill/DNS/ReverseMap.java \
	vmspecific/org/xbill/DNS/SIGBase.java \
	vmspecific/org/xbill/DNS/SIGRecord.java \
	vmspecific/org/xbill/DNS/SOARecord.java \
	vmspecific/org/xbill/DNS/SRVRecord.java \
	vmspecific/org/xbill/DNS/SSHFPRecord.java \
	vmspecific/org/xbill/DNS/Section.java \
	vmspecific/org/xbill/DNS/Serial.java \
	vmspecific/org/xbill/DNS/SetResponse.java \
	vmspecific/org/xbill/DNS/SimpleResolver.java \
	vmspecific/org/xbill/DNS/SingleCompressedNameBase.java \
	vmspecific/org/xbill/DNS/SingleNameBase.java \
	vmspecific/org/xbill/DNS/TCPClient.java \
	vmspecific/org/xbill/DNS/TKEYRecord.java \
	vmspecific/org/xbill/DNS/TSIG.java \
	vmspecific/org/xbill/DNS/TSIGRecord.java \
	vmspecific/org/xbill/DNS/TTL.java \
	vmspecific/org/xbill/DNS/TXTRecord.java \
	vmspecific/org/xbill/DNS/TextParseException.java \
	vmspecific/org/xbill/DNS/Tokenizer.java \
	vmspecific/org/xbill/DNS/Type.java \
	vmspecific/org/xbill/DNS/TypedObject.java \
	vmspecific/org/xbill/DNS/U16NameBase.java \
	vmspecific/org/xbill/DNS/UDPClient.java \
	vmspecific/org/xbill/DNS/UNKRecord.java \
	vmspecific/org/xbill/DNS/Update.java \
	vmspecific/org/xbill/DNS/Verifier.java \
	vmspecific/org/xbill/DNS/WKSRecord.java \
	vmspecific/org/xbill/DNS/WireParseException.java \
	vmspecific/org/xbill/DNS/X25Record.java \
	vmspecific/org/xbill/DNS/Zone.java \
	vmspecific/org/xbill/DNS/ZoneTransferException.java \
	vmspecific/org/xbill/DNS/ZoneTransferIn.java \
	vmspecific/org/xbill/DNS/security/CERTConverter.java \
	vmspecific/org/xbill/DNS/security/DHPubKey.java \
	vmspecific/org/xbill/DNS/security/DNSSECVerifier.java \
	vmspecific/org/xbill/DNS/security/DSAPubKey.java \
	vmspecific/org/xbill/DNS/security/DSASignature.java \
	vmspecific/org/xbill/DNS/security/KEYConverter.java \
	vmspecific/org/xbill/DNS/security/RSAPubKey.java \
	vmspecific/org/xbill/DNS/security/SIG0Signer.java \
	vmspecific/org/xbill/DNS/utils/HMAC.java \
	vmspecific/org/xbill/DNS/utils/base16.java \
	vmspecific/org/xbill/DNS/utils/base64.java \
	vmspecific/org/xbill/DNS/utils/hexdump.java
