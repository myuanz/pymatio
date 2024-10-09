import os
import shutil
import subprocess
import sys
import sysconfig
from pathlib import Path
import logging

from pybind11.commands import get_include as get_pybind11_include
from setuptools import Extension
from setuptools.command.build_ext import build_ext


class XmakeBuildExt(build_ext):
    def run(self):
        self.xmake_build()
        self.copy_output_file()

    def xmake_build(self):
        platform = sysconfig.get_platform()
        xmake_archs = {
            'win-amd64': 'x64',
            'win32': 'x86',
            'linux-x86_64': 'x86_64',
            'linux-i686': 'i386',
            'darwin-x86_64': 'x86_64',
            'darwin-arm64': 'arm64',
        }
        curr_arch = xmake_archs.get(platform)
        if curr_arch is None:
            raise Exception(f'Unsupported platform: {platform}, allowed: {xmake_archs}')
        logging.debug(f"{curr_arch=} {platform=}")
        python_include = sysconfig.get_path('include')
        if os.name != 'nt':
            python_lib = sysconfig.get_config_var('LIBDIR')
        else:
            python_lib = Path(python_include).parent.joinpath('libs').as_posix()
        python_version = f"{sys.version_info.major}.{sys.version_info.minor}"
        python_bin = sysconfig.get_path("scripts")
        python_site_packages = sysconfig.get_path("purelib")
        pybind11_include = get_pybind11_include()
        

        # print(f'{python_include=} {python_lib=} {python_version=} {python_bin=} {python_site_packages=}')
        # print(list(Path(python_bin).parent.glob("*")))
        # print(Path(python_bin).is_symlink())
        # os.system(f"ls -l {python_bin}")
        # exit(1)

        # 检查xmake是否存在
        if not shutil.which("xmake"):
            raise EnvironmentError(
                f"xmake is not installed or not found in PATH. \nTo install xmake, please refer to https://xmake.io/#/guide/installation\n"
                f"PATH: {os.environ['PATH']}"
            )
        sep = ";" if os.name == "nt" else ":"
        env = {
            **os.environ,
            "XMAKE_PYTHON_INCLUDE": python_include or '', 
            "XMAKE_PYTHON_LIB": python_lib or '',
            "XMAKE_PYTHON_VERSION": python_version or '',
            "XMAKE_PYTHON_SITE_PACKAGES": python_site_packages or '',
            "XMAKE_PYTHON_BIN": python_bin or '',
            "PATH": f"{python_bin}{sep}{python_site_packages}{sep}{os.environ['PATH']}",
            "XMAKE_PYBIND11_INCLUDE": pybind11_include,
        }
        if os.name != 'nt':
            subprocess.run(["env"], env=env)
        if os.environ.get("SLEEP"):
            print("sleep 100000")
            os.system("sleep 100000")

        # subprocess.run(["xmake", "config", "-c", "-a", curr_arch, "-v", "-D", '-y', f"--includedirs={python_include}", f"--linkdirs={python_lib}"])
        config_cmds = ["xmake", "config", "-c", "-a", curr_arch, "-v", "-D", '-y']
        if os.getenv("XMAKE_DEBUG"):
            print("debug mode")
            config_cmds.extend(["-m", "debug"])
        if not os.getenv("XMAKE_NO_RUN_CONFIG"):
            subprocess.run(config_cmds, env=env)

        p = subprocess.run(["xmake", "build", '-y', '-v', '-D'], env=env)
        if p.returncode != 0:
            raise Exception(f"xmake build failed: {p.returncode}")


    def copy_output_file(self):
        ext_suffix = sysconfig.get_config_var("EXT_SUFFIX")
        built_files = list(Path('build').glob(f'**/libpymatio{ext_suffix}'))
        if not built_files:
            raise FileNotFoundError(f"No libpymatio file found after xmake build @ {Path('build').absolute()}")
        built_file = str(built_files[0])
        logging.info(f"Found built library: {built_file} @ {Path('.').absolute()}")
        
        self.mkpath(os.path.join(self.build_lib, 'pymatio'))
        self.copy_file(built_file, os.path.join(self.build_lib, 'pymatio'))
        self.copy_extensions_to_source()


def build(setup_kwargs):
    print(setup_kwargs)
    setup_kwargs.update({
        'ext_modules': [Extension('pymatio.libpymatio', sources=[])],
        'cmdclass': {'build_ext': XmakeBuildExt},
        'package_data': {
            'pymatio': [f'libpymatio{sysconfig.get_config_var("EXT_SUFFIX")}'],
        },
    })

    print(setup_kwargs)
