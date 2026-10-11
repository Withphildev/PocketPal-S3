"""Refresh the marked JS block inside the embedded WebUI using apply_patch."""
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / "src/WebPortal.cpp"
START = "// BEGIN CARE ANIMATION"
END = "// END CARE ANIMATION"


def main():
    source = TARGET.read_text()
    block = START + "\n" + (ROOT / "src/CareAnimation.js").read_text().rstrip() + "\n" + END
    if START in source:
        old = source[source.index(START):source.index(END) + len(END)]
    else:
        old = "refresh();setInterval(refresh,500);"
        block += "\n" + old
    patch = "*** Begin Patch\n*** Update File: " + str(TARGET) + "\n@@\n"
    patch += "\n".join("-" + line for line in old.splitlines()) + "\n"
    patch += "\n".join("+" + line for line in block.splitlines()) + "\n*** End Patch"
    subprocess.run(["apply_patch", patch], check=True)


if __name__ == "__main__":
    main()
