#
# Javadoc library component.
#
# Copyright (c) 2004
# 	Kaffe.org contributors. All rights reserved. See ChageLog for details.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.

CLASSFILE = gjdoc.jar
SRCDIRS = antlr com gnu

toolslib_DATA = $(CLASSFILE)

antlr_SRCS = \
	antlr/collections/impl/ASTArray.java \
	antlr/collections/impl/BitSet.java \
	antlr/collections/impl/Vector.java \
	antlr/collections/impl/IntRange.java \
	antlr/collections/impl/VectorEnumerator.java \
	antlr/collections/impl/ASTEnumerator.java \
	antlr/collections/AST.java \
	antlr/collections/ASTEnumeration.java \
	antlr/collections/Enumerator.java \
	antlr/ANTLRException.java \
	antlr/ANTLRHashString.java \
	antlr/ASTFactory.java \
	antlr/ASTPair.java \
	antlr/ByteBuffer.java \
	antlr/CharBuffer.java \
	antlr/CharScanner.java \
	antlr/CharStreamException.java \
	antlr/CharStreamIOException.java \
	antlr/CommonToken.java \
	antlr/InputBuffer.java \
	antlr/LexerSharedInputState.java \
	antlr/LLkParser.java \
	antlr/MismatchedCharException.java \
	antlr/MismatchedTokenException.java \
	antlr/NoViableAltException.java \
	antlr/NoViableAltForCharException.java \
	antlr/ParserSharedInputState.java \
	antlr/RecognitionException.java \
	antlr/SemanticException.java \
	antlr/Token.java \
	antlr/TokenBuffer.java \
	antlr/TokenStream.java \
	antlr/TokenStreamException.java \
	antlr/TokenStreamIOException.java \
	antlr/TokenStreamRecognitionException.java \
	antlr/CharFormatter.java \
	antlr/CharQueue.java \
	antlr/ANTLRStringBuffer.java \
	antlr/CommonAST.java \
	antlr/Parser.java \
	antlr/TokenQueue.java \
	antlr/FileLineFormatter.java \
	antlr/BaseAST.java \
	antlr/debug/MessageListener.java \
	antlr/debug/ParserListener.java \
	antlr/debug/ParserMatchListener.java \
	antlr/debug/ParserTokenListener.java \
	antlr/debug/SemanticPredicateListener.java \
	antlr/debug/SyntacticPredicateListener.java \
	antlr/debug/TraceListener.java \
	antlr/debug/ListenerBase.java \
	antlr/debug/TraceEvent.java \
	antlr/debug/SyntacticPredicateEvent.java \
	antlr/debug/SemanticPredicateEvent.java \
	antlr/debug/ParserTokenEvent.java \
	antlr/debug/ParserMatchEvent.java \
	antlr/debug/MessageEvent.java \
	antlr/debug/Event.java \
	antlr/debug/GuessingEvent.java \
	antlr/DefaultFileLineFormatter.java \
	antlr/StringUtils.java \
	antlr/TreeParser.java \
	antlr/ASTNULLType.java \
	antlr/TreeParserSharedInputState.java 

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
	com/sun/javadoc/TypeVariable.java \
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
	gnu/classpath/tools/doclets/htmldoclet/HtmlTagletContext.java \
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
	gnu/classpath/tools/doclets/debugdoclet/DebugDoclet.java \
	gnu/classpath/tools/doclets/DocletOptionPackageWildcard.java \
	gnu/classpath/tools/doclets/InlineTagRenderer.java \
	gnu/classpath/tools/gjdoc/expr/AdditionExpression.java \
	gnu/classpath/tools/gjdoc/expr/AndExpression.java \
	gnu/classpath/tools/gjdoc/expr/BinaryBitwiseExpression.java \
	gnu/classpath/tools/gjdoc/expr/BinaryComputationExpression.java \
	gnu/classpath/tools/gjdoc/expr/BinaryEqualityExpression.java \
	gnu/classpath/tools/gjdoc/expr/BinaryExpression.java \
	gnu/classpath/tools/gjdoc/expr/BinaryLogicalExpression.java \
	gnu/classpath/tools/gjdoc/expr/BinaryRelationExpression.java \
	gnu/classpath/tools/gjdoc/expr/BinaryShiftExpression.java \
	gnu/classpath/tools/gjdoc/expr/BitShiftRightExpression.java \
	gnu/classpath/tools/gjdoc/expr/ConditionalExpression.java \
	gnu/classpath/tools/gjdoc/expr/ConstantBoolean.java \
	gnu/classpath/tools/gjdoc/expr/ConstantByte.java \
	gnu/classpath/tools/gjdoc/expr/ConstantChar.java \
	gnu/classpath/tools/gjdoc/expr/ConstantDouble.java \
	gnu/classpath/tools/gjdoc/expr/ConstantExpression.java \
	gnu/classpath/tools/gjdoc/expr/ConstantFloat.java \
	gnu/classpath/tools/gjdoc/expr/ConstantInteger.java \
	gnu/classpath/tools/gjdoc/expr/ConstantLong.java \
	gnu/classpath/tools/gjdoc/expr/ConstantNull.java \
	gnu/classpath/tools/gjdoc/expr/ConstantShort.java \
	gnu/classpath/tools/gjdoc/expr/ConstantString.java \
	gnu/classpath/tools/gjdoc/expr/Context.java \
	gnu/classpath/tools/gjdoc/expr/DivisionExpression.java \
	gnu/classpath/tools/gjdoc/expr/EqualExpression.java \
	gnu/classpath/tools/gjdoc/expr/EvaluatorEnvironment.java \
	gnu/classpath/tools/gjdoc/expr/Evaluator.java \
	gnu/classpath/tools/gjdoc/expr/ExclusiveOrExpression.java \
	gnu/classpath/tools/gjdoc/expr/Expression.java \
	gnu/classpath/tools/gjdoc/expr/GreaterThanExpression.java \
	gnu/classpath/tools/gjdoc/expr/GreaterThanOrEqualExpression.java \
	gnu/classpath/tools/gjdoc/expr/IdentifierExpression.java \
	gnu/classpath/tools/gjdoc/expr/IllegalExpressionException.java \
	gnu/classpath/tools/gjdoc/expr/InclusiveOrExpression.java \
	gnu/classpath/tools/gjdoc/expr/LessThanExpression.java \
	gnu/classpath/tools/gjdoc/expr/LessThanOrEqualExpression.java \
	gnu/classpath/tools/gjdoc/expr/LogicalAndExpression.java \
	gnu/classpath/tools/gjdoc/expr/LogicalNotExpression.java \
	gnu/classpath/tools/gjdoc/expr/LogicalOrExpression.java \
	gnu/classpath/tools/gjdoc/expr/ModuloExpression.java \
	gnu/classpath/tools/gjdoc/expr/MultiplicationExpression.java \
	gnu/classpath/tools/gjdoc/expr/NegateExpression.java \
	gnu/classpath/tools/gjdoc/expr/NotEqualExpression.java \
	gnu/classpath/tools/gjdoc/expr/NotExpression.java \
	gnu/classpath/tools/gjdoc/expr/ShiftLeftExpression.java \
	gnu/classpath/tools/gjdoc/expr/ShiftRightExpression.java \
	gnu/classpath/tools/gjdoc/expr/SubtractionExpression.java \
	gnu/classpath/tools/gjdoc/expr/TypeCastExpression.java \
	gnu/classpath/tools/gjdoc/expr/Type.java \
	gnu/classpath/tools/gjdoc/expr/UnaryExpression.java \
	gnu/classpath/tools/gjdoc/expr/UnknownIdentifierException.java \
	gnu/classpath/tools/gjdoc/expr/JavaLexer.java \
	gnu/classpath/tools/gjdoc/expr/JavaRecognizer.java \
	gnu/classpath/tools/gjdoc/expr/JavaTokenTypes.java \
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
	gnu/classpath/tools/gjdoc/TypeVariableImpl.java \
	gnu/classpath/tools/gjdoc/WritableType.java \
	gnu/classpath/tools/gjdoc/GjdocPackageDoc.java \
	gnu/classpath/tools/gjdoc/GjdocRootDoc.java \
	gnu/classpath/tools/gjdoc/SourcePositionImpl.java \
	gnu/classpath/tools/gjdoc/InheritDocTagImpl.java \
	gnu/classpath/tools/gjdoc/TagContainer.java \
	gnu/classpath/tools/gjdoc/ValueTagImpl.java \
	gnu/classpath/tools/gjdoc/ClassDocReflectedImpl.java \
	gnu/classpath/tools/taglets/CodeTaglet.java \
	gnu/classpath/tools/taglets/CopyrightTaglet.java \
	gnu/classpath/tools/taglets/AuthorTaglet.java \
	gnu/classpath/tools/taglets/DeprecatedTaglet.java \
	gnu/classpath/tools/taglets/GenericTaglet.java \
	gnu/classpath/tools/taglets/SinceTaglet.java \
	gnu/classpath/tools/taglets/TagletContext.java \
	gnu/classpath/tools/taglets/VersionTaglet.java \
	gnu/classpath/tools/taglets/GnuExtendedTaglet.java \
	gnu/classpath/tools/taglets/ValueTaglet.java \
	gnu/classpath/tools/java2xhtml/Java2xhtml.java \
	gnu/classpath/tools/FileSystemClassLoader.java \
	gnu/classpath/tools/IOToolkit.java \
	gnu/classpath/tools/StringToolkit.java \
	gnu/classpath/tools/MalformedInputEvent.java \
	gnu/classpath/tools/MalformedInputListener.java \
	gnu/classpath/tools/NotifyingInputStreamReader.java 

