#
# Java core library component.
#
# Copyright (c) 1997, 1998, 1999
#      Transvirtual Technologies, Inc.  All rights reserved.
#
# Copyright (c) 2002, 2003
#      Kaffe.org contributors.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.
#
# Class libraries are currently part of the integrate Make cycle. 
# Read FAQ/FAQ.classlibrary-compile for more information on the class library
# build process.
#
# Note: Makefile.am is generated from Makefile.am.in using 
# developers/update-class-list (called from developers/autogen.sh)
#
# Yes, it's slightly insane:
#
#   Makefile.am.in -> Makefile.am -> Makefile.in -> Makefile

SRCDIRS = gnu java javax kaffe org

## Pick source of java math implementation
if COND_PURE_JAVA_MATH
  JAVA_MATH_FILES = pure_java_math.files
else
  JAVA_MATH_FILES = native_java_math.files
endif

## taken from top level Makefile.am
JAVA=$(top_builddir)/kaffe/kaffe/kaffe-bin$(EXEEXT)
KAFFELIBRARYPATH=$(top_builddir)/libraries/clib/native/:$(top_builddir)/libraries/clib/io/:$(top_builddir)/libraries/clib/zip/
BOOTCLASSPATH=$(LIBDIR):$(srcdir)/Klasses.jar.bootstrap
JAR=BOOTCLASSPATH=$(BOOTCLASSPATH) KAFFELIBRARYPATH=$(KAFFELIBRARYPATH) $(JAVA) kaffe.tools.jar.Jar

jrelib_DATA = rt.jar
toolslib_DATA = kjc.jar

if HAVE_DNSJAVA
DNSJAVA_InetAddressImpl = gnu/java/net/DNSJavaInetAddressImpl.java
else
DNSJAVA_InetAddressImpl =
endif

Klasses_jar_SRCS = \
	$(gnu_classpath_SRCS) \
	$(gnu_inet_ftp_SRCS) \
	$(gnu_inet_util_SRCS) \
	$(gnu_java_awt_SRCS) \
	$(gnu_java_beans_SRCS) \
	$(gnu_java_beans_editors_SRCS) \
	$(gnu_java_io_SRCS) \
	$(gnu_java_io_decode_SRCS) \
	$(gnu_java_io_encode_SRCS) \
	$(gnu_java_lang_SRCS) \
	$(gnu_java_lang_reflect_SRCS) \
	$(gnu_java_locale_SRCS) \
	$(gnu_java_net_SRCS) \
	$(gnu_java_net_content_text_SRCS) \
	$(gnu_java_net_protocol_file_SRCS) \
	$(gnu_java_net_protocol_http_SRCS) \
	$(gnu_java_net_protocol_jar_SRCS) \
	$(gnu_java_nio_SRCS) \
	$(gnu_java_nio_charset_SRCS) \
	$(gnu_java_rmi_SRCS) \
	$(gnu_java_rmi_dgc_SRCS) \
	$(gnu_java_rmi_registry_SRCS) \
	$(gnu_java_rmi_rmic_SRCS) \
	$(gnu_java_rmi_server_SRCS) \
	$(gnu_java_security_SRCS) \
	$(gnu_java_security_der_SRCS) \
	$(gnu_java_security_provider_SRCS) \
	$(gnu_java_security_x509_SRCS) \
	$(gnu_java_text_SRCS) \
	$(gnu_java_util_SRCS) \
	$(gnu_java_util_prefs_SRCS) \
	$(gnu_javax_rmi_SRCS) \
	$(gnu_javax_rmi_CORBA_SRCS) \
	$(gnu_regexp_SRCS) \
	$(gnu_xml_aelfred2_SRCS) \
	$(gnu_xml_dom_SRCS) \
	$(gnu_xml_pipeline_SRCS) \
	$(gnu_xml_util_SRCS) \
	$(java_applet_SRCS) \
	$(java_awt_SRCS) \
	$(java_awt_color_SRCS) \
	$(java_awt_datatransfer_SRCS) \
	$(java_awt_dnd_SRCS) \
	$(java_awt_dnd_peer_SRCS) \
	$(java_awt_event_SRCS) \
	$(java_awt_font_SRCS) \
	$(java_awt_geom_SRCS) \
	$(java_awt_im_SRCS) \
	$(java_awt_image_SRCS) \
	$(java_awt_image_renderable_SRCS) \
	$(java_awt_peer_SRCS) \
	$(java_awt_print_SRCS) \
	$(java_beans_SRCS) \
	$(java_beans_beancontext_SRCS) \
	$(java_io_SRCS) \
	$(java_lang_SRCS) \
	$(java_lang_ref_SRCS) \
	$(java_lang_reflect_SRCS) \
	$(java_math_SRCS) \
	$(java_net_SRCS) \
	$(java_nio_SRCS) \
	$(java_nio_channels_SRCS) \
	$(java_nio_channels_spi_SRCS) \
	$(java_nio_charset_SRCS) \
	$(java_nio_charset_spi_SRCS) \
	$(java_rmi_SRCS) \
	$(java_rmi_activation_SRCS) \
	$(java_rmi_dgc_SRCS) \
	$(java_rmi_registry_SRCS) \
	$(java_rmi_server_SRCS) \
	$(java_security_SRCS) \
	$(java_security_acl_SRCS) \
	$(java_security_cert_SRCS) \
	$(java_security_interfaces_SRCS) \
	$(java_security_spec_SRCS) \
	$(java_sql_SRCS) \
	$(java_text_SRCS) \
	$(java_util_SRCS) \
	$(java_util_jar_SRCS) \
	$(java_util_logging_SRCS) \
	$(java_util_mime_SRCS) \
	$(java_util_prefs_SRCS) \
	$(java_util_regex_SRCS) \
	$(java_util_zip_SRCS) \
	$(javax_accessibility_SRCS) \
	$(javax_naming_SRCS) \
	$(javax_naming_directory_SRCS) \
	$(javax_naming_event_SRCS) \
	$(javax_naming_ldap_SRCS) \
	$(javax_naming_spi_SRCS) \
	$(javax_net_SRCS) \
	$(javax_net_ssl_SRCS) \
	$(javax_rmi_SRCS) \
	$(javax_rmi_CORBA_SRCS) \
	$(javax_security_auth_x500_SRCS) \
	$(javax_security_cert_SRCS) \
	$(javax_sound_midi_SRCS) \
	$(javax_sound_midi_spi_SRCS) \
	$(javax_sound_sampled_SRCS) \
	$(javax_sound_sampled_spi_SRCS) \
	$(javax_sql_SRCS) \
	$(javax_swing_event_SRCS) \
	$(javax_swing_text_SRCS) \
	$(javax_transaction_SRCS) \
	$(javax_transaction_xa_SRCS) \
	$(javax_xml_parsers_SRCS) \
	$(javax_xml_transform_SRCS) \
	$(javax_xml_transform_dom_SRCS) \
	$(javax_xml_transform_sax_SRCS) \
	$(javax_xml_transform_stream_SRCS) \
	$(kaffe_applet_SRCS) \
	$(kaffe_awt_SRCS) \
	$(kaffe_beans_editors_SRCS) \
	$(kaffe_io_SRCS) \
	$(kaffe_jar_SRCS) \
	$(kaffe_lang_SRCS) \
	$(kaffe_management_SRCS) \
	$(kaffe_security_SRCS) \
	$(kaffe_security_provider_SRCS) \
	$(kaffe_tools_jar_SRCS) \
	$(kaffe_tools_native2ascii_SRCS) \
	$(kaffe_tools_serialver_SRCS) \
	$(kaffe_util_SRCS) \
	$(kaffe_util_locale_SRCS) \
	$(kaffe_util_log_SRCS) \
	$(kaffe_util_zip_SRCS) \
	$(org_tritonus_core_SRCS) \
	$(org_tritonus_lowlevel_alsa_SRCS) \
	$(org_tritonus_lowlevel_esd_SRCS) \
	$(org_tritonus_midi_device_alsa_SRCS) \
	$(org_tritonus_midi_file_SRCS) \
	$(org_tritonus_sampled_convert_SRCS) \
	$(org_tritonus_sampled_file_SRCS) \
	$(org_tritonus_sampled_mixer_alsa_SRCS) \
	$(org_tritonus_sampled_mixer_esd_SRCS) \
	$(org_tritonus_share_SRCS) \
	$(org_tritonus_share_midi_SRCS) \
	$(org_tritonus_share_sampled_SRCS) \
	$(org_tritonus_share_sampled_convert_SRCS) \
	$(org_tritonus_share_sampled_file_SRCS) \
	$(org_tritonus_share_sampled_mixer_SRCS) \
	$(org_w3c_dom_SRCS) \
	$(org_w3c_dom_css_SRCS) \
	$(org_w3c_dom_events_SRCS) \
	$(org_w3c_dom_html_SRCS) \
	$(org_w3c_dom_ranges_SRCS) \
	$(org_w3c_dom_stylesheets_SRCS) \
	$(org_w3c_dom_traversal_SRCS) \
	$(org_w3c_dom_views_SRCS) \
	$(org_xml_sax_SRCS) \
	$(org_xml_sax_ext_SRCS) \
	$(org_xml_sax_helpers_SRCS)
gnu_classpath_SRCS = \
	gnu/classpath/Configuration.java \
	gnu/classpath/RawData.java
gnu_inet_ftp_SRCS = \
	gnu/inet/ftp/ActiveModeDTP.java \
	gnu/inet/ftp/BlockInputStream.java \
	gnu/inet/ftp/BlockOutputStream.java \
	gnu/inet/ftp/CompressedInputStream.java \
	gnu/inet/ftp/CompressedOutputStream.java \
	gnu/inet/ftp/DTP.java \
	gnu/inet/ftp/DTPInputStream.java \
	gnu/inet/ftp/DTPOutputStream.java \
	gnu/inet/ftp/FTPConnection.java \
	gnu/inet/ftp/FTPException.java \
	gnu/inet/ftp/FTPResponse.java \
	gnu/inet/ftp/FTPURLConnection.java \
	gnu/inet/ftp/FTPURLStreamHandler.java \
	gnu/inet/ftp/Handler.java \
	gnu/inet/ftp/PassiveModeDTP.java \
	gnu/inet/ftp/StreamInputStream.java \
	gnu/inet/ftp/StreamOutputStream.java
gnu_inet_util_SRCS = \
	gnu/inet/util/CRLFInputStream.java \
	gnu/inet/util/LineInputStream.java
gnu_java_awt_SRCS = \
	gnu/java/awt/BitMaskExtent.java \
	gnu/java/awt/Buffers.java \
	gnu/java/awt/ComponentDataBlitOp.java \
	gnu/java/awt/EventModifier.java
gnu_java_beans_SRCS = \
	gnu/java/beans/BeanInfoEmbryo.java \
	gnu/java/beans/ExplicitBeanInfo.java \
	gnu/java/beans/IntrospectionIncubator.java
gnu_java_beans_editors_SRCS = \
	gnu/java/beans/editors/ColorEditor.java \
	gnu/java/beans/editors/FontEditor.java \
	gnu/java/beans/editors/NativeBooleanEditor.java \
	gnu/java/beans/editors/NativeByteEditor.java \
	gnu/java/beans/editors/NativeDoubleEditor.java \
	gnu/java/beans/editors/NativeFloatEditor.java \
	gnu/java/beans/editors/NativeIntEditor.java \
	gnu/java/beans/editors/NativeLongEditor.java \
	gnu/java/beans/editors/NativeShortEditor.java \
	gnu/java/beans/editors/StringEditor.java
gnu_java_io_SRCS = \
	gnu/java/io/ASN1ParsingException.java \
	gnu/java/io/ClassLoaderObjectInputStream.java \
	gnu/java/io/EncodingManager.java \
	gnu/java/io/NullOutputStream.java \
	gnu/java/io/ObjectIdentityWrapper.java \
	gnu/java/io/PlatformHelper.java
gnu_java_io_decode_SRCS = \
	gnu/java/io/decode/Decoder.java \
	gnu/java/io/decode/KaffeDecoder.java
gnu_java_io_encode_SRCS = \
	gnu/java/io/encode/Encoder.java \
	gnu/java/io/encode/KaffeEncoder.java
gnu_java_lang_SRCS = \
	gnu/java/lang/ArrayHelper.java \
	gnu/java/lang/CharData.java \
	gnu/java/lang/ClassHelper.java
gnu_java_lang_reflect_SRCS = \
	gnu/java/lang/reflect/TypeSignature.java
