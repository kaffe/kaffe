#
# Javadoc library component.
#
# Copyright (c) 2004
# 	Kaffe.org contributors. All rights reserved. See ChageLog for details.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.

## taken from top level Makefile.am
JAVA=$(top_builddir)/kaffe/kaffe/kaffe-bin$(EXEEXT)
KAFFELIBRARYPATH=$(top_builddir)/libraries/clib/native/:$(top_builddir)/libraries/clib/io/:$(top_builddir)/libraries/clib/zip/:$(top_builddir)/libraries/clib/nio/
BOOTCLASSPATH=$(top_builddir)/libraries/javalib/rt.jar
JAR=BOOTCLASSPATH=$(BOOTCLASSPATH) KAFFELIBRARYPATH=$(KAFFELIBRARYPATH) $(JAVA) kaffe.tools.jar.Jar

CLASSFILE = gjdoc.jar
SRCDIRS = com gnu

toolslib_DATA = $(CLASSFILE)

gjdoc_jar_SRCS = \
	com/sun/javadoc/ClassDoc.java \
	com/sun/javadoc/ConstructorDoc.java \
	com/sun/javadoc/Doc.java \
	com/sun/javadoc/DocErrorReporter.java \
	com/sun/javadoc/Doclet.java \
	com/sun/javadoc/ExecutableMemberDoc.java \
	com/sun/javadoc/FieldDoc.java \
	com/sun/javadoc/MemberDoc.java \
	com/sun/javadoc/MethodDoc.java \
	com/sun/javadoc/PackageDoc.java \
	com/sun/javadoc/ParamTag.java \
	com/sun/javadoc/Parameter.java \
	com/sun/javadoc/ProgramElementDoc.java \
	com/sun/javadoc/RootDoc.java \
	com/sun/javadoc/SeeTag.java \
	com/sun/javadoc/SerialFieldTag.java \
	com/sun/javadoc/Tag.java \
	com/sun/javadoc/ThrowsTag.java \
	com/sun/javadoc/Type.java \
	com/sun/javadoc/SourcePosition.java \
	com/sun/tools/doclets/Taglet.java \
	gnu/classpath/tools/doclets/xmldoclet/doctranslet/DocTranslet.java \
	gnu/classpath/tools/doclets/xmldoclet/doctranslet/DocTransletConfigurationException.java \
	gnu/classpath/tools/doclets/xmldoclet/doctranslet/DocTransletException.java \
	gnu/classpath/tools/doclets/xmldoclet/doctranslet/JarClassLoader.java \
	gnu/classpath/tools/doclets/xmldoclet/doctranslet/OutputFileInfo.java \
	gnu/classpath/tools/doclets/xmldoclet/doctranslet/DocTransletOptions.java \
	gnu/classpath/tools/doclets/xmldoclet/Driver.java \
	gnu/classpath/tools/doclets/xmldoclet/Driver1_4.java \
	gnu/classpath/tools/doclets/xmldoclet/HtmlRepairer.java \
	gnu/classpath/tools/doclets/xmldoclet/TargetContext.java \
	gnu/classpath/tools/doclets/htmldoclet/CssClass.java \
	gnu/classpath/tools/doclets/htmldoclet/HtmlDoclet.java \
	gnu/classpath/tools/doclets/htmldoclet/HtmlPage.java \
	gnu/classpath/tools/doclets/htmldoclet/ExternalDocSet.java \
	gnu/classpath/tools/doclets/AbstractDoclet.java \
	gnu/classpath/tools/doclets/DocletConfigurationException.java \
	gnu/classpath/tools/doclets/DocletOption.java \
	gnu/classpath/tools/doclets/DocletOptionColonSeparated.java \
	gnu/classpath/tools/doclets/DocletOptionFile.java \
	gnu/classpath/tools/doclets/DocletOptionFlag.java \
	gnu/classpath/tools/doclets/DocletOptionString.java \
	gnu/classpath/tools/doclets/InvalidPackageWildcardException.java \
	gnu/classpath/tools/doclets/PackageGroup.java \
	gnu/classpath/tools/doclets/PackageMatcher.java \
	gnu/classpath/tools/doclets/StandardTaglet.java \
	gnu/classpath/tools/doclets/TagletPrinter.java \
	gnu/classpath/tools/gjdoc/AbstractTagImpl.java \
	gnu/classpath/tools/gjdoc/ArrayCharacterIterator.java \
	gnu/classpath/tools/gjdoc/ClassDocImpl.java \
	gnu/classpath/tools/gjdoc/ClassDocProxy.java \
	gnu/classpath/tools/gjdoc/ConstructorDocImpl.java \
	gnu/classpath/tools/gjdoc/Debug.java \
	gnu/classpath/tools/gjdoc/DirectoryTree.java \
	gnu/classpath/tools/gjdoc/DocImpl.java \
	gnu/classpath/tools/gjdoc/ErrorReporter.java \
	gnu/classpath/tools/gjdoc/ExecutableMemberDocImpl.java \
	gnu/classpath/tools/gjdoc/FieldDocImpl.java \
	gnu/classpath/tools/gjdoc/JavadocWrapper.java \
	gnu/classpath/tools/gjdoc/LinkTagImpl.java \
	gnu/classpath/tools/gjdoc/Main.java \
	gnu/classpath/tools/gjdoc/MemberDocImpl.java \
	gnu/classpath/tools/gjdoc/MethodDocImpl.java \
	gnu/classpath/tools/gjdoc/PackageDocImpl.java \
	gnu/classpath/tools/gjdoc/ParamTagImpl.java \
	gnu/classpath/tools/gjdoc/ParameterImpl.java \
	gnu/classpath/tools/gjdoc/ParseException.java \
	gnu/classpath/tools/gjdoc/Parser.java \
	gnu/classpath/tools/gjdoc/ProgramElementDocImpl.java \
	gnu/classpath/tools/gjdoc/RootDocImpl.java \
	gnu/classpath/tools/gjdoc/SeeTagImpl.java \
	gnu/classpath/tools/gjdoc/SerialFieldTagImpl.java \
	gnu/classpath/tools/gjdoc/TagImpl.java \
	gnu/classpath/tools/gjdoc/TemporaryStore.java \
	gnu/classpath/tools/gjdoc/TextTagImpl.java \
	gnu/classpath/tools/gjdoc/ThrowsTagImpl.java \
	gnu/classpath/tools/gjdoc/Timer.java \
	gnu/classpath/tools/gjdoc/TimerDoclet.java \
	gnu/classpath/tools/gjdoc/TypeImpl.java \
	gnu/classpath/tools/gjdoc/WritableType.java \
	gnu/classpath/tools/gjdoc/GjdocPackageDoc.java \
	gnu/classpath/tools/gjdoc/GjdocRootDoc.java \
	gnu/classpath/tools/gjdoc/SourcePositionImpl.java \
	gnu/classpath/tools/gjdoc/InheritDocTagImpl.java \
	gnu/classpath/tools/gjdoc/TagContainer.java \
	gnu/classpath/tools/taglets/CopyrightTaglet.java \
	gnu/classpath/tools/taglets/AuthorTaglet.java \
	gnu/classpath/tools/taglets/DeprecatedTaglet.java \
	gnu/classpath/tools/taglets/GenericTaglet.java \
	gnu/classpath/tools/taglets/SinceTaglet.java \
	gnu/classpath/tools/taglets/TagletContext.java \
	gnu/classpath/tools/taglets/VersionTaglet.java \
	gnu/classpath/tools/java2xhtml/Java2xhtml.java \
	gnu/classpath/tools/FileSystemClassLoader.java \
	gnu/classpath/tools/IOToolkit.java \
	gnu/classpath/tools/StringToolkit.java \
	gnu/classpath/tools/MalformedInputEvent.java \
	gnu/classpath/tools/MalformedInputListener.java \
	gnu/classpath/tools/NotifyingInputStreamReader.java


