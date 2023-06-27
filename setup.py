from skbuild import setup
from setuptools import find_packages

setup(
    name="libbids",
    packages=find_packages(exclude=["tests"])
)
