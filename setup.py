import os
import sys
import subprocess
import shlex
import shutil
import warnings
from pathlib import Path
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup, find_packages
from setuptools import Extension as BaseExtension


build_dir = Path("build")
if build_dir.exists():
    shutil.rmtree(build_dir)
build_dir.mkdir(exist_ok=True)

# configure
subprocess.run(
    ["cmake", ".."],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    cwd="build",
)
subprocess.run(
    ["cmake", "--build", "."],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    cwd="build",
)


class CustomExtBuilder(build_ext):

    def build_extension(self, ext):
        if isinstance(ext, Precompiled):
            return ext.copy_precompiled(self)
        return super().build_extension(ext)


class Precompiled(BaseExtension):

    def __init__(self, name, precompiled, *args, **kwargs):
        super().__init__(name, [], *args, **kwargs)
        self.precompiled = Path(precompiled)

        
    def copy_precompiled(self, builder):
        if self.precompiled.exists():
            path = Path(builder.get_ext_fullpath(self.name))
            path.parent.mkdir(parents=True)
            shutil.copyfile(
                str(self.precompiled),
                builder.get_ext_fullpath(self.name)
            )
        else:
            print(f"Error: specified file {self.precompiled} not found!", file=sys.stderr)


setup(
    ext_modules=[
        Precompiled(
            "clibbids",
            precompiled=next(build_dir.glob("clibbids*.*"))
        )
    ],
    cmdclass={"build_ext": CustomExtBuilder},
)