## Do not delete the previous blank line; update-class-list depends on it

gjdoc_ressources = \
        dtd/dbcentx.mod \
        dtd/gjdoc.dtd \
        dtd/ent/iso-amsa.ent \
        dtd/ent/iso-amsb.ent \
        dtd/ent/iso-amsc.ent \
        dtd/ent/iso-amsn.ent \
        dtd/ent/iso-amso.ent \
        dtd/ent/iso-amsr.ent \
        dtd/ent/iso-box.ent \
        dtd/ent/iso-cyr1.ent \
        dtd/ent/iso-cyr2.ent \
        dtd/ent/iso-dia.ent \
        dtd/ent/iso-grk1.ent \
        dtd/ent/iso-grk2.ent \
        dtd/ent/iso-grk3.ent \
        dtd/ent/iso-grk4.ent \
        dtd/ent/iso-lat1.ent \
        dtd/ent/iso-lat2.ent \
        dtd/ent/iso-num.ent \
        dtd/ent/iso-pub.ent \
        dtd/ent/iso-tech.ent \
        doctranslets/html/res/gjdoc.js \
        doctranslets/html/res/gjdochtml.css \
        doctranslets/html/res/default_help_en.html \
        doctranslets/html/res/gjdochtml-clean.css \
        doctranslets/html/res/gjdochtml-fixed.css \
        doctranslets/html/res/gjdochtml-sclara.css \
        doctranslets/html/allclasses.xsl \
        doctranslets/html/allpackages.xsl \
        doctranslets/html/classdoc.xsl \
        doctranslets/html/descriptor.xsl \
        doctranslets/html/doctranslet.xsl \
        doctranslets/html/fulltree.xsl \
        doctranslets/html/gjdoc.xsl \
        doctranslets/html/html_common.xsl \
        doctranslets/html/index.xsl \
        doctranslets/html/index_noframes.xsl \
        doctranslets/html/packageclasses.xsl \
        doctranslets/html/packagedoc.xsl \
        doctranslets/html/about.xsl \
        doctranslets/html/alphaindex.xsl \
        doctranslets/html/alphaindex_chunked.xsl \
        doctranslets/html/classdoc-source.xsl \
        doctranslets/html/classdoc-uses.xsl \
        doctranslets/html/deprecated.xsl \
        doctranslets/html/help.xsl \
        doctranslets/html/serialized.xsl \
        doctranslets/gjdoc_common.xsl \
        htmldoclet/HtmlDoclet.properties \
        htmldoclet/gjdochtml-vanilla.css \
        htmldoclet/help.xhtml \
        rng/gjdoc-classdoc.rng \
        rng/gjdoc-common.rng \
        rng/gjdoc-index.rng

