from setuptools import Extension
from setuptools.command.build_ext import build_ext

from pybind11.setup_helpers import Pybind11Extension, build_ext

ext_modules = [
    Extension("pymatio",[]),
]
class XMakeBuildExt(build_ext):
    def run(self):
        for ext_m in self.extensions:
            print(ext_m)

        import subprocess
        subprocess.run(["xmake", "build"])
        super().run()

def build(setup_kwargs):
    print(setup_kwargs)
    setup_kwargs.update({
        'ext_modules': ext_modules,
        'cmdclass': {'build_ext': XMakeBuildExt},
    })

print('build py')
# setup(
#     ext_modules=ext_modules, cmdclass={"build_ext": XMakeBuildExt},
#     name='pymatio',
#     version='0.1.0',
#     packages=['pymatio'],
#     description='Python wrapper for matio',

# )
