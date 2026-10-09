# Changelog

## v0.3.1

- Rotated the motion coordinate system 90 degrees left to match the supported landscape orientation.
- Defined landscape as the display upright with the blue Face button on the left.
- Limited rocking detection to the landscape side-to-side axis.

## v0.3.0

- Added 25 Hz accelerometer sampling with low-pass gravity filtering.
- Added steady left, right, forward, and back tilt reactions on the device and WebUI.
- Added shake-to-play with multi-sample confirmation and an 8-second cooldown.
- Added gentle side-to-side rocking to put the pet to sleep, with a 15-second cooldown.
- Added low, medium, and high persisted motion sensitivity settings and a WebUI activity meter.
- Changed the fresh-install sound sensitivity default to Low after physical-room testing.

## v0.2.0

- Added local-only microphone loudness analysis with no audio storage or transmission.
- Added quiet, low, medium, high, calibrating, muted, and unavailable sound states.
- Added automatic ambient-noise calibration, RMS smoothing, and transition hysteresis.
- Added low, medium, and high sensitivity settings persisted in NVS.
- Added animated sound reactions to the StickS3 pet and WebUI pet.
- Added a live WebUI loudness meter, privacy explanation, mute control, and sensitivity control.
- Added an on-device `MIC` action; muting stops the microphone hardware to save power.

## v0.1.3

- Replaced the numeric WebUI address on the connection screen with `http://pocketpal`.
- Added `pocketpal.local` mDNS service discovery as a fallback.
- Reduced display brightness from 125 to 90.
- Slowed the animated pet refresh from 280 ms to 600 ms while preserving immediate input redraws.

## v0.1.2

- Matched the connection-information screen to PocketLab's font sizes, colors, labels, and spacing.
- Matched the Wi-Fi QR screen to PocketLab's proven 120-pixel QR layout and visual hierarchy.

## v0.1.1

- Added a persistent connection-information screen at startup.
- Added a standard WPA Wi-Fi QR screen for joining the PocketPal access point.
- Added a third screen that launches the on-device pet interface.
- Added double-press Back navigation to the blue Face button.

## v0.1.0

- Added the first virtual-pet simulation, on-device UI, persistent state, and AP-mode WebUI.
