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
const server = http.createServer((req, res) => {
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
        page.on('pageerror', error => errors.push(error.message));
        await page.goto('http://127.0.0.1:' + server.address().port);
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
        for (const panel of ['care', 'sensors', 'home']) {
            await page.locator('[data-panel="' + panel + '"]').click();
            assert(await page.locator('#' + panel).isVisible());
        }
        assert.equal(await page.evaluate(() => document.documentElement.scrollWidth > innerWidth), false);
        await page.locator('#colorButton').click();
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
            assert.equal(style.size, 20);
        }
        for (const [theme, base] of colors) {
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
            await checkMoodLayout();
            assert.deepEqual(await page.locator('.stat .fill').evaluateAll(elements =>
                elements.map(element => getComputedStyle(element).backgroundImage)), gradients);
            assert.equal(await page.evaluate(() => document.documentElement.scrollWidth > innerWidth), false);
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
        assert.equal(await page.evaluate(() => document.documentElement.scrollWidth > innerWidth), false);
        const brandBox = await page.locator('.brand').boundingBox();
        const onlineBox = await page.locator('.online').boundingBox();
        assert(brandBox.x + brandBox.width <= onlineBox.x);
        await page.setViewportSize({width: 390, height: 844});
        await page.reload();
        await page.waitForFunction(() => document.documentElement.dataset.theme === 'white');
        assert(await page.locator('#themePicker').isHidden());
        failThemeSave = true;
        await page.locator('#colorButton').click();
        await page.locator('[data-theme-choice="purple"]').click();
        await page.waitForFunction(() => !savingTheme);
        assert.equal(await page.evaluate(() => document.documentElement.dataset.theme), 'white');
        assert.match(await page.locator('#colorMessage').textContent(), /Could not save/);
        failThemeSave = false;
        const output = path.join(root, 'assets/dragon');
        await page.locator('#colorButton').click();
        await page.evaluate(() => window.scrollTo(0, 0));
        await page.screenshot({path: path.join(output, 'webui-white-preview.png'), fullPage: true});
        for (const panel of ['care', 'sensors']) {
            await page.locator('[data-panel="' + panel + '"]').click();
            await page.screenshot({path: path.join(output, 'webui-white-' + panel + '-preview.png'), fullPage: true});
        }
        await page.locator('[data-panel="home"]').click();
        await page.locator('#colorButton').click();
        await page.locator('[data-theme-choice="teal"]').click();
        await page.waitForFunction(() => !savingTheme);
        await page.locator('#home').screenshot({path: path.join(output, 'theme-picker-preview.png')});
        await page.locator('#colorButton').click();
        await page.screenshot({path: path.join(output, 'webui-mobile-preview.png'), fullPage: true});
        await page.setViewportSize({width: 1100, height: 900});
        await checkMoodLayout();
        await page.evaluate(() => window.scrollTo(0, 0));
        await page.evaluate(() => new Promise(resolve => requestAnimationFrame(() => requestAnimationFrame(resolve))));
        await page.screenshot({path: path.join(output, 'webui-desktop-preview.png')});
        assert.deepEqual(errors, []);
        console.log('PASS: six poses, navigation, six themes, exact theme colors, themed surface gradients, readable light-theme text, distinct stat gradients, saved-state reload, save-failure recovery, 320/390px mobile widths, and JavaScript.');
    } finally {
        await browser.close();
        server.close();
    }
})().catch(error => { console.error(error); server.close(); process.exitCode = 1; });
