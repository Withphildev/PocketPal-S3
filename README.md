# PocketPal S3

PocketPal S3 is an interactive virtual pet with a live, offline WebUI for the M5StickS3.

> **Early development — v0.1.2.** The firmware compiles for the M5StickS3, but still needs physical-device validation.

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
- Double-press the blue Face button to go back one screen.
- No cloud account, telemetry, or internet connection required.

## WebUI

At startup, PocketPal opens a three-screen flow:

1. Read the unique `PocketPal-XXXX` Wi-Fi name, password, and WebUI address.
2. Press the blue Face button and scan the standard Wi-Fi QR code to join from a phone.
3. Press the Face button again to launch the on-device pet app. Open `http://192.168.4.1` if the browser room does not appear automatically.

Double-press the Face button to return to the previous screen.

The access-point password is generated from the individual ESP32-S3 rather than shared across every device.

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

- First-device testing and input/display calibration.
- Original production sprite artwork and more animation states.
- Motion-based play and rocking-to-sleep interactions.
- WebUI room decoration, inventory, memories, and achievements.
- Save-file backup and restore.
- Optional home-network mode and QR connection screen.
- Mini-games and personality development.
- Carefully designed BLE visits between two PocketPal devices.

## License

PocketPal S3 is released under the [MIT License](LICENSE).
