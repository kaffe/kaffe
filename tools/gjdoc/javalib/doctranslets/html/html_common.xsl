<?xml version="1.0" encoding="utf-8"?>

<!-- html_common.xsl
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

<!-- Common templates for HTML generation.
     -->

<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:gjdoc="http://www.gnu.org/software/cp-tools/gjdocxml"
  xmlns:html="http://www.w3.org/TR/REC-html40"
  xmlns="http://www.w3.org/TR/REC-html40">

  <xsl:include href="../gjdoc_common.xsl"/>

  <!-- HTML head directives to be included in all generated HTML files. -->

  <xsl:template name="include_common">
    <link rel="stylesheet" type="text/css" href="{concat($gjdoc.pathtoroot, 'gjdochtml.css')}"/>
  </xsl:template>

  <xsl:template name="output_copyright_footer">
    <xsl:copy-of select="document('index.xml', /)/gjdoc:rootdoc/gjdoc:bottomnote"/>
    <hr class="footer"/>
    <div class="footer">
      <xsl:text>Generated on</xsl:text><xsl:value-of select="document('index.xml', /)/gjdoc:rootdoc/gjdoc:created"/><xsl:text> by </xsl:text><a href="http://www.gnu.org/software/cp-tools" target="cptoolsinfo"><xsl:text>GNU Classpath Tools</xsl:text></a><xsl:text> (Gjdoc XmlDoclet </xsl:text><xsl:value-of select="$gjdoc.xmldoclet.version"/><xsl:text>).</xsl:text>
    </div>
  </xsl:template>

  <!-- If the given class is also included, create a link to it. -->
  <!-- Otherwise output the qualified name in plain text. -->
  
  <xsl:template name="link_to_class_full">
    <xsl:param name="p_qualifiedname" select="@qualifiedtypename"/>
    <xsl:variable name="p_classdoc" select="document('index.xml', /)/gjdoc:rootdoc/gjdoc:classdoc[attribute::qualifiedtypename=$p_qualifiedname]"/>
    <xsl:choose>
      <xsl:when test="$p_classdoc">
        <a href="{concat($gjdoc.pathtoroot, translate($p_classdoc/gjdoc:containingPackage/@name, '.', '/'), '/', $p_classdoc/@name, '.html')}">
          <xsl:value-of select="$p_qualifiedname"/>
        </a>
      </xsl:when>
      <xsl:when test="$gjdoc.refdocs1 and document(concat($gjdoc.refdocs1, '/descriptor.xml'), /)//gjdoc:class[attribute::qualifiedtypename=$p_qualifiedname]">
        <a href="{concat($gjdoc.refdocs1, '/', $p_qualifiedname, '.html')}">
          <xsl:value-of select="$p_qualifiedname"/>
        </a>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$p_qualifiedname"/>
      </xsl:otherwise>
    </xsl:choose> 
  </xsl:template>
  
  <!-- If the given class is also included, create a link to it. -->
  <!-- Otherwise output the qualified name in plain text. -->

  <xsl:template name="link_to_class">
    <xsl:param name="p_qualifiedname" select="@qualifiedtypename"/>
    <xsl:param name="p_name" select="@name"/>
    <xsl:variable name="p_classdoc" select="document('index.xml', /)/gjdoc:rootdoc/gjdoc:classdoc[attribute::qualifiedtypename=$p_qualifiedname]"/>
    <xsl:choose>
      <xsl:when test="$p_classdoc">
        <a href="{concat($gjdoc.pathtoroot, translate($p_classdoc/gjdoc:containingPackage/@name, '.', '/'), '/', $p_classdoc/@name, '.html')}"><xsl:value-of select="$p_name"/></a>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$p_qualifiedname"/>
      </xsl:otherwise>
    </xsl:choose> 
  </xsl:template>

</xsl:stylesheet>

