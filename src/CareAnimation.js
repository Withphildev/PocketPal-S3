// Included in the firmware page by tools/build_care_script.py.
// New care events replace old overlays; polling never restarts an animation.
let careId = null;
let careAnimations = [];
function clearCare() {
    careAnimations.forEach(animation => animation.cancel());
    careAnimations = [];
    byId('careLayer').replaceChildren();
}
function syncCare(state) {
    if (typeof syncMusicPlay === 'function') syncMusicPlay(state);
    const care = state.care;
    byId('careBlanket').hidden = !state.sleeping && care?.effect !== 'wake';
    if (!care) return;
    if (care.effect === 'none') { clearCare(); careId = care.id; return; }
    if (care.id === careId) return;
    clearCare();
    careId = care.id;
    const elapsed = Math.max(0, care.elapsed);
    const duration = care.duration;
    if (elapsed >= duration) return;
    const u = byId('pet').clientWidth / 200;
    const reduced = matchMedia('(prefers-reduced-motion: reduce)').matches;
    function prop(name, left, top, width, frames, delay = 0) {
        const image = document.createElement('img');
        image.className = 'care-prop';
        image.dataset.prop = name;
        image.src = '/props/' + name + '.png';
        image.alt = '';
        Object.assign(image.style, {left:left+'%', top:top+'%', width:width+'%'});
        byId('careLayer').append(image);
        const animation = image.animate(reduced ? [{opacity:.7},{opacity:0}] : frames,
            {duration:reduced ? 200 : duration, delay:reduced ? 0 : delay, fill:'both', easing:'linear'});
        animation.currentTime = reduced ? 0 : elapsed;
        careAnimations.push(animation);
        animation.finished.then(() => image.remove()).catch(() => {});
    }
    const frame = (x,y,scale=1,opacity=1,rotate=0) => ({
        transform:`translate(${x*u}px,${y*u}px) rotate(${rotate}deg) scale(${scale})`, opacity
    });
    if (care.effect === 'feed') {
        prop('strawberry',-14,44,22,[frame(0,0,1,0),frame(15,-12),
            {...frame(106,0,.9),offset:.65},frame(116,0,.15,0)]);
    } else if (care.effect === 'play') {
        prop('ball',72,73,24,[frame(0,-35,1,0),frame(-12,0,1,1,35),
            frame(-45,-45,1,1,90),frame(-70,0,1,1,145),frame(-105,-35,1,1,210),
            frame(-80,0,1,1,280),frame(-60,-15,.9,0,340)]);
    } else if (care.effect === 'clean') {
        prop('soap',-12,66,25,[frame(0,0,1,0),frame(20,-12),frame(25,-7),frame(0,0,1,0)]);
        prop('water',78,12,14,[frame(0,-20,1,0),frame(-10,15),frame(-20,70,.6,.6),frame(-20,90,.2,0)]);
        [0,1,2,3].forEach(i=>prop('bubble',12+i*22,72-i*3,10+i*2,
            [frame(0,0,.5,0),frame(i%2?10:-10,-20,1,.55),
             frame(i%2?-8:8,-80,1,.5),frame(0,-120,.7,0)],i*120));
    } else if (care.effect === 'pet') {
        // Entrance, exactly three down/up pats, then exit.
        prop('hand',44,-18,66,[{...frame(0,-35,1,0),offset:0},
            {...frame(0,0),offset:.125},
            {...frame(0,12),offset:.25},{...frame(0,0),offset:.375},
            {...frame(0,12),offset:.5},{...frame(0,0),offset:.625},
            {...frame(0,12),offset:.75},{...frame(0,0),offset:.875},
            {...frame(0,-35,1,0),offset:1}]);
    } else if (care.effect === 'sleep' || care.effect === 'wake') {
        const blanket = byId('careBlanket');
        const sleeping = care.effect === 'sleep';
        blanket.hidden = false;
        const animation = blanket.animate(reduced ? [{opacity:1},{opacity:1}] :
            (sleeping ? [frame(0,-15,1,0),frame(0,0)] : [frame(0,0),frame(15,-20,1,0)]),
            {duration:reduced ? 1 : duration, fill:'none', easing:'ease-out'});
        animation.currentTime = reduced ? 0 : elapsed;
        careAnimations.push(animation);
        animation.finished.then(()=>{if(careId===care.id)blanket.hidden=!sleeping}).catch(()=>{});
    }
}
