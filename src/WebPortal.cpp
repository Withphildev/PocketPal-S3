#include "WebPortal.h"

namespace {
constexpr uint16_t kDnsPort = 53;

const char kPage[] PROGMEM = R"HTML(
<!doctype html><html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>PocketPal S3</title><style>
:root{color-scheme:dark;--ink:#f8f4ff;--muted:#b9b0cb;--pink:#ff8fc7;--mint:#75e6c2;--sun:#ffd36a}
*{box-sizing:border-box}body{margin:0;min-height:100vh;background:radial-gradient(circle at 50% 5%,#513c77,#181229 58%,#0d0a17);font:16px system-ui;color:var(--ink)}
main{max-width:760px;margin:auto;padding:20px}.hero{text-align:center}.eyebrow{color:var(--mint);font-weight:800;letter-spacing:.12em;text-transform:uppercase;font-size:12px}h1{font-size:38px;margin:5px 0}.room{position:relative;min-height:330px;overflow:hidden;border:1px solid #8066a4;border-radius:28px;background:linear-gradient(#50427a 0 67%,#2f2850 67%);box-shadow:0 24px 70px #0008}
.room:before{content:'✦  ·  ✧  ·  ✦';position:absolute;inset:22px 0 auto;text-align:center;color:#d9c9ff99;font-size:25px}.rug{position:absolute;left:16%;right:16%;bottom:25px;height:68px;border-radius:50%;background:#ff8fc72b;border:2px solid #ff8fc755}
.pet{position:absolute;left:50%;bottom:58px;width:150px;height:155px;transform:translateX(-50%);animation:bob 2.2s ease-in-out infinite}.body{position:absolute;inset:25px 8px 0;border-radius:48% 48% 44% 44%;background:linear-gradient(145deg,#8ff3d2,#50c9ad);box-shadow:inset -12px -14px #249b8b55,0 15px 20px #0005}.ear{position:absolute;top:12px;width:52px;height:60px;background:#75e6c2;transform:rotate(28deg);border-radius:70% 20%}.ear.r{right:0;transform:rotate(-28deg)}.ear.l{left:0}.eye{position:absolute;top:69px;width:15px;height:22px;border-radius:50%;background:#251d3d}.eye.l{left:46px}.eye.r{right:46px}.mouth{position:absolute;left:65px;top:99px;width:22px;height:12px;border-bottom:4px solid #251d3d;border-radius:50%}.cheek{position:absolute;top:92px;width:22px;height:10px;border-radius:50%;background:#ff8fc788}.cheek.l{left:22px}.cheek.r{right:22px}.pet.sleeping .eye{height:8px;border-radius:0;background:transparent;border-bottom:3px solid #251d3d}.pet.sleeping:after{content:'Z z';position:absolute;right:-18px;top:25px;color:#fff;font-weight:900;font-size:23px}.bubble{position:absolute;top:54px;left:18px;right:18px;margin:auto;max-width:310px;padding:10px 14px;border-radius:15px;background:#fff;color:#34294c;font-weight:700;text-align:center;box-shadow:0 7px 20px #0005}
.card{margin-top:16px;padding:18px;border:1px solid #5a4877;border-radius:20px;background:#211a35e8}.identity{display:flex;align-items:center;justify-content:space-between;gap:12px}.identity h2{margin:0;font-size:27px}.mood{color:var(--pink);font-weight:800;text-transform:capitalize}.stats{display:grid;grid-template-columns:repeat(2,1fr);gap:12px;margin-top:16px}.stat label{display:flex;justify-content:space-between;color:var(--muted);font-size:14px}.track{height:11px;margin-top:5px;border-radius:99px;background:#0e0b18;overflow:hidden}.fill{height:100%;border-radius:inherit;background:linear-gradient(90deg,var(--pink),var(--sun));transition:width .35s}.actions{display:grid;grid-template-columns:repeat(5,1fr);gap:8px;margin-top:16px}button{border:0;border-radius:13px;padding:12px 6px;background:#654d8e;color:#fff;font-weight:800;cursor:pointer}button:hover{background:#8063ac}button:active{transform:translateY(1px)}.settings{display:flex;gap:8px;margin-top:14px}input{min-width:0;flex:1;border:1px solid #66527f;border-radius:12px;padding:11px;background:#120e20;color:#fff}.foot{text-align:center;color:var(--muted);font-size:13px;margin:15px}
@keyframes bob{50%{transform:translate(-50%,-7px)}}@media(max-width:540px){main{padding:11px}.room{min-height:310px}.actions{grid-template-columns:repeat(3,1fr)}h1{font-size:32px}}
</style></head><body><main><header class="hero"><div class="eyebrow">M5StickS3 virtual companion</div><h1>PocketPal S3</h1></header>
<section class="room"><div id="bubble" class="bubble">Waking up…</div><div class="rug"></div><div id="pet" class="pet"><div class="ear l"></div><div class="ear r"></div><div class="body"></div><div class="eye l"></div><div class="eye r"></div><div class="mouth"></div><div class="cheek l"></div><div class="cheek r"></div></div></section>
<section class="card"><div class="identity"><div><h2 id="name">Pip</h2><div id="mood" class="mood">content</div></div><div id="age">Age 0m</div></div><div id="stats" class="stats"></div>
<div class="actions"><button onclick="act('feed')">🍓 Feed</button><button onclick="act('play')">🎾 Play</button><button onclick="act('clean')">🫧 Clean</button><button onclick="act('sleep')">🌙 Sleep</button><button onclick="act('pet')">💗 Pet</button></div>
<form class="settings" onsubmit="renamePet(event)"><input id="newName" maxlength="12" placeholder="Give your pal a name"><button>Rename</button></form></section><p class="foot">Local and private · No cloud account required</p></main>
<script>
const labels={fullness:'Fullness',happiness:'Happiness',energy:'Energy',cleanliness:'Cleanliness',health:'Health'};
async function refresh(){try{const r=await fetch('/api/state',{cache:'no-store'}),s=await r.json();document.getElementById('name').textContent=s.name;document.getElementById('mood').textContent=s.mood;document.getElementById('age').textContent='Age '+s.ageMinutes+'m';document.getElementById('bubble').textContent=s.message;document.getElementById('pet').classList.toggle('sleeping',s.sleeping);document.getElementById('stats').innerHTML=Object.entries(labels).map(([k,v])=>`<div class="stat"><label><span>${v}</span><b>${s[k]}%</b></label><div class="track"><div class="fill" style="width:${s[k]}%"></div></div></div>`).join('')}catch(e){document.getElementById('bubble').textContent='Looking for PocketPal…'}}
async function act(a){await fetch('/api/action',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'action='+encodeURIComponent(a)});refresh()}
async function renamePet(e){e.preventDefault();const input=document.getElementById('newName'),v=input.value.trim();if(!v)return;await fetch('/api/name',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'name='+encodeURIComponent(v)});input.value='';refresh()}
refresh();setInterval(refresh,2000);
</script></body></html>
)HTML";
}

WebPortal::WebPortal(PetEngine &pet) : pet_(pet) {}

void WebPortal::begin() {
    const uint64_t chipId = ESP.getEfuseMac();
    char suffix[5];
    snprintf(suffix, sizeof(suffix), "%04X", static_cast<uint16_t>(chipId));
    char secret[13];
    snprintf(secret, sizeof(secret), "Pal%08X", static_cast<uint32_t>(chipId));
    ssid_ = String("PocketPal-") + suffix;
    password_ = secret;
    WiFi.mode(WIFI_AP);
    WiFi.setSleep(true);
    WiFi.softAP(ssid_.c_str(), password_.c_str());
    dns_.start(kDnsPort, "*", WiFi.softAPIP());
    configureRoutes();
    server_.begin();
}

void WebPortal::loop() {
    dns_.processNextRequest();
    server_.handleClient();
}

const String &WebPortal::ssid() const { return ssid_; }
const String &WebPortal::password() const { return password_; }
uint8_t WebPortal::connectedClients() const { return WiFi.softAPgetStationNum(); }

String WebPortal::jsonEscape(const String &value) {
    String result;
    result.reserve(value.length() + 8);
    for (size_t i = 0; i < value.length(); ++i) {
        const char c = value[i];
        if (c == '\\' || c == '"') result += '\\';
        if (c == '\n') result += "\\n";
        else if (c != '\r') result += c;
    }
    return result;
}

void WebPortal::sendState() {
    const PetSnapshot s = pet_.snapshot();
    String json;
    json.reserve(320);
    json += "{\"name\":\"" + jsonEscape(s.name) + "\"";
    json += ",\"fullness\":" + String(s.fullness);
    json += ",\"happiness\":" + String(s.happiness);
    json += ",\"energy\":" + String(s.energy);
    json += ",\"cleanliness\":" + String(s.cleanliness);
    json += ",\"health\":" + String(s.health);
    json += ",\"ageMinutes\":" + String(s.ageMinutes);
    json += ",\"sleeping\":" + String(s.sleeping ? "true" : "false");
    json += ",\"mood\":\"" + jsonEscape(s.mood) + "\"";
    json += ",\"message\":\"" + jsonEscape(s.message) + "\"}";
    server_.sendHeader("Cache-Control", "no-store");
    server_.send(200, "application/json", json);
}

void WebPortal::handleAction() {
    if (!server_.hasArg("action") || !pet_.apply(server_.arg("action"))) {
        server_.send(400, "application/json", "{\"ok\":false}");
        return;
    }
    server_.send(200, "application/json", "{\"ok\":true}");
}

void WebPortal::handleName() {
    if (!server_.hasArg("name") || !pet_.setName(server_.arg("name"))) {
        server_.send(400, "application/json", "{\"ok\":false,\"error\":\"Use 1-12 letters, numbers, spaces, hyphens, or underscores.\"}");
        return;
    }
    server_.send(200, "application/json", "{\"ok\":true}");
}

void WebPortal::configureRoutes() {
    server_.on("/", HTTP_GET, [this]() { server_.send_P(200, "text/html", kPage); });
    server_.on("/api/state", HTTP_GET, [this]() { sendState(); });
    server_.on("/api/action", HTTP_POST, [this]() { handleAction(); });
    server_.on("/api/name", HTTP_POST, [this]() { handleName(); });
    server_.on("/generate_204", HTTP_GET, [this]() { server_.sendHeader("Location", "/", true); server_.send(302); });
    server_.on("/hotspot-detect.html", HTTP_GET, [this]() { server_.send_P(200, "text/html", kPage); });
    server_.on("/connecttest.txt", HTTP_GET, [this]() { server_.sendHeader("Location", "/", true); server_.send(302); });
    server_.onNotFound([this]() {
        server_.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
        server_.send(302, "text/plain", "");
    });
}

