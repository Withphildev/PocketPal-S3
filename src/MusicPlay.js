// Trusted, static UI additions; no user content is interpolated here.
document.querySelector('#settings .settings-section:last-child').insertAdjacentHTML('beforebegin', `
<div class="settings-section"><h3>Music</h3><p class="note">A gentle forest theme, played by this browser. No internet needed.</p>
<div class="music-controls"><button id="musicButton" class="secondary" type="button" aria-pressed="false">Play music</button>
<label for="musicVolume">Volume <output id="musicVolumeValue" for="musicVolume">15%</output></label>
<input id="musicVolume" type="range" min="0" max="100" step="1" value="15"></div>
<p id="musicMessage" class="note" role="status"></p>
<p class="note">Preferences stay in this browser. Music pauses when you leave the page. Nearby speakers may trigger Pip’s Sound Reactions; you can mute those separately below.</p></div>`);
byId('care').insertAdjacentHTML('beforeend', `
<article class="card ball-card" aria-labelledby="ballTitle"><div class="section-title"><div><h2 id="ballTitle">Ball time</h2>
<p>A little keep-up game with your pal.</p></div></div>
<div class="ball-scores"><span>Bounces <strong id="ballScore">0</strong></span><span>Best <strong id="ballBest">0</strong></span></div>
<div id="ballCourt" class="ball-court"><img id="gamePip" src="/pet/happy.png" alt="Pip cheering you on" draggable="false">
<button id="gameBall" class="game-ball" type="button" aria-label="Bounce the soccer ball" disabled><img src="/props/ball.png" alt="" draggable="false"></button></div>
<div class="ball-controls"><button id="ballStart" class="primary" type="button" disabled>Play ball</button>
<button id="ballRetry" class="secondary" type="button" hidden>Retry save</button></div>
<p id="ballMessage" class="note" role="status">Keep the ball bouncing! Misses never lower Pip’s stats.</p>
<p class="note">Your best is saved on PocketPal. This game doesn’t spend energy or change care stats.</p></article>`);

// Offline browser-only music. Playback always begins with an explicit tap.
const music = {enabled:false, volume:.15, context:null, gain:null, buffer:null,
    source:null, loading:false, generation:0, notice:''};
function readLocal(key, fallback) {
    try { return JSON.parse(localStorage.getItem(key)) ?? fallback; } catch (_) { return fallback; }
}
function writeLocal(key, value) {
    try { localStorage.setItem(key, JSON.stringify(value)); } catch (_) { /* Private browsing is optional. */ }
}
const savedMusic = readLocal('pocketpal-music-v1', {});
music.enabled = savedMusic.enabled === true;
music.volume = typeof savedMusic.volume === 'number' && Number.isFinite(savedMusic.volume)
    ? Math.max(0, Math.min(1, savedMusic.volume)) : .15;
