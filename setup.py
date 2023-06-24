import importlib
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


class CMakeExtBuilder(build_ext):

    def build_extension(self, ext):
        if isinstance(ext, CMakeExtension):
            ext.prepare(self)
            ext.configure(self)
            ext.build(self)
            return ext.install(self)
        return super().build_extension(ext)


class CMakeExtension(BaseExtension):

    def __init__(self, name, build_dir, *args, **kwargs):
        super().__init__(name, [], *args, **kwargs)
        self.build_dir = Path(build_dir)

    def prepare(self, builder):
        if self.build_dir.exists():
            for root, dirs, files in os.walk(self.build_dir):
                for f in (dirs + files):
                    os.chmod(Path(root) / f, 0o777)
            
            shutil.rmtree(self.build_dir)
        
        self.build_dir.mkdir()

    def configure(self, builder):
        subprocess.run( ["cmake", ".."], cwd=self.build_dir)

    def build(self, builder):
        subprocess.run( ["cmake", "--build", "."], cwd=self.build_dir)

    def install(self, builder):
        sfx = importlib.machinery.EXTENSION_SUFFIXES[0]
        self.ext_file_path = next(self.build_dir.glob(f"**/*{sfx}"))
        if self.ext_file_path.exists():
            path = Path(builder.get_ext_fullpath(self.name))
            path.parent.mkdir(parents=True)

            shutil.copyfile(
                str(self.ext_file_path),
                builder.get_ext_fullpath(self.name)
            )
        else:
            print(f"Error: specified file {self.ext_file_path} not found!", file=sys.stderr)


setup(
    ext_modules=[
        CMakeExtension(
            "clibbids",
            build_dir=Path("build")
        )
    ],
    cmdclass={"build_ext": CMakeExtBuilder},
)
