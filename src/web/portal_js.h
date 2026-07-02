#pragma once
#include <pgmspace.h>

const char PORTAL_JS[] PROGMEM = R"JS(
const $ = id => document.getElementById(id);

function showOverlay(text) {
  $('overlay').classList.remove('hidden');
  $('overlayText').textContent = text;
}
function hideOverlay() { $('overlay').classList.add('hidden'); }

$('scanBtn').onclick = async () => {
  const b = $('scanBtn'); b.disabled = true;
  showOverlay('suche Netzwerke ...');
  try {
    const r = await fetch('/scan'); const nets = await r.json();
    const sel = $('ssidSel'); sel.innerHTML = '';
    nets.forEach(n => {
      const o = document.createElement('option');
      o.value = n.ssid;
      o.textContent = n.ssid + ' (' + n.rssi + ' dBm)' + (n.secure ? ' \u{1F512}' : '');
      sel.appendChild(o);
    });
    sel.onchange = () => { $('ssidManual').value = sel.value; };
    if (nets.length) sel.onchange();
  } catch (e) { $('status').textContent = 'Scan fehlgeschlagen'; }
  b.disabled = false; b.textContent = 'Netzwerke suchen';
  hideOverlay();
};

$('cfg').onsubmit = async (e) => {
  e.preventDefault();
  if (!$('ssidManual').value) { $('status').textContent = 'Bitte SSID wählen/eingeben'; return; }
  const fd = new FormData($('cfg'));
  $('saveBtn').disabled = true;
  showOverlay('verbinde ...');
  await fetch('/save', { method:'POST', body: fd });
  pollStatus();
};

async function pollStatus() {
  const r = await fetch('/status'); const s = await r.json();
  if (s.state === 'connecting') {
    $('status').textContent = 'verbinde ...';
    setTimeout(pollStatus, 1000);
  } else if (s.state === 'connected') {
    hideOverlay();
    $('status').innerHTML = 'Verbunden! Erreichbar unter <a href="http://' + s.ip + '">http://' + s.ip + '</a>';
  } else if (s.state === 'failed') {
    hideOverlay();
    $('status').textContent = 'Verbindung fehlgeschlagen — bitte erneut versuchen.';
    $('saveBtn').disabled = false;
  }
}
)JS";