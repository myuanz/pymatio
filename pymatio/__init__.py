import importlib
import importlib.util
import sys
from pathlib import Path

spec = importlib.util.spec_from_file_location("libpymatio", Path(__file__).parent / "libpymatio.cp311-win_amd64.pyd")
libpymatio = importlib.util.module_from_spec(spec)
sys.modules["libpymatio"] = libpymatio
spec.loader.exec_module(libpymatio)

from libpymatio import get_library_version
