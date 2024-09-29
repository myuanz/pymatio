import importlib
import importlib.util
import sys
from pathlib import Path
import sysconfig

EXT_SUFFIX = sysconfig.get_config_var("EXT_SUFFIX")

target_dll = Path(__file__).parent / f'libpymatio{EXT_SUFFIX}'
try:

    spec = importlib.util.spec_from_file_location("libpymatio", target_dll)
    libpymatio = importlib.util.module_from_spec(spec)
    sys.modules["libpymatio"] = libpymatio
    spec.loader.exec_module(libpymatio)
except Exception as e:
    import traceback
    traceback.print_exc()
    raise

from libpymatio import get_library_version
from libpymatio import *