gnu_java_locale_SRCS = \
	gnu/java/locale/Calendar.java \
	gnu/java/locale/Calendar_de.java \
	gnu/java/locale/Calendar_en.java \
	gnu/java/locale/Calendar_nl.java \
	gnu/java/locale/LocaleInformation.java \
	gnu/java/locale/LocaleInformation_af_ZA.java \
	gnu/java/locale/LocaleInformation_ar_AE.java \
	gnu/java/locale/LocaleInformation_ar_BH.java \
	gnu/java/locale/LocaleInformation_ar_DZ.java \
	gnu/java/locale/LocaleInformation_ar_EG.java \
	gnu/java/locale/LocaleInformation_ar_IN.java \
	gnu/java/locale/LocaleInformation_ar_IQ.java \
	gnu/java/locale/LocaleInformation_ar_JO.java \
	gnu/java/locale/LocaleInformation_ar_KW.java \
	gnu/java/locale/LocaleInformation_ar_LB.java \
	gnu/java/locale/LocaleInformation_ar_LY.java \
	gnu/java/locale/LocaleInformation_ar_MA.java \
	gnu/java/locale/LocaleInformation_ar_OM.java \
	gnu/java/locale/LocaleInformation_ar_QA.java \
	gnu/java/locale/LocaleInformation_ar_SD.java \
	gnu/java/locale/LocaleInformation_ar_SY.java \
	gnu/java/locale/LocaleInformation_ar_TN.java \
	gnu/java/locale/LocaleInformation_ar_YE.java \
	gnu/java/locale/LocaleInformation_be_BY.java \
	gnu/java/locale/LocaleInformation_bn_IN.java \
	gnu/java/locale/LocaleInformation_br_FR.java \
	gnu/java/locale/LocaleInformation_bs_BA.java \
	gnu/java/locale/LocaleInformation_ca_ES.java \
	gnu/java/locale/LocaleInformation_cs_CZ.java \
	gnu/java/locale/LocaleInformation_cy_GB.java \
	gnu/java/locale/LocaleInformation_da_DK.java \
	gnu/java/locale/LocaleInformation_de.java \
	gnu/java/locale/LocaleInformation_de_AT.java \
	gnu/java/locale/LocaleInformation_de_BE.java \
	gnu/java/locale/LocaleInformation_de_CH.java \
	gnu/java/locale/LocaleInformation_de_DE.java \
	gnu/java/locale/LocaleInformation_de_LU.java \
	gnu/java/locale/LocaleInformation_el_GR.java \
	gnu/java/locale/LocaleInformation_en.java \
	gnu/java/locale/LocaleInformation_en_AU.java \
	gnu/java/locale/LocaleInformation_en_BW.java \
	gnu/java/locale/LocaleInformation_en_CA.java \
	gnu/java/locale/LocaleInformation_en_DK.java \
	gnu/java/locale/LocaleInformation_en_GB.java \
	gnu/java/locale/LocaleInformation_en_HK.java \
	gnu/java/locale/LocaleInformation_en_IE.java \
	gnu/java/locale/LocaleInformation_en_IN.java \
	gnu/java/locale/LocaleInformation_en_NZ.java \
	gnu/java/locale/LocaleInformation_en_PH.java \
	gnu/java/locale/LocaleInformation_en_SG.java \
	gnu/java/locale/LocaleInformation_en_US.java \
	gnu/java/locale/LocaleInformation_en_ZA.java \
	gnu/java/locale/LocaleInformation_en_ZW.java \
	gnu/java/locale/LocaleInformation_es_AR.java \
	gnu/java/locale/LocaleInformation_es_BO.java \
	gnu/java/locale/LocaleInformation_es_CL.java \
	gnu/java/locale/LocaleInformation_es_CO.java \
	gnu/java/locale/LocaleInformation_es_CR.java \
	gnu/java/locale/LocaleInformation_es_DO.java \
	gnu/java/locale/LocaleInformation_es_EC.java \
	gnu/java/locale/LocaleInformation_es_ES.java \
	gnu/java/locale/LocaleInformation_es_GT.java \
	gnu/java/locale/LocaleInformation_es_HN.java \
	gnu/java/locale/LocaleInformation_es_MX.java \
	gnu/java/locale/LocaleInformation_es_NI.java \
	gnu/java/locale/LocaleInformation_es_PA.java \
	gnu/java/locale/LocaleInformation_es_PE.java \
	gnu/java/locale/LocaleInformation_es_PR.java \
	gnu/java/locale/LocaleInformation_es_PY.java \
	gnu/java/locale/LocaleInformation_es_SV.java \
	gnu/java/locale/LocaleInformation_es_US.java \
	gnu/java/locale/LocaleInformation_es_UY.java \
	gnu/java/locale/LocaleInformation_es_VE.java \
	gnu/java/locale/LocaleInformation_et_EE.java \
	gnu/java/locale/LocaleInformation_eu_ES.java \
	gnu/java/locale/LocaleInformation_fa_IR.java \
	gnu/java/locale/LocaleInformation_fi_FI.java \
	gnu/java/locale/LocaleInformation_fo_FO.java \
	gnu/java/locale/LocaleInformation_fr_BE.java \
	gnu/java/locale/LocaleInformation_fr_CA.java \
	gnu/java/locale/LocaleInformation_fr_CH.java \
	gnu/java/locale/LocaleInformation_fr_FR.java \
	gnu/java/locale/LocaleInformation_fr_LU.java \
	gnu/java/locale/LocaleInformation_ga_IE.java \
	gnu/java/locale/LocaleInformation_gd_GB.java \
	gnu/java/locale/LocaleInformation_gl_ES.java \
	gnu/java/locale/LocaleInformation_gv_GB.java \
	gnu/java/locale/LocaleInformation_he_IL.java \
	gnu/java/locale/LocaleInformation_hi_IN.java \
	gnu/java/locale/LocaleInformation_hr_HR.java \
	gnu/java/locale/LocaleInformation_hu_HU.java \
	gnu/java/locale/LocaleInformation_id_ID.java \
	gnu/java/locale/LocaleInformation_it_CH.java \
	gnu/java/locale/LocaleInformation_it_IT.java \
	gnu/java/locale/LocaleInformation_iw_IL.java \
	gnu/java/locale/LocaleInformation_ja_JP.java \
	gnu/java/locale/LocaleInformation_ka_GE.java \
	gnu/java/locale/LocaleInformation_kl_GL.java \
	gnu/java/locale/LocaleInformation_ko_KR.java \
	gnu/java/locale/LocaleInformation_kw_GB.java \
	gnu/java/locale/LocaleInformation_lt_LT.java \
	gnu/java/locale/LocaleInformation_lv_LV.java \
	gnu/java/locale/LocaleInformation_mi_NZ.java \
	gnu/java/locale/LocaleInformation_mk_MK.java \
	gnu/java/locale/LocaleInformation_mr_IN.java \
	gnu/java/locale/LocaleInformation_mt_MT.java \
	gnu/java/locale/LocaleInformation_nl.java \
	gnu/java/locale/LocaleInformation_nl_BE.java \
	gnu/java/locale/LocaleInformation_nl_NL.java \
	gnu/java/locale/LocaleInformation_nn_NO.java \
	gnu/java/locale/LocaleInformation_no_NO.java \
	gnu/java/locale/LocaleInformation_oc_FR.java \
	gnu/java/locale/LocaleInformation_pl_PL.java \
	gnu/java/locale/LocaleInformation_pt_BR.java \
	gnu/java/locale/LocaleInformation_pt_PT.java \
	gnu/java/locale/LocaleInformation_ro_RO.java \
	gnu/java/locale/LocaleInformation_ru_RU.java \
	gnu/java/locale/LocaleInformation_ru_UA.java \
	gnu/java/locale/LocaleInformation_se_NO.java \
	gnu/java/locale/LocaleInformation_sk_SK.java \
	gnu/java/locale/LocaleInformation_sl_SI.java \
	gnu/java/locale/LocaleInformation_sq_AL.java \
	gnu/java/locale/LocaleInformation_sr_YU.java \
	gnu/java/locale/LocaleInformation_sv_FI.java \
	gnu/java/locale/LocaleInformation_sv_SE.java \
	gnu/java/locale/LocaleInformation_ta_IN.java \
	gnu/java/locale/LocaleInformation_te_IN.java \
	gnu/java/locale/LocaleInformation_tg_TJ.java \
	gnu/java/locale/LocaleInformation_tl_PH.java \
	gnu/java/locale/LocaleInformation_tr_TR.java \
	gnu/java/locale/LocaleInformation_uk_UA.java \
	gnu/java/locale/LocaleInformation_ur_PK.java \
	gnu/java/locale/LocaleInformation_uz_UZ.java \
	gnu/java/locale/LocaleInformation_vi_VN.java \
	gnu/java/locale/LocaleInformation_yi_US.java \
	gnu/java/locale/LocaleInformation_zh_CN.java \
	gnu/java/locale/LocaleInformation_zh_HK.java \
	gnu/java/locale/LocaleInformation_zh_SG.java \
	gnu/java/locale/LocaleInformation_zh_TW.java
gnu_java_net_SRCS = \
	gnu/java/net/DNSJavaInetAddressImpl.java \
	gnu/java/net/HeaderFieldHelper.java \
	gnu/java/net/InetAddressImpl.java \
	gnu/java/net/PlainDatagramSocketImpl.java \
	gnu/java/net/PlainSocketImpl.java \
	gnu/java/net/SysInetAddressImpl.java \
	gnu/java/net/URLParseError.java
gnu_java_net_content_text_SRCS = \
	gnu/java/net/content/text/plain.java
gnu_java_net_protocol_file_SRCS = \
	gnu/java/net/protocol/file/Connection.java \
	gnu/java/net/protocol/file/Handler.java
gnu_java_net_protocol_http_SRCS = \
	gnu/java/net/protocol/http/Connection.java \
	gnu/java/net/protocol/http/Handler.java
gnu_java_net_protocol_jar_SRCS = \
	gnu/java/net/protocol/jar/Connection.java \
	gnu/java/net/protocol/jar/Handler.java
gnu_java_nio_SRCS = \
	gnu/java/nio/ChannelInputStream.java \
	gnu/java/nio/ChannelOutputStream.java \
	gnu/java/nio/DatagramChannelImpl.java \
	gnu/java/nio/DatagramChannelSelectionKey.java \
	gnu/java/nio/FileLockImpl.java \
	gnu/java/nio/InputStreamChannel.java \
	gnu/java/nio/NIOConstants.java \
	gnu/java/nio/NIODatagramSocket.java \
	gnu/java/nio/NIOServerSocket.java \
	gnu/java/nio/NIOSocket.java \
	gnu/java/nio/OutputStreamChannel.java \
	gnu/java/nio/PipeImpl.java \
	gnu/java/nio/SelectionKeyImpl.java \
	gnu/java/nio/SelectorImpl.java \
	gnu/java/nio/SelectorProviderImpl.java \
	gnu/java/nio/ServerSocketChannelImpl.java \
	gnu/java/nio/ServerSocketChannelSelectionKey.java \
	gnu/java/nio/SocketChannelImpl.java \
	gnu/java/nio/SocketChannelSelectionKey.java
gnu_java_nio_charset_SRCS = \
	gnu/java/nio/charset/ISO_8859_1.java \
	gnu/java/nio/charset/Provider.java \
	gnu/java/nio/charset/US_ASCII.java \
	gnu/java/nio/charset/UTF_16.java \
	gnu/java/nio/charset/UTF_16BE.java \
	gnu/java/nio/charset/UTF_16Decoder.java \
	gnu/java/nio/charset/UTF_16Encoder.java \
	gnu/java/nio/charset/UTF_16LE.java \
	gnu/java/nio/charset/UTF_8.java
gnu_java_rmi_SRCS = \
	gnu/java/rmi/RMIMarshalledObjectInputStream.java \
	gnu/java/rmi/RMIMarshalledObjectOutputStream.java
gnu_java_rmi_dgc_SRCS = \
	gnu/java/rmi/dgc/DGCImpl.java \
	gnu/java/rmi/dgc/DGCImpl_Skel.java \
	gnu/java/rmi/dgc/DGCImpl_Stub.java
gnu_java_rmi_registry_SRCS = \
	gnu/java/rmi/registry/RegistryImpl.java \
	gnu/java/rmi/registry/RegistryImpl_Skel.java \
	gnu/java/rmi/registry/RegistryImpl_Stub.java
gnu_java_rmi_rmic_SRCS = \
	gnu/java/rmi/rmic/Compile_gcj.java \
	gnu/java/rmi/rmic/Compile_jikes.java \
	gnu/java/rmi/rmic/Compile_kjc.java \
	gnu/java/rmi/rmic/Compiler.java \
	gnu/java/rmi/rmic/CompilerProcess.java \
	gnu/java/rmi/rmic/RMIC.java \
	gnu/java/rmi/rmic/RMICException.java \
	gnu/java/rmi/rmic/TabbedWriter.java
gnu_java_rmi_server_SRCS = \
	gnu/java/rmi/server/ConnectionRunnerPool.java \
	gnu/java/rmi/server/ProtocolConstants.java \
	gnu/java/rmi/server/RMIDefaultSocketFactory.java \
	gnu/java/rmi/server/RMIHashes.java \
	gnu/java/rmi/server/RMIObjectInputStream.java \
	gnu/java/rmi/server/RMIObjectOutputStream.java \
	gnu/java/rmi/server/RMIVoidValue.java \
	gnu/java/rmi/server/UnicastConnection.java \
	gnu/java/rmi/server/UnicastConnectionManager.java \
	gnu/java/rmi/server/UnicastRef.java \
	gnu/java/rmi/server/UnicastRemoteCall.java \
	gnu/java/rmi/server/UnicastRemoteStub.java \
	gnu/java/rmi/server/UnicastServer.java \
	gnu/java/rmi/server/UnicastServerRef.java
gnu_java_security_SRCS = \
	gnu/java/security/Engine.java \
	gnu/java/security/OID.java \
	gnu/java/security/PolicyFile.java
gnu_java_security_der_SRCS = \
	gnu/java/security/der/BitString.java \
	gnu/java/security/der/DER.java \
	gnu/java/security/der/DEREncodingException.java \
	gnu/java/security/der/DERReader.java \
	gnu/java/security/der/DERValue.java \
	gnu/java/security/der/DERWriter.java
gnu_java_security_provider_SRCS = \
	gnu/java/security/provider/DefaultPolicy.java
gnu_java_security_x509_SRCS = \
	gnu/java/security/x509/X500DistinguishedName.java
gnu_java_text_SRCS = \
	gnu/java/text/BaseBreakIterator.java \
	gnu/java/text/CharacterBreakIterator.java \
	gnu/java/text/LineBreakIterator.java \
	gnu/java/text/SentenceBreakIterator.java \
	gnu/java/text/WordBreakIterator.java
gnu_java_util_SRCS = \
	gnu/java/util/EmptyEnumeration.java
gnu_java_util_prefs_SRCS = \
	gnu/java/util/prefs/FileBasedFactory.java \
	gnu/java/util/prefs/MemoryBasedFactory.java \
	gnu/java/util/prefs/MemoryBasedPreferences.java \
	gnu/java/util/prefs/NodeReader.java \
	gnu/java/util/prefs/NodeWriter.java
gnu_javax_rmi_SRCS = \
	gnu/javax/rmi/PortableServer.java
gnu_javax_rmi_CORBA_SRCS = \
	gnu/javax/rmi/CORBA/DelegateFactory.java \
	gnu/javax/rmi/CORBA/GetDelegateInstanceException.java \
	gnu/javax/rmi/CORBA/PortableRemoteObjectDelegateImpl.java \
	gnu/javax/rmi/CORBA/StubDelegateImpl.java \
	gnu/javax/rmi/CORBA/UtilDelegateImpl.java \
	gnu/javax/rmi/CORBA/ValueHandlerImpl.java
gnu_regexp_SRCS = \
	gnu/regexp/CharIndexed.java \
	gnu/regexp/CharIndexedCharArray.java \
	gnu/regexp/CharIndexedInputStream.java \
	gnu/regexp/CharIndexedReader.java \
	gnu/regexp/CharIndexedString.java \
	gnu/regexp/CharIndexedStringBuffer.java \
	gnu/regexp/RE.java \
	gnu/regexp/REException.java \
	gnu/regexp/REFilterInputStream.java \
	gnu/regexp/REFilterReader.java \
	gnu/regexp/REMatch.java \
	gnu/regexp/REMatchEnumeration.java \
	gnu/regexp/RESyntax.java \
	gnu/regexp/REToken.java \
	gnu/regexp/RETokenAny.java \
	gnu/regexp/RETokenBackRef.java \
	gnu/regexp/RETokenChar.java \
	gnu/regexp/RETokenEnd.java \
	gnu/regexp/RETokenEndSub.java \
	gnu/regexp/RETokenLookAhead.java \
	gnu/regexp/RETokenOneOf.java \
	gnu/regexp/RETokenPOSIX.java \
	gnu/regexp/RETokenRange.java \
	gnu/regexp/RETokenRepeated.java \
	gnu/regexp/RETokenStart.java \
	gnu/regexp/RETokenWordBoundary.java \
	gnu/regexp/UncheckedRE.java
gnu_xml_aelfred2_SRCS = \
	gnu/xml/aelfred2/JAXPFactory.java \
	gnu/xml/aelfred2/SAXDriver.java \
	gnu/xml/aelfred2/XmlParser.java \
	gnu/xml/aelfred2/XmlReader.java
gnu_xml_dom_SRCS = \
	gnu/xml/dom/Consumer.java \
	gnu/xml/dom/DomAttr.java \
	gnu/xml/dom/DomCDATA.java \
	gnu/xml/dom/DomCharacterData.java \
	gnu/xml/dom/DomComment.java \
	gnu/xml/dom/DomDoctype.java \
	gnu/xml/dom/DomDocument.java \
	gnu/xml/dom/DomElement.java \
	gnu/xml/dom/DomEntity.java \
	gnu/xml/dom/DomEntityReference.java \
	gnu/xml/dom/DomEvent.java \
	gnu/xml/dom/DomEx.java \
	gnu/xml/dom/DomExtern.java \
	gnu/xml/dom/DomFragment.java \
	gnu/xml/dom/DomImpl.java \
	gnu/xml/dom/DomIterator.java \
	gnu/xml/dom/DomNamedNodeMap.java \
	gnu/xml/dom/DomNode.java \
	gnu/xml/dom/DomNotation.java \
	gnu/xml/dom/DomNsNode.java \
	gnu/xml/dom/DomPI.java \
	gnu/xml/dom/DomText.java \
	gnu/xml/dom/JAXPFactory.java
gnu_xml_pipeline_SRCS = \
	gnu/xml/pipeline/CallFilter.java \
	gnu/xml/pipeline/DomConsumer.java \
	gnu/xml/pipeline/EventConsumer.java \
	gnu/xml/pipeline/EventFilter.java \
	gnu/xml/pipeline/LinkFilter.java \
	gnu/xml/pipeline/NSFilter.java \
	gnu/xml/pipeline/PipelineFactory.java \
	gnu/xml/pipeline/TeeConsumer.java \
	gnu/xml/pipeline/TextConsumer.java \
	gnu/xml/pipeline/ValidationConsumer.java \
	gnu/xml/pipeline/WellFormednessFilter.java \
	gnu/xml/pipeline/XIncludeFilter.java \
	gnu/xml/pipeline/XsltFilter.java
gnu_xml_util_SRCS = \
	gnu/xml/util/DoParse.java \
	gnu/xml/util/DomParser.java \
	gnu/xml/util/Resolver.java \
	gnu/xml/util/SAXNullTransformerFactory.java \
	gnu/xml/util/XCat.java \
	gnu/xml/util/XHTMLWriter.java \
	gnu/xml/util/XMLWriter.java
