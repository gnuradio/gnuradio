<!-- XSLT script to extract document for class/function for swig docstring
     If you have xsltproc you could use:
     xsltproc swig.xsl index.xml > swig_doc.i
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="text"/>
   <xsl:template match="/">
      <!-- process each compound -->
      <xsl:for-each select="doxygenindex/compound">
        <xsl:apply-templates select="document( concat( @refid, '.xml' ) )/*" />
      </xsl:for-each>
   </xsl:template>

  <xsl:template match="doxygen">
    <xsl:for-each select="compounddef[@kind='class']">  
      <xsl:text>%feature(__QuOtE__docstring__QuOtE__) </xsl:text>
      <xsl:value-of select="compoundname"/>
      <xsl:text> __QuOtE__ &#10;</xsl:text>
      <xsl:value-of select="briefdescription"/><xsl:text>
      </xsl:text>
      <xsl:value-of select="detaileddescription"/>
      <xsl:text> see also: </xsl:text>
      <xsl:value-of select="includes"/>
      <xsl:text>__QuOtE__;&#10;&#10;</xsl:text>

      <!-- output for each function individually -->
      <xsl:for-each select="*/memberdef[@kind='function' and not(starts-with(name,'operator'))]"> 
        <xsl:text>%feature(__QuOtE__docstring__QuOtE__) </xsl:text><xsl:value-of select="../../compoundname"/>::<xsl:value-of select="name"/>
        <xsl:text> __QuOtE__&#10;</xsl:text>
        <xsl:value-of select="definition"/> <xsl:value-of select="argsstring"/>
        <xsl:text>
        </xsl:text><xsl:value-of select="briefdescription"/><xsl:text>
        </xsl:text><xsl:value-of select="detaileddescription"/>
        <xsl:text>__QuOtE__; &#10;&#10;</xsl:text>
      </xsl:for-each>
    </xsl:for-each>  
  </xsl:template>
</xsl:stylesheet>