function saveMusic() { writeLocal('pocketpal-music-v1', {enabled:music.enabled, volume:music.volume}); }
function musicPlaying() { return !!music.source && music.context?.state === 'running'; }
function renderMusic(message) {
    if (message !== undefined) music.notice=message;
    byId('musicButton').textContent = music.loading ? 'Cancel loading' : musicPlaying() ? 'Music off' : 'Play music';
    byId('musicButton').setAttribute('aria-pressed', String(musicPlaying()));
    byId('musicVolume').value = String(Math.round(music.volume*100));
    byId('musicVolumeValue').textContent = Math.round(music.volume*100)+'%';
    byId('musicVolume').setAttribute('aria-valuetext', Math.round(music.volume*100)+' percent');
    byId('musicMessage').textContent = music.notice || (musicPlaying() ? 'Afternoon in the Canopy · looping offline'
        : music.enabled ? 'Your preference is saved. Tap Play music to listen.' : 'Off · plays through this phone or browser only.');
}
function stopMusic() {
    ++music.generation;
    music.notice='';
    music.loading = false;
    if (music.source) { music.source.stop(); music.source.disconnect(); music.source = null; }
    if (music.context?.state === 'running') music.context.suspend().catch(()=>{});
}
async function toggleMusic() {
    if (music.source && !musicPlaying()) stopMusic(); // Restart interrupted audio with this tap.
    if (music.source || music.loading) {
        music.enabled = false; stopMusic(); saveMusic(); renderMusic(); return;
    }
    const generation = ++music.generation;
    music.loading = true; renderMusic('Loading your offline forest theme…');
    try {
        const Audio = window.AudioContext || window.webkitAudioContext;
        if (!Audio) throw Error('unsupported');
        if (!music.context) {
            music.context = new Audio();
            music.gain = music.context.createGain();
            music.gain.connect(music.context.destination);
            music.context.addEventListener('statechange', ()=>renderMusic());
        }
        // Resume during the user gesture, before awaiting network/decode work.
        await music.context.resume();
        if (!music.buffer) {
            const response = await fetch('/audio/canopy-v1.mp3', {cache:'force-cache'});
            if (!response.ok) throw Error('download');
            music.buffer = await music.context.decodeAudioData(await response.arrayBuffer());
        }
        if (generation !== music.generation || document.hidden) return;
        if (music.context.state !== 'running') throw Error('blocked');
        music.gain.gain.value = music.volume;
        music.source = music.context.createBufferSource();
        music.source.buffer = music.buffer;
        music.source.loop = true;
        music.source.connect(music.gain);
        music.source.start();
        music.enabled = true; saveMusic();
    } catch (_) {
        if (generation === music.generation) {
            stopMusic(); renderMusic('Could not start music. Check the connection and tap to retry.'); return;
        }
    } finally {
        if (generation === music.generation) { music.loading = false; music.notice=''; renderMusic(); }
    }
}
byId('musicButton').addEventListener('click', toggleMusic);
byId('musicVolume').addEventListener('input', event=>{
    music.volume = Number(event.target.value)/100;
    if (music.gain) music.gain.gain.setTargetAtTime(music.volume, music.context.currentTime, .025);
    renderMusic();
});
byId('musicVolume').addEventListener('change', saveMusic);
renderMusic();

// Small, penalty-free ball game. Coordinates are normalized to the play area.
const ballGame = {running:false, score:0, best:0, ready:false, sleeping:false,
    x:.5, y:.75, vx:.16, vy:-.9, previous:0, lastTap:-Infinity, frame:0,
    cheerUntil:0, savePending:false};
