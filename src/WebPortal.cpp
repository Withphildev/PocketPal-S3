#include "WebPortal.h"

#include <ESPmDNS.h>

namespace {
constexpr uint16_t kDnsPort = 53;

const char kPage[] PROGMEM = R"HTML(
<!doctype html><html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>PocketPal S3</title><style>
:root{color-scheme:dark;--bg:#100d1d;--surface:#211a36;--surface2:#2a2143;--line:#504269;--ink:#fcf9ff;--muted:#b9b0ca;--pink:#ff8fc7;--mint:#74e6c1;--sun:#ffd36a;--blue:#8db9ff}
*{box-sizing:border-box}body{margin:0;min-height:100vh;background:radial-gradient(circle at 50% -10%,#62498b 0,#211731 40%,var(--bg) 78%);font:15px/1.4 ui-rounded,system-ui,-apple-system,sans-serif;color:var(--ink)}button,input,select{font:inherit}button{color:inherit}main{width:min(100%,820px);margin:auto;padding:18px 18px 34px}.topbar{display:flex;align-items:center;justify-content:space-between;margin:0 2px 14px}.brand{display:flex;align-items:center;gap:10px}.logo{display:grid;place-items:center;width:40px;height:40px;border-radius:13px;background:linear-gradient(145deg,var(--mint),#4bb39f);color:#152820;font-size:23px;box-shadow:0 8px 24px #0005}.brand h1{margin:0;font-size:22px;line-height:1}.brand small{display:block;margin-top:5px;color:var(--muted);font-weight:650}.online{display:flex;align-items:center;gap:7px;padding:7px 10px;border:1px solid #6e5c88;border-radius:99px;background:#251d3bcc;color:#dcd4eb;font-size:12px;font-weight:800}.dot{width:8px;height:8px;border-radius:50%;background:var(--mint);box-shadow:0 0 12px var(--mint)}
.room{position:relative;height:390px;overflow:hidden;border:1px solid #806ca0;border-radius:30px;background:linear-gradient(#66548c 0 61%,#393157 61% 66%,#2d2746 66%);box-shadow:0 24px 70px #0009}.room:before{content:'';position:absolute;inset:0;background:radial-gradient(circle at 50% 35%,#ffffff18,transparent 38%);pointer-events:none}.window{position:absolute;left:28px;top:32px;width:94px;height:85px;border:7px solid #4a3b69;border-radius:18px;background:linear-gradient(#253b65,#705881);box-shadow:inset 0 0 0 2px #a190b8}.window:after{content:'✦  ·  ✧';position:absolute;inset:22px 0;text-align:center;color:#fff5b8;font-size:17px}.shelf{position:absolute;right:24px;top:61px;width:100px;height:8px;border-radius:8px;background:#3e315c}.shelf:before{content:'●  ▲  ◆';position:absolute;bottom:8px;left:8px;color:#ffd36a;font-size:17px;letter-spacing:5px}.plant{position:absolute;right:38px;bottom:83px;font-size:44px;filter:drop-shadow(0 7px 5px #0004)}.rug{position:absolute;left:20%;right:20%;bottom:27px;height:75px;border-radius:50%;background:#ff8fc72b;border:2px solid #ffb9dd55;box-shadow:inset 0 0 30px #ff8fc722}.bubble{position:absolute;z-index:4;top:22px;left:50%;max-width:min(330px,62%);transform:translateX(-50%);padding:10px 15px;border-radius:17px 17px 17px 5px;background:#fff;color:#34294c;font-weight:800;text-align:center;box-shadow:0 8px 22px #0005}.room-meta{position:absolute;z-index:5;left:18px;right:18px;bottom:13px;display:flex;justify-content:space-between;pointer-events:none}.chip{padding:6px 9px;border:1px solid #ffffff25;border-radius:99px;background:#171122c9;backdrop-filter:blur(8px);color:#ddd5ea;font-size:11px;font-weight:850;text-transform:uppercase;letter-spacing:.04em}
.pet{position:absolute;z-index:3;left:50%;bottom:64px;width:150px;height:155px;transform:translateX(-50%);animation:bob 2.2s ease-in-out infinite}.pet-body{position:absolute;inset:25px 8px 0;border-radius:48% 48% 44% 44%;background:linear-gradient(145deg,#99f5d7,#50c9ad);box-shadow:inset -12px -14px #249b8b55,0 15px 20px #0005}.ear{position:absolute;top:12px;width:52px;height:60px;background:#75e6c2;transform:rotate(28deg);border-radius:70% 20%}.ear.r{right:0;transform:rotate(-28deg)}.ear.l{left:0}.eye{position:absolute;top:69px;width:15px;height:22px;border-radius:50%;background:#251d3d}.eye.l{left:46px}.eye.r{right:46px}.mouth{position:absolute;left:65px;top:99px;width:22px;height:12px;border-bottom:4px solid #251d3d;border-radius:50%}.cheek{position:absolute;top:92px;width:22px;height:10px;border-radius:50%;background:#ff8fc799}.cheek.l{left:22px}.cheek.r{right:22px}.pet.sleeping .eye{height:8px;border-radius:0;background:transparent;border-bottom:3px solid #251d3d}.pet.sleeping:after{content:'Z z';position:absolute;right:-22px;top:24px;color:#fff;font-weight:900;font-size:23px}.pet.sound-low .ear.l{transform:rotate(12deg)}.pet.sound-low .ear.r{transform:rotate(-12deg)}.pet.sound-medium{animation:bob .72s ease-in-out infinite}.pet.sound-high{animation:jump .42s ease-out infinite}.pet.sound-high .eye{height:27px;width:19px}.pet.motion-tilt-left{animation:none;transform:translateX(-56%) rotate(-7deg)}.pet.motion-tilt-right{animation:none;transform:translateX(-44%) rotate(7deg)}.pet.motion-tilt-forward{animation:none;transform:translateX(-50%) scale(.94)}.pet.motion-tilt-back{animation:none;transform:translateX(-50%) scale(1.06)}.pet.motion-shake{animation:shake .18s linear infinite}.pet.motion-rocking{animation:rock .7s ease-in-out infinite}
.tabs{display:grid;grid-template-columns:repeat(3,1fr);gap:7px;margin:14px 0;padding:6px;border:1px solid var(--line);border-radius:18px;background:#191327cc}.tab{border:0;border-radius:12px;padding:10px;background:transparent;color:var(--muted);font-weight:850;cursor:pointer}.tab.active{background:#6c5292;color:#fff;box-shadow:0 5px 14px #0004}.panel{display:none}.panel.active{display:block}.card{padding:18px;border:1px solid var(--line);border-radius:22px;background:linear-gradient(145deg,#251d3c,#1d172f);box-shadow:0 14px 35px #0004}.section-title{display:flex;align-items:flex-start;justify-content:space-between;gap:12px;margin-bottom:15px}.section-title h2,.identity h2{margin:0;font-size:24px}.section-title p{margin:4px 0 0;color:var(--muted)}.identity{display:flex;align-items:center;justify-content:space-between;gap:12px}.mood{margin-top:3px;color:var(--pink);font-weight:850;text-transform:capitalize}.age{color:var(--muted);font-weight:700}.stats{display:grid;grid-template-columns:repeat(2,1fr);gap:12px;margin-top:17px}.stat{padding:12px;border:1px solid #ffffff12;border-radius:15px;background:#120e20aa}.stat label{display:flex;justify-content:space-between;color:var(--muted);font-size:13px}.stat b{color:#fff}.track,.meter{height:9px;margin-top:8px;border-radius:99px;background:#0b0812;overflow:hidden}.fill,.meter-fill{height:100%;width:0;border-radius:inherit;background:linear-gradient(90deg,var(--pink),var(--sun));transition:width .3s}.settings{display:flex;gap:8px;margin-top:14px}.settings input{min-width:0;flex:1;border:1px solid #66527f;border-radius:12px;padding:11px;background:#120e20;color:#fff}.primary,.secondary{border:0;border-radius:12px;padding:11px 14px;background:#74559d;color:#fff;font-weight:850;cursor:pointer}.secondary{border:1px solid #76668d;background:#2b2340}.primary:active,.secondary:active,.action:active{transform:translateY(1px)}
.actions{display:grid;grid-template-columns:repeat(5,1fr);gap:10px}.action{min-height:104px;border:1px solid #ffffff17;border-radius:18px;padding:13px 8px;background:linear-gradient(145deg,#3a2c56,#29203f);color:#fff;cursor:pointer}.action span{display:block;font-size:29px}.action b{display:block;margin-top:7px}.action small{display:block;margin-top:3px;color:var(--muted);font-size:11px}.action:hover{border-color:#a887cc;background:#493664}.sensor-grid{display:grid;grid-template-columns:1fr 1fr;gap:12px}.sensor{padding:16px;border:1px solid #4c7e76;border-radius:18px;background:linear-gradient(145deg,#173338,#12282d)}.sensor.motion{border-color:#68558a;background:linear-gradient(145deg,#2d2345,#211a35)}.sensor-head{display:flex;justify-content:space-between;align-items:center;gap:10px}.sensor-title{font-weight:900}.sensor-state{color:var(--mint);font-size:12px;font-weight:900;text-transform:capitalize}.meter{height:12px;margin:14px 0 12px}.meter-fill{background:linear-gradient(90deg,var(--mint),var(--sun),var(--pink))}.controls{display:flex;align-items:center;justify-content:space-between;gap:8px;flex-wrap:wrap}.controls label{color:var(--muted);font-size:12px}.controls select{margin-left:4px;padding:8px;border:1px solid #716686;border-radius:10px;background:#120e20;color:#fff}.note{margin:12px 0 0;color:#abc9c4;font-size:12px}.privacy{margin-top:12px;padding:12px;border:1px solid #4d4162;border-radius:14px;background:#171124;color:var(--muted);font-size:12px}.foot{text-align:center;color:#958ba6;font-size:12px;margin:17px 0 0}
@keyframes bob{50%{transform:translate(-50%,-7px)}}@keyframes jump{50%{transform:translate(-50%,-18px) scale(1.06)}}@keyframes shake{25%{transform:translateX(-57%) rotate(-4deg)}75%{transform:translateX(-43%) rotate(4deg)}}@keyframes rock{25%{transform:translateX(-50%) rotate(-9deg)}75%{transform:translateX(-50%) rotate(9deg)}}
@media(max-width:620px){main{padding:10px 10px 24px}.topbar{margin:4px 3px 10px}.online span:last-child{display:none}.room{height:330px;border-radius:23px}.window{left:18px;top:26px;transform:scale(.78);transform-origin:top left}.shelf{right:14px;top:54px;transform:scale(.8);transform-origin:top right}.plant{right:20px;bottom:70px;font-size:35px}.bubble{top:17px;max-width:66%;font-size:13px}.pet{bottom:53px;transform:translateX(-50%) scale(.88)}.room-meta{left:10px;right:10px;bottom:9px}.tabs{position:sticky;z-index:20;bottom:8px;order:3;box-shadow:0 10px 35px #000b}.stats,.sensor-grid{grid-template-columns:1fr}.actions{grid-template-columns:repeat(2,1fr)}.action{min-height:91px}.action:last-child{grid-column:1/-1}.card{padding:15px}.settings{flex-direction:column}}
@media(prefers-reduced-motion:reduce){*,*:before,*:after{animation-duration:.01ms!important;animation-iteration-count:1!important;transition:none!important}}
</style></head><body><main>
<header class="topbar"><div class="brand"><div class="logo">♥</div><div><h1>PocketPal S3</h1><small>Your tiny offline companion</small></div></div><div class="online"><span class="dot"></span><span>Local &amp; live</span></div></header>
<section class="room" aria-label="PocketPal room"><div class="window"></div><div class="shelf"></div><div class="plant">♣</div><div id="bubble" class="bubble" aria-live="polite">Waking up…</div><div class="rug"></div><div id="pet" class="pet"><div class="ear l"></div><div class="ear r"></div><div class="pet-body"></div><div class="eye l"></div><div class="eye r"></div><div class="mouth"></div><div class="cheek l"></div><div class="cheek r"></div></div><div class="room-meta"><span id="soundChip" class="chip">Mic · calibrating</span><span id="motionChip" class="chip">Motion · calibrating</span></div></section>
<nav class="tabs" aria-label="PocketPal sections"><button class="tab active" data-panel="home" onclick="showPanel('home')">♥ Home</button><button class="tab" data-panel="care" onclick="showPanel('care')">✦ Care</button><button class="tab" data-panel="sensors" onclick="showPanel('sensors')">◉ Sensors</button></nav>
<section id="home" class="panel active"><div class="card"><div class="identity"><div><h2 id="name">Pip</h2><div id="mood" class="mood">Content</div></div><div id="age" class="age">Age 0m</div></div><div id="stats" class="stats"></div><form class="settings" onsubmit="renamePet(event)"><input id="newName" maxlength="12" aria-label="New pet name" placeholder="Give your pal a new name"><button class="primary">Rename</button></form></div></section>
<section id="care" class="panel"><div class="card"><div class="section-title"><div><h2>Care time</h2><p>Every little action shapes your pal’s day.</p></div></div><div class="actions"><button class="action" onclick="act('feed')"><span>🍓</span><b>Feed</b><small>Restore fullness</small></button><button class="action" onclick="act('play')"><span>🎾</span><b>Play</b><small>Build happiness</small></button><button class="action" onclick="act('clean')"><span>🫧</span><b>Clean</b><small>Fresh and sparkly</small></button><button class="action" onclick="act('sleep')"><span>🌙</span><b>Sleep</b><small>Rest or wake up</small></button><button class="action" onclick="act('pet')"><span>💗</span><b>Pet</b><small>Share affection</small></button></div></div></section>
<section id="sensors" class="panel"><div class="card"><div class="section-title"><div><h2>Reactions</h2><p>Live signals processed privately on your PocketPal.</p></div></div><div class="sensor-grid"><article class="sensor"><div class="sensor-head"><span class="sensor-title">🎙 Sound</span><span id="soundState" class="sensor-state">Calibrating</span></div><div class="meter"><div id="soundFill" class="meter-fill"></div></div><div class="controls"><button id="muteButton" class="secondary" onclick="setSound('toggle')">Mute</button><label>Sensitivity <select id="sensitivity" onchange="setSensitivity(this.value)"><option value="1">Low</option><option value="2">Medium</option><option value="3">High</option></select></label></div><p class="note">Reacts to loudness only. No audio is saved.</p></article><article class="sensor motion"><div class="sensor-head"><span class="sensor-title">🧭 Motion</span><span id="motionState" class="sensor-state">Calibrating</span></div><div class="meter"><div id="motionFill" class="meter-fill"></div></div><div class="controls"><label>Sensitivity <select id="motionSensitivity" onchange="setMotionSensitivity(this.value)"><option value="1">Low</option><option value="2">Medium</option><option value="3">High</option></select></label></div><p class="note">Tilt to lean, shake to play, rock to sleep.</p></article></div><div class="privacy">🔒 Everything stays between this browser and PocketPal. No internet, cloud account, recordings, or telemetry.</div></div></section>
<p class="foot">Connected directly to PocketPal · http://pocketpal</p></main>
<script>
const labels={fullness:['🍓','Fullness'],happiness:['♥','Happiness'],energy:['⚡','Energy'],cleanliness:['✦','Cleanliness'],health:['✚','Health']};
const soundMessages={low:"I'm listening…",medium:'I hear you!',high:'Whoa! That was loud!'};
const motionMessages={shake:'That was fun!',rocking:'Getting sleepy…'};
const byId=id=>document.getElementById(id);
function showPanel(id){document.querySelectorAll('.panel').forEach(p=>p.classList.toggle('active',p.id===id));document.querySelectorAll('.tab').forEach(t=>t.classList.toggle('active',t.dataset.panel===id))}
function prettyAge(minutes){if(minutes<60)return'Age '+minutes+'m';const hours=Math.floor(minutes/60);return hours<48?'Age '+hours+'h':'Age '+Math.floor(hours/24)+'d'}
async function post(url,body){return fetch(url,{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body})}
async function refresh(){try{const r=await fetch('/api/state',{cache:'no-store'});if(!r.ok)throw Error();const s=await r.json();byId('name').textContent=s.name;byId('mood').textContent=s.mood;byId('age').textContent=prettyAge(s.ageMinutes);byId('bubble').textContent=motionMessages[s.motion.state]||soundMessages[s.sound.level]||s.message;const p=byId('pet');p.className='pet';p.classList.toggle('sleeping',s.sleeping);if(['low','medium','high'].includes(s.sound.level))p.classList.add('sound-'+s.sound.level);if(!['still','calibrating','unavailable'].includes(s.motion.state))p.classList.add('motion-'+s.motion.state);byId('stats').innerHTML=Object.entries(labels).map(([k,v])=>`<div class="stat"><label><span>${v[0]} ${v[1]}</span><b>${s[k]}%</b></label><div class="track"><div class="fill" style="width:${s[k]}%"></div></div></div>`).join('');const soundLabel=s.sound.muted?'muted':s.sound.level;byId('soundState').textContent=soundLabel;byId('soundChip').textContent='Mic · '+soundLabel;byId('soundFill').style.width=s.sound.meter+'%';byId('muteButton').textContent=s.sound.muted?'Unmute':'Mute';byId('sensitivity').value=String(s.sound.sensitivity);const motionLabel=s.motion.state.replace('tilt-','');byId('motionState').textContent=motionLabel;byId('motionChip').textContent='Motion · '+motionLabel;byId('motionFill').style.width=s.motion.meter+'%';byId('motionSensitivity').value=String(s.motion.sensitivity)}catch(e){byId('bubble').textContent='Looking for PocketPal…';byId('soundChip').textContent='Mic · offline';byId('motionChip').textContent='Motion · offline'}}
async function act(a){await post('/api/action','action='+encodeURIComponent(a));await refresh()}
async function renamePet(e){e.preventDefault();const input=byId('newName'),v=input.value.trim();if(!v)return;await post('/api/name','name='+encodeURIComponent(v));input.value='';await refresh()}
async function setSound(command){await post('/api/sound','command='+encodeURIComponent(command));await refresh()}
async function setSensitivity(value){await post('/api/sound','sensitivity='+encodeURIComponent(value));await refresh()}
async function setMotionSensitivity(value){await post('/api/motion','sensitivity='+encodeURIComponent(value));await refresh()}
refresh();setInterval(refresh,500);
</script></body></html>
)HTML";
}

WebPortal::WebPortal(PetEngine &pet, SoundSensor &sound, MotionSensor &motion) : pet_(pet), sound_(sound), motion_(motion) {}

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
    if (MDNS.begin("pocketpal")) MDNS.addService("http", "tcp", 80);
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
    const SoundSnapshot sound = sound_.snapshot();
    const MotionSnapshot motion = motion_.snapshot();
    String json;
    json.reserve(640);
    json += "{\"name\":\"" + jsonEscape(s.name) + "\"";
    json += ",\"fullness\":" + String(s.fullness);
    json += ",\"happiness\":" + String(s.happiness);
    json += ",\"energy\":" + String(s.energy);
    json += ",\"cleanliness\":" + String(s.cleanliness);
    json += ",\"health\":" + String(s.health);
    json += ",\"ageMinutes\":" + String(s.ageMinutes);
    json += ",\"sleeping\":" + String(s.sleeping ? "true" : "false");
    json += ",\"mood\":\"" + jsonEscape(s.mood) + "\"";
    json += ",\"message\":\"" + jsonEscape(s.message) + "\"";
    json += ",\"sound\":{\"level\":\"" + jsonEscape(sound.levelName) + "\"";
    json += ",\"meter\":" + String(sound.meter);
    json += ",\"sensitivity\":" + String(sound.sensitivity);
    json += ",\"muted\":" + String(sound.muted ? "true" : "false");
    json += ",\"available\":" + String(sound.available ? "true" : "false");
    json += ",\"calibrated\":" + String(sound.calibrated ? "true" : "false") + "}";
    json += ",\"motion\":{\"state\":\"" + jsonEscape(motion.stateName) + "\"";
    json += ",\"meter\":" + String(motion.meter);
    json += ",\"sensitivity\":" + String(motion.sensitivity);
    json += ",\"available\":" + String(motion.available ? "true" : "false");
    json += ",\"calibrated\":" + String(motion.calibrated ? "true" : "false") + "}}";
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

void WebPortal::handleSound() {
    bool changed = false;
    if (server_.hasArg("command") && server_.arg("command") == "toggle") {
        sound_.toggleMuted();
        changed = true;
    }
    if (server_.hasArg("sensitivity")) {
        changed = sound_.setSensitivity(static_cast<uint8_t>(server_.arg("sensitivity").toInt())) || changed;
    }
    if (!changed) {
        server_.send(400, "application/json", "{\"ok\":false}");
        return;
    }
    server_.send(200, "application/json", "{\"ok\":true}");
}

void WebPortal::handleMotion() {
    if (!server_.hasArg("sensitivity") ||
        !motion_.setSensitivity(static_cast<uint8_t>(server_.arg("sensitivity").toInt()))) {
        server_.send(400, "application/json", "{\"ok\":false}");
        return;
    }
    server_.send(200, "application/json", "{\"ok\":true}");
}

void WebPortal::configureRoutes() {
    server_.on("/", HTTP_GET, [this]() { server_.send_P(200, "text/html", kPage); });
    server_.on("/api/state", HTTP_GET, [this]() { sendState(); });
    server_.on("/api/action", HTTP_POST, [this]() { handleAction(); });
    server_.on("/api/name", HTTP_POST, [this]() { handleName(); });
    server_.on("/api/sound", HTTP_POST, [this]() { handleSound(); });
    server_.on("/api/motion", HTTP_POST, [this]() { handleMotion(); });
    server_.on("/generate_204", HTTP_GET, [this]() { server_.sendHeader("Location", "/", true); server_.send(302); });
    server_.on("/hotspot-detect.html", HTTP_GET, [this]() { server_.send_P(200, "text/html", kPage); });
    server_.on("/connecttest.txt", HTTP_GET, [this]() { server_.sendHeader("Location", "/", true); server_.send(302); });
    server_.onNotFound([this]() {
        server_.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
        server_.send(302, "text/plain", "");
    });
}
