#pragma once
#include <pgmspace.h>

const char PORTAL_CSS[] PROGMEM = R"CSS(
body { font-family: system-ui, sans-serif; max-width: 480px; margin: 2em auto; padding: 0 1em; }
h1 { color: #2c3e50; }
form { display: flex; flex-direction: column; gap: 0.8em; margin-top: 1.5em; }
label { display: flex; flex-direction: column; gap: 0.2em; }
button { padding: 0.6em; background: #2c3e50; color: #fff; border: none; border-radius: 4px; }
button:disabled { opacity: 0.5; }
.overlay {
  position: fixed; top: 0; left: 0; right: 0; bottom: 0;
  background: rgba(255,255,255,0.7);
  display: flex; flex-direction: column;
  align-items: center; justify-content: center;
  z-index: 10; gap: 1em;
}
.overlay.hidden { display: none; }
.spinner {
  width: 40px; height: 40px;
  border: 4px solid #ccc;
  border-top-color: #2c3e50;
  border-radius: 50%;
  animation: spin 0.8s linear infinite;
}
@keyframes spin { to { transform: rotate(360deg); } }
)CSS";