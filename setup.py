import os
import subprocess
from pathlib import Path
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup, find_packages

build_dir = Path("build")
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

setup(
    ext_modules=[
        Pybind11Extension(
            "clibbids",
            [
                "src/add.cpp",
                "src/entity.cpp",
                "src/ext.cpp",
                "src/session.cpp",
                "src/subject.cpp",
            ],
            include_dirs=[
                "include",
                build_dir / "json" / "include"
            ],
        )
    ],
    cmdclass={"build_ext": build_ext},
)
