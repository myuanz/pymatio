import importlib
import importlib.util
import sys
from pathlib import Path
import sysconfig

EXT_SUFFIX = sysconfig.get_config_var("EXT_SUFFIX")

candidate_dlls = [
    Path(__file__).parent / f'libpymatio{EXT_SUFFIX}',
    Path(__file__).parent.parent / 'build' / f'libpymatio{EXT_SUFFIX}',
    # *list(Path(__file__).parent.parent.glob(f'libpymatio{EXT_SUFFIX}'))
]
candidate_dlls = filter(lambda x: x.exists(), candidate_dlls)
candidate_dlls = sorted(candidate_dlls, key=lambda x: x.stat().st_size, reverse=True)

for target_dll in candidate_dlls:
    if not target_dll.exists():
        continue
    try:
        spec = importlib.util.spec_from_file_location("libpymatio", target_dll)
        libpymatio = importlib.util.module_from_spec(spec)
        sys.modules["libpymatio"] = libpymatio
        spec.loader.exec_module(libpymatio)
        break
    except Exception as e:
        raise
else:
    raise RuntimeError(f"Failed to load pymatio. Candidates: {candidate_dlls}")

# from libpymatio import get_library_version
from libpymatio import *
