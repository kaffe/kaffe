CLASSFILE = kaffe-dnsjava.jar
SRCDIRS = org
jrelib_DATA = $(CLASSFILE)
LIBDIR = lib
JAR_CMD1 = ($(FASTJAR) cf $(CLASSFILE) -C $(LIBDIR) $(SRCDIRS) -C $(LIBDIR) gnu)

CPATH =  .:$(srcdir):$(LIBDIR):$(top_builddir)/libraries/javalib/vmspecific/kaffe-vminterface.jar:$(GLIBJ_ZIP)
AM_JAVACFLAGS = -classpath $(CPATH)
JAVAROOT = $(LIBDIR)

$(CLASSFILE): classdist_dnsjava.stamp
	rm -f $(CLASSFILE)
	$(JAR_CMD1)

clean-local:
	rm -rf $(LIBDIR) $(CLASSFILE)

# Use a fake install class and dir to keep automake's JAVA (un)install tasks happy.
FAKE=fakeinstall
install-data-local: 
	touch $(FAKE).class
uninstall-local: 
	rm $(FAKE).class
dnsjavadir = $(FAKE)

dist_dnsjava_JAVA = \
	org/xbill/DNS/security/CERTConverter.java \
	org/xbill/DNS/security/DHPubKey.java \
	org/xbill/DNS/security/DNSSECVerifier.java \
	org/xbill/DNS/security/DSAPubKey.java \
	org/xbill/DNS/security/DSASignature.java \
	org/xbill/DNS/security/KEYConverter.java \
	org/xbill/DNS/security/RSAPubKey.java \
	org/xbill/DNS/security/SIG0Signer.java \
	org/xbill/DNS/A6Record.java \
	org/xbill/DNS/AAAARecord.java \
	org/xbill/DNS/AFSDBRecord.java \
	org/xbill/DNS/APLRecord.java \
	org/xbill/DNS/ARecord.java \
	org/xbill/DNS/Address.java \
	org/xbill/DNS/CERTRecord.java \
	org/xbill/DNS/CNAMERecord.java \
	org/xbill/DNS/Cache.java \
	org/xbill/DNS/Client.java \
	org/xbill/DNS/Compression.java \
	org/xbill/DNS/Credibility.java \
	org/xbill/DNS/DClass.java \
	org/xbill/DNS/DNAMERecord.java \
	org/xbill/DNS/DNSInput.java \
	org/xbill/DNS/DNSKEYRecord.java \
	org/xbill/DNS/DNSOutput.java \
	org/xbill/DNS/DNSSEC.java \
	org/xbill/DNS/DSRecord.java \
	org/xbill/DNS/EmptyRecord.java \
	org/xbill/DNS/ExtendedFlags.java \
	org/xbill/DNS/ExtendedResolver.java \
	org/xbill/DNS/Flags.java \
	org/xbill/DNS/FormattedTime.java \
	org/xbill/DNS/GPOSRecord.java \
	org/xbill/DNS/Generator.java \
	org/xbill/DNS/HINFORecord.java \
	org/xbill/DNS/Header.java \
	org/xbill/DNS/ISDNRecord.java \
	org/xbill/DNS/InvalidDClassException.java \
	org/xbill/DNS/InvalidTTLException.java \
	org/xbill/DNS/InvalidTypeException.java \
	org/xbill/DNS/KEYBase.java \
	org/xbill/DNS/KEYRecord.java \
	org/xbill/DNS/KXRecord.java \
	org/xbill/DNS/LOCRecord.java \
	org/xbill/DNS/Lookup.java \
	org/xbill/DNS/MBRecord.java \
	org/xbill/DNS/MDRecord.java \
	org/xbill/DNS/MFRecord.java \
	org/xbill/DNS/MGRecord.java \
	org/xbill/DNS/MINFORecord.java \
	org/xbill/DNS/MRRecord.java \
	org/xbill/DNS/MXRecord.java \
	org/xbill/DNS/Master.java \
	org/xbill/DNS/Message.java \
	org/xbill/DNS/Mnemonic.java \
	org/xbill/DNS/NAPTRRecord.java \
	org/xbill/DNS/NSAPRecord.java \
	org/xbill/DNS/NSAP_PTRRecord.java \
	org/xbill/DNS/NSECRecord.java \
	org/xbill/DNS/NSRecord.java \
	org/xbill/DNS/NULLRecord.java \
	org/xbill/DNS/NXTRecord.java \
	org/xbill/DNS/Name.java \
	org/xbill/DNS/NameTooLongException.java \
	org/xbill/DNS/OPTRecord.java \
	org/xbill/DNS/Opcode.java \
	org/xbill/DNS/Options.java \
	org/xbill/DNS/PTRRecord.java \
	org/xbill/DNS/PXRecord.java \
	org/xbill/DNS/RPRecord.java \
	org/xbill/DNS/RRSIGRecord.java \
	org/xbill/DNS/RRset.java \
	org/xbill/DNS/RTRecord.java \
	org/xbill/DNS/Rcode.java \
	org/xbill/DNS/Record.java \
	org/xbill/DNS/RelativeNameException.java \
	org/xbill/DNS/ResolveThread.java \
	org/xbill/DNS/Resolver.java \
	org/xbill/DNS/ResolverConfig.java \
	org/xbill/DNS/ResolverListener.java \
	org/xbill/DNS/ReverseMap.java \
	org/xbill/DNS/SIGBase.java \
	org/xbill/DNS/SIGRecord.java \
	org/xbill/DNS/SOARecord.java \
	org/xbill/DNS/SRVRecord.java \
	org/xbill/DNS/SSHFPRecord.java \
	org/xbill/DNS/Section.java \
	org/xbill/DNS/Serial.java \
	org/xbill/DNS/SetResponse.java \
	org/xbill/DNS/SimpleResolver.java \
	org/xbill/DNS/SingleCompressedNameBase.java \
	org/xbill/DNS/SingleNameBase.java \
	org/xbill/DNS/TCPClient.java \
	org/xbill/DNS/TKEYRecord.java \
	org/xbill/DNS/TSIG.java \
	org/xbill/DNS/TSIGRecord.java \
	org/xbill/DNS/TTL.java \
	org/xbill/DNS/TXTRecord.java \
	org/xbill/DNS/TextParseException.java \
	org/xbill/DNS/Tokenizer.java \
	org/xbill/DNS/Type.java \
	org/xbill/DNS/TypedObject.java \
	org/xbill/DNS/U16NameBase.java \
	org/xbill/DNS/UDPClient.java \
	org/xbill/DNS/UNKRecord.java \
	org/xbill/DNS/Update.java \
	org/xbill/DNS/Verifier.java \
	org/xbill/DNS/WKSRecord.java \
	org/xbill/DNS/WireParseException.java \
	org/xbill/DNS/X25Record.java \
	org/xbill/DNS/Zone.java \
	org/xbill/DNS/ZoneTransferException.java \
	org/xbill/DNS/ZoneTransferIn.java \
	org/xbill/DNS/utils/HMAC.java \
	org/xbill/DNS/utils/base16.java \
	org/xbill/DNS/utils/base64.java \
	org/xbill/DNS/utils/hexdump.java \
	gnu/java/net/DNSJavaInetAddressImpl.java 