## Do not delete the previous blank line; update-class-list depends on it

gjdoc_ressources = \
	dtd/gjdoc-alphaindex.dtd \
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
	htmldoclet/gjdochtml-clean-color1.css \
	htmldoclet/gjdochtml-clean-layout.css \
	htmldoclet/gjdochtml-vanilla.css \
	htmldoclet/gjdoc.js \
	htmldoclet/help.xhtml \
	htmldoclet/HtmlDoclet.properties \
	htmldoclet/inherit.png \
	htmldoclet/xhtml11-target10.dtd \
        rng/gjdoc-classdoc.rng \
        rng/gjdoc-common.rng \
        rng/gjdoc-index.rng \
	java.lang-classes-1.2.txt \
	java.lang-classes-1.3.txt \
	java.lang-classes-1.4.txt \
	java.lang-classes-1.5.txt

EXTRA_DIST = \
	$(antlr_SRCS) \
	$(gjdoc_jar_SRCS) \
	$(gjdoc_ressources) \
	gnu/classpath/tools/doclets/xmldoclet/doctranslet/package.html \
	gnu/classpath/tools/gjdoc/expr/java-expression.g

JAR_CMD1 = (cd $(LIBDIR) && $(ZIP) -q -r $(CURDIR)/$@.new $(SRCDIRS))
JAR_CMD2 = (cd $(srcdir) && $(ZIP) -q -DX -r $(CURDIR)/$@.new $(gjdoc_ressources))
JAR_CMD3 = ($(ZIP) -q -DX -r $(CURDIR)/$@.new version.properties)

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
compile-classes $(LIBDIR)/stamp classes: $(rebuildLib) $(antlr_SRCS) $(gjdoc_jar_SRCS) $(gjdoc_jar_GENSOURCES)
	rm -rf $(LIBDIR)
	mkdir $(LIBDIR)
	echo $(SHELL) $(rebuildLib)
	BOOTCLASSPATH=$(top_builddir)/libraries/javalib/rt.jar $(SHELL) $(rebuildLib) $(antlr_SRCS) $(gjdoc_jar_SRCS) $(gjdoc_jar_GENSOURCES)
	echo timestamp > $(LIBDIR)/stamp

.PHONY: jar-classes
jar-classes: $(CLASSFILE)
$(CLASSFILE): $(LIBDIR)/stamp
	rm -f $@ $@.new
	$(JAR_CMD1)
	$(JAR_CMD2)
	$(JAR_CMD3)
	mv $@.new $@

export JIKES top_builddir LIBDIR srcdir
