from pathlib import Path
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
source = ROOT / "assets" / "3ds" / "icon.png"
destination = ROOT / "assets" / "3ds" / "icon48.png"

with Image.open(source) as image:
    image = image.convert("RGBA")
    image.resize((48, 48), Image.Resampling.LANCZOS).save(destination)

print(f"Wrote {destination}")