java_applet_SRCS = \
	java/applet/Applet.java \
	java/applet/AppletContext.java \
	java/applet/AppletStub.java \
	java/applet/AudioClip.java
java_awt_SRCS = \
	java/awt/AWTError.java \
	java/awt/AWTEvent.java \
	java/awt/AWTEventMulticaster.java \
	java/awt/AWTException.java \
	java/awt/AWTPermission.java \
	java/awt/ActionEvt.java \
	java/awt/ActiveEvent.java \
	java/awt/Adjustable.java \
	java/awt/AdjustmentEvt.java \
	java/awt/AlphaComposite.java \
	java/awt/AttributeValue.java \
	java/awt/BarMenu.java \
	java/awt/BasicStroke.java \
	java/awt/BorderLayout.java \
	java/awt/BufferCapabilities.java \
	java/awt/Button.java \
	java/awt/Canvas.java \
	java/awt/CardLayout.java \
	java/awt/Checkbox.java \
	java/awt/CheckboxGroup.java \
	java/awt/CheckboxMenuItem.java \
	java/awt/Choice.java \
	java/awt/ClassAnalyzer.java \
	java/awt/ClassProperties.java \
	java/awt/Color.java \
	java/awt/ColorPaintContext.java \
	java/awt/Component.java \
	java/awt/ComponentEvt.java \
	java/awt/ComponentOrientation.java \
	java/awt/Composite.java \
	java/awt/CompositeContext.java \
	java/awt/Container.java \
	java/awt/ContainerEvt.java \
	java/awt/Cursor.java \
	java/awt/DefKeyFilter.java \
	java/awt/Defaults.java \
	java/awt/Dialog.java \
	java/awt/Dimension.java \
	java/awt/DisplayMode.java \
	java/awt/Event.java \
	java/awt/EventDispatchThread.java \
	java/awt/EventQueue.java \
	java/awt/FileDialog.java \
	java/awt/FlowLayout.java \
	java/awt/FocusEvt.java \
	java/awt/FocusTraversalPolicy.java \
	java/awt/Font.java \
	java/awt/FontFormatException.java \
	java/awt/FontMetrics.java \
	java/awt/Frame.java \
	java/awt/GradientPaint.java \
	java/awt/Graphics.java \
	java/awt/Graphics2D.java \
	java/awt/GraphicsConfigTemplate.java \
	java/awt/GraphicsConfiguration.java \
	java/awt/GraphicsDevice.java \
	java/awt/GraphicsEnvironment.java \
	java/awt/GraphicsLink.java \
	java/awt/GridBagConstraints.java \
	java/awt/GridBagLayout.java \
	java/awt/GridBagLayoutInfo.java \
	java/awt/GridLayout.java \
	java/awt/HeadlessException.java \
	java/awt/IllegalComponentStateException.java \
	java/awt/Image.java \
	java/awt/ImageCapabilities.java \
	java/awt/ImageFrameLoader.java \
	java/awt/ImageLoader.java \
	java/awt/ImageNativeProducer.java \
	java/awt/Insets.java \
	java/awt/ItemEvt.java \
	java/awt/ItemSelectable.java \
	java/awt/JobAttributes.java \
	java/awt/KeyEventDispatcher.java \
	java/awt/KeyEventPostProcessor.java \
	java/awt/KeyEvt.java \
	java/awt/Label.java \
	java/awt/LayoutManager.java \
	java/awt/LayoutManager2.java \
	java/awt/List.java \
	java/awt/MediaTracker.java \
	java/awt/Menu.java \
	java/awt/MenuBar.java \
	java/awt/MenuComponent.java \
	java/awt/MenuContainer.java \
	java/awt/MenuItem.java \
	java/awt/MenuShortcut.java \
	java/awt/MouseEvt.java \
	java/awt/NativeClipboard.java \
	java/awt/NativeGraphics.java \
	java/awt/NativeSelection.java \
	java/awt/OpaqueComponent.java \
	java/awt/PSGraphics.java \
	java/awt/PSPrintJob.java \
	java/awt/PageAttributes.java \
	java/awt/Paint.java \
	java/awt/PaintContext.java \
	java/awt/PaintEvt.java \
	java/awt/Panel.java \
	java/awt/Point.java \
	java/awt/Polygon.java \
	java/awt/PopupMenu.java \
	java/awt/PopupWindow.java \
	java/awt/PrintGraphics.java \
	java/awt/PrintJob.java \
	java/awt/Rectangle.java \
	java/awt/RenderingHints.java \
	java/awt/Robot.java \
	java/awt/RootWindow.java \
	java/awt/RowCanvas.java \
	java/awt/ScrollPane.java \
	java/awt/ScrollPaneAdjustable.java \
	java/awt/Scrollbar.java \
	java/awt/Shape.java \
	java/awt/ShortcutConsumer.java \
	java/awt/ShortcutHandler.java \
	java/awt/Stroke.java \
	java/awt/SystemColor.java \
	java/awt/TextArea.java \
	java/awt/TextBuffer.java \
	java/awt/TextComponent.java \
	java/awt/TextCursor.java \
	java/awt/TextEvt.java \
	java/awt/TextField.java \
	java/awt/TexturePaint.java \
	java/awt/Toolkit.java \
	java/awt/Transparency.java \
	java/awt/WMEvent.java \
	java/awt/Window.java \
	java/awt/WindowEvt.java
java_awt_color_SRCS = \
	java/awt/color/CMMException.java \
	java/awt/color/ColorSpace.java \
	java/awt/color/ICC_ColorSpace.java \
	java/awt/color/ICC_Profile.java \
	java/awt/color/ICC_ProfileGray.java \
	java/awt/color/ICC_ProfileRGB.java \
	java/awt/color/ProfileDataException.java
java_awt_datatransfer_SRCS = \
	java/awt/datatransfer/Clipboard.java \
	java/awt/datatransfer/ClipboardOwner.java \
	java/awt/datatransfer/DataFlavor.java \
	java/awt/datatransfer/FlavorMap.java \
	java/awt/datatransfer/FlavorTable.java \
	java/awt/datatransfer/MimeTypeParseException.java \
	java/awt/datatransfer/StringSelection.java \
	java/awt/datatransfer/SystemFlavorMap.java \
	java/awt/datatransfer/Transferable.java \
	java/awt/datatransfer/UnsupportedFlavorException.java
java_awt_dnd_SRCS = \
	java/awt/dnd/Autoscroll.java \
	java/awt/dnd/DnDConstants.java \
	java/awt/dnd/DnDEventMulticaster.java \
	java/awt/dnd/DragGestureEvent.java \
	java/awt/dnd/DragGestureListener.java \
	java/awt/dnd/DragGestureRecognizer.java \
	java/awt/dnd/DragSource.java \
	java/awt/dnd/DragSourceAdapter.java \
	java/awt/dnd/DragSourceContext.java \
	java/awt/dnd/DragSourceDragEvent.java \
	java/awt/dnd/DragSourceDropEvent.java \
	java/awt/dnd/DragSourceEvent.java \
	java/awt/dnd/DragSourceListener.java \
	java/awt/dnd/DragSourceMotionListener.java \
	java/awt/dnd/DropTarget.java \
	java/awt/dnd/DropTargetAdapter.java \
	java/awt/dnd/DropTargetContext.java \
	java/awt/dnd/DropTargetDragEvent.java \
	java/awt/dnd/DropTargetDropEvent.java \
	java/awt/dnd/DropTargetEvent.java \
	java/awt/dnd/DropTargetListener.java \
	java/awt/dnd/InvalidDnDOperationException.java \
	java/awt/dnd/MouseDragGestureRecognizer.java
java_awt_dnd_peer_SRCS = \
	java/awt/dnd/peer/DragSourceContextPeer.java \
	java/awt/dnd/peer/DropTargetContextPeer.java \
	java/awt/dnd/peer/DropTargetPeer.java
java_awt_event_SRCS = \
	java/awt/event/AWTEventListener.java \
	java/awt/event/ActionEvent.java \
	java/awt/event/ActionListener.java \
	java/awt/event/AdjustmentEvent.java \
	java/awt/event/AdjustmentListener.java \
	java/awt/event/ComponentAdapter.java \
	java/awt/event/ComponentEvent.java \
	java/awt/event/ComponentListener.java \
	java/awt/event/ContainerAdapter.java \
	java/awt/event/ContainerEvent.java \
	java/awt/event/ContainerListener.java \
	java/awt/event/FocusAdapter.java \
	java/awt/event/FocusEvent.java \
	java/awt/event/FocusListener.java \
	java/awt/event/HierarchyBoundsAdapter.java \
	java/awt/event/HierarchyBoundsListener.java \
	java/awt/event/HierarchyEvent.java \
	java/awt/event/HierarchyListener.java \
	java/awt/event/InputEvent.java \
	java/awt/event/InputMethodEvent.java \
	java/awt/event/InputMethodListener.java \
	java/awt/event/InvocationEvent.java \
	java/awt/event/ItemEvent.java \
	java/awt/event/ItemListener.java \
	java/awt/event/KeyAdapter.java \
	java/awt/event/KeyEvent.java \
	java/awt/event/KeyListener.java \
	java/awt/event/MouseAdapter.java \
	java/awt/event/MouseEvent.java \
	java/awt/event/MouseListener.java \
	java/awt/event/MouseMotionAdapter.java \
	java/awt/event/MouseMotionListener.java \
	java/awt/event/MouseWheelEvent.java \
	java/awt/event/MouseWheelListener.java \
	java/awt/event/PaintEvent.java \
	java/awt/event/TextEvent.java \
	java/awt/event/TextListener.java \
	java/awt/event/WindowAdapter.java \
	java/awt/event/WindowEvent.java \
	java/awt/event/WindowFocusListener.java \
	java/awt/event/WindowListener.java \
	java/awt/event/WindowStateListener.java
java_awt_font_SRCS = \
	java/awt/font/FontRenderContext.java \
	java/awt/font/GlyphJustificationInfo.java \
	java/awt/font/GlyphMetrics.java \
	java/awt/font/GlyphVector.java \
	java/awt/font/GraphicAttribute.java \
	java/awt/font/ImageGraphicAttribute.java \
	java/awt/font/LineBreakMeasurer.java \
	java/awt/font/LineMetrics.java \
	java/awt/font/MultipleMaster.java \
	java/awt/font/NumericShaper.java \
	java/awt/font/OpenType.java \
	java/awt/font/ShapeGraphicAttribute.java \
	java/awt/font/TextAttribute.java \
	java/awt/font/TextHitInfo.java \
	java/awt/font/TextLayout.java \
	java/awt/font/TextMeasurer.java \
	java/awt/font/TransformAttribute.java
java_awt_geom_SRCS = \
	java/awt/geom/AffineTransform.java \
	java/awt/geom/Arc2D.java \
	java/awt/geom/Area.java \
	java/awt/geom/CubicCurve2D.java \
	java/awt/geom/Dimension2D.java \
	java/awt/geom/Ellipse2D.java \
	java/awt/geom/FlatteningPathIterator.java \
	java/awt/geom/GeneralPath.java \
	java/awt/geom/IllegalPathStateException.java \
	java/awt/geom/Line2D.java \
	java/awt/geom/NoninvertibleTransformException.java \
	java/awt/geom/PathIterator.java \
	java/awt/geom/Point2D.java \
	java/awt/geom/QuadCurve2D.java \
	java/awt/geom/Rectangle2D.java \
	java/awt/geom/RectangularShape.java \
	java/awt/geom/RoundRectangle2D.java
java_awt_im_SRCS = \
	java/awt/im/InputMethodHighlight.java \
	java/awt/im/InputMethodRequests.java \
	java/awt/im/InputSubset.java
java_awt_image_SRCS = \
	java/awt/image/AreaAveragingScaleFilter.java \
	java/awt/image/BufferStrategy.java \
	java/awt/image/BufferedImage.java \
	java/awt/image/BufferedImageOp.java \
	java/awt/image/ColorModel.java \
	java/awt/image/ComponentColorModel.java \
	java/awt/image/ComponentSampleModel.java \
	java/awt/image/CropImageFilter.java \
	java/awt/image/DataBuffer.java \
	java/awt/image/DataBufferByte.java \
	java/awt/image/DataBufferInt.java \
	java/awt/image/DataBufferUShort.java \
	java/awt/image/DirectColorModel.java \
	java/awt/image/FilteredImageSource.java \
	java/awt/image/ImageConsumer.java \
	java/awt/image/ImageFilter.java \
	java/awt/image/ImageObserver.java \
	java/awt/image/ImageProducer.java \
	java/awt/image/ImagingOpException.java \
	java/awt/image/IndexColorModel.java \
	java/awt/image/MemoryImageSource.java \
	java/awt/image/PackedColorModel.java \
	java/awt/image/PixelGrabber.java \
	java/awt/image/RGBImageFilter.java \
	java/awt/image/Raster.java \
	java/awt/image/RasterFormatException.java \
	java/awt/image/RasterOp.java \
	java/awt/image/RenderedImage.java \
	java/awt/image/ReplicateScaleFilter.java \
	java/awt/image/SampleModel.java \
	java/awt/image/SinglePixelPackedSampleModel.java \
	java/awt/image/TileObserver.java \
	java/awt/image/VolatileImage.java \
	java/awt/image/WritableRaster.java \
	java/awt/image/WritableRenderedImage.java
java_awt_image_renderable_SRCS = \
	java/awt/image/renderable/ContextualRenderedImageFactory.java \
	java/awt/image/renderable/ParameterBlock.java \
	java/awt/image/renderable/RenderContext.java \
	java/awt/image/renderable/RenderableImage.java \
	java/awt/image/renderable/RenderableImageOp.java \
	java/awt/image/renderable/RenderableImageProducer.java \
	java/awt/image/renderable/RenderedImageFactory.java
java_awt_peer_SRCS = \
	java/awt/peer/ActiveEvent.java \
	java/awt/peer/ButtonPeer.java \
	java/awt/peer/CanvasPeer.java \
	java/awt/peer/CheckboxMenuItemPeer.java \
	java/awt/peer/CheckboxPeer.java \
	java/awt/peer/ChoicePeer.java \
	java/awt/peer/ComponentPeer.java \
	java/awt/peer/ContainerPeer.java \
	java/awt/peer/DialogPeer.java \
	java/awt/peer/FileDialogPeer.java \
	java/awt/peer/FontPeer.java \
	java/awt/peer/FramePeer.java \
	java/awt/peer/LabelPeer.java \
	java/awt/peer/LightweightPeer.java \
	java/awt/peer/ListPeer.java \
	java/awt/peer/MenuBarPeer.java \
	java/awt/peer/MenuComponentPeer.java \
	java/awt/peer/MenuItemPeer.java \
	java/awt/peer/MenuPeer.java \
	java/awt/peer/PanelPeer.java \
	java/awt/peer/PopupMenuPeer.java \
	java/awt/peer/RobotPeer.java \
	java/awt/peer/ScrollPanePeer.java \
	java/awt/peer/ScrollbarPeer.java \
	java/awt/peer/TextAreaPeer.java \
	java/awt/peer/TextComponentPeer.java \
	java/awt/peer/TextFieldPeer.java \
	java/awt/peer/WindowPeer.java
java_awt_print_SRCS = \
	java/awt/print/Book.java \
	java/awt/print/PageFormat.java \
	java/awt/print/Pageable.java \
	java/awt/print/Paper.java \
	java/awt/print/Printable.java \
	java/awt/print/PrinterAbortException.java \
	java/awt/print/PrinterException.java \
	java/awt/print/PrinterGraphics.java \
	java/awt/print/PrinterIOException.java \
	java/awt/print/PrinterJob.java
