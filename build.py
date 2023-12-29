from setuptools import Extension
from setuptools.command.build_ext import build_ext


ext_modules = [
    Extension("pymatio.libpymatio", []),
]
class XMakeBuildExt(build_ext):
    def run(self):
        for ext_m in self.extensions:
            print(ext_m)

        import subprocess
        subprocess.run(["xmake", "build"])
        super().run()

def build(setup_kwargs):
    setup_kwargs.update({
        'ext_modules': ext_modules,
        'cmdclass': {'build_ext': XMakeBuildExt},
        'include_package_data': True
    })

print('build py')
