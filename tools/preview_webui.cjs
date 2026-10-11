// Browser QA against the actual embedded page and converted PNGs.
// Set NODE_PATH to a directory containing Playwright; run node tools/preview_webui.cjs.
const fs = require('node:fs');
const path = require('node:path');
const http = require('node:http');
const assert = require('node:assert/strict');
const { chromium } = require('playwright');
const root = path.resolve(__dirname, '..');
const html = fs.readFileSync(path.join(root, 'src/WebPortal.cpp'), 'utf8').match(/R"HTML\(([\s\S]*?)\)HTML"/)[1];
const poses = ['idle', 'happy', 'listening', 'excited', 'sleeping', 'surprised'];
const state = {name: 'Pip', theme: 'teal', mood: 'joyful', fullness: 82, happiness: 86, energy: 78,
    cleanliness: 90, health: 100, ageMinutes: 12, sleeping: false, pose: 'happy',
    message: 'Welcome back, Pip!', sound: {level: 'quiet', meter: 0, sensitivity: 1, muted: false},
    motion: {state: 'still', meter: 0, sensitivity: 2}};
let failThemeSave = false;
const commands = [];
const server = http.createServer((req, res) => {
    if (req.url === '/fonts/nunito-sans-v1.woff2') {
        res.setHeader('Content-Type', 'font/woff2');
        return res.end(fs.readFileSync(path.join(root, 'assets/fonts/nunito-sans-latin.woff2')));
    }
    if (req.url === '/api/theme' && req.method === 'POST') {
        let body = '';
        req.on('data', chunk => body += chunk);
        req.on('end', () => {
            const theme = new URLSearchParams(body).get('theme');
            const valid = ['teal', 'purple', 'pine', 'navy', 'charcoal', 'white'].includes(theme);
            res.setHeader('Content-Type', 'application/json');
            res.statusCode = failThemeSave ? 500 : (valid ? 200 : 400);
            if (!failThemeSave && valid) state.theme = theme;
            res.end(JSON.stringify({ok: res.statusCode === 200}));
        });
        return;
    }
    if (req.url === '/api/state') {
        res.setHeader('Content-Type', 'application/json');
        return res.end(JSON.stringify(state));
    }
    if (['/api/action', '/api/name', '/api/sound', '/api/motion'].includes(req.url) && req.method === 'POST') {
        let body = '';
        req.on('data', chunk => body += chunk);
        req.on('end', () => {
            commands.push({url: req.url, body});
            const fields = new URLSearchParams(body);
            if (req.url === '/api/name') state.name = fields.get('name');
            if (req.url === '/api/sound' && fields.has('command')) state.sound.muted = !state.sound.muted;
            if (req.url === '/api/sound' && fields.has('sensitivity')) state.sound.sensitivity = Number(fields.get('sensitivity'));
            if (req.url === '/api/motion') state.motion.sensitivity = Number(fields.get('sensitivity'));
            res.setHeader('Content-Type', 'application/json');
            res.end(JSON.stringify({ok: true}));
        });
        return;
    }
    const match = req.url.match(/^\/pet\/(\w+)\.png$/);
    if (req.url === '/room/forest-mobile.jpg' || req.url === '/room/forest-wide.jpg') {
        res.setHeader('Content-Type', 'image/jpeg');
        return res.end(fs.readFileSync(path.join(root, 'assets/room',
            req.url.includes('mobile') ? 'forestMobile.jpg' : 'forestWide.jpg')));
    }
    if (match && poses.includes(match[1])) {
        res.setHeader('Content-Type', 'image/png');
        return res.end(fs.readFileSync(path.join(root, 'assets/dragon/web', match[1] + '.png')));
    }
    res.setHeader('Content-Type', 'text/html');
    res.end(html);
});
(async () => {
    await new Promise(resolve => server.listen(0, '127.0.0.1', resolve));
    const browser = await chromium.launch({headless: true,
        executablePath: process.env.POCKETPAL_BROWSER || chromium.executablePath()});
    try {
        const page = await browser.newPage({viewport: {width: 390, height: 844}, deviceScaleFactor: 1});
        const errors = [];
        const requests = [];
        page.on('request', request => requests.push(request.url()));
        page.on('pageerror', error => errors.push(error.message));
        await page.goto('http://127.0.0.1:' + server.address().port);
        await page.evaluate(() => document.fonts.ready);
        assert(await page.evaluate(() => document.fonts.check('400 15px "Nunito Sans"')));
        assert(await page.evaluate(() => [...document.fonts].some(font => font.family === 'Nunito Sans' && font.status === 'loaded')));
        assert.match(await page.locator('body').evaluate(element => getComputedStyle(element).fontFamily), /Nunito Sans/);
        for (const pose of poses) {
            state.pose = pose;
            state.sleeping = pose === 'sleeping';
            await page.evaluate(() => refresh());
            await page.waitForFunction(() => {
                const img = document.getElementById('dragon');
                return img.complete && img.naturalWidth === 240;
            });
            assert.equal(await page.locator('#dragon').getAttribute('src'), '/pet/' + pose + '.png');
        }
        state.pose = 'happy'; state.sleeping = false;
        await page.evaluate(() => refresh());
        const dragon = await page.locator('#dragon').boundingBox();
        const room = await page.locator('.room').boundingBox();
        assert(dragon.x >= room.x && dragon.x + dragon.width <= room.x + room.width);
        assert(dragon.y >= room.y && dragon.y + dragon.height <= room.y + room.height);
        for (const panel of ['care', 'settings', 'home']) {
            await page.locator('[data-panel="' + panel + '"]').click();
            assert(await page.locator('#' + panel).isVisible());
        }
        assert.equal(await page.evaluate(() => document.documentElement.scrollWidth > innerWidth), false);
        await page.locator('[data-panel="settings"]').click();
        assert.equal(await page.locator('#settings h3').allTextContents().then(text => text.join(',')), 'Rename,Color,Reactions');
        assert.equal(await page.locator('#home .settings, #home .theme-picker').count(), 0);
        await page.locator('#newName').fill('Nova');
        await page.locator('.settings .primary').click();
        await page.waitForFunction(() => document.getElementById('name').textContent === 'Nova');
        assert(commands.some(command => command.url === '/api/name' && command.body === 'name=Nova'));
        state.name = 'Pip';
        await page.locator('[data-panel="care"]').click();
        assert.equal(await page.locator('.action[onclick="act(\'play\')"] use').getAttribute('href'), '#icon-ball');
        for (const action of ['feed', 'play', 'clean', 'sleep', 'pet']) {
            await Promise.all([
                page.waitForResponse(response => response.url().endsWith('/api/action') && response.request().postData() === 'action=' + action),
                page.locator('.action[onclick="act(\'' + action + '\')"]').click()
            ]);
            assert(commands.some(command => command.url === '/api/action' && command.body === 'action=' + action));
        }
        await page.locator('[data-panel="settings"]').click();
        await page.locator('#muteButton').click();
        await page.waitForFunction(() => document.getElementById('muteButton').textContent === 'Unmute');
        await page.locator('#muteButton').click();
        await page.waitForFunction(() => document.getElementById('muteButton').textContent === 'Mute');
        await page.locator('#sensitivity').selectOption('2');
        await page.waitForFunction(() => document.getElementById('sensitivity').value === '2');
        assert(commands.some(command => command.url === '/api/sound' && command.body === 'sensitivity=2'));
        await Promise.all([
            page.waitForResponse(response => response.url().endsWith('/api/motion')),
            page.locator('#motionSensitivity').selectOption('1')
        ]);
        assert(commands.some(command => command.url === '/api/motion' && command.body === 'sensitivity=1'));
        state.sound.sensitivity = 1; state.motion.sensitivity = 2;
        await page.locator('[data-panel="home"]').click();
        await page.evaluate(() => refresh());
        await page.locator('[data-panel="settings"]').click();
        assert(await page.locator('#themePicker').isVisible());
        const gradients = await page.locator('.stat .fill').evaluateAll(elements =>
            elements.map(element => getComputedStyle(element).backgroundImage));
        assert.equal(new Set(gradients).size, 5);
        const colors = [['teal', '#163D3A'], ['purple', '#513c77'], ['pine', '#1E352D'],
                        ['navy', '#172A3A'], ['charcoal', '#292A2D'], ['white', '#ffffff']];
        const themeBackgrounds = [];
        async function checkMoodLayout() {
            const mood = await page.locator('#mood').boundingBox();
            const identity = await page.locator('.identity').boundingBox();
            assert(mood.y >= identity.y + identity.height);
            assert(Math.abs((mood.x + mood.width / 2) - (identity.x + identity.width / 2)) < 1);
            const style = await page.locator('#mood').evaluate(element => ({
                alignment: getComputedStyle(element).textAlign,
                size: parseFloat(getComputedStyle(element).fontSize)
            }));
            assert.equal(style.alignment, 'center');
            assert.equal(style.size, 28);
            assert.equal(style.size, await page.locator('#name').evaluate(element => parseFloat(getComputedStyle(element).fontSize)));
            assert.equal(await page.locator('#age').evaluate(element => parseFloat(getComputedStyle(element).fontSize)), 14);
        }
        for (const [theme, base] of colors) {
            await page.locator('[data-panel="settings"]').click();
            await page.locator('[data-theme-choice="' + theme + '"]').click();
            await page.waitForFunction(() => !savingTheme);
            assert.equal(state.theme, theme);
            assert.equal(await page.evaluate(() => document.documentElement.dataset.theme), theme);
            assert.equal((await page.evaluate(() => getComputedStyle(document.documentElement)
                .getPropertyValue('--base'))).trim().toLowerCase(), base.toLowerCase());
            const backgrounds = await page.locator('body, .topbar, .tabs, .tab.active, .card, .primary, .secondary, .action, .sensor, .theme-choice').evaluateAll(elements =>
                elements.map(element => getComputedStyle(element).backgroundImage));
            for (const background of backgrounds) assert.match(background, /linear-gradient/);
            themeBackgrounds.push(backgrounds[0]);
            await page.locator('[data-panel="home"]').click();
            await checkMoodLayout();
            assert.deepEqual(await page.locator('.stat .fill').evaluateAll(elements =>
                elements.map(element => getComputedStyle(element).backgroundImage)), gradients);
            assert.equal(await page.evaluate(() => document.documentElement.scrollWidth > innerWidth), false);
            for (const panel of ['care', 'settings', 'home']) {
                await page.locator('[data-panel="' + panel + '"]').click();
                assert.equal(await page.evaluate(() => document.documentElement.scrollWidth > innerWidth), false);
            }
            assert.equal(await page.locator('.stat .icon').count(), 5);
        }
        assert.equal(new Set(themeBackgrounds).size, 6);
        assert.equal(await page.evaluate(() => getComputedStyle(document.documentElement).colorScheme), 'light');
        for (const selector of ['.stat b', '.settings input', '.controls select', '.secondary', '.action']) {
            assert.equal(await page.locator(selector).first().evaluate(element => getComputedStyle(element).color), 'rgb(32, 60, 51)');
        }
        for (const selector of ['.stat label span', '.settings input::placeholder']) {
            const placeholder = selector.includes('::placeholder');
            assert.equal(await page.locator(placeholder ? '.settings input' : selector).first()
                .evaluate((element, pseudo) => getComputedStyle(element, pseudo).color, placeholder ? '::placeholder' : null), 'rgb(80, 101, 92)');
        }
        await page.setViewportSize({width: 320, height: 740});
        await checkMoodLayout();
        state.name = 'LongPalName12';
        await page.evaluate(() => refresh());
        const nameBox = await page.locator('#name').boundingBox();
        const ageBox = await page.locator('#age').boundingBox();
        assert(nameBox.x + nameBox.width <= ageBox.x);
        state.name = 'Pip';
        await page.evaluate(() => refresh());
        assert.equal(await page.evaluate(() => document.documentElement.scrollWidth > innerWidth), false);
        const brandBox = await page.locator('.brand').boundingBox();
        const onlineBox = await page.locator('.online').boundingBox();
        assert(brandBox.x + brandBox.width <= onlineBox.x);
        await page.setViewportSize({width: 390, height: 844});
        await page.reload();
        await page.waitForFunction(() => document.documentElement.dataset.theme === 'white');
        assert(await page.locator('#themePicker').isHidden());
        await page.locator('[data-panel="settings"]').click();
        assert(await page.locator('#themePicker').isVisible());
        failThemeSave = true;
        await page.locator('[data-panel="settings"]').click();
        await page.locator('[data-theme-choice="purple"]').click();
        await page.waitForFunction(() => !savingTheme);
        assert.equal(await page.evaluate(() => document.documentElement.dataset.theme), 'white');
        assert.match(await page.locator('#colorMessage').textContent(), /Could not save/);
        failThemeSave = false;
        await page.locator('[data-theme-choice="white"]').click();
        await page.waitForFunction(() => !savingTheme);
        assert.match(await page.locator('#colorMessage').textContent(), /Color saved/);
        const output = path.join(root, 'assets/dragon');
        await page.locator('[data-panel="home"]').click();
        await page.evaluate(() => window.scrollTo(0, 0));
        await page.screenshot({path: path.join(output, 'webui-white-preview.png'), fullPage: true});
        for (const panel of ['care', 'settings']) {
            await page.locator('[data-panel="' + panel + '"]').click();
            await page.screenshot({path: path.join(output, 'webui-white-' + panel + '-preview.png'), fullPage: true});
        }
        await page.locator('[data-panel="home"]').click();
        await page.locator('[data-panel="settings"]').click();
        await page.locator('[data-theme-choice="teal"]').click();
        await page.waitForFunction(() => !savingTheme);
        await page.locator('#settings').screenshot({path: path.join(output, 'theme-picker-preview.png')});
        await page.locator('[data-panel="home"]').click();
        await page.screenshot({path: path.join(output, 'webui-mobile-preview.png'), fullPage: true});
        await page.setViewportSize({width: 1100, height: 900});
        await checkMoodLayout();
        await page.evaluate(() => window.scrollTo(0, 0));
        await page.evaluate(() => new Promise(resolve => requestAnimationFrame(() => requestAnimationFrame(resolve))));
        await page.screenshot({path: path.join(output, 'webui-desktop-preview.png')});
        assert.deepEqual(errors, []);
        assert(requests.some(url => url.endsWith('/fonts/nunito-sans-v1.woff2')));
        assert(requests.every(url => url.startsWith('http://127.0.0.1:')));
        const iconSource = fs.readFileSync(path.join(root, 'src/StatIcons.h'), 'utf8');
        const glyphs = [...iconSource.matchAll(/\{((?:0x[0-9a-f]+,?\s*)+)\}/g)]
            .map(match => match[1].match(/0x[0-9a-f]+/g).map(value => parseInt(value, 16)));
        assert.equal(glyphs.length, 5);
        for (const glyph of glyphs) {
            assert.equal(glyph.length, 9);
            assert(glyph.every(row => row >= 0 && row < 512));
        }
        // Native pixel layout: 6px letter, gap, 9px icon, gap, original 65px bar.
        assert(10 + 9 < 23);
        assert(112 + 23 + 65 <= 240);
        assert(90 + 9 < 101);
        const iconPreview = await browser.newPage({viewport: {width: 400, height: 240}});
        await iconPreview.setContent('<style>body{margin:0;background:#000}canvas{width:400px;height:240px;image-rendering:pixelated}</style><canvas width="100" height="60"></canvas>');
        await iconPreview.evaluate(glyphs => {
            const ctx = document.querySelector('canvas').getContext('2d');
            const starts = ['#ff6b6b', '#ff70a6', '#f6c445', '#4fc3f7', '#55d98a'];
            const ends = ['#ff9a76', '#ff9ec4', '#ffe47a', '#76e4e8', '#8be6a8'];
            const labels = ['F', 'H', 'E', 'C', '+'];
            ctx.font = '8px monospace'; ctx.textBaseline = 'top';
            glyphs.forEach((glyph, index) => {
                const y = 5 + index * 10;
                ctx.fillStyle = '#ccc'; ctx.fillText(labels[index], 5, y);
                ctx.fillStyle = starts[index];
                glyph.forEach((row, dy) => {
                    for (let dx = 0; dx < 9; dx++) if (row & (1 << (8 - dx))) ctx.fillRect(15 + dx, y + dy, 1, 1);
                });
                ctx.strokeStyle = '#555'; ctx.strokeRect(28.5, y + 1.5, 64, 6);
                const gradient = ctx.createLinearGradient(30, 0, 81, 0);
                gradient.addColorStop(0, starts[index]); gradient.addColorStop(1, ends[index]);
                ctx.fillStyle = gradient; ctx.fillRect(30, y + 3, 51, 3);
            });
        }, glyphs);
        await iconPreview.screenshot({path: path.join(output, 'device-stat-icons-preview.png')});
        await iconPreview.close();
        const fallback = await browser.newPage({viewport: {width: 320, height: 740}, reducedMotion: 'reduce'});
        await fallback.route('**/fonts/**', route => route.abort());
        await fallback.goto('http://127.0.0.1:' + server.address().port);
        await fallback.waitForFunction(() => document.querySelectorAll('.stat').length === 5);
        assert.equal(await fallback.evaluate(() => document.documentElement.scrollWidth > innerWidth), false);
        const animation = await fallback.locator('#pet').evaluate(element => parseFloat(getComputedStyle(element).animationDuration));
        assert(animation <= .001);
        await fallback.close();
        console.log('PASS: offline font/icons, care/rename/sensor commands, six poses, six themes, centered mood, exact theme/stat colors, saved-state and failure recovery, 320/390px widths, long names, font fallback, reduced motion, and JavaScript.');
    } finally {
        await browser.close();
        server.close();
    }
})().catch(error => { console.error(error); server.close(); process.exitCode = 1; });
