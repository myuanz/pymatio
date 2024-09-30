import os
import shutil
import subprocess
import sys
import sysconfig
from pathlib import Path
import logging

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
        python_lib = sysconfig.get_config_var('LIBDIR')
        python_version = f"{sys.version_info.major}.{sys.version_info.minor}"
        python_bin = sysconfig.get_path("scripts")
        python_site_packages = sysconfig.get_path("purelib")

        print(f'{python_include=} {python_lib=} {python_version=} {python_bin=} {python_site_packages=}')
        print(list(Path(python_bin).parent.glob("*")))

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
        }
        for k, v in env.items():
            print(f"{k}={v} {type(v)}")


        # subprocess.run(["xmake", "config", "-c", "-a", curr_arch, "-v", "-D", '-y', f"--includedirs={python_include}", f"--linkdirs={python_lib}"])
        subprocess.run(["xmake", "config", "-c", "-a", curr_arch, "-v", "-D", '-y'], env=env)
        subprocess.run(["xmake", "build", '-y', '-v', '-D'], env=env)


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
