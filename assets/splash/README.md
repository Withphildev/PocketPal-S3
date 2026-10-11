# Startup splash

`source/splash.png` preserves the user's approved Nova-and-Pip artwork.
`splash.jpg` is the 240x135 device copy, resized with Lanczos without cropping.
The source aspect ratio is virtually identical to the screen's.

Regenerate using Pillow:

```sh
python3 tools/build_splash_assets.py
```

This creates `src/SplashAssets.h`. The firmware displays the JPEG for 2500 ms
after initialization, then automatically shows connection details. Network,
pet, and sensor processing continue during the splash; navigation starts
afterward. Returning to connection screens does not replay the splash.

Refresh-policy tests (no hardware needed):

```sh
c++ -std=c++11 tools/test_display_refresh.cpp -o /tmp/pocketpal-refresh-test
/tmp/pocketpal-refresh-test
```
