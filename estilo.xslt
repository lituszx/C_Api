<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" encoding="UTF-8" indent="yes"/>
  <xsl:template match="/">
    <html>
      <head>
        <title>Informacion del Adaptador de Red</title>
        <style>
          table {
            width: 100%;
            border-collapse: collapse;
          }
          th, td {
            border: 1px solid black;
            padding: 8px;
            text-align: left;
          }
          th {
            background-color: #33A5FF;
          }
          tr:nth-child(even) {
            background-color: #33DDFF;
          }
          tr:nth-child(odd) {
            background-color: #4EFEF9;
          }
        </style>
      </head>
      <body>
        <h2>Informacion del Adaptador de Red</h2>
        <table>
          <tr>
            <th>Campo</th>
            <th>Valor</th>
          </tr>
          <tr>
            <td>IP</td>
            <td><xsl:value-of select="/adaptador/ip"/></td>
          </tr>
          <tr>
            <td>Mascara</td>
            <td><xsl:value-of select="/adaptador/mascara"/></td>
          </tr>
          <tr>
            <td>Puerta de Enlace</td>
            <td><xsl:value-of select="/adaptador/puerta_enlace"/></td>
          </tr>
          <tr>
            <td>Servidor DNS IP</td>
            <td><xsl:value-of select="/adaptador/servidor_dns/ip"/></td>
          </tr>
          <tr>
            <td>Velocidad Media de Respuesta</td>
            <td><xsl:value-of select="/adaptador/servidor_dns/velocidad_media_respuesta"/> segundos</td>
          </tr>
          <tr>
            <td>Numero de Saltos</td>
            <td><xsl:value-of select="/adaptador/servidor_dns/hop_count"/></td>
          </tr>
          <tr>
            <td>IPs de los Saltos</td>
            <td>
              <xsl:for-each select="/adaptador/servidor_dns/ips_saltos/salto">
                <div><xsl:value-of select="."/></div>
              </xsl:for-each>
            </td>
          </tr>
        </table>
      </body>
    </html>
  </xsl:template>
</xsl:stylesheet>
