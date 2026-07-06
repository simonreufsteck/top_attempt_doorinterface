#pragma once
#include <pgmspace.h>

const char MAIN_JS[] PROGMEM = R"JS(
const $ = id => document.getElementById(id);
let lastActHtml = '';

async function refresh() {
  try {
    const r = await fetch('/api/status'); const s = await r.json();
    const w = s.wifi;
    $('wifiStatus').innerHTML = '<span class="badge ' + (w.connected ? 'ok' : 'err') + '">' + (w.connected ? 'verbunden' : 'getrennt') + '</span>';
    $('wifiDetails').innerHTML = w.connected
      ? '<dt>SSID</dt><dd>' + w.ssid + '</dd><dt>RSSI</dt><dd>' + w.rssi + ' dBm</dd><dt>IP</dt><dd>' + w.ip + '</dd>'
      : '';
    const lk = s.locks;
    let html = '';
    if (lk.pairing) {
      html = '<p class="muted">Pairing läuft … Nuki-Taste 10s drücken</p>';
    } else if (lk.paired) {
      const cls = lk.lockState === 'locked' ? 'err' : (lk.lockState === 'unlocked' ? 'ok' : '');
      html = '<p><span class="badge ' + cls + '">' + lk.lockState + '</span></p>';
      html += '<dl><dt>Akku</dt><dd>' + (lk.batteryPct >= 0 ? lk.batteryPct + '%' : '—') + (lk.batteryCritical ? ' ⚠' : '') + '</dd>';
      html += '<dt>RSSI</dt><dd>' + (lk.rssi || '—') + ' dBm</dd></dl>';
      html += '<div class="btn-row"><button class="btn" id="btnUnlock">Öffnen</button><button class="btn" id="btnLock">Sperren</button></div>';
    } else {
      html = '<p class="muted">nicht eingerichtet</p><a class="btn" href="/setup">Konfiguration starten</a>';
    }
    if (html !== lastActHtml) { lastActHtml = html; $('actuator').innerHTML = html; bindActuator(); }
    $('fwVersion').textContent = s.firmware.version;
  } catch (e) {}
}

function bindActuator() {
  const u = $('btnUnlock'), l = $('btnLock');
  if (u) u.onclick = async () => { u.disabled = true; await fetch('/api/nuki/unlock', { method:'POST' }); setTimeout(refresh, 500); };
  if (l) l.onclick = async () => { l.disabled = true; await fetch('/api/nuki/lock', { method:'POST' }); setTimeout(refresh, 500); };
}

refresh();
setInterval(refresh, 3000);

$('menuBtn').onclick = (e) => { e.stopPropagation(); $('menuList').classList.toggle('hidden'); };
document.addEventListener('click', () => $('menuList').classList.add('hidden'));
)JS";
