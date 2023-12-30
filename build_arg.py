from setuptools import Extension
import sysconfig

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
    import subprocess
    subprocess.run(["xmake", "config", "-c", "-a", curr_arch, "-v", "-D", '-y'])
    subprocess.run(["xmake", "build", '-y', '-v', '-D'])

def build(setup_kwargs):
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
