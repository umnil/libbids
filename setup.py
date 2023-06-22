from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup, find_packages

setup(
    ext_modules=[
        Pybind11Extension(
            "clibbids",
            [
                "src/add.cpp",
                "src/entity.cpp",
                "src/ext.cpp"
            ],
            include_dirs=["include"],
        )
    ],
    cmdclass={"build_ext": build_ext}
)
