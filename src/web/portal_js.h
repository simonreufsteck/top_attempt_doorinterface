#pragma once
#include <pgmspace.h>

const char PORTAL_JS[] PROGMEM = R"JS(
const $ = id => document.getElementById(id);

function showOverlay(text) {
  $('overlay').classList.remove('hidden');
  $('overlayText').textContent = text;
}
function hideOverlay() { $('overlay').classList.add('hidden'); }

(async () => {
  try { const r = await fetch('/config'); const c = await r.json(); if (c.hostname) $('hostnameField').value = c.hostname; } catch(e){}
})();

async function copyText(text) {
  try { if (navigator.clipboard && window.isSecureContext) { await navigator.clipboard.writeText(text); return true; } } catch(e){}
  const ta = document.createElement('textarea'); ta.value = text;
  ta.style.position = 'fixed'; ta.style.opacity = '0';
  document.body.appendChild(ta); ta.select();
  let ok = false; try { ok = document.execCommand('copy'); } catch(e){}
  document.body.removeChild(ta); return ok;
}

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
    const addr = 'http://' + s.ip;
    showOverlay('Verbunden! Neue Adresse: ' + addr);
    const sp = document.querySelector('#overlay .spinner');
    if (sp) sp.style.display = 'none';
    if (!$('finishBtn')) {
      const b = document.createElement('button'); b.id = 'finishBtn';
      b.textContent = 'Adresse kopieren & Setup beenden';
      b.onclick = async () => {
        await copyText(addr);
        $('overlayText').textContent = 'Kopiert – AP wird geschlossen …';
        b.disabled = true;
        try { await fetch('/close', { method:'POST' }); } catch(e) {}
      };
      $('overlay').appendChild(b);
    }
  } else if (s.state === 'failed') {
    hideOverlay();
    $('status').textContent = 'Verbindung fehlgeschlagen — bitte erneut versuchen.';
    $('saveBtn').disabled = false;
  }
}
)JS";