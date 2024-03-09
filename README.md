# libbids

[![Build, Lint, and
Test](https://github.com/umnil/libbids/actions/workflows/build-and-test.yml/badge.svg)
](https://github.com/umnil/libbids/actions/workflows/build-and-test.yml)
[![PyPI version](https://badge.fury.io/py/libbids.svg)](https://badge.fury.io/py/libbids)
![PyPI - Downloads](https://img.shields.io/pypi/dm/libbids)
[![Black](https://img.shields.io/badge/code%20style-black-000000.svg)](https://github.com/psf/black)

libbids is a library API for connecting data acquisition devices, or
*instruments*, to synchronized data collection using defined *tasks* and
*events* and store data in
a [BIDS](https://bids-specification.readthedocs.io/en/stable/) compliant
organizational structure.

# Installation

```bash
pip install libbids
```

# Features

libbids attempts to follow the defitions and abstractions defined within the
[Brain Imaging Data Structure
specification](https://bids-specification.readthedocs.io/en/stable/). While
current development is geared toward collection time-series data through EEG,
iEEG, and EMG, other methods should be easily implemented.
