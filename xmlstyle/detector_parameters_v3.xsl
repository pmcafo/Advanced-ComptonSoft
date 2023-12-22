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
    <p>Comment: <span class="comment"><xsl:value-of select="." /></span></p>
  </xsl:template>
  <xsl:template match="root_file">
    <p>ROOT file: <xsl:value-of select="." /> [<xsl:value-of select="@name" />]</p>
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
        <p>Sensitive detector not found warning: ON</p>
      </xsl:when>
      <xsl:otherwise>
        <p>Sensitive detector not found warning: OFF</p>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template match="sensitive_detector">
    <h2 class="detectors"><xsl:value-of select="@name" /> (sensitive detector)</h2>
    <p class="setting"><xsl:text>Type: </xsl:text><xsl:value-of select="@type" /></p>
    <p class="setting"><xsl:text>Identification method: </xsl:text><xsl:value-of select="detector_list/@id_method" /></p>
    <p class="setting"><xsl:text>Layer offset: </xsl:text><xsl:value-of select="detector_list/@layer_offset" /></p>
    <xsl:call-template name="detectors" />
  </xsl:template>
  <xsl:template match="detector_set">
    <h2 class="detectors"><xsl:value-of select="@prefix" /> (prefix)</h2>
    <p class="setting"><xsl:text>Type: </xsl:text><xsl:value-of select="@type" /></p>
    <xsl:call-template name="detectors" />
  </xsl:template>
  <xsl:template name="detectors">
    <xsl:variable name="detector_type" select="@type" />
    <table>
      <colgroup class="col_detector_id">
        <col />
        <col />
        <col />
      </colgroup>
      <colgroup class="col_upside">
        <col />
      </colgroup>
      <colgroup class="col_depth">
        <col />
        <col />
      </colgroup>
      <colgroup class="col_conditions">
        <col />
        <col />
      </colgroup>
      <colgroup class="col_efield">
        <col />
        <col />
        <col />
        <col />
        <col />
        <col />
        <col />
        <col />
      </colgroup>
      <colgroup class="col_charge_collection">
        <col />
        <col />
        <col />
        <col />
        <col />
        <col />
        <col />
        <col />
      </colgroup>
      <colgroup class="col_diffusion">
        <col />
        <col />
        <col />
      </colgroup>
      <colgroup class="col_readout">
        <col />
        <col />
        <col />
      </colgroup>
      <xsl:choose>
        <xsl:when test="$detector_type='2DStrip'">
          <colgroup class="col_channel_properties">
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
          </colgroup>
          <colgroup class="col_channel_properties">
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
          </colgroup>
        </xsl:when>
        <xsl:otherwise>
          <colgroup class="col_channel_properties">
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
          </colgroup>
        </xsl:otherwise>
      </xsl:choose>
      <colgroup class="col_reconstruction">
        <col />
        <xsl:if test="$detector_type='2DStrip'">
          <col />
          <col />
          <col />
          <col />
        </xsl:if>
      </colgroup>
      <tr>
        <th colspan="3"></th>
        <th></th>
        <th colspan="26">Sensor device simulation</th>
        <xsl:choose>
          <xsl:when test="$detector_type='2DStrip'">
            <th colspan="13">Channel properties (cathode)</th>
            <th colspan="13">Channel properties (anode)</th>
          </xsl:when>
          <xsl:otherwise>
            <th colspan="13">Channel properties</th>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$detector_type='2DStrip'">
            <th colspan="5">Reconstruction</th>
          </xsl:when>
          <xsl:otherwise>
            <th colspan="1">Reconstruction</th>
          </xsl:otherwise>
        </xsl:choose>
      </tr>
      <tr>
        <th colspan="3">Detector ID</th>
        <th>Upside</th>
        <th colspan="2">Depth sensing</th>
        <th>Quenching</th>
        <th>Temperature</th>
        <th colspan="8">E-field</th>
        <th colspan="8">Charge collection</th>
        <th colspan="3">Diffusion</th>
        <th colspan="2">Timing resolution</th>
        <th>Pedestal gen.</th>
        <xsl:choose>
          <xsl:when test="$detector_type='2DStrip'">
            <th>Disable</th>
            <th colspan="2">Trigger discrimination</th>
            <th colspan="3">Noise level</th>
            <th>Compensation</th>
            <th>Pedestal</th>
            <th colspan="4">Gain correction</th>
            <th>Threshold</th>
            <th>Disable</th>
            <th colspan="2">Trigger discrimination</th>
            <th colspan="3">Noise level</th>
            <th>Compensation</th>
            <th>Pedestal</th>
            <th colspan="4">Gain correction</th>
            <th>Threshold</th>
          </xsl:when>
          <xsl:otherwise>
            <th>Disable</th>
            <th colspan="2">Trigger discrimination</th>
            <th colspan="3">Noise level</th>
            <th>Compensation</th>
            <th>Pedestal</th>
            <th colspan="4">Gain correction</th>
            <th>Threshold</th>
          </xsl:otherwise>
        </xsl:choose>
        <th>Reconstruction</th>
        <xsl:if test="$detector_type='2DStrip'">
          <th colspan="4">Energy consistency</th>
        </xsl:if>
      </tr>
      <tr>
        <th>ID</th>
        <th>Path</th>
        <th>Copy No.</th>
        <th></th>
        <th>mode</th>
        <th>resolution</th>
        <th>factor</th>
        <th></th>
        <th>Bias [V]</th>
        <th>mode</th>
        <th>P0</th>
        <th>P1</th>
        <th>P2</th>
        <th>P3</th>
        <th>WP map</th>
        <th>EField map</th>
        <th>mode</th>
        <th>CCE map</th>
        <th>MuTau e [cm2/V]</th>
        <th>MuTau h [cm2/V]</th>
        <th>Mobility e [cm2/V/s]</th>
        <th>Mobility h [cm2/V/s]</th>
        <th>Lifetime e [s]</th>
        <th>Lifetime h [s]</th>
        <th>mode</th>
        <th>anode</th>
        <th>cathode</th>
        <th>trigger</th>
        <th>energy measurement</th>
        <th>flag</th>
        <xsl:choose>
          <xsl:when test="$detector_type='2DStrip'">
            <th>status</th>
            <th>center</th>
            <th>sigma</th>
            <th>param0</th>
            <th>param1</th>
            <th>param2</th>
            <th>factor</th>
            <th>value</th>
            <th>c0</th>
            <th>c1</th>
            <th>c2</th>
            <th>c3</th>
            <th>value</th>
            <th>status</th>
            <th>center</th>
            <th>sigma</th>
            <th>param0</th>
            <th>param1</th>
            <th>param2</th>
            <th>factor</th>
            <th>value</th>
            <th>c0</th>
            <th>c1</th>
            <th>c2</th>
            <th>c3</th>
            <th>value</th>
          </xsl:when>
          <xsl:otherwise>
            <th>status</th>
            <th>center</th>
            <th>sigma</th>
            <th>param0</th>
            <th>param1</th>
            <th>param2</th>
            <th>factor</th>
            <th>value</th>
            <th>c0</th>
            <th>c1</th>
            <th>c2</th>
            <th>c3</th>
            <th>value</th>
          </xsl:otherwise>
        </xsl:choose>
        <th>mode</th>
        <xsl:if test="$detector_type='2DStrip'">
          <th>lower C0</th>
          <th>lower C1</th>
          <th>upper C0</th>
          <th>upper C0</th>
        </xsl:if>
      </tr>
      <xsl:variable name="dummy" select="common" />
      <xsl:call-template name="make_row">
        <xsl:with-param name="detector_type" select="$detector_type" />
        <xsl:with-param name="common_parameters" select="common/parameters" />
        <xsl:with-param name="tr_class" select="'common_param'" />
        <xsl:with-param name="id" select="'COM'" />
        <xsl:with-param name="parameters" select="$dummy" />
      </xsl:call-template>
      <xsl:apply-templates select="detector_list/detector">
        <xsl:with-param name="detector_type" select="$detector_type" />
        <xsl:with-param name="common_parameters" select="common/parameters" />
      </xsl:apply-templates>
    </table>
  </xsl:template>
  <xsl:template match="detector">
    <xsl:param name="detector_type" select="''" />
    <xsl:param name="common_parameters" select="''" />
    <xsl:variable name="parameters" select="parameters" />
    <xsl:call-template name="make_row">
      <xsl:with-param name="detector_type" select="$detector_type" />
      <xsl:with-param name="common_parameters" select="$common_parameters" />
      <xsl:with-param name="id" select="@id" />
      <xsl:with-param name="path" select="@path" />
      <xsl:with-param name="copyno" select="@copyno" />
      <xsl:with-param name="parameters" select="parameters" />
    </xsl:call-template>
  </xsl:template>
  <xsl:template name="make_row">
    <xsl:param name="detector_type" select="''" />
    <xsl:param name="common_parameters" select="''" />
    <xsl:param name="tr_class" select="''" />
    <xsl:param name="id" select="''" />
    <xsl:param name="path" select="''" />
    <xsl:param name="copyno" select="''" />
    <xsl:param name="parameters" select="''" />
    <tr>
      <xsl:if test="string-length($tr_class)!=0">
        <xsl:attribute name="class"><xsl:value-of select="$tr_class" /></xsl:attribute>
      </xsl:if>
      <th><xsl:value-of select="$id" /></th>
      <td><xsl:value-of select="$path" /></td>
      <td><xsl:value-of select="$copyno" /></td>
      <xsl:variable name="final_upside_anode">
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/upside/@anode" />
          <xsl:with-param name="special" select="$parameters/upside/@anode" />
        </xsl:call-template>
      </xsl:variable>
      <xsl:variable name="final_upside_pixel">
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_par