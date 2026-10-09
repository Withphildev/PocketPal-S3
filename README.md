# PocketPal S3

PocketPal S3 is an interactive virtual pet with a live, offline WebUI for the M5StickS3.

> **Early development — v0.2.0.** The firmware compiles for the M5StickS3, but Sound Reactions still need calibration on physical hardware.

## First milestone

- Animated original pet rendered directly on the StickS3.
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
- Double-press the blue Face button to go back one screen.
- No cloud account, telemetry, or internet connection required.

## WebUI

At startup, PocketPal opens a three-screen flow:

1. Read the unique `PocketPal-XXXX` Wi-Fi name, password, and WebUI address.
2. Press the blue Face button and scan the standard Wi-Fi QR code to join from a phone.
3. Press the Face button again to launch the on-device pet app. Open `http://pocketpal` if the browser room does not appear automatically.

Double-press the Face button to return to the previous screen.

The access-point password is generated from the individual ESP32-S3 rather than shared across every device.

PocketPal also advertises `http://pocketpal.local` through mDNS as a fallback on clients that prefer `.local` hostnames.

## Sound Reactions and privacy

PocketPal samples short microphone windows and immediately reduces each one to a single loudness measurement. The reusable sample buffer is never written to flash, exposed through an API, sent to the WebUI, or retained as a recording.

- **Quiet:** normal idle behavior.
- **Low:** listening animation.
- **Medium:** excited animation.
- **High:** surprised animation.
- **Muted:** microphone capture is stopped completely.

The first few seconds after enabling the microphone calibrate the ambient noise floor. Use the WebUI to select low, medium, or high sensitivity. On the device, select the `MIC` action and press the blue Face button to mute or unmute.

## Supported hardware

- M5Stack StickS3

PocketPal S3 has not been validated on the M5StickC family, Cardputer, or other ESP32 boards.

## Build

Install [PlatformIO](https://platformio.org/), then run:

```sh
platformio run
```

The environment uses the same tested M5StickS3 board configuration as PocketLab.

## Planned

1. Physical-room testing and threshold tuning for Sound Reactions.
2. Motion interactions: shake to play, tilt reactions, rocking to sleep, filtering, and sensitivity.
3. WebUI redesign: refined room, responsive controls, navigation, and live sound/motion feedback.
4. Original production pet artwork and expanded animation states.
5. Progression and content: personality, growth, inventory, memories, achievements, decorations, and mini-games.
6. Data and connectivity: save backup/restore, optional home-network mode, and carefully designed BLE visits.
7. Release preparation: battery testing, stability testing, privacy documentation, final packaging, and M5Burner submission.

## License

PocketPal S3 is released under the [MIT License](LICENSE).