java_beans_SRCS = \
	java/beans/AppletInitializer.java \
	java/beans/BeanDescriptor.java \
	java/beans/BeanInfo.java \
	java/beans/Beans.java \
	java/beans/Customizer.java \
	java/beans/DesignMode.java \
	java/beans/EventSetDescriptor.java \
	java/beans/ExceptionListener.java \
	java/beans/FeatureDescriptor.java \
	java/beans/GenericBeanInfo.java \
	java/beans/IndexedPropertyDescriptor.java \
	java/beans/IntrospectionException.java \
	java/beans/Introspector.java \
	java/beans/MethodDescriptor.java \
	java/beans/ParameterDescriptor.java \
	java/beans/PropertyChangeEvent.java \
	java/beans/PropertyChangeListener.java \
	java/beans/PropertyChangeListenerProxy.java \
	java/beans/PropertyChangeSupport.java \
	java/beans/PropertyDescriptor.java \
	java/beans/PropertyEditor.java \
	java/beans/PropertyEditorManager.java \
	java/beans/PropertyEditorSupport.java \
	java/beans/PropertyVetoException.java \
	java/beans/SimpleBeanInfo.java \
	java/beans/VetoableChangeListener.java \
	java/beans/VetoableChangeListenerProxy.java \
	java/beans/VetoableChangeSupport.java \
	java/beans/Visibility.java
java_beans_beancontext_SRCS = \
	java/beans/beancontext/BeanContext.java \
	java/beans/beancontext/BeanContextChild.java \
	java/beans/beancontext/BeanContextChildComponentProxy.java \
	java/beans/beancontext/BeanContextChildSupport.java \
	java/beans/beancontext/BeanContextContainerProxy.java \
	java/beans/beancontext/BeanContextEvent.java \
	java/beans/beancontext/BeanContextMembershipEvent.java \
	java/beans/beancontext/BeanContextMembershipListener.java \
	java/beans/beancontext/BeanContextProxy.java \
	java/beans/beancontext/BeanContextServiceAvailableEvent.java \
	java/beans/beancontext/BeanContextServiceProvider.java \
	java/beans/beancontext/BeanContextServiceProviderBeanInfo.java \
	java/beans/beancontext/BeanContextServiceRevokedEvent.java \
	java/beans/beancontext/BeanContextServiceRevokedListener.java \
	java/beans/beancontext/BeanContextServices.java \
	java/beans/beancontext/BeanContextServicesListener.java \
	java/beans/beancontext/BeanContextServicesSupport.java \
	java/beans/beancontext/BeanContextSupport.java
java_io_SRCS = \
	java/io/BufferedInputStream.java \
	java/io/BufferedOutputStream.java \
	java/io/BufferedReader.java \
	java/io/BufferedWriter.java \
	java/io/ByteArrayInputStream.java \
	java/io/ByteArrayOutputStream.java \
	java/io/CharArrayReader.java \
	java/io/CharArrayWriter.java \
	java/io/CharConversionException.java \
	java/io/DataInput.java \
	java/io/DataInputStream.java \
	java/io/DataOutput.java \
	java/io/DataOutputStream.java \
	java/io/EOFException.java \
	java/io/Externalizable.java \
	java/io/File.java \
	java/io/FileDescriptor.java \
	java/io/FileFilter.java \
	java/io/FileInputStream.java \
	java/io/FileNotFoundException.java \
	java/io/FileOutputStream.java \
	java/io/FilePermission.java \
	java/io/FileReader.java \
	java/io/FileWriter.java \
	java/io/FilenameFilter.java \
	java/io/FilterInputStream.java \
	java/io/FilterOutputStream.java \
	java/io/FilterReader.java \
	java/io/FilterWriter.java \
	java/io/IOException.java \
	java/io/InputStream.java \
	java/io/InputStreamReader.java \
	java/io/InterruptedIOException.java \
	java/io/InvalidClassException.java \
	java/io/InvalidObjectException.java \
	java/io/LineNumberInputStream.java \
	java/io/LineNumberReader.java \
	java/io/NotActiveException.java \
	java/io/NotSerializableException.java \
	java/io/ObjectInput.java \
	java/io/ObjectInputStream.java \
	java/io/ObjectInputValidation.java \
	java/io/ObjectOutput.java \
	java/io/ObjectOutputStream.java \
	java/io/ObjectStreamClass.java \
	java/io/ObjectStreamConstants.java \
	java/io/ObjectStreamException.java \
	java/io/ObjectStreamField.java \
	java/io/OptionalDataException.java \
	java/io/OutputStream.java \
	java/io/OutputStreamWriter.java \
	java/io/PipedInputStream.java \
	java/io/PipedOutputStream.java \
	java/io/PipedReader.java \
	java/io/PipedWriter.java \
	java/io/PrintStream.java \
	java/io/PrintWriter.java \
	java/io/PushbackInputStream.java \
	java/io/PushbackReader.java \
	java/io/RandomAccessFile.java \
	java/io/Reader.java \
	java/io/SequenceInputStream.java \
	java/io/Serializable.java \
	java/io/SerializablePermission.java \
	java/io/StreamCorruptedException.java \
	java/io/StreamTokenizer.java \
	java/io/StringBufferInputStream.java \
	java/io/StringReader.java \
	java/io/StringWriter.java \
	java/io/SyncFailedException.java \
	java/io/UTFDataFormatException.java \
	java/io/UnsupportedEncodingException.java \
	java/io/VMObjectStreamClass.java \
	java/io/WriteAbortedException.java \
	java/io/Writer.java
java_lang_SRCS = \
	java/lang/AbstractMethodError.java \
	java/lang/ArithmeticException.java \
	java/lang/ArrayIndexOutOfBoundsException.java \
	java/lang/ArrayStoreException.java \
	java/lang/AssertionError.java \
	java/lang/Boolean.java \
	java/lang/Byte.java \
	java/lang/CharSequence.java \
	java/lang/Character.java \
	java/lang/Class.java \
	java/lang/ClassCastException.java \
	java/lang/ClassCircularityError.java \
	java/lang/ClassFormatError.java \
	java/lang/ClassLoader.java \
	java/lang/ClassNotFoundException.java \
	java/lang/CloneNotSupportedException.java \
	java/lang/Cloneable.java \
	java/lang/Comparable.java \
	java/lang/Compiler.java \
	java/lang/Double.java \
	java/lang/Error.java \
	java/lang/Exception.java \
	java/lang/ExceptionInInitializerError.java \
	java/lang/Float.java \
	java/lang/IllegalAccessError.java \
	java/lang/IllegalAccessException.java \
	java/lang/IllegalArgumentException.java \
	java/lang/IllegalMonitorStateException.java \
	java/lang/IllegalStateException.java \
	java/lang/IllegalThreadStateException.java \
	java/lang/IncompatibleClassChangeError.java \
	java/lang/IndexOutOfBoundsException.java \
	java/lang/InheritableThreadLocal.java \
	java/lang/InstantiationError.java \
	java/lang/InstantiationException.java \
	java/lang/Integer.java \
	java/lang/InternalError.java \
	java/lang/InterruptedException.java \
	java/lang/LinkageError.java \
	java/lang/Long.java \
	java/lang/Math.java \
	java/lang/NativeLibrary.java \
	java/lang/NegativeArraySizeException.java \
	java/lang/NoClassDefFoundError.java \
	java/lang/NoSuchFieldError.java \
	java/lang/NoSuchFieldException.java \
	java/lang/NoSuchMethodError.java \
	java/lang/NoSuchMethodException.java \
	java/lang/NullPointerException.java \
	java/lang/Number.java \
	java/lang/NumberFormatException.java \
	java/lang/Object.java \
	java/lang/OutOfMemoryError.java \
	java/lang/Package.java \
	java/lang/Process.java \
	java/lang/ProcessInputStream.java \
	java/lang/Runnable.java \
	java/lang/Runtime.java \
	java/lang/RuntimeException.java \
	java/lang/RuntimePermission.java \
	java/lang/SecurityException.java \
	java/lang/SecurityManager.java \
	java/lang/Short.java \
	java/lang/StackOverflowError.java \
	java/lang/StackTraceElement.java \
	java/lang/StrictMath.java \
	java/lang/String.java \
	java/lang/StringBuffer.java \
	java/lang/StringIndexOutOfBoundsException.java \
	java/lang/System.java \
	java/lang/Thread.java \
	java/lang/ThreadDeath.java \
	java/lang/ThreadGroup.java \
	java/lang/ThreadLocal.java \
	java/lang/Throwable.java \
	java/lang/UnknownError.java \
	java/lang/UnsatisfiedLinkError.java \
	java/lang/UnsupportedClassVersionError.java \
	java/lang/UnsupportedOperationException.java \
	java/lang/VMThrowable.java \
	java/lang/VerifyError.java \
	java/lang/VirtualMachineError.java \
	java/lang/Void.java
java_lang_ref_SRCS = \
	java/lang/ref/PhantomReference.java \
	java/lang/ref/Reference.java \
	java/lang/ref/ReferenceQueue.java \
	java/lang/ref/SoftReference.java \
	java/lang/ref/WeakReference.java
java_lang_reflect_SRCS = \
	java/lang/reflect/AccessibleObject.java \
	java/lang/reflect/Array.java \
	java/lang/reflect/Constructor.java \
	java/lang/reflect/Field.java \
	java/lang/reflect/InvocationHandler.java \
	java/lang/reflect/InvocationTargetException.java \
	java/lang/reflect/Member.java \
	java/lang/reflect/Method.java \
	java/lang/reflect/Modifier.java \
	java/lang/reflect/Proxy.java \
	java/lang/reflect/ReflectPermission.java \
	java/lang/reflect/UndeclaredThrowableException.java
java_math_SRCS = \
	java/math/BigDecimal.java \
	java/math/BigInteger.java
java_net_SRCS = \
	java/net/Authenticator.java \
	java/net/BindException.java \
	java/net/ConnectException.java \
	java/net/ContentHandler.java \
	java/net/ContentHandlerFactory.java \
	java/net/DatagramPacket.java \
	java/net/DatagramSocket.java \
	java/net/DatagramSocketImpl.java \
	java/net/DatagramSocketImplFactory.java \
	java/net/FileNameMap.java \
	java/net/HttpURLConnection.java \
	java/net/Inet4Address.java \
	java/net/Inet6Address.java \
	java/net/InetAddress.java \
	java/net/InetSocketAddress.java \
	java/net/JarURLConnection.java \
	java/net/MalformedURLException.java \
	java/net/MimeTypeMapper.java \
	java/net/MulticastSocket.java \
	java/net/NetPermission.java \
	java/net/NetworkInterface.java \
	java/net/NoRouteToHostException.java \
	java/net/PasswordAuthentication.java \
	java/net/PortUnreachableException.java \
	java/net/ProtocolException.java \
	java/net/ServerSocket.java \
	java/net/Socket.java \
	java/net/SocketAddress.java \
	java/net/SocketException.java \
	java/net/SocketImpl.java \
	java/net/SocketImplFactory.java \
	java/net/SocketOptions.java \
	java/net/SocketPermission.java \
	java/net/SocketTimeoutException.java \
	java/net/URI.java \
	java/net/URISyntaxException.java \
	java/net/URL.java \
	java/net/URLClassLoader.java \
	java/net/URLConnection.java \
	java/net/URLDecoder.java \
	java/net/URLEncoder.java \
	java/net/URLStreamHandler.java \
	java/net/URLStreamHandlerFactory.java \
	java/net/UnknownHostException.java \
	java/net/UnknownServiceException.java
java_nio_SRCS = \
	java/nio/Buffer.java \
	java/nio/BufferOverflowException.java \
	java/nio/BufferUnderflowException.java \
	java/nio/ByteBuffer.java \
	java/nio/ByteBufferHelper.java \
	java/nio/ByteBufferImpl.java \
	java/nio/ByteOrder.java \
	java/nio/CharBuffer.java \
	java/nio/CharBufferImpl.java \
	java/nio/CharViewBufferImpl.java \
	java/nio/DirectByteBufferImpl.java \
	java/nio/DoubleBuffer.java \
	java/nio/DoubleBufferImpl.java \
	java/nio/DoubleViewBufferImpl.java \
	java/nio/FloatBuffer.java \
	java/nio/FloatBufferImpl.java \
	java/nio/FloatViewBufferImpl.java \
	java/nio/IntBuffer.java \
	java/nio/IntBufferImpl.java \
	java/nio/IntViewBufferImpl.java \
	java/nio/InvalidMarkException.java \
	java/nio/LongBuffer.java \
	java/nio/LongBufferImpl.java \
	java/nio/LongViewBufferImpl.java \
	java/nio/MappedByteBuffer.java \
	java/nio/MappedByteBufferImpl.java \
	java/nio/ReadOnlyBufferException.java \
	java/nio/ShortBuffer.java \
	java/nio/ShortBufferImpl.java \
	java/nio/ShortViewBufferImpl.java
java_nio_channels_SRCS = \
	java/nio/channels/AlreadyConnectedException.java \
	java/nio/channels/AsynchronousCloseException.java \
	java/nio/channels/ByteChannel.java \
	java/nio/channels/CancelledKeyException.java \
	java/nio/channels/Channel.java \
	java/nio/channels/Channels.java \
	java/nio/channels/ClosedByInterruptException.java \
	java/nio/channels/ClosedChannelException.java \
	java/nio/channels/ClosedSelectorException.java \
	java/nio/channels/ConnectionPendingException.java \
	java/nio/channels/DatagramChannel.java \
	java/nio/channels/FileChannel.java \
	java/nio/channels/FileChannelImpl.java \
	java/nio/channels/FileLock.java \
	java/nio/channels/FileLockInterruptionException.java \
	java/nio/channels/GatheringByteChannel.java \
	java/nio/channels/IllegalBlockingModeException.java \
	java/nio/channels/IllegalSelectorException.java \
	java/nio/channels/InterruptibleChannel.java \
	java/nio/channels/NoConnectionPendingException.java \
	java/nio/channels/NonReadableChannelException.java \
	java/nio/channels/NonWritableChannelException.java \
	java/nio/channels/NotYetBoundException.java \
	java/nio/channels/NotYetConnectedException.java \
	java/nio/channels/OverlappingFileLockException.java \
	java/nio/channels/Pipe.java \
	java/nio/channels/ReadableByteChannel.java \
	java/nio/channels/ScatteringByteChannel.java \
	java/nio/channels/SelectableChannel.java \
	java/nio/channels/SelectionKey.java \
	java/nio/channels/Selector.java \
	java/nio/channels/ServerSocketChannel.java \
	java/nio/channels/SocketChannel.java \
	java/nio/channels/UnresolvedAddressException.java \
	java/nio/channels/UnsupportedAddressTypeException.java \
	java/nio/channels/WritableByteChannel.java
java_nio_channels_spi_SRCS = \
	java/nio/channels/spi/AbstractInterruptibleChannel.java \
	java/nio/channels/spi/AbstractSelectableChannel.java \
	java/nio/channels/spi/AbstractSelectionKey.java \
	java/nio/channels/spi/AbstractSelector.java \
	java/nio/channels/spi/SelectorProvider.java
java_nio_charset_SRCS = \
	java/nio/charset/CharacterCodingException.java \
	java/nio/charset/Charset.java \
	java/nio/charset/CharsetDecoder.java \
	java/nio/charset/CharsetEncoder.java \
	java/nio/charset/CoderMalfunctionError.java \
	java/nio/charset/CoderResult.java \
	java/nio/charset/CodingErrorAction.java \
	java/nio/charset/IllegalCharsetNameException.java \
	java/nio/charset/MalformedInputException.java \
	java/nio/charset/UnmappableCharacterException.java \
	java/nio/charset/UnsupportedCharsetException.java
