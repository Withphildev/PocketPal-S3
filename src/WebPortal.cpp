#include "WebPortal.h"

#include <ESPmDNS.h>

#include "PetAssets.h"
#include "RoomAssets.h"
#include "PetVisual.h"

namespace {
constexpr uint16_t kDnsPort = 53;

const char kPage[] PROGMEM = R"HTML(
<!doctype html><html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>PocketPal S3</title><style>
:root{color-scheme:dark;--base:#163D3A;--bg:#071c1b;--surface:#10312e;--surface2:#163D3A;--line:#38645f;--ink:#f4fcfa;--muted:#b1c9c5;--accent:#218b80;--accent-hover:#2aa496;--input:#081f1d;--panel:#0b2825;--track:#041512;--pink:#ff8fc7;--mint:#74e6c1;--sun:#ffd36a;--blue:#8db9ff}
:root[data-theme="purple"]{--base:#513c77;--bg:#100d1d;--surface:#211a36;--surface2:#2a2143;--line:#504269;--ink:#fcf9ff;--muted:#b9b0ca;--accent:#74559d;--accent-hover:#8063ac;--input:#120e20;--panel:#1d172f;--track:#0b0812}
:root[data-theme="pine"]{--base:#1E352D;--bg:#0a1913;--surface:#162e24;--surface2:#1E352D;--line:#416754;--ink:#f5fcf7;--muted:#b5cbbc;--accent:#377d5f;--accent-hover:#469b75;--input:#0d2118;--panel:#11261c;--track:#06150d}
:root[data-theme="navy"]{--base:#172A3A;--bg:#07131e;--surface:#112a3d;--surface2:#172A3A;--line:#35556e;--ink:#f4faff;--muted:#b2c5d5;--accent:#2d6a96;--accent-hover:#3984b8;--input:#091b2a;--panel:#0d2232;--track:#04111d}
:root[data-theme="charcoal"]{--base:#292A2D;--bg:#131415;--surface:#242528;--surface2:#292A2D;--line:#54565c;--ink:#fafafa;--muted:#c4c3c1;--accent:#62656c;--accent-hover:#7b7e86;--input:#191a1c;--panel:#202124;--track:#101112}
:root{--page-top:#205957;--header-top:#246361;--card-top:#20514d;--button-top:#329e91}
:root[data-theme="purple"]{--page-top:#4c366b;--header-top:#614782;--card-top:#42305b;--button-top:#9473bb}
:root[data-theme="pine"]{--page-top:#2c5041;--header-top:#355e4c;--card-top:#2a493b;--button-top:#519b76}
:root[data-theme="navy"]{--page-top:#214761;--header-top:#285775;--card-top:#203f57;--button-top:#4188b8}
:root[data-theme="charcoal"]{--page-top:#3e3f43;--header-top:#4b4c50;--card-top:#3b3c40;--button-top:#81838a}
*{box-sizing:border-box}body{margin:0;min-height:100vh;background:linear-gradient(180deg,var(--page-top) 0,var(--base) 210px,var(--bg) 760px);background-color:var(--bg);font:15px/1.4 ui-rounded,system-ui,-apple-system,sans-serif;color:var(--ink)}button,input,select{font:inherit}button{color:inherit}main{width:min(100%,820px);margin:auto;padding:18px 18px 34px}.topbar{display:flex;align-items:center;justify-content:space-between;margin:0 0 14px;padding:12px;border:1px solid var(--line);border-radius:20px;background:linear-gradient(135deg,var(--header-top),var(--surface) 85%)}.brand{display:flex;align-items:center;gap:10px}.logo{display:grid;place-items:center;width:40px;height:40px;border-radius:13px;background:linear-gradient(145deg,var(--mint),#4bb39f);color:#152820;font-size:23px;box-shadow:0 8px 24px #0005}.brand h1{margin:0;font-size:22px;line-height:1}.brand small{display:block;margin-top:5px;color:var(--muted);font-weight:650}.online{display:flex;align-items:center;gap:7px;padding:7px 10px;border:1px solid var(--line);border-radius:99px;background:linear-gradient(145deg,var(--surface2),var(--input));color:var(--ink);font-size:12px;font-weight:800}.dot{width:8px;height:8px;border-radius:50%;background:var(--mint);box-shadow:0 0 12px var(--mint)}
.room{position:relative;height:390px;overflow:hidden;border:1px solid #71937c;border-radius:30px;background:#365941 url('/room/forest-wide.jpg') center center/cover no-repeat;box-shadow:0 24px 70px #0009}.room:before{content:'';position:absolute;inset:0;background:linear-gradient(transparent 70%,#10251d66);pointer-events:none}.ground-shadow{position:absolute;z-index:2;left:50%;bottom:45px;width:165px;height:25px;transform:translateX(-50%);border-radius:50%;background:radial-gradient(ellipse,#16382980,transparent 70%);filter:blur(3px)}.bubble{position:absolute;z-index:4;top:22px;left:50%;max-width:min(330px,62%);transform:translateX(-50%);padding:10px 15px;border-radius:17px 17px 17px 5px;background:#fff;color:#34294c;font-weight:800;text-align:center;box-shadow:0 8px 22px #0005}.room-meta{position:absolute;z-index:5;left:18px;right:18px;bottom:13px;display:flex;justify-content:space-between;pointer-events:none}.chip{padding:6px 9px;border:1px solid #ffffff25;border-radius:99px;background:#171122c9;backdrop-filter:blur(8px);color:#ddd5ea;font-size:11px;font-weight:850;text-transform:uppercase;letter-spacing:.04em}
.pet{position:absolute;z-index:3;left:50%;bottom:49px;width:230px;height:230px;transform:translateX(-50%);transform-origin:50% 95%;animation:bob 2.2s ease-in-out infinite}.dragon{display:block;width:100%;height:100%;object-fit:contain;filter:drop-shadow(0 10px 8px #0004)}.pet.sleeping{animation:none}.pet.sleeping:after{content:'Z z';position:absolute;right:12px;top:72px;color:#fff;font-weight:900;font-size:23px}.pet.excited{animation:bob .72s ease-in-out infinite}.pet.surprised{animation:jump .42s ease-out infinite}.pet.motion-tilt-left{animation:none;transform:translateX(-56%) rotate(-7deg)}.pet.motion-tilt-right{animation:none;transform:translateX(-44%) rotate(7deg)}.pet.motion-tilt-forward{animation:none;transform:translateX(-50%) scale(.94)}.pet.motion-tilt-back{animation:none;transform:translateX(-50%) scale(1.06)}.pet.motion-shake{animation:shake .18s linear infinite}.pet.motion-rocking{animation:rock .7s ease-in-out infinite}
.tabs{display:grid;grid-template-columns:repeat(3,1fr);gap:7px;margin:14px 0;padding:6px;border:1px solid var(--line);border-radius:18px;background:linear-gradient(135deg,var(--surface),var(--input))}.tab{border:0;border-radius:12px;padding:10px;background:transparent;color:var(--muted);font-weight:850;cursor:pointer}.tab.active{background:linear-gradient(145deg,var(--button-top),var(--accent));color:#fff;box-shadow:0 5px 14px #0004}.panel{display:none}.panel.active{display:block}.card{padding:18px;border:1px solid var(--line);border-radius:22px;background:linear-gradient(145deg,var(--card-top),var(--panel) 85%);box-shadow:0 14px 35px #0004}.section-title{display:flex;align-items:flex-start;justify-content:space-between;gap:12px;margin-bottom:15px}.section-title h2,.identity h2{margin:0;font-size:24px}.section-title p{margin:4px 0 0;color:var(--muted)}.identity{display:flex;align-items:center;justify-content:space-between;gap:12px}.mood{margin-top:3px;color:var(--pink);font-weight:850;text-transform:capitalize}.age{color:var(--muted);font-weight:700}.stats{display:grid;grid-template-columns:repeat(2,1fr);gap:12px;margin-top:17px}.stat{padding:12px;border:1px solid #ffffff12;border-radius:15px;background:var(--input)}.stat label{display:flex;justify-content:space-between;color:var(--muted);font-size:13px}.stat b{color:#fff}.track,.meter{height:9px;margin-top:8px;border-radius:99px;background:var(--track);overflow:hidden}.fill,.meter-fill{height:100%;width:0;border-radius:inherit;background:linear-gradient(90deg,var(--pink),var(--sun));transition:width .3s}.settings{display:flex;gap:8px;margin-top:14px}.settings input{min-width:0;flex:1;border:1px solid var(--line);border-radius:12px;padding:11px;background:var(--input);color:#fff}.primary,.secondary{border:0;border-radius:12px;padding:11px 14px;background:linear-gradient(145deg,var(--button-top),var(--accent));color:#fff;font-weight:850;cursor:pointer}.secondary{border:1px solid var(--line);background:linear-gradient(145deg,var(--card-top),var(--surface))}.primary:active,.secondary:active,.action:active{transform:translateY(1px)}
.actions{display:grid;grid-template-columns:repeat(5,1fr);gap:10px}.action{min-height:104px;border:1px solid #ffffff17;border-radius:18px;padding:13px 8px;background:linear-gradient(145deg,var(--card-top),var(--panel) 85%);color:#fff;cursor:pointer}.action span{display:block;font-size:29px}.action b{display:block;margin-top:7px}.action small{display:block;margin-top:3px;color:var(--muted);font-size:11px}.action:hover{border-color:var(--muted);background:var(--accent-hover)}.sensor-grid{display:grid;grid-template-columns:1fr 1fr;gap:12px}.sensor{padding:16px;border:1px solid var(--line);border-radius:18px;background:linear-gradient(145deg,var(--card-top),var(--panel) 85%)}.sensor.motion{border-color:var(--line)}.sensor-head{display:flex;justify-content:space-between;align-items:center;gap:10px}.sensor-title{font-weight:900}.sensor-state{color:var(--mint);font-size:12px;font-weight:900;text-transform:capitalize}.meter{height:12px;margin:14px 0 12px}.meter-fill{background:linear-gradient(90deg,var(--mint),var(--sun),var(--pink))}.controls{display:flex;align-items:center;justify-content:space-between;gap:8px;flex-wrap:wrap}.controls label{color:var(--muted);font-size:12px}.controls select{margin-left:4px;padding:8px;border:1px solid var(--line);border-radius:10px;background:var(--input);color:#fff}.note{margin:12px 0 0;color:var(--muted);font-size:12px}.privacy{margin-top:12px;padding:12px;border:1px solid var(--line);border-radius:14px;background:var(--input);color:var(--muted);font-size:12px}.foot{text-align:center;color:var(--muted);font-size:12px;margin:17px 0 0}
@keyframes bob{50%{transform:translate(-50%,-7px)}}@keyframes jump{50%{transform:translate(-50%,-18px) scale(1.06)}}@keyframes shake{25%{transform:translateX(-57%) rotate(-4deg)}75%{transform:translateX(-43%) rotate(4deg)}}@keyframes rock{25%{transform:translateX(-50%) rotate(-9deg)}75%{transform:translateX(-50%) rotate(9deg)}}
@media(max-width:620px){main{padding:10px 10px 24px}.topbar{margin:0 0 10px;padding:10px}.brand h1{font-size:20px}.brand small{font-size:11px}.online span:last-child{display:none}.room{height:390px;border-radius:23px;background-image:url('/room/forest-mobile.jpg');background-position:center 65%}.ground-shadow{bottom:36px;width:140px}.bubble{top:17px;max-width:66%;font-size:13px}.pet{bottom:39px;width:200px;height:200px}.room-meta{left:10px;right:10px;bottom:9px}.tabs{position:sticky;z-index:20;bottom:8px;order:3;box-shadow:0 10px 35px #000b}.stats,.sensor-grid{grid-template-columns:1fr}.actions{grid-template-columns:repeat(2,1fr)}.action{min-height:91px}.action:last-child{grid-column:1/-1}.card{padding:15px}.settings{flex-direction:column}}
@media(prefers-reduced-motion:reduce){*,*:before,*:after{animation-duration:.01ms!important;animation-iteration-count:1!important;transition:none!important}}

.stat[data-stat="fullness"]{--stat-start:#FF6B6B;--stat-end:#FF9A76}.stat[data-stat="happiness"]{--stat-start:#FF70A6;--stat-end:#FF9EC4}.stat[data-stat="energy"]{--stat-start:#F6C445;--stat-end:#FFE47A}.stat[data-stat="cleanliness"]{--stat-start:#4FC3F7;--stat-end:#76E4E8}.stat[data-stat="health"]{--stat-start:#55D98A;--stat-end:#8BE6A8}.stat .fill{background:linear-gradient(90deg,var(--stat-start),var(--stat-end))}.stat label span{color:var(--stat-end)}
.settings{display:grid;grid-template-columns:minmax(0,1fr) auto auto}.theme-picker[hidden]{display:none}.theme-picker{margin:12px 0 0;padding:12px;border:1px solid var(--line);border-radius:15px;background:var(--input)}.theme-picker legend{color:var(--muted);padding:0 6px;font-size:13px}.theme-options{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:8px}.theme-choice{display:flex;align-items:center;gap:9px;min-height:44px;padding:9px;border:1px solid var(--line);border-radius:11px;background:linear-gradient(145deg,var(--surface2),var(--panel));text-align:left;font-size:12px;font-weight:750;cursor:pointer}.theme-choice[aria-pressed="true"]{border-color:var(--mint);box-shadow:inset 0 0 0 1px var(--mint)}.swatch{flex:none;width:21px;height:21px;border:1px solid #ffffff66;border-radius:50%;background:linear-gradient(145deg,#ffffff28,transparent 75%),var(--swatch)}.theme-choice:disabled{opacity:.65;cursor:wait}.color-message{margin:8px 0 0;font-size:12px;color:var(--muted)}button:focus-visible,input:focus-visible,select:focus-visible{outline:2px solid var(--mint);outline-offset:3px}
@media(max-width:620px){.settings{grid-template-columns:1fr 1fr}.settings input{grid-column:1/-1}.theme-options{grid-template-columns:1fr 1fr}}
</style></head><body><main>
<header class="topbar"><div class="brand"><div class="logo">♥</div><div><h1>PocketPal S3</h1><small>Your tiny offline companion</small></div></div><div class="online"><span class="dot"></span><span>Local &amp; live</span></div></header>
<section class="room" aria-label="PocketPal forest clearing"><div id="bubble" class="bubble" aria-live="polite">Waking up…</div><div class="ground-shadow"></div><div id="pet" class="pet"><img id="dragon" class="dragon" src="/pet/idle.png" alt="Mint-green PocketPal dragon" width="240" height="240"></div><div class="room-meta"><span id="soundChip" class="chip">Mic · calibrating</span><span id="motionChip" class="chip">Motion · calibrating</span></div></section>
<nav class="tabs" aria-label="PocketPal sections"><button class="tab active" data-panel="home" onclick="showPanel('home')">♥ Home</button><button class="tab" data-panel="care" onclick="showPanel('care')">✦ Care</button><button class="tab" data-panel="sensors" onclick="showPanel('sensors')">◉ Sensors</button></nav>
<section id="home" class="panel active"><div class="card"><div class="identity"><div><h2 id="name">Pip</h2><div id="mood" class="mood">Content</div></div><div id="age" class="age">Age 0m</div></div><div id="stats" class="stats"></div><form class="settings" onsubmit="renamePet(event)"><input id="newName" maxlength="12" aria-label="New pet name" placeholder="Give your pal a new name"><button class="primary">Rename</button><button id="colorButton" class="secondary" type="button" aria-expanded="false" aria-controls="themePicker" onclick="toggleColors()">◐ Color</button></form><fieldset id="themePicker" class="theme-picker" hidden><legend>Choose your color</legend><div class="theme-options"><button type="button" class="theme-choice" data-theme-choice="teal" aria-pressed="true" onclick="setTheme('teal')"><span class="swatch" style="--swatch:#163D3A" aria-hidden="true"></span>Deep Forest Teal</button><button type="button" class="theme-choice" data-theme-choice="purple" aria-pressed="false" onclick="setTheme('purple')"><span class="swatch" style="--swatch:#513c77" aria-hidden="true"></span>Purple</button><button type="button" class="theme-choice" data-theme-choice="pine" aria-pressed="false" onclick="setTheme('pine')"><span class="swatch" style="--swatch:#1E352D" aria-hidden="true"></span>Dark Pine</button><button type="button" class="theme-choice" data-theme-choice="navy" aria-pressed="false" onclick="setTheme('navy')"><span class="swatch" style="--swatch:#172A3A" aria-hidden="true"></span>Deep Navy</button><button type="button" class="theme-choice" data-theme-choice="charcoal" aria-pressed="false" onclick="setTheme('charcoal')"><span class="swatch" style="--swatch:#292A2D" aria-hidden="true"></span>Warm Charcoal</button></div><p id="colorMessage" class="color-message" role="status">Saved on your PocketPal.</p></fieldset></div></section>
<section id="care" class="panel"><div class="card"><div class="section-title"><div><h2>Care time</h2><p>Every little action shapes your pal’s day.</p></div></div><div class="actions"><button class="action" onclick="act('feed')"><span>🍓</span><b>Feed</b><small>Restore fullness</small></button><button class="action" onclick="act('play')"><span>🎾</span><b>Play</b><small>Build happiness</small></button><button class="action" onclick="act('clean')"><span>🫧</span><b>Clean</b><small>Fresh and sparkly</small></button><button class="action" onclick="act('sleep')"><span>🌙</span><b>Sleep</b><small>Rest or wake up</small></button><button class="action" onclick="act('pet')"><span>💗</span><b>Pet</b><small>Share affection</small></button></div></div></section>
<section id="sensors" class="panel"><div class="card"><div class="section-title"><div><h2>Reactions</h2><p>Live signals processed privately on your PocketPal.</p></div></div><div class="sensor-grid"><article class="sensor"><div class="sensor-head"><span class="sensor-title">🎙 Sound</span><span id="soundState" class="sensor-state">Calibrating</span></div><div class="meter"><div id="soundFill" class="meter-fill"></div></div><div class="controls"><button id="muteButton" class="secondary" onclick="setSound('toggle')">Mute</button><label>Sensitivity <select id="sensitivity" onchange="setSensitivity(this.value)"><option value="1">Low</option><option value="2">Medium</option><option value="3">High</option></select></label></div><p class="note">Reacts to loudness only. No audio is saved.</p></article><article class="sensor motion"><div class="sensor-head"><span class="sensor-title">🧭 Motion</span><span id="motionState" class="sensor-state">Calibrating</span></div><div class="meter"><div id="motionFill" class="meter-fill"></div></div><div class="controls"><label>Sensitivity <select id="motionSensitivity" onchange="setMotionSensitivity(this.value)"><option value="1">Low</option><option value="2">Medium</option><option value="3">High</option></select></label></div><p class="note">Tilt to lean, shake to play, rock to sleep.</p></article></div><div class="privacy">🔒 Everything stays between this browser and PocketPal. No internet, cloud account, recordings, or telemetry.</div></div></section>
<p class="foot">Connected directly to PocketPal · http://pocketpal</p></main>
<script>
const labels={fullness:['🍓','Fullness'],happiness:['♥','Happiness'],energy:['⚡','Energy'],cleanliness:['✦','Cleanliness'],health:['✚','Health']};
const soundMessages={low:"I'm listening…",medium:'I hear you!',high:'Whoa! That was loud!'};
const motionMessages={shake:'That was fun!',rocking:'Getting sleepy…'};
const byId=id=>document.getElementById(id);
let savingTheme=false;
function applyTheme(theme){if(!['teal','purple','pine','navy','charcoal'].includes(theme))theme='teal';document.documentElement.dataset.theme=theme;document.querySelectorAll('[data-theme-choice]').forEach(button=>button.setAttribute('aria-pressed',String(button.dataset.themeChoice===theme)))}
function toggleColors(){const picker=byId('themePicker');picker.hidden=!picker.hidden;byId('colorButton').setAttribute('aria-expanded',String(!picker.hidden));if(!picker.hidden)picker.querySelector('[aria-pressed="true"]').focus()}
async function setTheme(theme){if(savingTheme)return;const previous=document.documentElement.dataset.theme||'teal';savingTheme=true;applyTheme(theme);document.querySelectorAll('.theme-choice').forEach(button=>button.disabled=true);byId('colorMessage').textContent='Saving color…';try{const response=await post('/api/theme','theme='+encodeURIComponent(theme));if(!response.ok)throw Error();byId('colorMessage').textContent='Color saved on your PocketPal.'}catch(error){applyTheme(previous);byId('colorMessage').textContent='Could not save. Check your connection and try again.'}finally{savingTheme=false;document.querySelectorAll('.theme-choice').forEach(button=>button.disabled=false)}}
function showPanel(id){document.querySelectorAll('.panel').forEach(p=>p.classList.toggle('active',p.id===id));document.querySelectorAll('.tab').forEach(t=>t.classList.toggle('active',t.dataset.panel===id))}
function prettyAge(minutes){if(minutes<60)return'Age '+minutes+'m';const hours=Math.floor(minutes/60);return hours<48?'Age '+hours+'h':'Age '+Math.floor(hours/24)+'d'}
async function post(url,body){return fetch(url,{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body})}
async function refresh(){try{const r=await fetch('/api/state',{cache:'no-store'});if(!r.ok)throw Error();const s=await r.json();if(!savingTheme)applyTheme(s.theme);byId('name').textContent=s.name;byId('mood').textContent=s.mood;byId('age').textContent=prettyAge(s.ageMinutes);byId('bubble').textContent=s.sleeping?s.message:(motionMessages[s.motion.state]||soundMessages[s.sound.level]||s.message);const p=byId('pet');let petClass='pet '+s.pose;const dragon=byId('dragon'),image='/pet/'+s.pose+'.png';if(dragon.getAttribute('src')!==image)dragon.src=image;if(!['still','calibrating','unavailable'].includes(s.motion.state))petClass+=' motion-'+s.motion.state;if(p.className!==petClass)p.className=petClass;byId('stats').innerHTML=Object.entries(labels).map(([k,v])=>`<div class="stat" data-stat="${k}"><label><span>${v[0]} ${v[1]}</span><b>${s[k]}%</b></label><div class="track"><div class="fill" style="width:${s[k]}%"></div></div></div>`).join('');const soundLabel=s.sound.muted?'muted':s.sound.level;byId('soundState').textContent=soundLabel;byId('soundChip').textContent='Mic · '+soundLabel;byId('soundFill').style.width=s.sound.meter+'%';byId('muteButton').textContent=s.sound.muted?'Unmute':'Mute';byId('sensitivity').value=String(s.sound.sensitivity);const motionLabel=s.motion.state.replace('tilt-','');byId('motionState').textContent=motionLabel;byId('motionChip').textContent='Motion · '+motionLabel;byId('motionFill').style.width=s.motion.meter+'%';byId('motionSensitivity').value=String(s.motion.sensitivity)}catch(e){byId('bubble').textContent='Looking for PocketPal…';byId('soundChip').textContent='Mic · offline';byId('motionChip').textContent='Motion · offline'}}
async function act(a){await post('/api/action','action='+encodeURIComponent(a));await refresh()}
async function renamePet(e){e.preventDefault();const input=byId('newName'),v=input.value.trim();if(!v)return;await post('/api/name','name='+encodeURIComponent(v));input.value='';await refresh()}
async function setSound(command){await post('/api/sound','command='+encodeURIComponent(command));await refresh()}
async function setSensitivity(value){await post('/api/sound','sensitivity='+encodeURIComponent(value));await refresh()}
async function setMotionSensitivity(value){await post('/api/motion','sensitivity='+encodeURIComponent(value));await refresh()}
['idle','happy','listening','excited','sleeping','surprised'].forEach(pose=>{const img=new Image();img.src='/pet/'+pose+'.png'});
refresh();setInterval(refresh,500);
</script></body></html>
)HTML";
}

WebPortal::WebPortal(PetEngine &pet, SoundSensor &sound, MotionSensor &motion) : pet_(pet), sound_(sound), motion_(motion) {}

void WebPortal::begin() {
    preferences_.begin("palui", false);
    theme_ = preferences_.getString("theme", "teal");
    if (!validTheme(theme_)) theme_ = "teal";
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
    json += ",\"theme\":\"" + theme_ + "\"";
    json += ",\"fullness\":" + String(s.fullness);
    json += ",\"happiness\":" + String(s.happiness);
    json += ",\"energy\":" + String(s.energy);
    json += ",\"cleanliness\":" + String(s.cleanliness);
    json += ",\"health\":" + String(s.health);
    json += ",\"ageMinutes\":" + String(s.ageMinutes);
    json += ",\"sleeping\":" + String(s.sleeping ? "true" : "false");
    json += ",\"mood\":\"" + jsonEscape(s.mood) + "\"";
    json += ",\"pose\":\"" + String(petPoseName(selectPetPose(s, sound.level, motion.state))) + "\"";
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

bool WebPortal::validTheme(const String &theme) {
    return theme == "teal" || theme == "purple" || theme == "pine" ||
           theme == "navy" || theme == "charcoal";
}

void WebPortal::handleTheme() {
    if (!server_.hasArg("theme") || !validTheme(server_.arg("theme"))) {
        server_.send(400, "application/json", "{\"ok\":false,\"error\":\"Choose one of the five themes.\"}");
        return;
    }
    const String requested = server_.arg("theme");
    if (requested != theme_) {
        if (preferences_.putString("theme", requested) == 0) {
            server_.send(500, "application/json", "{\"ok\":false,\"error\":\"Could not save the color.\"}");
            return;
        }
        theme_ = requested;
    }
    server_.send(200, "application/json", "{\"ok\":true}");
}

void WebPortal::configureRoutes() {
    server_.on("/api/theme", HTTP_POST, [this]() { handleTheme(); });
    server_.on("/room/forest-mobile.jpg", HTTP_GET, [this]() {
        server_.sendHeader("Cache-Control", "no-cache");
        server_.send_P(200, "image/jpeg", reinterpret_cast<const char *>(RoomAssets::forestMobile),
                       sizeof(RoomAssets::forestMobile));
    });
    server_.on("/room/forest-wide.jpg", HTTP_GET, [this]() {
        server_.sendHeader("Cache-Control", "no-cache");
        server_.send_P(200, "image/jpeg", reinterpret_cast<const char *>(RoomAssets::forestWide),
                       sizeof(RoomAssets::forestWide));
    });
    for (uint8_t index = 0; index < 6; ++index) {
        const String path = String("/pet/") + petPoseName(static_cast<PetPose>(index)) + ".png";
        server_.on(path.c_str(), HTTP_GET, [this, index]() {
            const PetImage &image = PetAssets::web[index];
            server_.sendHeader("Cache-Control", "no-cache");
            server_.send_P(200, "image/png", reinterpret_cast<const char *>(image.data), image.size);
        });
    }
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
