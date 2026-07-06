#pragma once
#include <pgmspace.h>

const char MAIN_JS[] PROGMEM = R"JS(
const $ = id => document.getElementById(id);
let lastAct = null;

async function refresh() {
  try {
    const r = await fetch('/api/status'); const s = await r.json();
    const w = s.wifi;
    $('wifiStatus').innerHTML = '<span class="badge ' + (w.connected ? 'ok' : 'err') + '">' + (w.connected ? 'verbunden' : 'getrennt') + '</span>';
    $('wifiDetails').innerHTML = w.connected
      ? '<dt>SSID</dt><dd>' + w.ssid + '</dd><dt>RSSI</dt><dd>' + w.rssi + ' dBm</dd><dt>IP</dt><dd>' + w.ip + '</dd>'
      : '';
    const act = s.relay.available || s.locks.available;
    if (act !== lastAct) {
      lastAct = act;
      $('actuator').innerHTML = act ? '' : '<p class="muted">nicht eingerichtet</p><a class="btn" href="/setup">Konfiguration starten</a>';
    }
    $('fwVersion').textContent = s.firmware.version;
  } catch (e) {}
}
refresh();
setInterval(refresh, 3000);

$('menuBtn').onclick = (e) => { e.stopPropagation(); $('menuList').classList.toggle('hidden'); };
document.addEventListener('click', () => $('menuList').classList.add('hidden'));
)JS";