java_nio_charset_spi_SRCS = \
	java/nio/charset/spi/CharsetProvider.java
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
	java/rmi/server/RMIClassLoaderSpi.java \
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
java_security_SRCS = \
	java/security/AccessControlContext.java \
	java/security/AccessControlException.java \
	java/security/AccessController.java \
	java/security/AlgorithmParameterGenerator.java \
	java/security/AlgorithmParameterGeneratorSpi.java \
	java/security/AlgorithmParameters.java \
	java/security/AlgorithmParametersSpi.java \
	java/security/AllPermission.java \
	java/security/BasicPermission.java \
	java/security/Certificate.java \
	java/security/CodeSource.java \
	java/security/DigestException.java \
	java/security/DigestInputStream.java \
	java/security/DigestOutputStream.java \
	java/security/DomainCombiner.java \
	java/security/GeneralSecurityException.java \
	java/security/Guard.java \
	java/security/GuardedObject.java \
	java/security/Identity.java \
	java/security/IdentityScope.java \
	java/security/InvalidAlgorithmParameterException.java \
	java/security/InvalidKeyException.java \
	java/security/InvalidParameterException.java \
	java/security/Key.java \
	java/security/KeyException.java \
	java/security/KeyFactory.java \
	java/security/KeyFactorySpi.java \
	java/security/KeyManagementException.java \
	java/security/KeyPair.java \
	java/security/KeyPairGenerator.java \
	java/security/KeyPairGeneratorSpi.java \
	java/security/KeyStore.java \
	java/security/KeyStoreException.java \
	java/security/KeyStoreSpi.java \
	java/security/MessageDigest.java \
	java/security/MessageDigestSpi.java \
	java/security/NoSuchAlgorithmException.java \
	java/security/NoSuchProviderException.java \
	java/security/Permission.java \
	java/security/PermissionCollection.java \
	java/security/Permissions.java \
	java/security/Policy.java \
	java/security/Principal.java \
	java/security/PrivateKey.java \
	java/security/PrivilegedAction.java \
	java/security/PrivilegedActionException.java \
	java/security/PrivilegedExceptionAction.java \
	java/security/ProtectionDomain.java \
	java/security/Provider.java \
	java/security/ProviderException.java \
	java/security/PublicKey.java \
	java/security/SecureClassLoader.java \
	java/security/SecureRandom.java \
	java/security/SecureRandomSpi.java \
	java/security/Security.java \
	java/security/SecurityPermission.java \
	java/security/Signature.java \
	java/security/SignatureException.java \
	java/security/SignatureSpi.java \
	java/security/SignedObject.java \
	java/security/Signer.java \
	java/security/UnrecoverableKeyException.java \
	java/security/UnresolvedPermission.java
java_security_acl_SRCS = \
	java/security/acl/Acl.java \
	java/security/acl/AclEntry.java \
	java/security/acl/AclNotFoundException.java \
	java/security/acl/Group.java \
	java/security/acl/LastOwnerException.java \
	java/security/acl/NotOwnerException.java \
	java/security/acl/Owner.java \
	java/security/acl/Permission.java
java_security_cert_SRCS = \
	java/security/cert/CRL.java \
	java/security/cert/CRLException.java \
	java/security/cert/CRLSelector.java \
	java/security/cert/CertPath.java \
	java/security/cert/CertPathBuilder.java \
	java/security/cert/CertPathBuilderException.java \
	java/security/cert/CertPathBuilderResult.java \
	java/security/cert/CertPathBuilderSpi.java \
	java/security/cert/CertPathParameters.java \
	java/security/cert/CertPathValidator.java \
	java/security/cert/CertPathValidatorException.java \
	java/security/cert/CertPathValidatorResult.java \
	java/security/cert/CertPathValidatorSpi.java \
	java/security/cert/CertSelector.java \
	java/security/cert/CertStore.java \
	java/security/cert/CertStoreException.java \
	java/security/cert/CertStoreParameters.java \
	java/security/cert/CertStoreSpi.java \
	java/security/cert/Certificate.java \
	java/security/cert/CertificateEncodingException.java \
	java/security/cert/CertificateException.java \
	java/security/cert/CertificateExpiredException.java \
	java/security/cert/CertificateFactory.java \
	java/security/cert/CertificateFactorySpi.java \
	java/security/cert/CertificateNotYetValidException.java \
	java/security/cert/CertificateParsingException.java \
	java/security/cert/CollectionCertStoreParameters.java \
	java/security/cert/LDAPCertStoreParameters.java \
	java/security/cert/PKIXBuilderParameters.java \
	java/security/cert/PKIXCertPathBuilderResult.java \
	java/security/cert/PKIXCertPathChecker.java \
	java/security/cert/PKIXCertPathValidatorResult.java \
	java/security/cert/PKIXParameters.java \
	java/security/cert/PolicyNode.java \
	java/security/cert/PolicyQualifierInfo.java \
	java/security/cert/TrustAnchor.java \
	java/security/cert/X509CRL.java \
	java/security/cert/X509CRLEntry.java \
	java/security/cert/X509Certificate.java \
	java/security/cert/X509Extension.java
java_security_interfaces_SRCS = \
	java/security/interfaces/DSAKey.java \
	java/security/interfaces/DSAKeyPairGenerator.java \
	java/security/interfaces/DSAParams.java \
	java/security/interfaces/DSAPrivateKey.java \
	java/security/interfaces/DSAPublicKey.java \
	java/security/interfaces/RSAKey.java \
	java/security/interfaces/RSAMultiPrimePrivateCrtKey.java \
	java/security/interfaces/RSAPrivateCrtKey.java \
	java/security/interfaces/RSAPrivateKey.java \
	java/security/interfaces/RSAPublicKey.java
java_security_spec_SRCS = \
	java/security/spec/AlgorithmParameterSpec.java \
	java/security/spec/DSAParameterSpec.java \
	java/security/spec/DSAPrivateKeySpec.java \
	java/security/spec/DSAPublicKeySpec.java \
	java/security/spec/EncodedKeySpec.java \
	java/security/spec/InvalidKeySpecException.java \
	java/security/spec/InvalidParameterSpecException.java \
	java/security/spec/KeySpec.java \
	java/security/spec/PKCS8EncodedKeySpec.java \
	java/security/spec/PSSParameterSpec.java \
	java/security/spec/RSAKeyGenParameterSpec.java \
	java/security/spec/RSAMultiPrimePrivateCrtKeySpec.java \
	java/security/spec/RSAOtherPrimeInfo.java \
	java/security/spec/RSAPrivateCrtKeySpec.java \
	java/security/spec/RSAPrivateKeySpec.java \
	java/security/spec/RSAPublicKeySpec.java \
	java/security/spec/X509EncodedKeySpec.java
java_sql_SRCS = \
	java/sql/Array.java \
	java/sql/BatchUpdateException.java \
	java/sql/Blob.java \
	java/sql/CallableStatement.java \
	java/sql/Clob.java \
	java/sql/Connection.java \
	java/sql/DataTruncation.java \
	java/sql/DatabaseMetaData.java \
	java/sql/Date.java \
	java/sql/Driver.java \
	java/sql/DriverManager.java \
	java/sql/DriverPropertyInfo.java \
	java/sql/ParameterMetaData.java \
	java/sql/PreparedStatement.java \
	java/sql/Ref.java \
	java/sql/ResultSet.java \
	java/sql/ResultSetMetaData.java \
	java/sql/SQLData.java \
	java/sql/SQLException.java \
	java/sql/SQLInput.java \
	java/sql/SQLOutput.java \
	java/sql/SQLPermission.java \
	java/sql/SQLWarning.java \
	java/sql/Savepoint.java \
	java/sql/Statement.java \
	java/sql/Struct.java \
	java/sql/Time.java \
	java/sql/Timestamp.java \
	java/sql/Types.java
java_text_SRCS = \
	java/text/Annotation.java \
	java/text/AttributedCharacterIterator.java \
	java/text/AttributedString.java \
	java/text/AttributedStringIterator.java \
	java/text/BreakIterator.java \
	java/text/CharacterIterator.java \
	java/text/ChoiceFormat.java \
	java/text/CollationElementIterator.java \
	java/text/CollationKey.java \
	java/text/Collator.java \
	java/text/DateFormat.java \
	java/text/DateFormatSymbols.java \
	java/text/DecimalFormat.java \
	java/text/DecimalFormatSymbols.java \
	java/text/FieldPosition.java \
	java/text/Format.java \
	java/text/FormatCharacterIterator.java \
	java/text/MessageFormat.java \
	java/text/NumberFormat.java \
	java/text/ParseException.java \
	java/text/ParsePosition.java \
	java/text/RuleBasedCollator.java \
	java/text/SimpleDateFormat.java \
	java/text/SimpleLineIterator.java \
	java/text/SimpleSentenceIterator.java \
	java/text/SimpleWordIterator.java \
	java/text/StringCharacterIterator.java
java_util_SRCS = \
	java/util/AbstractCollection.java \
	java/util/AbstractList.java \
	java/util/AbstractListIterator.java \
	java/util/AbstractMap.java \
	java/util/AbstractMapEntry.java \
	java/util/AbstractMapEntrySet.java \
	java/util/AbstractSequentialList.java \
	java/util/AbstractSet.java \
	java/util/ArrayList.java \
	java/util/Arrays.java \
	java/util/BitSet.java \
	java/util/Calendar.java \
	java/util/Collection.java \
	java/util/Collections.java \
	java/util/Comparator.java \
	java/util/ConcurrentModificationException.java \
	java/util/Currency.java \
	java/util/Date.java \
	java/util/Dictionary.java \
	java/util/EmptyStackException.java \
	java/util/Enumeration.java \
	java/util/EventListener.java \
	java/util/EventListenerProxy.java \
	java/util/EventObject.java \
	java/util/GregorianCalendar.java \
	java/util/HashMap.java \
	java/util/HashSet.java \
	java/util/Hashtable.java \
	java/util/IdentityHashMap.java \
	java/util/Iterator.java \
	java/util/LinkedHashMap.java \
	java/util/LinkedHashSet.java \
	java/util/LinkedList.java \
	java/util/List.java \
	java/util/ListIterator.java \
	java/util/ListResourceBundle.java \
	java/util/Locale.java \
	java/util/Map.java \
	java/util/MissingResourceException.java \
	java/util/NoSuchElementException.java \
	java/util/Observable.java \
	java/util/Observer.java \
	java/util/Properties.java \
	java/util/PropertyPermission.java \
	java/util/PropertyPermissionCollection.java \
	java/util/PropertyResourceBundle.java \
	java/util/Random.java \
	java/util/RandomAccess.java \
	java/util/ResourceBundle.java \
	java/util/Set.java \
	java/util/SimpleTimeZone.java \
	java/util/SortedMap.java \
	java/util/SortedSet.java \
	java/util/Stack.java \
	java/util/StringTokenizer.java \
	java/util/TimeZone.java \
	java/util/Timer.java \
	java/util/TimerTask.java \
	java/util/TooManyListenersException.java \
	java/util/TreeMap.java \
	java/util/TreeSet.java \
	java/util/Vector.java \
	java/util/WeakHashMap.java
java_util_jar_SRCS = \
	java/util/jar/Attributes.java \
	java/util/jar/JarEntry.java \
	java/util/jar/JarException.java \
	java/util/jar/JarFile.java \
	java/util/jar/JarInputStream.java \
	java/util/jar/JarOutputStream.java \
	java/util/jar/Manifest.java
java_util_logging_SRCS = \
	java/util/logging/ConsoleHandler.java \
	java/util/logging/ErrorManager.java \
	java/util/logging/FileHandler.java \
	java/util/logging/Filter.java \
	java/util/logging/Formatter.java \
	java/util/logging/Handler.java \
	java/util/logging/Level.java \
	java/util/logging/LogManager.java \
	java/util/logging/LogRecord.java \
	java/util/logging/Logger.java \
	java/util/logging/LoggingPermission.java \
	java/util/logging/MemoryHandler.java \
	java/util/logging/SimpleFormatter.java \
	java/util/logging/SocketHandler.java \
	java/util/logging/StreamHandler.java \
	java/util/logging/XMLFormatter.java
java_util_mime_SRCS = \
	java/util/mime/MimeType.java \
	java/util/mime/MimeTypeParameterList.java \
	java/util/mime/MimeTypeParseException.java
java_util_prefs_SRCS = \
	java/util/prefs/AbstractPreferences.java \
	java/util/prefs/BackingStoreException.java \
	java/util/prefs/InvalidPreferencesFormatException.java \
	java/util/prefs/NodeChangeEvent.java \
	java/util/prefs/NodeChangeListener.java \
	java/util/prefs/PreferenceChangeEvent.java \
	java/util/prefs/PreferenceChangeListener.java \
	java/util/prefs/Preferences.java \
	java/util/prefs/PreferencesFactory.java
java_util_regex_SRCS = \
	java/util/regex/Matcher.java \
	java/util/regex/Pattern.java \
	java/util/regex/PatternSyntaxException.java
java_util_zip_SRCS = \
	java/util/zip/Adler32.java \
	java/util/zip/CRC32.java \
	java/util/zip/CheckedInputStream.java \
	java/util/zip/CheckedOutputStream.java \
	java/util/zip/Checksum.java \
	java/util/zip/DataFormatException.java \
	java/util/zip/Deflater.java \
	java/util/zip/DeflaterOutputStream.java \
	java/util/zip/GZIPInputStream.java \
	java/util/zip/GZIPOutputStream.java \
	java/util/zip/Inflater.java \
	java/util/zip/InflaterInputStream.java \
	java/util/zip/ZipConstants.java \
	java/util/zip/ZipEntry.java \
	java/util/zip/ZipException.java \
	java/util/zip/ZipFile.java \
	java/util/zip/ZipInputStream.java \
	java/util/zip/ZipOutputStream.java
javax_accessibility_SRCS = \
	javax/accessibility/Accessible.java \
	javax/accessibility/AccessibleAction.java \
	javax/accessibility/AccessibleBundle.java \
	javax/accessibility/AccessibleComponent.java \
	javax/accessibility/AccessibleContext.java \
	javax/accessibility/AccessibleEditableText.java \
	javax/accessibility/AccessibleExtendedComponent.java \
	javax/accessibility/AccessibleExtendedTable.java \
	javax/accessibility/AccessibleHyperlink.java \
	javax/accessibility/AccessibleHypertext.java \
	javax/accessibility/AccessibleIcon.java \
	javax/accessibility/AccessibleKeyBinding.java \
	javax/accessibility/AccessibleRelation.java \
	javax/accessibility/AccessibleRelationSet.java \
	javax/accessibility/AccessibleResourceBundle.java \
	javax/accessibility/AccessibleRole.java \
	javax/accessibility/AccessibleSelection.java \
	javax/accessibility/AccessibleState.java \
	javax/accessibility/AccessibleStateSet.java \
	javax/accessibility/AccessibleTable.java \
	javax/accessibility/AccessibleTableModelChange.java \
	javax/accessibility/AccessibleText.java \
	javax/accessibility/AccessibleValue.java
javax_naming_SRCS = \
	javax/naming/AuthenticationException.java \
	javax/naming/AuthenticationNotSupportedException.java \
	javax/naming/BinaryRefAddr.java \
	javax/naming/Binding.java \
	javax/naming/CannotProceedException.java \
	javax/naming/CommunicationException.java \
	javax/naming/CompositeName.java \
	javax/naming/CompoundName.java \
	javax/naming/ConfigurationException.java \
	javax/naming/Context.java \
	javax/naming/ContextNotEmptyException.java \
	javax/naming/InitialContext.java \
	javax/naming/InsufficientResourcesException.java \
	javax/naming/InterruptedNamingException.java \
	javax/naming/InvalidNameException.java \
	javax/naming/LimitExceededException.java \
	javax/naming/LinkException.java \
	javax/naming/LinkLoopException.java \
	javax/naming/LinkRef.java \
	javax/naming/MalformedLinkException.java \
	javax/naming/Name.java \
	javax/naming/NameAlreadyBoundException.java \
	javax/naming/NameClassPair.java \
	javax/naming/NameNotFoundException.java \
	javax/naming/NameParser.java \
	javax/naming/NamingEnumeration.java \
	javax/naming/NamingException.java \
	javax/naming/NamingSecurityException.java \
	javax/naming/NoInitialContextException.java \
	javax/naming/NoPermissionException.java \
	javax/naming/NotContextException.java \
	javax/naming/OperationNotSupportedException.java \
	javax/naming/PartialResultException.java \
	javax/naming/RefAddr.java \
	javax/naming/Reference.java \
	javax/naming/Referenceable.java \
	javax/naming/ReferralException.java \
	javax/naming/ServiceUnavailableException.java \
	javax/naming/SizeLimitExceededException.java \
	javax/naming/StringRefAddr.java \
	javax/naming/TimeLimitExceededException.java
