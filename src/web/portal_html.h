#pragma once
#include <pgmspace.h>

const char PORTAL_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>DoorInterface Setup</title>
  <link rel="stylesheet" href="/portal.css">
</head>
<body>
  <h1>DoorInterface</h1>
  <p>Bitte WLAN-Credentials eingeben.</p>
  <form id="cfg" action="/save" method="post">
    <label>SSID
      <select id="ssidSel">
        <option value="">-- bitte scannen --</option>
      </select>
    </label>
    <button type="button" id="scanBtn">Netzwerke suchen</button>
    <label>SSID (manuell)
      <input type="text" name="ssid" id="ssidManual" placeholder="optional überschreiben">
    </label>
    <label>Passwort
      <input type="password" name="pass">
    </label>
    <label>Gerätename
      <input type="text" name="hostname" id="hostnameField" placeholder="z. B. door-eingang">
    </label>
    <button type="submit" id="saveBtn">Speichern &amp; Verbinden</button>
  </form>
  <div id="status"></div>
  <div id="overlay" class="overlay hidden">
    <div class="spinner"></div>
    <p id="overlayText">suche Netzwerke ...</p>
  </div>
  <script src="/portal.js"></script>
</body>
</html>
)HTML";