# Care props

Seven original transparent PNGs supplied by the user for PocketPal:
strawberry, ball, water, soap, bubble, blanket, and hand.

`source/` preserves the originals. `web/` and `device/` contain mechanically
cropped and resized versions, keeping alpha and aspect ratio. No artwork was
redrawn. `src/PropAssets.h` embeds both sizes in firmware.

Regenerate from the repository root with Pillow installed:

```sh
python3 tools/build_prop_assets.py
python3 tools/build_care_script.py
```

`src/CareAnimation.js` is the editable browser animation source; the second
script synchronizes its marked block in `src/WebPortal.cpp`. Device rendering
is in `src/main.cpp`. Browser QA is `tools/preview_webui.cjs`; effect previews
in this directory are screenshots of the actual embedded page with mock state,
not photographs or confirmation of hardware behavior.