javax_naming_directory_SRCS = \
	javax/naming/directory/Attribute.java \
	javax/naming/directory/AttributeInUseException.java \
	javax/naming/directory/AttributeModificationException.java \
	javax/naming/directory/Attributes.java \
	javax/naming/directory/BasicAttribute.java \
	javax/naming/directory/BasicAttributes.java \
	javax/naming/directory/DirContext.java \
	javax/naming/directory/InitialDirContext.java \
	javax/naming/directory/InvalidAttributeIdentifierException.java \
	javax/naming/directory/InvalidAttributeValueException.java \
	javax/naming/directory/InvalidAttributesException.java \
	javax/naming/directory/InvalidSearchControlsException.java \
	javax/naming/directory/InvalidSearchFilterException.java \
	javax/naming/directory/ModificationItem.java \
	javax/naming/directory/NoSuchAttributeException.java \
	javax/naming/directory/SchemaViolationException.java \
	javax/naming/directory/SearchControls.java \
	javax/naming/directory/SearchResult.java
javax_naming_event_SRCS = \
	javax/naming/event/EventContext.java \
	javax/naming/event/EventDirContext.java \
	javax/naming/event/NamespaceChangeListener.java \
	javax/naming/event/NamingEvent.java \
	javax/naming/event/NamingExceptionEvent.java \
	javax/naming/event/NamingListener.java \
	javax/naming/event/ObjectChangeListener.java
javax_naming_ldap_SRCS = \
	javax/naming/ldap/Control.java \
	javax/naming/ldap/ControlFactory.java \
	javax/naming/ldap/ExtendedRequest.java \
	javax/naming/ldap/ExtendedResponse.java \
	javax/naming/ldap/HasControls.java \
	javax/naming/ldap/InitialLdapContext.java \
	javax/naming/ldap/LdapContext.java \
	javax/naming/ldap/LdapReferralException.java \
	javax/naming/ldap/UnsolicitedNotification.java \
	javax/naming/ldap/UnsolicitedNotificationEvent.java \
	javax/naming/ldap/UnsolicitedNotificationListener.java
javax_naming_spi_SRCS = \
	javax/naming/spi/DirObjectFactory.java \
	javax/naming/spi/DirStateFactory.java \
	javax/naming/spi/DirectoryManager.java \
	javax/naming/spi/InitialContextFactory.java \
	javax/naming/spi/InitialContextFactoryBuilder.java \
	javax/naming/spi/NamingManager.java \
	javax/naming/spi/ObjectFactory.java \
	javax/naming/spi/ObjectFactoryBuilder.java \
	javax/naming/spi/ResolveResult.java \
	javax/naming/spi/Resolver.java \
	javax/naming/spi/StateFactory.java
javax_net_SRCS = \
	javax/net/ServerSocketFactory.java \
	javax/net/SocketFactory.java \
	javax/net/VanillaServerSocketFactory.java \
	javax/net/VanillaSocketFactory.java
javax_net_ssl_SRCS = \
	javax/net/ssl/Engine.java \
	javax/net/ssl/HandshakeCompletedEvent.java \
	javax/net/ssl/HandshakeCompletedListener.java \
	javax/net/ssl/HostnameVerifier.java \
	javax/net/ssl/HttpsURLConnection.java \
	javax/net/ssl/KeyManager.java \
	javax/net/ssl/KeyManagerFactory.java \
	javax/net/ssl/KeyManagerFactorySpi.java \
	javax/net/ssl/ManagerFactoryParameters.java \
	javax/net/ssl/SSLContext.java \
	javax/net/ssl/SSLContextSpi.java \
	javax/net/ssl/SSLException.java \
	javax/net/ssl/SSLHandshakeException.java \
	javax/net/ssl/SSLKeyException.java \
	javax/net/ssl/SSLPeerUnverifiedException.java \
	javax/net/ssl/SSLPermission.java \
	javax/net/ssl/SSLProtocolException.java \
	javax/net/ssl/SSLServerSocket.java \
	javax/net/ssl/SSLServerSocketFactory.java \
	javax/net/ssl/SSLSession.java \
	javax/net/ssl/SSLSessionBindingEvent.java \
	javax/net/ssl/SSLSessionBindingListener.java \
	javax/net/ssl/SSLSessionContext.java \
	javax/net/ssl/SSLSocket.java \
	javax/net/ssl/SSLSocketFactory.java \
	javax/net/ssl/TrivialHostnameVerifier.java \
	javax/net/ssl/TrustManager.java \
	javax/net/ssl/TrustManagerFactory.java \
	javax/net/ssl/TrustManagerFactorySpi.java \
	javax/net/ssl/X509KeyManager.java \
	javax/net/ssl/X509TrustManager.java
javax_rmi_SRCS = \
	javax/rmi/BAD_OPERATION.java \
	javax/rmi/ORB.java \
	javax/rmi/PortableRemoteObject.java
javax_rmi_CORBA_SRCS = \
	javax/rmi/CORBA/ClassDesc.java \
	javax/rmi/CORBA/ObjectImpl.java \
	javax/rmi/CORBA/PortableRemoteObjectDelegate.java \
	javax/rmi/CORBA/Stub.java \
	javax/rmi/CORBA/StubDelegate.java \
	javax/rmi/CORBA/SystemException.java \
	javax/rmi/CORBA/Tie.java \
	javax/rmi/CORBA/Util.java \
	javax/rmi/CORBA/UtilDelegate.java \
	javax/rmi/CORBA/ValueHandler.java
javax_security_auth_x500_SRCS = \
	javax/security/auth/x500/X500Principal.java
javax_security_cert_SRCS = \
	javax/security/cert/Certificate.java \
	javax/security/cert/CertificateEncodingException.java \
	javax/security/cert/CertificateException.java \
	javax/security/cert/CertificateExpiredException.java \
	javax/security/cert/CertificateNotYetValidException.java \
	javax/security/cert/CertificateParsingException.java \
	javax/security/cert/X509CertBridge.java \
	javax/security/cert/X509Certificate.java
javax_sound_midi_SRCS = \
	javax/sound/midi/ControllerEventListener.java \
	javax/sound/midi/Instrument.java \
	javax/sound/midi/InvalidMidiDataException.java \
	javax/sound/midi/MetaEventListener.java \
	javax/sound/midi/MetaMessage.java \
	javax/sound/midi/MidiChannel.java \
	javax/sound/midi/MidiDevice.java \
	javax/sound/midi/MidiEvent.java \
	javax/sound/midi/MidiFileFormat.java \
	javax/sound/midi/MidiMessage.java \
	javax/sound/midi/MidiSystem.java \
	javax/sound/midi/MidiUnavailableException.java \
	javax/sound/midi/Patch.java \
	javax/sound/midi/Receiver.java \
	javax/sound/midi/Sequence.java \
	javax/sound/midi/Sequencer.java \
	javax/sound/midi/ShortMessage.java \
	javax/sound/midi/Soundbank.java \
	javax/sound/midi/SoundbankResource.java \
	javax/sound/midi/Synthesizer.java \
	javax/sound/midi/SysexMessage.java \
	javax/sound/midi/Track.java \
	javax/sound/midi/Transmitter.java \
	javax/sound/midi/VoiceStatus.java
javax_sound_midi_spi_SRCS = \
	javax/sound/midi/spi/MidiDeviceProvider.java \
	javax/sound/midi/spi/MidiFileReader.java \
	javax/sound/midi/spi/MidiFileWriter.java \
	javax/sound/midi/spi/SoundbankReader.java
javax_sound_sampled_SRCS = \
	javax/sound/sampled/AudioFileFormat.java \
	javax/sound/sampled/AudioFormat.java \
	javax/sound/sampled/AudioInputStream.java \
	javax/sound/sampled/AudioPermission.java \
	javax/sound/sampled/AudioSystem.java \
	javax/sound/sampled/BooleanControl.java \
	javax/sound/sampled/Clip.java \
	javax/sound/sampled/CompoundControl.java \
	javax/sound/sampled/Control.java \
	javax/sound/sampled/DataLine.java \
	javax/sound/sampled/EnumControl.java \
	javax/sound/sampled/FloatControl.java \
	javax/sound/sampled/Line.java \
	javax/sound/sampled/LineEvent.java \
	javax/sound/sampled/LineListener.java \
	javax/sound/sampled/LineUnavailableException.java \
	javax/sound/sampled/Mixer.java \
	javax/sound/sampled/Port.java \
	javax/sound/sampled/ReverbType.java \
	javax/sound/sampled/SourceDataLine.java \
	javax/sound/sampled/TargetDataLine.java \
	javax/sound/sampled/UnsupportedAudioFileException.java
javax_sound_sampled_spi_SRCS = \
	javax/sound/sampled/spi/AudioFileReader.java \
	javax/sound/sampled/spi/AudioFileWriter.java \
	javax/sound/sampled/spi/FormatConversionProvider.java \
	javax/sound/sampled/spi/MixerProvider.java
javax_sql_SRCS = \
	javax/sql/ConnectionEvent.java \
	javax/sql/ConnectionEventListener.java \
	javax/sql/ConnectionPoolDataSource.java \
	javax/sql/DataSource.java \
	javax/sql/PooledConnection.java \
	javax/sql/RowSet.java \
	javax/sql/RowSetEvent.java \
	javax/sql/RowSetInternal.java \
	javax/sql/RowSetListener.java \
	javax/sql/RowSetMetaData.java \
	javax/sql/RowSetReader.java \
	javax/sql/RowSetWriter.java \
	javax/sql/XAConnection.java \
	javax/sql/XADataSource.java
javax_swing_event_SRCS = \
	javax/swing/event/EventListenerList.java
javax_swing_text_SRCS = \
	javax/swing/text/AttributeSet.java \
	javax/swing/text/BadLocationException.java
javax_transaction_SRCS = \
	javax/transaction/HeuristicCommitException.java \
	javax/transaction/HeuristicMixedException.java \
	javax/transaction/HeuristicRollbackException.java \
	javax/transaction/InvalidTransactionException.java \
	javax/transaction/NotSupportedException.java \
	javax/transaction/RollbackException.java \
	javax/transaction/Status.java \
	javax/transaction/Synchronization.java \
	javax/transaction/SystemException.java \
	javax/transaction/Transaction.java \
	javax/transaction/TransactionManager.java \
	javax/transaction/TransactionRequiredException.java \
	javax/transaction/TransactionRolledbackException.java \
	javax/transaction/UserTransaction.java
javax_transaction_xa_SRCS = \
	javax/transaction/xa/XAException.java \
	javax/transaction/xa/XAResource.java \
	javax/transaction/xa/Xid.java
javax_xml_parsers_SRCS = \
	javax/xml/parsers/ClassStuff.java \
	javax/xml/parsers/DocumentBuilder.java \
	javax/xml/parsers/DocumentBuilderFactory.java \
	javax/xml/parsers/FactoryConfigurationError.java \
	javax/xml/parsers/ParserConfigurationException.java \
	javax/xml/parsers/SAXParser.java \
	javax/xml/parsers/SAXParserFactory.java
javax_xml_transform_SRCS = \
	javax/xml/transform/ClassStuff.java \
	javax/xml/transform/ErrorListener.java \
	javax/xml/transform/OutputKeys.java \
	javax/xml/transform/Result.java \
	javax/xml/transform/Source.java \
	javax/xml/transform/SourceLocator.java \
	javax/xml/transform/Templates.java \
	javax/xml/transform/Transformer.java \
	javax/xml/transform/TransformerConfigurationException.java \
	javax/xml/transform/TransformerException.java \
	javax/xml/transform/TransformerFactory.java \
	javax/xml/transform/TransformerFactoryConfigurationError.java \
	javax/xml/transform/URIResolver.java
javax_xml_transform_dom_SRCS = \
	javax/xml/transform/dom/DOMLocator.java \
	javax/xml/transform/dom/DOMResult.java \
	javax/xml/transform/dom/DOMSource.java
javax_xml_transform_sax_SRCS = \
	javax/xml/transform/sax/SAXResult.java \
	javax/xml/transform/sax/SAXSource.java \
	javax/xml/transform/sax/SAXTransformerFactory.java \
	javax/xml/transform/sax/TemplatesHandler.java \
	javax/xml/transform/sax/TransformerHandler.java
javax_xml_transform_stream_SRCS = \
	javax/xml/transform/stream/StreamResult.java \
	javax/xml/transform/stream/StreamSource.java
kaffe_applet_SRCS = \
	kaffe/applet/AppletTag.java \
	kaffe/applet/AppletViewer.java \
	kaffe/applet/AudioPlayer.java
kaffe_awt_SRCS = \
	kaffe/awt/ExportTransferable.java \
	kaffe/awt/FlavorConverter.java \
	kaffe/awt/FocusHook.java \
	kaffe/awt/HookInstaller.java \
	kaffe/awt/ImageClass.java \
	kaffe/awt/ImageDict.java \
	kaffe/awt/ImageSpec.java \
	kaffe/awt/JavaColorModel.java \
	kaffe/awt/KeyHook.java \
	kaffe/awt/MouseHook.java \
	kaffe/awt/ObjectSelection.java \
	kaffe/awt/OpaqueComponent.java \
	kaffe/awt/SerializerConverter.java
kaffe_beans_editors_SRCS = \
	kaffe/beans/editors/BooleanEditor.java \
	kaffe/beans/editors/ByteEditor.java \
	kaffe/beans/editors/DoubleEditor.java \
	kaffe/beans/editors/FloatEditor.java \
	kaffe/beans/editors/IntegerEditor.java \
	kaffe/beans/editors/LongEditor.java \
	kaffe/beans/editors/ShortEditor.java \
	kaffe/beans/editors/StringEditor.java
kaffe_io_SRCS = \
	kaffe/io/AccessibleBAOStream.java \
	kaffe/io/ByteToChar8859_1.java \
	kaffe/io/ByteToCharConverter.java \
	kaffe/io/ByteToCharDefault.java \
	kaffe/io/ByteToCharHashBased.java \
	kaffe/io/ByteToCharIconv.java \
	kaffe/io/ByteToCharMS932.java \
	kaffe/io/ByteToCharSHIFT_JIS.java \
	kaffe/io/ByteToCharUTF8.java \
	kaffe/io/CharToByte8859_1.java \
	kaffe/io/CharToByteConverter.java \
	kaffe/io/CharToByteDefault.java \
	kaffe/io/CharToByteHashBased.java \
	kaffe/io/CharToByteIconv.java \
	kaffe/io/CharToByteMS932.java \
	kaffe/io/CharToByteSHIFT_JIS.java \
	kaffe/io/CharToByteUTF8.java \
	kaffe/io/CircularBuffer.java \
	kaffe/io/ClassLoaderObjectInputStream.java \
	kaffe/io/ConverterAlias.java \
	kaffe/io/NullOutputStream.java \
	kaffe/io/StdErrorStream.java \
	kaffe/io/StdInputStream.java \
	kaffe/io/StdOutputStream.java
kaffe_jar_SRCS = \
	kaffe/jar/ExecJar.java \
	kaffe/jar/ExecJarName.java
kaffe_lang_SRCS = \
	kaffe/lang/AppClassLoader.java \
	kaffe/lang/ClassPathReader.java \
	kaffe/lang/MemoryAdvice.java \
	kaffe/lang/MemoryAdviceCallback.java \
	kaffe/lang/PackageHelper.java \
	kaffe/lang/PrimordialClassLoader.java \
	kaffe/lang/ResourceReader.java \
	kaffe/lang/ThreadStack.java \
	kaffe/lang/UNIXProcess.java
kaffe_management_SRCS = \
	kaffe/management/Classpath.java \
	kaffe/management/Debug.java \
	kaffe/management/JIT.java \
	kaffe/management/XProfiler.java
kaffe_security_SRCS = \
	kaffe/security/LameRandomness.java \
	kaffe/security/Randomness.java \
	kaffe/security/UnixRandomness.java
kaffe_security_provider_SRCS = \
	kaffe/security/provider/Kaffe.java \
	kaffe/security/provider/MD2.java \
	kaffe/security/provider/MD4.java \
	kaffe/security/provider/MD5.java \
	kaffe/security/provider/SHA.java \
	kaffe/security/provider/SHA1PRNG.java \
	kaffe/security/provider/UpdateDigest.java
kaffe_tools_jar_SRCS = \
	kaffe/tools/jar/Jar.java \
	kaffe/tools/jar/XPFile.java \
	kaffe/tools/jar/XPFileInputStream.java \
	kaffe/tools/jar/XPFileOutputStream.java \
	kaffe/tools/jar/XPFileReader.java \
	kaffe/tools/jar/XPFileWriter.java