EXTRA_DIST = \
	$(toolslib_DATA) \
	$(gjdoc_jar_SRCS) \
	$(gjdoc_ressources)

if HAVE_ZIP
JAR_CMD1 = (dstdir=`pwd`; cd $(LIBDIR) && $(ZIP) -r $$dstdir/$(CLASSFILE) $(SRCDIRS))
JAR_CMD2 = (dstdir=`pwd`; cd $(srcdir) && $(ZIP) -DX -r $$dstdir/$(CLASSFILE) $(gjdoc_ressources))
else
JAR_CMD1 = $(JAR) -cvf $(CLASSFILE) -C $(LIBDIR) $(SRCDIRS)
JAR_CMD2 = $(JAR) -uvf $(CLASSFILE) -C $(srcdir) $(gjdoc_ressources)
endif

LIBDIR = lib

clean-local:
	rm -rf $(LIBDIR)

.PHONY: new-classes
new-classes: $(top_srcdir)/developers/update-class-list
	cd $(srcdir) && $(SHELL) $(top_srcdir)/developers/update-class-list

rebuildLib = $(top_builddir)/libraries/javalib/rebuildLib
Klasses_jar = $(top_srcdir)/libraries/javalib/rt.jar

.PHONY: compile-classes classes
## classes is provided for backward compatibility; use compile-classes
compile-classes $(LIBDIR)/stamp classes: $(rebuildLib) $(gjdoc_jar_SRCS)
	rm -rf $(LIBDIR)
	mkdir $(LIBDIR)
	echo $(SHELL) $(rebuildLib)
	CLASSPATH=$(Klasses_jar) BOOTCLASSPATH=$(top_srcdir)/libraries/javalib/Klasses.jar.bootstrap $(SHELL) $(rebuildLib) $(gjdoc_jar_SRCS)
	echo timestamp > $(LIBDIR)/stamp

.PHONY: jar-classes
jar-classes $(CLASSFILE): $(LIBDIR)/stamp
	rm -f $(CLASSFILE)
	$(JAR_CMD1)
	$(JAR_CMD2)

export JIKES top_builddir LIBDIR srcdir
