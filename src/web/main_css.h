#pragma once
#include <pgmspace.h>

const char MAIN_CSS[] PROGMEM = R"CSS(
* { box-sizing: border-box; }
body { font-family: system-ui, sans-serif; max-width: 720px; margin: 0 auto; padding: 0 1em 2em; }
header { display: flex; justify-content: space-between; align-items: center; padding: 1em 0; }
h1 { color: #2c3e50; margin: 0; }
h2 { margin-top: 0; }
main { display: grid; grid-template-columns: 1fr; gap: 1em; }
@media (min-width: 600px) { main { grid-template-columns: 1fr 1fr; } }
.card { background: #f7f8fa; border: 1px solid #e1e4e8; border-radius: 8px; padding: 1em 1.2em; }
.card:last-child { grid-column: 1 / -1; }
dl { margin: 0.5em 0 0; display: grid; grid-template-columns: auto 1fr; gap: 0.2em 0.8em; }
dt { color: #6a737d; }
dd { margin: 0; }
.badge { display: inline-block; padding: 0.2em 0.6em; border-radius: 12px; font-size: 0.85em; color: #fff; background: #999; }
.badge.ok { background: #2e7d32; }
.badge.err { background: #c62828; }
.muted { color: #6a737d; }
.btn { display: inline-block; margin-top: 0.5em; padding: 0.5em 0.9em; background: #2c3e50; color: #fff; text-decoration: none; border: none; border-radius: 4px; cursor: pointer; }
.btn:active { opacity: 0.7; }
.btn:disabled { opacity: 0.5; cursor: default; }
.btn-row { display: flex; gap: 0.5em; margin-top: 0.5em; }
.icon { font-size: 1.4em; background: none; border: none; cursor: pointer; color: #2c3e50; }
.menu { position: relative; }
.menu-list { position: absolute; right: 0; top: 100%; background: #fff; border: 1px solid #e1e4e8; border-radius: 6px; box-shadow: 0 2px 8px rgba(0,0,0,0.08); min-width: 160px; display: flex; flex-direction: column; z-index: 20; }
.menu-list a { padding: 0.6em 1em; text-decoration: none; color: #2c3e50; }
.menu-list a:hover { background: #f0f1f3; }
.hidden { display: none !important; }
)CSS";
