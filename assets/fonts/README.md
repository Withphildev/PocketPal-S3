# Offline typography

Nunito Sans, Latin subset, upright variable weights 400–800, is embedded in
firmware and served at `/fonts/nunito-sans-v1.woff2`. No Adobe kit, Google Fonts
request, account, or internet access is used by the running app.

Source: Google Fonts CSS API for `Nunito Sans:wght@400..800` (2026-10-10).
Binary: https://fonts.gstatic.com/s/nunitosans/v19/pe0TMImSLYBIv1o4X1M8ce2xCx3yop4tQpF_MeTm0lfGWVpNn64CL7U8upHZIbMV51Q42ptCp7t1R-s.woff2
Upstream: https://github.com/google/fonts/tree/main/ofl/nunitosans
License: SIL Open Font License 1.1, retained in `OFL.txt`.

Run `python3 tools/build_font_assets.py` to regenerate `src/FontAssets.h`.
UI icons are original inline SVG paths in `src/WebPortal.cpp`.
