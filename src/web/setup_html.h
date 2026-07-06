#pragma once
#include <pgmspace.h>

const char SETUP_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>DoorInterface – Einrichtung</title>
  <link rel="stylesheet" href="/main.css">
</head>
<body>
  <header><h1>Einrichtung</h1></header>
  <main>
    <section class="card" id="wifi">
      <h2>Gerätename</h2>
      <p class="muted">Name für mDNS (&lt;name&gt;.local). Nach Änderung startet das Gerät neu.</p>
      <form id="hostForm">
        <label>Hostname
          <input type="text" id="hostInput" pattern="[a-z0-9\-]{1,63}" title="a-z, 0-9, Bindestrich; max. 63 Zeichen">
        </label>
        <button type="submit" id="hostBtn">Speichern &amp; Neustart</button>
      </form>
      <div id="hostStatus"></div>
    </section>
    <section class="card">
      <h2>NUKI Smart Lock</h2>
      <div id="nukiStatus"></div>
    </section>
    <section class="card" id="about">
      <h2>Zugriffsschutz</h2>
      <p class="muted">Login/Auth folgt in einem späteren Schritt.</p>
      <a class="btn" href="/">&larr; Zurück zum Dashboard</a>
    </section>
  </main>
  <script src="/setup.js"></script>
</body>
</html>
)HTML";
