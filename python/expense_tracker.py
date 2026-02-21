from pathlib import Path
import sys

# Allow importing from python/src when running from repo root
sys.path.insert(0, str(Path(__file__).resolve().parent / "src"))

from expense_tracker import menu_loop

if __name__ == "__main__":
    menu_loop()