function renderGame() {
    byId('ballScore').textContent = ballGame.score;
    byId('ballBest').textContent = ballGame.best;
    byId('ballStart').textContent = ballGame.running ? 'Finish game' : 'Play ball';
    byId('gameBall').disabled = !ballGame.running;
    byId('ballStart').disabled = !ballGame.ready || ballGame.sleeping;
    byId('ballStart').disabled ||= ballGame.savePending;
    byId('ballRetry').hidden = ballGame.running || ballGame.score<=ballGame.best || ballGame.savePending;
    byId('ballCourt').classList.toggle('playing', ballGame.running);
}
async function saveBest() {
    if (ballGame.savePending || ballGame.score <= ballGame.best) return;
    const score = ballGame.score;
    ballGame.savePending = true;
    renderGame();
    byId('ballMessage').textContent += ' Saving best…';
    try {
        const response = await post('/api/ball-best', 'score='+score);
        if (!response.ok) throw Error();
        const result = await response.json();
        if (!Number.isInteger(result.best) || result.best<0 || result.best>99999) throw Error();
        ballGame.best = Math.max(ballGame.best, result.best);
        byId('ballMessage').textContent = 'New best saved on PocketPal! Play again whenever you like.';
    } catch (_) {
        byId('ballMessage').textContent = 'Could not save your best. Keep this page open, reconnect, and tap Retry save.';
    } finally {
        ballGame.savePending = false;
        byId('ballRetry').hidden = ballGame.score <= ballGame.best;
        renderGame();
    }
}
function finishBall(message='Nice playing! No stats are lost—try again whenever you like.') {
    if (!ballGame.running) return;
    ballGame.running = false;
    ballGame.cheerUntil = 0;
    cancelAnimationFrame(ballGame.frame);
    byId('ballMessage').textContent = message;
    byId('gamePip').src = '/pet/happy.png';
    renderGame(); saveBest();
}
function placeBall() {
    const court = byId('ballCourt'), ball = byId('gameBall');
    ball.style.left = ballGame.x * (court.clientWidth-ball.offsetWidth)+'px';
    ball.style.top = ballGame.y * (court.clientHeight-ball.offsetHeight)+'px';
    // Reduced-motion users still see the essential game position, without rotation.
    ball.style.rotate = matchMedia('(prefers-reduced-motion: reduce)').matches ? '0deg' : ballGame.x*240+'deg';
}
function advanceBall(dt) {
    ballGame.vy += 1.7*dt;
    ballGame.y += ballGame.vy*dt;
    ballGame.x += ballGame.vx*dt;
    if (ballGame.x<.04 || ballGame.x>.96) {
        ballGame.x = Math.max(.04, Math.min(.96, ballGame.x)); ballGame.vx *= -1;
    }
    if (ballGame.y<0) { ballGame.y=0; ballGame.vy=Math.max(0,ballGame.vy); }
    if (ballGame.y>=1) { ballGame.y=1; placeBall(); finishBall(); return; }
    placeBall();
}
function ballFrame(now) {
    if (!ballGame.running) return;
    // A hidden/suspended page must never count a background miss.
    if (ballGame.previous && now-ballGame.previous>500) { finishBall('Game paused. Play again when you’re ready.'); return; }
    const dt = ballGame.previous ? Math.min(.05,(now-ballGame.previous)/1000) : 0;
    ballGame.previous=now;
    advanceBall(dt);
    if (ballGame.running) ballGame.frame=requestAnimationFrame(ballFrame);
}
function startBall() {
    if (ballGame.running) { finishBall(); return; }
    if (!ballGame.ready || ballGame.sleeping || ballGame.savePending) return;
    // Preserve an unsaved record until the user can retry it.
    if (ballGame.score>ballGame.best) { saveBest(); return; }
    Object.assign(ballGame,{running:true,score:0,x:.5,y:.65,vx:.16,vy:-.9,previous:0,lastTap:-Infinity});
    byId('ballRetry').hidden=true;
    byId('ballMessage').textContent='Tap the ball to keep it in the air. Space or Enter works when the ball is focused.';
    byId('gamePip').src='/pet/happy.png';
    renderGame(); placeBall();
    byId('gameBall').focus({preventScroll:true});
    ballGame.frame=requestAnimationFrame(ballFrame);
}
function tapBall() {
    const now=performance.now();
    if (!ballGame.running || now-ballGame.lastTap<160) return;
    ballGame.lastTap=now;
    ballGame.score=Math.min(99999,ballGame.score+1);
    ballGame.vy=-.9;
    ballGame.cheerUntil=now+650;
    byId('gamePip').src='/pet/excited.png';
    renderGame();
}
byId('ballStart').addEventListener('click',startBall);
byId('ballRetry').addEventListener('click',saveBest);
byId('gameBall').addEventListener('click',tapBall);
byId('gameBall').addEventListener('keydown',event=>{if(event.repeat)event.preventDefault();});
function syncMusicPlay(state) {
    ballGame.ready=true;
    ballGame.sleeping=state.sleeping;
    const best=Number(state.ballBest);
    if (Number.isInteger(best) && best>=0 && best<=99999) ballGame.best=Math.max(ballGame.best,best);
    if (state.sleeping) {
        finishBall('Pip is asleep. Wake him in Care before playing.');
        byId('gamePip').src='/pet/sleeping.png';
        byId('ballMessage').textContent='Pip is asleep. Wake him in Care before playing.';
    } else {
        if (byId('gamePip').getAttribute('src')==='/pet/sleeping.png') {
            byId('gamePip').src='/pet/happy.png';
            byId('ballMessage').textContent='Keep the ball bouncing! Misses never lower Pip’s stats.';
        }
        if (ballGame.running) {
            const cheering=performance.now()<ballGame.cheerUntil;
            state.pose=cheering?'excited':'happy';
            state.message=cheering?'Great bounce!':'Let’s play ball!';
            byId('gamePip').src='/pet/'+state.pose+'.png';
        }
    }
    renderGame();
}
document.addEventListener('click',event=>{
    if(event.target.closest('[data-panel]') && !byId('care').classList.contains('active')) finishBall('Game finished. Come back for another round anytime.');
    if(event.target.closest('.action')) finishBall();
});
function pauseMusicPlay() {
    stopMusic(); renderMusic(music.enabled?'Paused. Tap Play music when you return.':undefined);
    finishBall('Game paused. Come back for another round anytime.');
}
document.addEventListener('visibilitychange',()=>{if(document.hidden)pauseMusicPlay();});
window.addEventListener('pagehide',pauseMusicPlay);
renderGame();
