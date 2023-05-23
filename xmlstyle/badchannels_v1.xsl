<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">

  <xsl:output method="xml" indent="yes" doctype-public="-//W3C//DTD XHTML 1.1//EN"  doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd" />

  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
        <title>Bad channels</title>
	<style type="text/css">
          html {background-color: #DDDDEE; color: #000000;}
          body {padding: 3%;}
          h1 {text-align: center;}
          table {margin:1em; border-collapse: collapse; border: 2px solid #000000;}
          thead {border: 2px solid #000000; text-align: center;}
          tbody {border: 2px solid #000000; text-align: right;}
          th, td {border: 1px solid #000000;}
	</style>
      </head>

      <body>
        <h1>Bad channels</h1>
        <xsl:apply-templates />
      </body>
    </html>
  </xsl:template>

  <xsl:template match="badchannels">
    <p>Instrument: <xsl:value-of select="name" /></p>
    <p>Comment: <xsl:value-of select="comment" /></p>
    <xsl:apply-templates select="data" />
    <hr />
  </xsl:template>

  <xsl:template match="detector">
    <h2>Detector <xsl:value-of select="./@id" /></h2>
    <xsl:apply-templates select="section" />
  </xsl:template>

  <xsl:template match="readout_module">
    <h2>Readout Module <xsl:value-of select="./@id" /></h2>
    <xsl:apply-templates select="section" />
  </xsl:template>

  <xsl:template match="section">
    <h3>Section <xsl:value-of select="./@id" /></h3>
    <table>
      <thead>
        <tr>
          <th>Channel</th>
          <th>Status</th>
        </tr>
      </thead>
      <tbody>
        <xsl:apply-templates select="channel" />
      </tbody>
    </table>
  </xsl:template>

  <xsl:template match="channel">
    <tr>
      <th><xsl:value-of select="./@id" /></th>
      <td><xsl:value-of select="./@status" /></td>
    </tr>
  </xsl:template>

</xsl:stylesheet>