kaffe_tools_native2ascii_SRCS = \
	kaffe/tools/native2ascii/Native2ASCII.java
kaffe_tools_serialver_SRCS = \
	kaffe/tools/serialver/SerialVer.java
kaffe_util_SRCS = \
	kaffe/util/Assert.java \
	kaffe/util/Base64.java \
	kaffe/util/DateParser.java \
	kaffe/util/DateScanner.java \
	kaffe/util/Deprecated.java \
	kaffe/util/DoubleLinkedObject.java \
	kaffe/util/IdentityHashtable.java \
	kaffe/util/IntegerHashtable.java \
	kaffe/util/IntegerIntegerHashtable.java \
	kaffe/util/LongHashtable.java \
	kaffe/util/NotImplemented.java \
	kaffe/util/NotImplementedReport.java \
	kaffe/util/Ptr.java \
	kaffe/util/SupportDisabled.java \
	kaffe/util/Timer.java \
	kaffe/util/TimerClient.java \
	kaffe/util/UNIXTimeZone.java \
	kaffe/util/UTF8.java \
	kaffe/util/VectorSnapshot.java
kaffe_util_locale_SRCS = \
	kaffe/util/locale/Country.java \
	kaffe/util/locale/CountryCodes.java \
	kaffe/util/locale/Country_de.java \
	kaffe/util/locale/Country_en.java \
	kaffe/util/locale/Country_fr.java \
	kaffe/util/locale/Country_it.java \
	kaffe/util/locale/Country_ja.java \
	kaffe/util/locale/Country_ko.java \
	kaffe/util/locale/Country_zh.java \
	kaffe/util/locale/Language.java \
	kaffe/util/locale/LanguageCodes.java \
	kaffe/util/locale/Language_de.java \
	kaffe/util/locale/Language_en.java \
	kaffe/util/locale/Language_fr.java \
	kaffe/util/locale/Language_it.java \
	kaffe/util/locale/Language_ja.java \
	kaffe/util/locale/Language_ko.java \
	kaffe/util/locale/Language_zh.java
kaffe_util_log_SRCS = \
	kaffe/util/log/LogClient.java \
	kaffe/util/log/LogStream.java \
	kaffe/util/log/SimpleLogViewer.java
kaffe_util_zip_SRCS = \
	kaffe/util/zip/SwitchInflater.java
org_tritonus_core_SRCS = \
	org/tritonus/core/Service.java \
	org/tritonus/core/TAudioConfig.java \
	org/tritonus/core/TInit.java \
	org/tritonus/core/TMidiConfig.java
org_tritonus_lowlevel_alsa_SRCS = \
	org/tritonus/lowlevel/alsa/Alsa.java \
	org/tritonus/lowlevel/alsa/AlsaCtl.java \
	org/tritonus/lowlevel/alsa/AlsaCtlCardInfo.java \
	org/tritonus/lowlevel/alsa/AlsaMixer.java \
	org/tritonus/lowlevel/alsa/AlsaMixerElement.java \
	org/tritonus/lowlevel/alsa/AlsaPcm.java \
	org/tritonus/lowlevel/alsa/AlsaPcmHWParams.java \
	org/tritonus/lowlevel/alsa/AlsaPcmHWParamsFormatMask.java \
	org/tritonus/lowlevel/alsa/AlsaPcmSWParams.java \
	org/tritonus/lowlevel/alsa/AlsaSeq.java \
	org/tritonus/lowlevel/alsa/AlsaSeqClientInfo.java \
	org/tritonus/lowlevel/alsa/AlsaSeqEvent.java \
	org/tritonus/lowlevel/alsa/AlsaSeqPortInfo.java \
	org/tritonus/lowlevel/alsa/AlsaSeqPortSubscribe.java \
	org/tritonus/lowlevel/alsa/AlsaSeqQueueInfo.java \
	org/tritonus/lowlevel/alsa/AlsaSeqQueueStatus.java \
	org/tritonus/lowlevel/alsa/AlsaSeqQueueTempo.java \
	org/tritonus/lowlevel/alsa/AlsaSeqQueueTimer.java \
	org/tritonus/lowlevel/alsa/AlsaSeqRemoveEvents.java \
	org/tritonus/lowlevel/alsa/AlsaSeqSystemInfo.java
org_tritonus_lowlevel_esd_SRCS = \
	org/tritonus/lowlevel/esd/Esd.java \
	org/tritonus/lowlevel/esd/EsdRecordingStream.java \
	org/tritonus/lowlevel/esd/EsdSample.java \
	org/tritonus/lowlevel/esd/EsdStream.java
org_tritonus_midi_device_alsa_SRCS = \
	org/tritonus/midi/device/alsa/AlsaMidiChannel.java \
	org/tritonus/midi/device/alsa/AlsaMidiDevice.java \
	org/tritonus/midi/device/alsa/AlsaMidiDeviceProvider.java \
	org/tritonus/midi/device/alsa/AlsaMidiIn.java \
	org/tritonus/midi/device/alsa/AlsaMidiOut.java \
	org/tritonus/midi/device/alsa/AlsaQueueHolder.java \
	org/tritonus/midi/device/alsa/AlsaReceiver.java \
	org/tritonus/midi/device/alsa/AlsaSequencer.java \
	org/tritonus/midi/device/alsa/AlsaSequencerProvider.java \
	org/tritonus/midi/device/alsa/AlsaSequencerReceiver.java \
	org/tritonus/midi/device/alsa/AlsaSynthesizer.java
org_tritonus_midi_file_SRCS = \
	org/tritonus/midi/file/MidiConstants.java \
	org/tritonus/midi/file/StandardMidiFileReader.java \
	org/tritonus/midi/file/StandardMidiFileWriter.java
org_tritonus_sampled_convert_SRCS = \
	org/tritonus/sampled/convert/AlawFormatConversionProvider.java \
	org/tritonus/sampled/convert/PCM2PCMConversionProvider.java \
	org/tritonus/sampled/convert/SampleRateConversionProvider.java \
	org/tritonus/sampled/convert/SmartFormatConversionProvider.java \
	org/tritonus/sampled/convert/UlawFormatConversionProvider.java
org_tritonus_sampled_file_SRCS = \
	org/tritonus/sampled/file/AiffAudioFileReader.java \
	org/tritonus/sampled/file/AiffAudioFileWriter.java \
	org/tritonus/sampled/file/AiffAudioOutputStream.java \
	org/tritonus/sampled/file/AiffTool.java \
	org/tritonus/sampled/file/AuAudioFileReader.java \
	org/tritonus/sampled/file/AuAudioFileWriter.java \
	org/tritonus/sampled/file/AuAudioOutputStream.java \
	org/tritonus/sampled/file/AuTool.java \
	org/tritonus/sampled/file/WaveAudioFileReader.java \
	org/tritonus/sampled/file/WaveAudioFileWriter.java \
	org/tritonus/sampled/file/WaveAudioOutputStream.java \
	org/tritonus/sampled/file/WaveTool.java
org_tritonus_sampled_mixer_alsa_SRCS = \
	org/tritonus/sampled/mixer/alsa/AlsaBaseDataLine.java \
	org/tritonus/sampled/mixer/alsa/AlsaDataLineMixer.java \
	org/tritonus/sampled/mixer/alsa/AlsaDataLineMixerProvider.java \
	org/tritonus/sampled/mixer/alsa/AlsaPortMixer.java \
	org/tritonus/sampled/mixer/alsa/AlsaPortMixerProvider.java \
	org/tritonus/sampled/mixer/alsa/AlsaSourceDataLine.java \
	org/tritonus/sampled/mixer/alsa/AlsaTargetDataLine.java \
	org/tritonus/sampled/mixer/alsa/AlsaUtils.java
org_tritonus_sampled_mixer_esd_SRCS = \
	org/tritonus/sampled/mixer/esd/EsdClip.java \
	org/tritonus/sampled/mixer/esd/EsdMixer.java \
	org/tritonus/sampled/mixer/esd/EsdMixerProvider.java \
	org/tritonus/sampled/mixer/esd/EsdSourceDataLine.java \
	org/tritonus/sampled/mixer/esd/EsdTargetDataLine.java \
	org/tritonus/sampled/mixer/esd/EsdUtils.java
org_tritonus_share_SRCS = \
	org/tritonus/share/ArraySet.java \
	org/tritonus/share/GlobalInfo.java \
	org/tritonus/share/StringHashedSet.java \
	org/tritonus/share/TCircularBuffer.java \
	org/tritonus/share/TDebug.java \
	org/tritonus/share/TNotifier.java \
	org/tritonus/share/TSettings.java
org_tritonus_share_midi_SRCS = \
	org/tritonus/share/midi/MidiUtils.java \
	org/tritonus/share/midi/TMidiDevice.java \
	org/tritonus/share/midi/TMidiFileFormat.java \
	org/tritonus/share/midi/TSequencer.java
org_tritonus_share_sampled_SRCS = \
	org/tritonus/share/sampled/AudioFileTypes.java \
	org/tritonus/share/sampled/AudioFormatSet.java \
	org/tritonus/share/sampled/AudioFormats.java \
	org/tritonus/share/sampled/AudioSystemShadow.java \
	org/tritonus/share/sampled/AudioUtils.java \
	org/tritonus/share/sampled/Encodings.java \
	org/tritonus/share/sampled/FloatSampleBuffer.java \
	org/tritonus/share/sampled/TConversionTool.java \
	org/tritonus/share/sampled/TVolumeUtils.java
org_tritonus_share_sampled_convert_SRCS = \
	org/tritonus/share/sampled/convert/TAsynchronousFilteredAudioInputStream.java \
	org/tritonus/share/sampled/convert/TAudioInputStream.java \
	org/tritonus/share/sampled/convert/TEncodingFormatConversionProvider.java \
	org/tritonus/share/sampled/convert/TFormatConversionProvider.java \
	org/tritonus/share/sampled/convert/TMatrixFormatConversionProvider.java \
	org/tritonus/share/sampled/convert/TSimpleFormatConversionProvider.java \
	org/tritonus/share/sampled/convert/TSynchronousFilteredAudioInputStream.java
org_tritonus_share_sampled_file_SRCS = \
	org/tritonus/share/sampled/file/AudioOutputStream.java \
	org/tritonus/share/sampled/file/HeaderlessAudioOutputStream.java \
	org/tritonus/share/sampled/file/TAudioFileFormat.java \
	org/tritonus/share/sampled/file/TAudioFileReader.java \
	org/tritonus/share/sampled/file/TAudioFileWriter.java \
	org/tritonus/share/sampled/file/TAudioOutputStream.java \
	org/tritonus/share/sampled/file/TDataOutputStream.java \
	org/tritonus/share/sampled/file/THeaderlessAudioFileWriter.java \
	org/tritonus/share/sampled/file/TNonSeekableDataOutputStream.java \
	org/tritonus/share/sampled/file/TSeekableDataOutputStream.java
org_tritonus_share_sampled_mixer_SRCS = \
	org/tritonus/share/sampled/mixer/TBaseDataLine.java \
	org/tritonus/share/sampled/mixer/TBooleanControl.java \
	org/tritonus/share/sampled/mixer/TClip.java \
	org/tritonus/share/sampled/mixer/TCompoundControl.java \
	org/tritonus/share/sampled/mixer/TCompoundControlType.java \
	org/tritonus/share/sampled/mixer/TControlController.java \
	org/tritonus/share/sampled/mixer/TControllable.java \
	org/tritonus/share/sampled/mixer/TDataLine.java \
	org/tritonus/share/sampled/mixer/TEnumControl.java \
	org/tritonus/share/sampled/mixer/TFloatControl.java \
	org/tritonus/share/sampled/mixer/TLine.java \
	org/tritonus/share/sampled/mixer/TMixer.java \
	org/tritonus/share/sampled/mixer/TMixerInfo.java \
	org/tritonus/share/sampled/mixer/TMixerProvider.java \
	org/tritonus/share/sampled/mixer/TPort.java \
	org/tritonus/share/sampled/mixer/TSoftClip.java
org_w3c_dom_SRCS = \
	org/w3c/dom/Attr.java \
	org/w3c/dom/CDATASection.java \
	org/w3c/dom/CharacterData.java \
	org/w3c/dom/Comment.java \
	org/w3c/dom/DOMException.java \
	org/w3c/dom/DOMImplementation.java \
	org/w3c/dom/Document.java \
	org/w3c/dom/DocumentFragment.java \
	org/w3c/dom/DocumentType.java \
	org/w3c/dom/Element.java \
	org/w3c/dom/Entity.java \
	org/w3c/dom/EntityReference.java \
	org/w3c/dom/NamedNodeMap.java \
	org/w3c/dom/Node.java \
	org/w3c/dom/NodeList.java \
	org/w3c/dom/Notation.java \
	org/w3c/dom/ProcessingInstruction.java \
	org/w3c/dom/Text.java
org_w3c_dom_css_SRCS = \
	org/w3c/dom/css/CSS2Azimuth.java \
	org/w3c/dom/css/CSS2BackgroundPosition.java \
	org/w3c/dom/css/CSS2BorderSpacing.java \
	org/w3c/dom/css/CSS2CounterIncrement.java \
	org/w3c/dom/css/CSS2CounterReset.java \
	org/w3c/dom/css/CSS2Cursor.java \
	org/w3c/dom/css/CSS2FontFaceSrc.java \
	org/w3c/dom/css/CSS2FontFaceWidths.java \
	org/w3c/dom/css/CSS2PageSize.java \
	org/w3c/dom/css/CSS2PlayDuring.java \
	org/w3c/dom/css/CSS2Properties.java \
	org/w3c/dom/css/CSS2TextShadow.java \
	org/w3c/dom/css/CSSCharsetRule.java \
	org/w3c/dom/css/CSSFontFaceRule.java \
	org/w3c/dom/css/CSSImportRule.java \
	org/w3c/dom/css/CSSMediaRule.java \
	org/w3c/dom/css/CSSPageRule.java \
	org/w3c/dom/css/CSSPrimitiveValue.java \
	org/w3c/dom/css/CSSRule.java \
	org/w3c/dom/css/CSSRuleList.java \
	org/w3c/dom/css/CSSStyleDeclaration.java \
	org/w3c/dom/css/CSSStyleRule.java \
	org/w3c/dom/css/CSSStyleSheet.java \
	org/w3c/dom/css/CSSUnknownRule.java \
	org/w3c/dom/css/CSSValue.java \
	org/w3c/dom/css/CSSValueList.java \
	org/w3c/dom/css/Counter.java \
	org/w3c/dom/css/DOMImplementationCSS.java \
	org/w3c/dom/css/DocumentCSS.java \
	org/w3c/dom/css/ElementCSSInlineStyle.java \
	org/w3c/dom/css/RGBColor.java \
	org/w3c/dom/css/Rect.java \
	org/w3c/dom/css/ViewCSS.java
org_w3c_dom_events_SRCS = \
	org/w3c/dom/events/DocumentEvent.java \
	org/w3c/dom/events/Event.java \
	org/w3c/dom/events/EventException.java \
	org/w3c/dom/events/EventListener.java \
	org/w3c/dom/events/EventTarget.java \
	org/w3c/dom/events/MouseEvent.java \
	org/w3c/dom/events/MutationEvent.java \
	org/w3c/dom/events/UIEvent.java
