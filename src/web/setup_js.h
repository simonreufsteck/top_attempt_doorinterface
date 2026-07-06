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
)JS";
