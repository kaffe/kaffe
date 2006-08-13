CLASSFILE = kaffe-gnuinet.jar
SRCDIRS = gnu
jrelib_DATA = $(CLASSFILE)
LIBDIR = lib
JAR_CMD1 = ($(FASTJAR) cf $(CLASSFILE) -C $(LIBDIR) $(SRCDIRS))

CPATH =  .:$(srcdir):$(LIBDIR):$(top_builddir)/libraries/javalib/external/classpath/lib/glibj.zip
AM_JAVACFLAGS = -classpath $(CPATH)
JAVAC = $(JAVA_COMPILER)
JAVAROOT = $(LIBDIR)

$(CLASSFILE): classdist_gnuinet.stamp
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
gnuinetdir = $(FAKE)

dist_gnuinet_JAVA = \
	gnu/inet/comsat/ComsatClient.java \
	gnu/inet/comsat/ComsatInfo.java \
	gnu/inet/finger/FingerConnection.java \
	gnu/inet/finger/FingerURLConnection.java \
	gnu/inet/finger/Handler.java \
	gnu/inet/gopher/DirectoryEntry.java \
	gnu/inet/gopher/DirectoryListing.java \
	gnu/inet/gopher/GopherConnection.java \
	gnu/inet/gopher/GopherContentHandler.java \
	gnu/inet/gopher/GopherURLConnection.java \
	gnu/inet/gopher/Handler.java \
	gnu/inet/imap/IMAPConnection.java \
	gnu/inet/imap/IMAPConstants.java \
	gnu/inet/imap/IMAPException.java \
	gnu/inet/imap/IMAPResponse.java \
	gnu/inet/imap/IMAPResponseTokenizer.java \
	gnu/inet/imap/ListEntry.java \
	gnu/inet/imap/MailboxStatus.java \
	gnu/inet/imap/MessageSetTokenizer.java \
	gnu/inet/imap/MessageStatus.java \
	gnu/inet/imap/Namespaces.java \
	gnu/inet/imap/Pair.java \
	gnu/inet/imap/Quota.java \
	gnu/inet/imap/UIDPlusHandler.java \
	gnu/inet/imap/UTF7imap.java \
	gnu/inet/ldap/AttributeValues.java \
	gnu/inet/ldap/BERConstants.java \
	gnu/inet/ldap/BERDecoder.java \
	gnu/inet/ldap/BEREncoder.java \
	gnu/inet/ldap/BERException.java \
	gnu/inet/ldap/LDAPConnection.java \
	gnu/inet/ldap/LDAPResult.java \
	gnu/inet/ldap/Modification.java \
	gnu/inet/ldap/ResultHandler.java \
	gnu/inet/nntp/ActiveTime.java \
	gnu/inet/nntp/ActiveTimesIterator.java \
	gnu/inet/nntp/ArticleNumberIterator.java \
	gnu/inet/nntp/ArticleResponse.java \
	gnu/inet/nntp/ArticleStream.java \
	gnu/inet/nntp/FileNewsrc.java \
	gnu/inet/nntp/Group.java \
	gnu/inet/nntp/GroupIterator.java \
	gnu/inet/nntp/GroupResponse.java \
	gnu/inet/nntp/HeaderEntry.java \
	gnu/inet/nntp/HeaderIterator.java \
	gnu/inet/nntp/LineIterator.java \
	gnu/inet/nntp/NNTPConnection.java \
	gnu/inet/nntp/NNTPConstants.java \
	gnu/inet/nntp/NNTPException.java \
	gnu/inet/nntp/Newsrc.java \
	gnu/inet/nntp/Overview.java \
	gnu/inet/nntp/OverviewIterator.java \
	gnu/inet/nntp/Pair.java \
	gnu/inet/nntp/PairIterator.java \
	gnu/inet/nntp/PendingData.java \
	gnu/inet/nntp/PostStream.java \
	gnu/inet/nntp/Range.java \
	gnu/inet/nntp/StatusResponse.java \
	gnu/inet/pop3/POP3Connection.java \
	gnu/inet/smtp/Parameter.java \
	gnu/inet/smtp/ParameterList.java \
	gnu/inet/smtp/SMTPConnection.java \
	gnu/inet/util/BASE64.java \
	gnu/inet/util/CRLFInputStream.java \
	gnu/inet/util/CRLFOutputStream.java \
	gnu/inet/util/EmptyX509TrustManager.java \
	gnu/inet/util/GetLocalHostAction.java \
	gnu/inet/util/LineInputStream.java \
	gnu/inet/util/MessageInputStream.java \
	gnu/inet/util/MessageOutputStream.java \
	gnu/inet/util/SaslCallbackHandler.java \
	gnu/inet/util/SaslCramMD5.java \
	gnu/inet/util/SaslInputStream.java \
	gnu/inet/util/SaslLogin.java \
	gnu/inet/util/SaslOutputStream.java \
	gnu/inet/util/SaslPlain.java \
	gnu/inet/util/TraceLevel.java 
