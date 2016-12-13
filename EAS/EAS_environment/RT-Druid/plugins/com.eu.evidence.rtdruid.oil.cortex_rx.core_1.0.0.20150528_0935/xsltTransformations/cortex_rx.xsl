<?xml version="1.0" ?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.1">
 <xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes" omit-xml-declaration="no"/>  

    <!-- ============================================================================================================ -->
    <!--                              OS    (eecfg.c, stack definition)                                               -->
    <!-- ============================================================================================================ -->
<xsl:template match="object[@Type='OS']/parameter[@Name='CPU_DATA']/enumerator[@Name='CORTEX_RX']" mode="build_eecfg_c">
#include "ee.h"
</xsl:template> 
    
    
    <!-- ============================================================================================================ -->
    <!--                              OS    (eecfg.h)                                                                 -->
    <!-- ============================================================================================================ -->
    
<xsl:template match="object[@Type='OS']/parameter[@Name='CPU_DATA']/enumerator[@Name='CORTEX_RX']" mode="build_eecfg_h">
</xsl:template>



    <!-- ============================================================================================================ -->
    <!--                              CPU         (makefile, add a define)                                             -->
    <!-- ============================================================================================================ -->
<xsl:template match="object[@Type='OS']/parameter[@Name='CPU_DATA']/enumerator[@Name='CORTEX_RX']" mode="build_makefile">
## CPU OPTIONS ##
EEOPT += __CORTEX_RX__
<xsl:if test="parameter[@Name='MODEL']"># CPU Model 
EEOPT += __CORTEX_<xsl:value-of select="parameter[@Name='MODEL']/@CurrValue"/>__
</xsl:if>
<xsl:if test="parameter[@Name='COMPILER_TYPE']"># Compiler Type
EEOPT += __<xsl:value-of select="parameter[@Name='COMPILER_TYPE']/@CurrValue"/>__
</xsl:if>
<!-- Define __RTD_TMS570__ so that the build script knows we are using a real RT-Druid -->
EEOPT += __RTD_TMS570__
#################
</xsl:template>
    
    <!-- ============================================================================================================ -->
    <!--                              MCU         (makefile, add a define)                                             -->
    <!-- ============================================================================================================ -->
<!-- MCU: CUSTOM -->
<xsl:template match="object[@Type='OS']/parameter[@Name='MCU_DATA']/enumerator[@Name='TI']/parameter[@Name='MODEL']/enumerator[@Name='CUSTOM']" mode="build_makefile">
## MCU OPTIONS ## 
EEOPT  += __TI__
<xsl:if test="parameter[@Name='MODEL']">
CFLAGS  += <xsl:value-of select="parameter[@Name='MODEL']/@CurrValue"/>
ASFLAGS += <xsl:value-of select="parameter[@Name='MODEL']/@CurrValue"/>
LDFLAGS += <xsl:value-of select="parameter[@Name='MODEL']/@CurrValue"/>
</xsl:if>
#################
</xsl:template>

<!-- MCU: TMS570 -->
<xsl:template match="object[@Type='OS']/parameter[@Name='MCU_DATA']/enumerator[@Name='TI']/parameter[@Name='MODEL']/enumerator[@Name='TMS570']" mode="build_makefile">
## MCU OPTIONS ## 
EEOPT += __RTD_CYGWIN__
EEOPT += __TI__
EEOPT += __TMS570__
<xsl:if test="parameter[@Name='COMPILER_TYPE']">
EEOPT += __<xsl:value-of select="parameter[@Name='COMPILER_TYPE']/@CurrValue"/>__
</xsl:if>
APPBASE := ..
OUTBASE := Debug
#################
</xsl:template><!-- TMS570 -->
    

    <!-- ============================================================================================================ -->
    <!--                             STARTING POINT                                                                   -->
    <!-- ============================================================================================================ -->

<xsl:template match="application">
<CODE_GENERATION>
<OUTPUT_BUFFER name="eecfg.h">
		<xsl:apply-templates select="/" mode="build_eecfg_h"/> 
</OUTPUT_BUFFER>
<OUTPUT_BUFFER name="eecfg.c">
        <xsl:apply-templates select="/" mode="build_eecfg_c"/>
</OUTPUT_BUFFER>
<OUTPUT_BUFFER name="makefile">
        <xsl:apply-templates select="/" mode="build_makefile"/>
</OUTPUT_BUFFER>
</CODE_GENERATION>
</xsl:template>

</xsl:stylesheet>
