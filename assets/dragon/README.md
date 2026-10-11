# PocketPal dragon

Artwork supplied by the project owner, generated in ChatGPT for PocketPal S3.
The source images are preserved without modification. `reference.png` is the
earlier, more detailed reference and is not included in firmware.

| Pose | Original attachment |
| --- | --- |
| happy | Image 1 |
| reference | Image 2 |
| excited | Image 3 |
| idle | Image 4 |
| listening | Image 5 |
| sleeping | Image 6 |
| surprised | Image 7 |

`tools/build_pet_assets.py` creates aligned 240 × 240 transparent browser PNGs,
80 × 80 device PNGs on black, `preview.png`, and `src/PetAssets.h`. Requires
Python with Pillow. Regenerate with `python3 tools/build_pet_assets.py`.

Both interfaces use `PetVisual.h` to choose the expression: sleeping first,
then loud sound, excited sound/shake/Play, listening, joyful, and idle. These
are expression poses with procedural bobbing and motion, not frame-by-frame
wing or walking animations.
