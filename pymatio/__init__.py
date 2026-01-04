import importlib
import importlib.util
import sys
from pathlib import Path
import sysconfig

EXT_SUFFIX = sysconfig.get_config_var("EXT_SUFFIX")
plat_tag = sysconfig.get_platform().replace("-", "_").replace(".", "_")
python_version = f"cp{sys.version_info.major}{sys.version_info.minor}"

# Get all possible build directories
build_root = Path(__file__).parent.parent / 'build'
potential_build_dirs = []
if build_root.exists():
    potential_build_dirs = [d for d in build_root.iterdir() if d.is_dir()]

raw_candidate_dlls = [
    Path(__file__).parent / f'libpymatio{EXT_SUFFIX}',
    Path(__file__).parent.parent / 'build' / f'libpymatio{EXT_SUFFIX}',
]

# Add all build subdirectories to candidate paths
for build_dir in potential_build_dirs:
    raw_candidate_dlls.extend([
        build_dir / f'libpymatio{EXT_SUFFIX}',
        build_dir / 'Release' / f'libpymatio{EXT_SUFFIX}',
    ])

candidate_dlls = filter(lambda x: x.exists(), raw_candidate_dlls)
candidate_dlls = sorted(candidate_dlls, key=lambda x: x.stat().st_size, reverse=True)

for target_dll in candidate_dlls:
    if not target_dll.exists():
        continue
    try:
        spec = importlib.util.spec_from_file_location("libpymatio", target_dll)
        assert spec is not None and spec.loader is not None, f"Could not create spec for {target_dll}"
        libpymatio = importlib.util.module_from_spec(spec)
        sys.modules["libpymatio"] = libpymatio
        spec.loader.exec_module(libpymatio)
        break
    except Exception as e:
        raise
else:
    raise RuntimeError(f"Failed to load pymatio. Candidates: {raw_candidate_dlls}")

# from libpymatio import get_library_version
from libpymatio import * # noqa
