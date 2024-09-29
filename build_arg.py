import os
import subprocess
import sysconfig
from pathlib import Path

from setuptools import Extension
from setuptools.command.build_ext import build_ext


def xmake_build():
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
    print(f"{curr_arch=} {platform=}", flush=True)
    subprocess.run(["xmake", "config", "-c", "-a", curr_arch, "-v", "-D", '-y'])
    subprocess.run(["xmake", "build", '-y', '-v', '-D'])

class XmakeBuildExt(build_ext):
    def run(self):
        self.xmake_build()
        # 找到 xmake 构建的输出文件
        built_lib = list(Path('.').glob('**/libpymatio*'))
        if not built_lib:
            raise FileNotFoundError("No libpymatio file found after xmake build")
        built_lib = str(built_lib[0])
        print(f"Found built library: {built_lib}")
        
        # 将构建的库文件复制到正确的位置
        self.copy_output_file(built_lib)

    def xmake_build(self):
        # 这里调用 xmake 进行实际的构建
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
        print(f"{curr_arch=} {platform=}", flush=True)
        subprocess.run(["xmake", "config", "-c", "-a", curr_arch, "-v", "-D", '-y'])
        subprocess.run(["xmake", "build", '-y', '-v', '-D'])


    def copy_output_file(self, built_lib):
        # 确保目标目录存在
        self.mkpath(self.build_lib)
        # 复制构建的库文件到正确的位置
        self.copy_file(built_lib, os.path.join(self.build_lib, 'pymatio'))


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

def build_old(setup_kwargs):
    print(setup_kwargs)
    xmake_build()
    setup_kwargs.update({
        'ext_modules': [
            Extension(
                'pymatio.libpymatio',
                sources=[],
            )
        ],
        'package_data': {
            '': [f'libpymatio{sysconfig.get_config_var("EXT_SUFFIX")}'],
        },
    })
    print(setup_kwargs)

print('build py')
