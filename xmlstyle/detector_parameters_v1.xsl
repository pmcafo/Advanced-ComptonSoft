<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">
  <xsl:output method="xml" indent="yes" doctype-public="-//W3C//DTD XHTML 1.1//EN" doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"/>
  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
        <title>Detector Parameters</title>
        <style type="text/css">
          html {background-color: #FFFFEE; color: #000000;}
          body {padding: 3%;}
          h1 {text-align: center;}
          table {margin:1em; border-collapse: collapse; border: 2px solid #000000; background-color: #FFFFDD}
          thead {border: 2px solid #000000; text-align: center;}
          tbody {border: 2px solid #000000; text-align: right;}
          th, td {border: 1px solid #000000; padding: 0.25em; }
          .detectors {border-top: solid 1px; border-left: solid 0.5em; padding: 0.375em; }
          .comment {color: #663300;}
          .col_efield {background-color: #EEEEFF;}
          .col_charge_collection {background-color: #EEEEFF;}
          .col_diffusion {background-color: #DDFFDD;}
          .col_channel_properties {background-color: #DDFFFF;}
          .common_param {background-color: #FFDDCC;}
          .special_param {color: #FF0000; }
        </style>
      </head>
      <body>
        <h1>Detector Parameters</h1>
        <p><xsl:text>Detector Name: </xsl:text><xsl:value-of select="detector_parameters/name" /></p>
        <xsl:apply-templates select="detector_parameters/comment" />
        <xsl:apply-templates select="detector_parameters/root_file" />
        <xsl:apply-templates select="detector_parameters/auto_position" />
        <xsl:apply-templates select="detector_parameters/sensitive_detector_not_found_warning" />
        <xsl:apply-templates select="detector_parameters/data/sensitive_detector" />
        <xsl:apply-templates select="detector_parameters/data/detector_set" />
      </body>
    </html>
  </xsl:template>
  <xsl:template match="comment">
    <p>COMMENT: <span class="comment"><xsl:value-of select="." /></span></p>
  </xsl:template>
  <xsl:template match="root_file">
    <p>ROOT file: <xsl:value-of select="." /></p>
  </xsl:template>
  <xsl:template match="auto_position">
    <xsl:choose>
      <xsl:when test="@flag=1">
        <p>Auto Position Mode: ON</p>
      </xsl:when>
      <xsl:otherwise>
        <p>Auto Position Mode: OFF</p>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template match="sensitive_detector_not_found_warning">
    <xsl:choose>
      <xsl:when test="@flag=1">
        <p>Sensitive detector not found warning: O