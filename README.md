# PocketPal S3

PocketPal S3 is an interactive virtual pet with a live, offline WebUI for the M5StickS3.

> **Early development — v0.9.0 “Music & Play.”** Optional offline browser music and a soccer-ball keep-up game join Pip's forest room. Existing saved pet data, splash, care props, and event-driven StickS3 redraws are retained.

## First milestone

- Animated original pet rendered directly on the StickS3.
- Matching chibi dragon artwork with six expressions on the device and browser.
- Fullness, happiness, energy, cleanliness, and health needs.
- Feed, play, clean, sleep/wake, and pet interactions.
- Persistent pet name, age, needs, and sleep state in NVS.
- Gentle need decay while running; the pet is not punished while powered off.
- Button controls: M5 button changes the selected action and the blue Face button performs it.
- Private AP-mode WebUI with a larger animated pet room.
- Live state synchronization between the browser and device.
- Browser controls for every care action and pet naming.
- Three-screen onboarding: connection details, Wi-Fi QR, then the pet app.
- PocketLab-matched connection typography, colors, spacing, and QR layout.
- Lower display brightness and a slower animation refresh for improved battery life and reduced visual jitter.
- Private, on-device Sound Reactions with quiet, low, medium, and high classifications.
- Automatic ambient-noise calibration, smoothing, hysteresis, and three sensitivity settings.
- Microphone mute controls on both the StickS3 and WebUI; muting powers the microphone down.
- No audio recording, storage, playback, or transmission.
- IMU-powered tilt reactions, shake-to-play, and gentle rocking-to-sleep.
- Motion sensitivity control and live motion feedback in the WebUI.
- Independent shake and rocking cooldowns prevent repeated accidental stat changes.
- Double-press the blue Face button to go back one screen.
- No cloud account, telemetry, or internet connection required.

## WebUI

At startup, PocketPal shows the supplied Nova-and-Pip splash for 2.5 seconds,
then opens the existing three-screen flow:

1. Read the unique `PocketPal-XXXX` Wi-Fi name, password, and WebUI address.
2. Press the blue Face button and scan the standard Wi-Fi QR code to join from a phone.
3. Press the Face button again to launch the on-device pet app. Open `http://pocketpal` if the browser room does not appear automatically.

Double-press the Face button to return to the previous screen.

The access-point password is generated from the individual ESP32-S3 rather than shared across every device.

PocketPal also advertises `http://pocketpal.local` through mDNS as a fallback on clients that prefer `.local` hostnames.

The responsive WebUI is organized into three sections:

- **Home:** live room, pet identity, mood, age, and care stats.
- **Care:** touch-friendly care controls and an optional Ball time keep-up game.
- **Settings:** Rename, Color, Music, then Reactions (sound and motion meters, sensitivity, mute, and privacy).

The interface is bundled inside the firmware with no external fonts, scripts, images, or internet dependencies.
Nunito Sans is served locally from flash, with system-font fallback. A consistent
inline SVG icon set, calmer type hierarchy, and grouped stats keep the interface
readable across phones and desktop. Font source and license are in
[assets/fonts](assets/fonts/README.md).

The dragon's WebUI setting is an illustrated forest clearing. Phone and desktop
layouts use separate crops to keep the waterfall and open grass visible. The
background is served directly from firmware; source and conversion instructions
are in [assets/room](assets/room/README.md).

Open **Settings → Color** to choose Deep Forest Teal
(the default), Purple, Dark Pine, Deep Navy, Warm Charcoal, or Soft White. The choice is saved
on PocketPal, survives a restart, and is shared by connected browsers.
Each theme uses a richer colored top fading to a dark page background, with
matching gradients on the header, cards, and buttons.
Soft White instead uses light surfaces, dark text, softer shadows, and teal
controls. The existing dark themes and stat bar colors are unchanged.

Fullness is coral, Happiness pink, Energy gold, Cleanliness aqua, and Health
green. These stat gradients stay consistent across themes and match the five
bars on the StickS3. Each device row has a matching small icon between its letter
and bar; Health is labeled `+`.

## Music & Play

**Settings → Music** plays the supplied *Short Afternoon in the Canopy* track
through the phone/browser, not the StickS3. The original MP3 is embedded
unchanged and downloaded only on the first Play tap. The browser decodes it
once and loops the decoded audio locally, without a request at every loop.
Music starts off, with volume at 15%; volume and the enabled preference are
saved in that browser's local storage when available. Returning or reloading
still requires a tap, so music never unexpectedly autoplays. Switching away
or locking the phone pauses playback; tap Play music to resume.

The nearby phone speaker can trigger the S3 microphone's Sound Reactions.
Mute those separately in Settings if desired. Music requires a browser with
Web Audio support; use a normal browser if a captive-portal window restricts it.

**Care → Ball time** is a browser-only, optional keep-up game. Tap Play ball,
then tap the ball (or focus it and use Space/Enter) to keep it airborne. Pip
cheers using existing happy/excited artwork. The score counts successful taps,
and a miss ends the round without changing care stats, happiness, or energy.
The best score is saved in device NVS, survives reboot, and is shared by
connected browsers. Only a new record writes to NVS. If saving fails, Retry
save keeps the result available in the current page until the connection returns
(keep the page open until saved). Sleep, leaving
Care, or hiding the page ends the round without a penalty. Reduced-motion
mode keeps essential ball movement but removes decorative rotation.

