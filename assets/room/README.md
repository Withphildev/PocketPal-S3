# Forest clearing

Original background supplied by the project owner, generated in ChatGPT for
PocketPal. `source/forest.png` is preserved unchanged.

Run `python3 tools/build_room_assets.py` with Pillow to regenerate:

- `forestMobile.jpg`: portrait composition at 640 × 853.
- `forestWide.jpg`: landscape crop at 960 × 480, focused on waterfall and grass.
- `src/RoomAssets.h`: JPEG bytes embedded in firmware flash.

The WebUI chooses the crop by viewport width. Background routes are local to
PocketPal, so the room needs neither internet access nor a filesystem upload.
The StickS3 retains its black care dashboard for text readability.
