#
# servlet extension library component.
#
# Copyright (c) 1999
#      Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.
#
# Class libraries are not currently part of the integrate Make cycle.
#

CLASSFILE = servlet.jar
SRCDIRS = javax

jrelib_DATA = $(CLASSFILE)

servlet_jar_SRCS = \
	$(javax_servlet_SRCS) \
	$(javax_servlet_http_SRCS)
javax_servlet_SRCS = \
	javax/servlet/GenericServlet.java \
	javax/servlet/Servlet.java \
	javax/servlet/ServletConfig.java \
	javax/servlet/ServletContext.java \
	javax/servlet/ServletException.java \
	javax/servlet/ServletInputStream.java \
	javax/servlet/ServletOutputStream.java \
	javax/servlet/ServletRequest.java \
	javax/servlet/ServletResponse.java \
	javax/servlet/SingleThreadModel.java \
	javax/servlet/UnavailableException.java
javax_servlet_http_SRCS = \
	javax/servlet/http/Cookie.java \
	javax/servlet/http/HttpServlet.java \
	javax/servlet/http/HttpServletRequest.java \
	javax/servlet/http/HttpServletResponse.java \
	javax/servlet/http/HttpSession.java \
	javax/servlet/http/HttpSessionBindingEvent.java \
	javax/servlet/http/HttpSessionBindingListener.java \
	javax/servlet/http/HttpSessionContext.java \
	javax/servlet/http/HttpUtils.java

## Do not delete the previous blank line; update-class-list depends on it

EXTRA_DIST = $(servlet_jar_SRCS)

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
compile-classes $(LIBDIR)/stamp classes: $(rebuildLib) $(servlet_jar_SRCS)
	@echo $(SHELL) $(rebuildLib)
	@CPATH=$(Klasses_jar) $(SHELL) $(rebuildLib) $(servlet_jar_SRCS)
	@echo timestamp > $(LIBDIR)/stamp

.PHONY: jar-classes
jar-classes $(LIBDIR)/servlet.jar: $(LIBDIR)/stamp
	rm -f $(LIBDIR)/$(CLASSFILE)
	(cd $(LIBDIR) && $(ZIP) -r $(CLASSFILE) $(SRCDIRS))

.PHONY: build-classes Klasses
build-classes Klasses: $(LIBDIR)/$(CLASSFILE)
	rm -f $(srcdir)/$(CLASSFILE)
	cp $(LIBDIR)/$(CLASSFILE) $(srcdir)/$(CLASSFILE)
