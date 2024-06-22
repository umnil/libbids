from skbuild import setup  # type: ignore
from setuptools import find_packages  # type: ignore

setup(
    name="libbids",
    packages=find_packages(exclude=["include", "scripts", "src", "tests"]),
)