org_w3c_dom_html_SRCS = \
	org/w3c/dom/html/HTMLAnchorElement.java \
	org/w3c/dom/html/HTMLAppletElement.java \
	org/w3c/dom/html/HTMLAreaElement.java \
	org/w3c/dom/html/HTMLBRElement.java \
	org/w3c/dom/html/HTMLBaseElement.java \
	org/w3c/dom/html/HTMLBaseFontElement.java \
	org/w3c/dom/html/HTMLBodyElement.java \
	org/w3c/dom/html/HTMLButtonElement.java \
	org/w3c/dom/html/HTMLCollection.java \
	org/w3c/dom/html/HTMLDListElement.java \
	org/w3c/dom/html/HTMLDOMImplementation.java \
	org/w3c/dom/html/HTMLDirectoryElement.java \
	org/w3c/dom/html/HTMLDivElement.java \
	org/w3c/dom/html/HTMLDocument.java \
	org/w3c/dom/html/HTMLElement.java \
	org/w3c/dom/html/HTMLFieldSetElement.java \
	org/w3c/dom/html/HTMLFontElement.java \
	org/w3c/dom/html/HTMLFormElement.java \
	org/w3c/dom/html/HTMLFrameElement.java \
	org/w3c/dom/html/HTMLFrameSetElement.java \
	org/w3c/dom/html/HTMLHRElement.java \
	org/w3c/dom/html/HTMLHeadElement.java \
	org/w3c/dom/html/HTMLHeadingElement.java \
	org/w3c/dom/html/HTMLHtmlElement.java \
	org/w3c/dom/html/HTMLIFrameElement.java \
	org/w3c/dom/html/HTMLImageElement.java \
	org/w3c/dom/html/HTMLInputElement.java \
	org/w3c/dom/html/HTMLIsIndexElement.java \
	org/w3c/dom/html/HTMLLIElement.java \
	org/w3c/dom/html/HTMLLabelElement.java \
	org/w3c/dom/html/HTMLLegendElement.java \
	org/w3c/dom/html/HTMLLinkElement.java \
	org/w3c/dom/html/HTMLMapElement.java \
	org/w3c/dom/html/HTMLMenuElement.java \
	org/w3c/dom/html/HTMLMetaElement.java \
	org/w3c/dom/html/HTMLModElement.java \
	org/w3c/dom/html/HTMLOListElement.java \
	org/w3c/dom/html/HTMLObjectElement.java \
	org/w3c/dom/html/HTMLOptGroupElement.java \
	org/w3c/dom/html/HTMLOptionElement.java \
	org/w3c/dom/html/HTMLParagraphElement.java \
	org/w3c/dom/html/HTMLParamElement.java \
	org/w3c/dom/html/HTMLPreElement.java \
	org/w3c/dom/html/HTMLQuoteElement.java \
	org/w3c/dom/html/HTMLScriptElement.java \
	org/w3c/dom/html/HTMLSelectElement.java \
	org/w3c/dom/html/HTMLStyleElement.java \
	org/w3c/dom/html/HTMLTableCaptionElement.java \
	org/w3c/dom/html/HTMLTableCellElement.java \
	org/w3c/dom/html/HTMLTableColElement.java \
	org/w3c/dom/html/HTMLTableElement.java \
	org/w3c/dom/html/HTMLTableRowElement.java \
	org/w3c/dom/html/HTMLTableSectionElement.java \
	org/w3c/dom/html/HTMLTextAreaElement.java \
	org/w3c/dom/html/HTMLTitleElement.java \
	org/w3c/dom/html/HTMLUListElement.java
org_w3c_dom_ranges_SRCS = \
	org/w3c/dom/ranges/DocumentRange.java \
	org/w3c/dom/ranges/Range.java \
	org/w3c/dom/ranges/RangeException.java
org_w3c_dom_stylesheets_SRCS = \
	org/w3c/dom/stylesheets/DocumentStyle.java \
	org/w3c/dom/stylesheets/LinkStyle.java \
	org/w3c/dom/stylesheets/MediaList.java \
	org/w3c/dom/stylesheets/StyleSheet.java \
	org/w3c/dom/stylesheets/StyleSheetList.java
org_w3c_dom_traversal_SRCS = \
	org/w3c/dom/traversal/DocumentTraversal.java \
	org/w3c/dom/traversal/NodeFilter.java \
	org/w3c/dom/traversal/NodeIterator.java \
	org/w3c/dom/traversal/TreeWalker.java
org_w3c_dom_views_SRCS = \
	org/w3c/dom/views/AbstractView.java \
	org/w3c/dom/views/DocumentView.java
org_xml_sax_SRCS = \
	org/xml/sax/AttributeList.java \
	org/xml/sax/Attributes.java \
	org/xml/sax/ContentHandler.java \
	org/xml/sax/DTDHandler.java \
	org/xml/sax/DocumentHandler.java \
	org/xml/sax/EntityResolver.java \
	org/xml/sax/ErrorHandler.java \
	org/xml/sax/HandlerBase.java \
	org/xml/sax/InputSource.java \
	org/xml/sax/Locator.java \
	org/xml/sax/Parser.java \
	org/xml/sax/SAXException.java \
	org/xml/sax/SAXNotRecognizedException.java \
	org/xml/sax/SAXNotSupportedException.java \
	org/xml/sax/SAXParseException.java \
	org/xml/sax/XMLFilter.java \
	org/xml/sax/XMLReader.java
org_xml_sax_ext_SRCS = \
	org/xml/sax/ext/Attributes2.java \
	org/xml/sax/ext/Attributes2Impl.java \
	org/xml/sax/ext/DeclHandler.java \
	org/xml/sax/ext/DefaultHandler2.java \
	org/xml/sax/ext/EntityResolver2.java \
	org/xml/sax/ext/LexicalHandler.java \
	org/xml/sax/ext/Locator2.java \
	org/xml/sax/ext/Locator2Impl.java
org_xml_sax_helpers_SRCS = \
	org/xml/sax/helpers/AttributeListImpl.java \
	org/xml/sax/helpers/AttributesImpl.java \
	org/xml/sax/helpers/DefaultHandler.java \
	org/xml/sax/helpers/LocatorImpl.java \
	org/xml/sax/helpers/NamespaceSupport.java \
	org/xml/sax/helpers/NewInstance.java \
	org/xml/sax/helpers/ParserAdapter.java \
	org/xml/sax/helpers/ParserFactory.java \
	org/xml/sax/helpers/XMLFilterImpl.java \
	org/xml/sax/helpers/XMLReaderAdapter.java \
	org/xml/sax/helpers/XMLReaderFactory.java
OMITTED_PACKAGES_SRCS = \
	$(java_awt_win32_SRCS)
java_awt_win32_SRCS = \
	java/awt/win32/ActionEvt.java \
	java/awt/win32/Button.java \
	java/awt/win32/Canvas.java \
	java/awt/win32/Checkbox.java \
	java/awt/win32/CheckboxMenuItem.java \
	java/awt/win32/Choice.java \
	java/awt/win32/Defaults.java \
	java/awt/win32/FileDialog.java \
	java/awt/win32/Frame.java \
	java/awt/win32/Label.java \
	java/awt/win32/List.java \
	java/awt/win32/Menu.java \
	java/awt/win32/MenuBar.java \
	java/awt/win32/MenuItem.java \
	java/awt/win32/NativeComponent.java \
	java/awt/win32/NativeContainer.java \
	java/awt/win32/NativeGraphics.java \
	java/awt/win32/Panel.java \
	java/awt/win32/PopupMenu.java \
	java/awt/win32/ScrollPane.java \
	java/awt/win32/Scrollbar.java \
	java/awt/win32/SystemColor.java \
	java/awt/win32/TextArea.java \
	java/awt/win32/TextComponent.java \
	java/awt/win32/TextField.java \
	java/awt/win32/Toolkit.java \
	java/awt/win32/WMEvent.java \
	java/awt/win32/Window.java


## Do not delete the previous blank line; update-class-list depends on it

serialized_converters = \
	kaffe/io/ByteToChar8859_2.ser \
	kaffe/io/ByteToChar8859_3.ser \
	kaffe/io/ByteToChar8859_4.ser \
	kaffe/io/ByteToChar8859_5.ser \
	kaffe/io/ByteToChar8859_6.ser \
	kaffe/io/ByteToChar8859_7.ser \
	kaffe/io/ByteToChar8859_8.ser \
	kaffe/io/ByteToChar8859_9.ser \
	kaffe/io/ByteToCharASCII.ser \
	kaffe/io/ByteToCharCP037.ser \
	kaffe/io/ByteToCharCP1026.ser \
	kaffe/io/ByteToCharCP1046.ser \
	kaffe/io/ByteToCharCP1250.ser \
	kaffe/io/ByteToCharCP1251.ser \
	kaffe/io/ByteToCharCP1252.ser \
	kaffe/io/ByteToCharCP1253.ser \
	kaffe/io/ByteToCharCP1254.ser \
	kaffe/io/ByteToCharCP1255.ser \
	kaffe/io/ByteToCharCP1256.ser \
	kaffe/io/ByteToCharCP1257.ser \
	kaffe/io/ByteToCharCP1258.ser \
	kaffe/io/ByteToCharCP850.ser \
	kaffe/io/ByteToCharCP852.ser \
	kaffe/io/ByteToCharCP855.ser \
	kaffe/io/ByteToCharCP857.ser \
	kaffe/io/ByteToCharCP860.ser \
	kaffe/io/ByteToCharCP861.ser \
	kaffe/io/ByteToCharCP862.ser \
	kaffe/io/ByteToCharCP863.ser \
	kaffe/io/ByteToCharCP864.ser \
	kaffe/io/ByteToCharCP865.ser \
	kaffe/io/ByteToCharCP866.ser \
	kaffe/io/ByteToCharCP868.ser \
	kaffe/io/ByteToCharCP869.ser \
	kaffe/io/ByteToCharCP870.ser \
	kaffe/io/ByteToCharCP871.ser \
	kaffe/io/ByteToCharCP875.ser \
	kaffe/io/ByteToCharCP918.ser \
	kaffe/io/ByteToCharKOI8_R.ser \
	kaffe/io/CharToByte8859_2.ser \
	kaffe/io/CharToByte8859_3.ser \
	kaffe/io/CharToByte8859_4.ser \
	kaffe/io/CharToByte8859_5.ser \
	kaffe/io/CharToByte8859_6.ser \
	kaffe/io/CharToByte8859_7.ser \
	kaffe/io/CharToByte8859_8.ser \
	kaffe/io/CharToByte8859_9.ser \
	kaffe/io/CharToByteASCII.ser \
	kaffe/io/CharToByteCP037.ser \
	kaffe/io/CharToByteCP1026.ser \
	kaffe/io/CharToByteCP1046.ser \
	kaffe/io/CharToByteCP1250.ser \
	kaffe/io/CharToByteCP1251.ser \
	kaffe/io/CharToByteCP1252.ser \
	kaffe/io/CharToByteCP1253.ser \
	kaffe/io/CharToByteCP1254.ser \
	kaffe/io/CharToByteCP1255.ser \
	kaffe/io/CharToByteCP1256.ser \
	kaffe/io/CharToByteCP1257.ser \
	kaffe/io/CharToByteCP1258.ser \
	kaffe/io/CharToByteCP850.ser \
	kaffe/io/CharToByteCP852.ser \
	kaffe/io/CharToByteCP855.ser \
	kaffe/io/CharToByteCP857.ser \
	kaffe/io/CharToByteCP860.ser \
	kaffe/io/CharToByteCP861.ser \
	kaffe/io/CharToByteCP862.ser \
	kaffe/io/CharToByteCP863.ser \
	kaffe/io/CharToByteCP864.ser \
	kaffe/io/CharToByteCP865.ser \
	kaffe/io/CharToByteCP866.ser \
	kaffe/io/CharToByteCP868.ser \
	kaffe/io/CharToByteCP869.ser \
	kaffe/io/CharToByteCP870.ser \
	kaffe/io/CharToByteCP871.ser \
	kaffe/io/CharToByteCP875.ser \
	kaffe/io/CharToByteCP918.ser \
	kaffe/io/CharToByteKOI8_R.ser

gnu_regexp_message_bundles = \
	gnu/regexp/MessagesBundle_fr.properties \
	gnu/regexp/MessagesBundle.properties

gnu_java_locale_bundles = \
	gnu/java/locale/iso3166-a3.properties \
	gnu/java/locale/iso3166_de.properties \
	gnu/java/locale/iso3166.properties \
	gnu/java/locale/iso639-a2-old.properties \
	gnu/java/locale/iso639-a3.properties \
	gnu/java/locale/iso639_de.properties \
	gnu/java/locale/iso639_fr.properties \
	gnu/java/locale/iso639_ga.properties \
	gnu/java/locale/iso639.properties

EXTRA_DIST = \
	Makefile.am.in \
	Klasses.jar.bootstrap \
        $(toolslib_DATA) \
	META-INF \
	gnu \
	java \
	javax \
	kaffe \
	org \
	pure-java \
	essential.files \
	pure_java_math.files \
	native_java_math.files \
	profiles \
        bootstrap.classlist \
	$(OMITTED_PACKAGES_SRCS) \
	$(serialized_converters) \
	$(gnu_regexp_message_bundles) \
	$(gnu_java_locale_bundles)

dist-hook:
	rm -rf `find $(distdir) -name CVS`
	rm -rf `find $(distdir) -name *~`

LIBDIR = lib

clean-local:
	-rm -rf $(LIBDIR) rt.jar

.PHONY: new-classes
new-classes: $(top_srcdir)/developers/update-class-list
	cd $(srcdir) && $(SHELL) $(top_srcdir)/developers/update-class-list

rebuildLib = ./rebuildLib

.PHONY: compile-classes classes
## classes is provided for backward compatibility; use compile-classes
compile-classes $(LIBDIR)/stamp classes: $(rebuildLib) $(Klasses_jar_SRCS)
	rm -rf $(LIBDIR)
	mkdir $(LIBDIR)
	$(SHELL) $(rebuildLib) @essential.files
	$(SHELL) $(rebuildLib) $(DNSJAVA_InetAddressImpl)
	$(SHELL) $(rebuildLib) @$(JAVA_MATH_FILES)
	for i in `cat $(srcdir)/profiles/$(PROFILE)/profile` ;  do \
		$(SHELL) $(rebuildLib) @profiles/$(PROFILE)/$$i || exit $$? ; \
	done
	echo timestamp > $(LIBDIR)/stamp

bootstrap: $(srcdir)/Klasses.jar.bootstrap rt.jar
	if [ -f $(srcdir)/Klasses.jar.bootstrap ]; then \
		mv -f $(srcdir)/Klasses.jar.bootstrap $(srcdir)/Klasses.jar.bootstrap.save; \
	fi
	(cd $(LIBDIR) && $(ZIP) -DX -r - `grep '\.class$$' $(abssrcdir)/libraries/javalib/bootstrap.classlist` \
	) > $(srcdir)/Klasses.jar.bootstrap

.PHONY: bootstrap
$(srcdir)/Klasses.jar.bootstrap: bootstrap.classlist

if HAVE_ZIP
JAR_CMD1 = (dstdir=`pwd`; cd $(srcdir) && $(ZIP) -r $$dstdir/rt.jar META-INF)
JAR_CMD2 = (cd $(LIBDIR) && $(ZIP) -DX -r ../rt.jar $(SRCDIRS))
else
JAR_CMD1 = $(JAR) -cvf rt.jar -C $(LIBDIR) $(SRCDIRS)
JAR_CMD2 = $(JAR) -uvf rt.jar -C $(srcdir) META-INF/
endif

.PHONY: jar-classes
if USE_PRECOMPILED_RT_JAR
jar-classes rt.jar:
	cp $(PATH_TO_RT_JAR) rt.jar
else
jar-classes rt.jar: $(LIBDIR)/stamp $(Klasses_jar_SRCS)
	if [ ! -d $(LIBDIR)/kaffe ] ; then \
	       mkdir $(LIBDIR)/kaffe ; \
	       mkdir $(LIBDIR)/kaffe/io ; \
	fi
	for i in $(serialized_converters) ; do cp $(srcdir)/$$i $(LIBDIR)/$$i ; done
	if [ ! -d $(LIBDIR)/gnu ] ; then \
		mkdir $(LIBDIR)/gnu ; \
		mkdir $(LIBDIR)/gnu/regexp ; \
		mkdir $(LIBDIR)/gnu/java ; \
		mkdir $(LIBDIR)/gnu/java/locale ; \
	fi
	for i in $(gnu_regexp_message_bundles) $(gnu_java_locale_bundles) ; do cp $(srcdir)/$$i $(LIBDIR)/$$i ; done
	rm -f rt.jar
	$(JAR_CMD1)
	$(JAR_CMD2)
endif

.PHONY: build-classes Klasses
build-classes Klasses: rt.jar

all-am: rebuildLib