Sources and regeneration: [assets/music](assets/music/README.md).

## Dragon artwork

The original mint-green chibi dragon has idle, happy, listening, excited,
sleeping, and surprised poses. Sleeping takes priority over microphone
reactions. Playing briefly triggers the excited pose. Movement still uses
gentle bobbing, tilt, shake, and rocking; the artwork is a set of expressions,
not a frame-by-frame animation sheet.

Browser artwork is served from firmware at 240 pixels; the StickS3 uses
80-pixel images. Originals, converted images, a preview, and regeneration
instructions are in [assets/dragon](assets/dragon/README.md).

## Care props

Successful care actions add floating artwork without replacing Pip's existing poses or motion:

- Feed: a strawberry floats toward Pip.
- Play: a soccer ball bounces by his feet.
- Clean: soap, a water droplet, and rising bubbles.
- Sleep: a blanket settles over his lower body and stays while he sleeps; waking removes it.
- Pet: a hand gives exactly three gentle pats, then leaves.

All seven props are embedded in firmware, with no internet requests. Effects
also follow physical-button and motion-triggered care. New actions replace
previous effects; browser polling does not restart them. A browser joining
mid-effect starts at its current elapsed time. Temporary effects are not saved
or replayed after reboot, but the blanket follows the saved sleep state.
The WebUI respects reduced-motion preferences. The StickS3 leaves the idle
image still, with no periodic full-screen redraws. Visible data changes
(buttons, browser commands, care stats, sound/motion category, or connected
client count) redraw the interface. Movement and sound reactions update just
the pet region every 900 ms; care effects use 120 ms updates and clear once
when finished. This changes application redraws, not the LCD hardware scan
rate. Sensor sampling and browser animations are unchanged.

Sources and regeneration instructions: [assets/props](assets/props/README.md).
Splash artwork and conversion: [assets/splash](assets/splash/README.md).

## Sound Reactions and privacy

PocketPal samples short microphone windows and immediately reduces each one to a single loudness measurement. The reusable sample buffer is never written to flash, exposed through an API, sent to the WebUI, or retained as a recording.

- **Quiet:** normal idle behavior.
- **Low:** listening animation.
- **Medium:** excited animation.
- **High:** surprised animation.
- **Muted:** microphone capture is stopped completely.

The first few seconds after enabling the microphone calibrate the ambient noise floor. Use the WebUI to select low, medium, or high sensitivity. Low is the default for fresh installs and requires substantially louder sound than Medium after physical-room testing. On the device, select the `MIC` action and press the blue Face button to mute or unmute.

## Motion Interactions

PocketPal samples the StickS3 accelerometer at 25 Hz and separates steady orientation from quick motion:

- Tilt the StickS3 to make the pet lean in the matching direction.
- Give it a short shake to trigger Play. An 8-second cooldown prevents repeated stat changes.
- Gently rock it side-to-side several times to put the pet to sleep. This has a 15-second cooldown.
- Choose low, medium, or high motion sensitivity in the WebUI.

Hold the device reasonably still for about one second after startup while the motion sensor calibrates. Motion processing is fully local; only the current category and activity meter are exposed to the WebUI.

The supported orientation is landscape with the blue Face button on the left. Motion directions and side-to-side rocking are mapped to that screen orientation.

## Supported hardware

- M5Stack StickS3

PocketPal S3 has not been validated on the M5StickC family, Cardputer, or other ESP32 boards.

## Install a release

Download the `.factory.bin` from this repository's GitHub Releases. Use an
ESP32-S3-compatible flashing tool configured for the M5StickS3 and write the
combined image at **0x0000** (not 0x10000). It includes the bootloader,
partition table, and application; an internet connection is not needed to run
PocketPal afterward.

Avoid an optional full-flash erase when updating if you want to retain Pip's
save and settings. Erasing flash clears the NVS save. This is an early-development
release tested only for the StickS3; other M5 devices are not supported.

At boot, the splash lasts 2.5 seconds. Follow the displayed Wi-Fi connection
details and Face-button prompts. A single Face press advances/performs the
selected action; two presses go back. The M5 button selects an action in the
pet screen. The WebUI is available at `http://pocketpal` after joining the
device's Wi-Fi.

## Build

Install [PlatformIO](https://platformio.org/), then run:

```sh
platformio run
```

The environment uses the same tested M5StickS3 board configuration as PocketLab.

## Planned

1. Physical-device testing and threshold tuning for Motion Interactions and the redesigned WebUI.
2. Further dragon animation frames and expression tuning after physical testing.
3. Progression and content: personality, growth, inventory, memories, achievements, decorations, and mini-games.
4. Data and connectivity: save backup/restore, optional home-network mode, and carefully designed BLE visits.
5. Release preparation: battery testing, stability testing, privacy documentation, final packaging, and M5Burner submission.

## License

PocketPal S3 is released under the [MIT License](LICENSE).
