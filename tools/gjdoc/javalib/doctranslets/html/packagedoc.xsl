<?xml version="1.0" encoding="utf-8"?>

<!-- packagedoc.xsl
     Copyright (C) 2003 Free Software Foundation, Inc.
     
     This file is part of GNU Classpath.
     
     GNU Classpath is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2, or (at your option)
     any later version.
      
     GNU Classpath is distributed in the hope that it will be useful, but
     WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     General Public License for more details.
     
     You should have received a copy of the GNU General Public License
     along with GNU Classpath; see the file COPYING.  If not, write to the
     Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
     02111-1307 USA.
     -->

<!-- Creates the package descriptor files for HTML documentation. 
     -->

<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:gjdoc="http://www.gnu.org/software/cp-tools/gjdocxml"
  xmlns:html="http://www.w3.org/TR/REC-html40"
  xmlns="http://www.w3.org/TR/REC-html40">

  <xsl:include href="html_common.xsl"/>

  <xsl:output method="xml"
    encoding="utf-8"
    indent="no"/>

  <xsl:strip-space elements="*"/>

  <xsl:template match="/">

    <html>
      <head>
        <title>
          <xsl:if test="document('index.xml',/)/gjdoc:rootdoc/gjdoc:title">
            <xsl:value-of select="document('index.xml',/)/gjdoc:rootdoc/gjdoc:title"/>
            <xsl:text> - </xsl:text>
          </xsl:if>
          <xsl:value-of select="@name"/>
          Package <xsl:value-of select="$gjdoc.outputfile.info"/>
        </title>
        <xsl:call-template name="include_common"/>
      </head>
      <body class="classdoc" onload="top.document.title=document.title;">

        <div class="header">
          <span class="navi-button">&#8658;&#160;<a href="{concat($gjdoc.pathtoroot, 'index.html')}" target="_top">Index&#160;(Frames)</a></span>&#160;|&#160;
          <span class="navi-button">&#8658;&#160;<a href="{concat($gjdoc.pathtoroot, 'index_noframes.html')}" target="_top">Index&#160;(No&#160;Frames)</a></span>&#160;|&#160;
          <span class="navi-button">&#8658;&#160;Package</span>&#160;|&#160;
          <span class="navi-button">&#8658;&#160;<a href="package-tree.html">Package&#160;Tree</a></span>&#160;|&#160;
          <span class="navi-button">&#8658;&#160;<a href="{concat($gjdoc.pathtoroot, 'fulltree.html')}">Full Tree</a></span>&#160;
        </div>

        <h1 class="classdoc-header">Package <xsl:value-of select="$gjdoc.outputfile.info"/></h1>

        <div class="classdoc-package-comment-body">
          <xsl:apply-templates select="/gjdoc:rootdoc/gjdoc:packagedoc[@name=$gjdoc.outputfile.info]/gjdoc:firstSentenceTags/node()"/>
        </div>

        <table border="1" cellspacing="0" width="100%" class="classdoc-table">
          <xsl:for-each select="/gjdoc:rootdoc/gjdoc:classdoc[gjdoc:containingPackage/@name=$gjdoc.outputfile.info]">
            <xsl:sort select="@name" order="ascending"/>
            <xsl:variable name="v_currentclass" select="@qualifiedtypename"/>
            <xsl:variable name="v_sub_xml_filename" select="concat(@qualifiedtypename,'.xml')"/>
            <tr>
              <td width="1%" align="right" valign="top" class="no-border-r">
                <a href="{concat(@name, '.html')}">
                  <xsl:value-of select="@name"/>
                </a>
              </td>
              <td align="left" valign="top" class="with-border">
                <xsl:variable name="v_docstring" select="document($v_sub_xml_filename,/gjdoc:rootdoc)//gjdoc:classdoc[@qualifiedtypename=$v_currentclass]/gjdoc:firstSentenceTags/node()"/>
                <xsl:choose>
                  <xsl:when test="$v_docstring">
                    <xsl:for-each select="$v_docstring">
                      <xsl:value-of select="." disable-output-escaping="yes"/>
                    </xsl:for-each>
                  </xsl:when>
                  <xsl:otherwise>
                    <i>No description available.</i>
                  </xsl:otherwise>
                </xsl:choose>
              </td>
            </tr>
          </xsl:for-each>
        </table>
        <xsl:call-template name="output_copyright_footer"/>
      </body>
    </html>

  </xsl:template>
</xsl:stylesheet>
