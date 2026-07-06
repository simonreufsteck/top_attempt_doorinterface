#pragma once
#include <pgmspace.h>

const char SETUP_JS[] PROGMEM = R"JS(
const $ = id => document.getElementById(id);

(async () => {
  try { const r = await fetch('/api/hostname'); const h = await r.json(); if (h.hostname) $('hostInput').value = h.hostname; } catch(e){}
})();

$('hostForm').onsubmit = async (e) => {
  e.preventDefault();
  const v = $('hostInput').value.trim();
  if (!v) { $('hostStatus').textContent = 'Hostname darf nicht leer sein'; return; }
  $('hostBtn').disabled = true;
  $('hostStatus').textContent = 'speichere ...';
  try {
    const r = await fetch('/api/hostname', { method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:'hostname='+encodeURIComponent(v) });
    const s = await r.json();
    if (r.ok) {
      $('hostStatus').innerHTML = 'Gespeichert. Gerät startet neu — erreichbar unter <a href="http://'+v+'.local">http://'+v+'.local</a>';
    } else {
      $('hostStatus').textContent = 'Fehler: ' + (s.error || 'ungültiger Hostname');
      $('hostBtn').disabled = false;
    }
  } catch(err) { $('hostStatus').textContent = 'Fehler'; $('hostBtn').disabled = false; }
};

async function refreshNuki() {
  try {
    const r = await fetch('/api/status'); const s = await r.json();
    const lk = s.locks;
    let html = '';
    if (lk.pairing) {
      html = '<p class="muted">Pairing läuft … Taste am Nuki 10s gedrückt halten, bis der LED-Ring leuchtet.</p>';
      html += '<button class="btn" id="nukiCancel">Pairing abbrechen</button>';
    } else if (lk.paired) {
      const cls = lk.lockState === 'locked' ? 'err' : (lk.lockState === 'unlocked' ? 'ok' : '');
      html = '<p><span class="badge ' + cls + '">' + lk.lockState + '</span></p>';
      html += '<dl><dt>Akku</dt><dd>' + (lk.batteryPct >= 0 ? lk.batteryPct + '%' : '—') + (lk.batteryCritical ? ' ⚠' : '') + '</dd>';
      html += '<dt>RSSI</dt><dd>' + (lk.rssi || '—') + ' dBm</dd></dl>';
      html += '<div class="btn-row"><button class="btn" id="nukiUnlock">Öffnen (Test)</button><button class="btn" id="nukiLock">Sperren (Test)</button></div>';
    } else {
      html = '<p class="muted">Nicht gekoppelt.</p>';
      html += '<p class="muted">1. In der Nuki App: Bluetooth Pairing aktivieren (Settings → Features & Configuration → Button and LED).</p>';
      html += '<p class="muted">2. Taste am Nuki 10s drücken, bis der LED-Ring leuchtet.</p>';
      html += '<p class="muted">3. Hier Pairing starten:</p>';
      html += '<button class="btn" id="nukiPair">Pairing starten</button>';
    }
    $('nukiStatus').innerHTML = html;
    bindNuki();
  } catch (e) {}
}

function bindNuki() {
  const p = $('nukiPair'), c = $('nukiCancel'), u = $('nukiUnlock'), l = $('nukiLock');
  if (p) p.onclick = async () => { p.disabled = true; await fetch('/api/nuki/pair', { method:'POST' }); setTimeout(refreshNuki, 500); };
  if (c) c.onclick = async () => { c.disabled = true; await fetch('/api/nuki/cancel', { method:'POST' }); setTimeout(refreshNuki, 500); };
  if (u) u.onclick = async () => { u.disabled = true; await fetch('/api/nuki/unlock', { method:'POST' }); setTimeout(refreshNuki, 1000); };
  if (l) l.onclick = async () => { l.disabled = true; await fetch('/api/nuki/lock', { method:'POST' }); setTimeout(refreshNuki, 1000); };
}

refreshNuki();
setInterval(refreshNuki, 2000);
)JS";
