#pragma once
#include <pgmspace.h>

const char MAIN_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>DoorInterface</title>
  <link rel="stylesheet" href="/main.css">
</head>
<body>
  <header>
    <h1>DoorInterface</h1>
    <div class="menu">
      <button id="menuBtn" class="icon" aria-label="Setup">&#9881;</button>
      <div id="menuList" class="menu-list hidden">
        <a href="/setup">Einrichtung</a>
        <a href="/setup#wifi">WLAN</a>
        <a href="/setup#about">Über</a>
      </div>
    </div>
  </header>
  <main>
    <section class="card">
      <h2>WLAN</h2>
      <p id="wifiStatus"></p>
      <dl id="wifiDetails"></dl>
    </section>
    <section class="card">
      <h2>Türöffner (Relais / NUKI)</h2>
      <div id="actuator"></div>
    </section>
    <section class="card">
      <h2>Firmware</h2>
      <p id="fwVersion">…</p>
    </section>
  </main>
  <script src="/main.js"></script>
</body>
</html>
)HTML";
