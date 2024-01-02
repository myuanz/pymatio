import importlib
import importlib.util
import sys
from pathlib import Path
import sysconfig

_EXT_SUFFIX = sysconfig.get_config_var("EXT_SUFFIX")

dll_files = list(Path(__file__).parent.glob(f'*{_EXT_SUFFIX}*'))
if not dll_files:
    print('no dll files', flush=True)
    import time
    time.sleep(3600)
target_dll = Path(__file__).with_name(f'libpymatio{_EXT_SUFFIX}')
print(f'{dll_files=} {target_dll=}')
try:

    spec = importlib.util.spec_from_file_location("libpymatio", target_dll)
    libpymatio = importlib.util.module_from_spec(spec)
    sys.modules["libpymatio"] = libpymatio
    spec.loader.exec_module(libpymatio)
except Exception as e:
    print(e, flush=True)
    import time 
    time.sleep(3600)

from libpymatio import get_library_version
