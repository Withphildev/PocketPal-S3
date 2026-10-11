# Changelog

## v0.6.2

- Added Soft White as the sixth saved WebUI theme, keeping Deep Forest Teal as default.
- Added subtle light gradients, dark readable text, teal controls, and softer shadows for the white theme.
- Kept all five dark themes, forest artwork, and stat bar gradients unchanged.

## v0.6.1

- Added richer light-to-dark page, header, card, and button gradients across all five WebUI themes.
- Kept the forest artwork, saved theme selection, and shared stat colors unchanged.

## v0.6.0

- Added a Color button beside Rename with five selectable WebUI themes.
- Made Deep Forest Teal (#163D3A) the default; added Purple, Dark Pine, Deep Navy, and Warm Charcoal.
- Persisted the selected theme in device NVS and synchronized it through the state API.
- Added instant color preview, save feedback, and recovery if a color change cannot be saved.
- Applied distinct coral, pink, gold, aqua, and green stat gradients across every WebUI theme.
- Matched stat gradients on the StickS3 and added Health as the fifth device bar.

## v0.5.1

- Replaced the WebUI indoor room with the owner's illustrated forest clearing.
- Added separate portrait and landscape background crops for phone and desktop.
- Replaced the rug with a soft ground shadow beneath the dragon.
- Embedded optimized JPEG backgrounds in firmware and preserved the source artwork.

## v0.5.0

- Replaced the placeholder pet with the owner's original chibi dragon artwork on the StickS3 and WebUI.
- Added aligned idle, happy, listening, excited, sleeping, and surprised expressions.
- Added a shared pose selector so both displays use the same reaction priorities.
- Added a short excited reaction after Play and retained bobbing and tilt/shake/rock movement.
- Embedded browser PNG routes and compact device PNGs in flash; no filesystem upload or internet access is required.
- Preserved source artwork and added a repeatable asset conversion tool and small-screen preview.
- Moved the device motion label clear of the cleanliness stat.

## v0.4.0

- Redesigned the offline WebUI with a richer pet room and clearer visual hierarchy.
- Added Home, Care, and Sensors navigation optimized for phones and desktop browsers.
- Added larger touch-friendly care controls with short descriptions.
- Added persistent room-level sound and motion status chips.
- Refined the live stats, pet identity, naming, sensor meters, and privacy presentation.
- Added responsive layouts, accessible labels, live status messaging, and reduced-motion support.
- Kept the complete interface embedded in firmware with no external web dependencies.

## v0.3.2

- Swapped the landscape Left and Right motion results so sprite movement follows the physical device.
- Reduced Low sound sensitivity by raising its reaction thresholds while leaving Medium and High unchanged.

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
