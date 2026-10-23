const path=require('node:path');
const fs=require('node:fs');
module.exports=async({page,browser,state,controls,commands,root,assert})=>{
    const output=path.join(root,'assets/music');
    assert(fs.readFileSync(path.join(root,'src/MusicPlayAssets.h'),'utf8').includes(fs.readFileSync(path.join(root,'src/MusicPlay.js'),'utf8')));
    assert.equal(await page.evaluate(()=>music.source),null);
    assert.equal(await page.locator('#musicVolume').inputValue(),'15');
    const audioRequests=[];
    page.on('request',r=>{if(r.url().includes('/audio/'))audioRequests.push(r.url());});
    await page.locator('[data-panel="settings"]').click();
    await page.locator('#musicButton').click();
    await page.waitForFunction(()=>musicPlaying());
    assert.equal(audioRequests.length,1);
    assert(await page.evaluate(()=>music.source.loop));
    const duration=await page.evaluate(()=>music.buffer.duration);
    assert(duration>12 && duration<13);
    await page.locator('#musicVolume').fill('23');
    await page.locator('#musicVolume').dispatchEvent('change');
    assert.equal(await page.evaluate(()=>music.volume),.23);
    await page.locator('#musicButton').click();
    assert.equal(await page.evaluate(()=>music.source),null);
    await page.locator('#musicButton').click();
    await page.waitForFunction(()=>musicPlaying());
    assert.equal(audioRequests.length,1); // Decoded buffer is reused, not streamed per loop.
    await page.reload();
    await page.waitForFunction(()=>ballGame.ready);
    assert.equal(await page.evaluate(()=>music.source),null); // No autoplay on reconnect.
    assert.equal(await page.locator('#musicVolume').inputValue(),'23');
    assert.match(await page.locator('#musicMessage').textContent(),/Tap Play music/);
    await page.locator('[data-panel="settings"]').click();
    await page.locator('#musicButton').click();
    await page.waitForFunction(()=>musicPlaying());
    await page.evaluate(()=>pauseMusicPlay());
    assert.equal(await page.evaluate(()=>music.source),null);
    assert(await page.evaluate(()=>music.enabled)); // Preference retained on background pause.
    await page.evaluate(()=>{stopMusic();music.enabled=false;saveMusic();renderMusic();});
    await page.locator('#musicVolume').fill('15');
    await page.locator('#musicVolume').dispatchEvent('change');
    await page.locator('#musicButton').locator('..').locator('..').screenshot({path:path.join(output,'music-settings-preview.png')});

    await page.locator('[data-panel="care"]').click();
    const originalStats=[state.fullness,state.happiness,state.energy,state.cleanliness,state.health];
    async function startFrozen() {
        await page.locator('#ballStart').click();
        await page.evaluate(()=>{cancelAnimationFrame(ballGame.frame);ballGame.previous=0;});
    }
    await startFrozen();
    // A real pointer tap on the actual image button.
    const ball=await page.locator('#gameBall').boundingBox();
    await page.mouse.click(ball.x+ball.width/2,ball.y+ball.height/2);
    assert.equal(await page.locator('#ballScore').textContent(),'1');
    assert.equal(await page.locator('#gamePip').getAttribute('src'),'/pet/excited.png');
    await page.evaluate(()=>{ballGame.lastTap=performance.now();tapBall();});
    assert.equal(await page.locator('#ballScore').textContent(),'1'); // Debounce.
    await page.evaluate(()=>{ballGame.lastTap=-Infinity;tapBall();});
    assert(await page.locator('#ballRetry').isHidden());
    await page.evaluate(()=>refresh());
    assert.equal(await page.locator('#dragon').getAttribute('src'),'/pet/excited.png');
    await page.locator('.ball-card').screenshot({path:path.join(output,'ball-game-preview.png')});
    await page.evaluate(()=>{for(let i=0;i<100&&ballGame.running;i++)advanceBall(.05);});
    await page.waitForFunction(()=>!ballGame.savePending);
    assert.equal(state.ballBest,2);
    assert.equal(await page.locator('#ballBest').textContent(),'2');
    assert.deepEqual([state.fullness,state.happiness,state.energy,state.cleanliness,state.health],originalStats);
    const writes=commands.filter(c=>c.url==='/api/ball-best').length;
    await startFrozen();
    await page.evaluate(()=>finishBall());
    assert.equal(commands.filter(c=>c.url==='/api/ball-best').length,writes); // Only a new record writes.
    await startFrozen();
    await page.locator('#gameBall').press('Space');
    assert.equal(await page.locator('#ballScore').textContent(),'1');
    await page.evaluate(()=>finishBall());
    controls.failScore=true;
    await startFrozen();
    await page.evaluate(()=>{for(let i=0;i<3;i++){ballGame.lastTap=-Infinity;tapBall();}finishBall();});
    await page.waitForFunction(()=>!ballGame.savePending);
    assert(await page.locator('#ballRetry').isVisible());
    assert.equal(state.ballBest,2);
    controls.failScore=false;
    await page.locator('#ballRetry').click();
    await page.waitForFunction(()=>!ballGame.savePending);
    assert.equal(state.ballBest,3);
    await page.reload();await page.waitForFunction(()=>ballGame.ready);
    assert.equal(await page.locator('#ballBest').textContent(),'3');
    await page.locator('[data-panel="care"]').click();
    await startFrozen();
    await page.locator('[data-panel="home"]').click();
    assert.equal(await page.evaluate(()=>ballGame.running),false);
    state.sleeping=true;
    await page.evaluate(()=>refresh());
    assert(await page.locator('#ballStart').isDisabled());
    state.sleeping=false;await page.evaluate(()=>refresh());
    await page.locator('[data-panel="care"]').click();
    await startFrozen();await page.evaluate(()=>pauseMusicPlay());
    assert.equal(await page.evaluate(()=>ballGame.running),false);
    await startFrozen();
    await page.emulateMedia({reducedMotion:'reduce'});
    await page.evaluate(()=>placeBall());
    assert.equal(await page.locator('#gameBall').evaluate(e=>e.style.rotate),'0deg');
    await page.evaluate(()=>finishBall());
    await page.emulateMedia({reducedMotion:'no-preference'});
    const failed=await browser.newPage();
    await failed.route('**/audio/**',route=>route.fulfill({status:503,body:''}));
    await failed.goto(page.url());
    await failed.locator('[data-panel="settings"]').click();
    await failed.locator('#musicButton').click();
    await failed.waitForFunction(()=>!music.loading);
    assert.match(await failed.locator('#musicMessage').textContent(),/Could not start/);
    assert.equal(await failed.evaluate(()=>music.source),null);
    await failed.close();
    const slow=await browser.newPage();
    let releaseAudio;
    await slow.route('**/audio/**',async route=>{
        await new Promise(resolve=>{releaseAudio=resolve;});
        await route.fulfill({status:200,contentType:'audio/mpeg',body:fs.readFileSync(path.join(root,'assets/music/Short_Afternoon_in_the_Canopy.mp3'))});
    });
    await slow.goto(page.url());
    await slow.locator('[data-panel="settings"]').click();
    await slow.locator('#musicButton').click();
    await slow.waitForFunction(()=>music.loading);
    while(!releaseAudio) await new Promise(resolve=>setTimeout(resolve,10));
    await slow.locator('#musicButton').click();
    assert.equal(await slow.evaluate(()=>music.loading),false);
    releaseAudio();
    await slow.waitForFunction(()=>music.buffer!==null);
    assert.equal(await slow.evaluate(()=>music.source),null); // Cancelled download never starts playback.
    await slow.close();
    for(const width of [320,390,1100]) {
        await page.setViewportSize({width,height:900});
        for(const panel of ['care','settings']) {
            await page.locator('[data-panel="'+panel+'"]').click();
            assert.equal(await page.evaluate(()=>document.documentElement.scrollWidth>innerWidth),false);
        }
    }
    await page.setViewportSize({width:390,height:844});
    console.log('PASS: real MP3 decode/loop, lazy loading, reuse, music preference/volume, no autoplay, pause/retry; real ball tap, debounce, scoring/miss, saved best/retry, no stat penalties, sleep guard, navigation pause, and responsive game.');
};
