# Music & Play assets

`Short_Afternoon_in_the_Canopy.mp3` is the user-supplied, user-tested loop,
copied byte-for-byte from their original. It is approximately 12.38 seconds,
48 kHz stereo, 356730 bytes. No trim, fade, volume normalization, or
transcoding has been applied. The user created the music for PocketPal;
this file does not assert a separate third-party license.

Regenerate the embedded audio and editable browser script after edits:

```sh
python3 tools/build_music_assets.py
python3 tools/build_care_script.py
platformio run
```

The first script embeds this MP3 and `src/MusicPlay.js` in
`src/MusicPlayAssets.h`. The second synchronizes the care/state integration
in the main embedded page. Native score validation lives in `src/BallScore.h`:

```sh
c++ -std=c++11 tools/test_ball_score.cpp -o /tmp/pocketpal-ball-score-test
/tmp/pocketpal-ball-score-test
```

`tools/preview_webui.cjs` runs browser regression tests including
`tools/test_music_play.cjs`, serving the real MP3 and script. The PNG previews
in this folder are browser screenshots with mock device state, not evidence
of physical S3/phone validation. The device endpoint saves only a new best
score (0–99999) and reports NVS write failure. This is a casual offline game,
not a trusted competitive leaderboard.
