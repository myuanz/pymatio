[中文](./README.md) [English](./README_en.md)

# pymatio

Fast, compact MAT file reader supporting MAT5/MAT7.3 formats within a 2MiB footprint. Eliminates dependency on bulky scipy and h5py. Based on Python binding for matio.

## Background

There has never been a one-stop library for reading mat files in Python. mat5 always relies on scipy.io, mat7.3 always depends on h5py, and reading mat files directly with h5py requires a lot of manual conversion. There is a mat73 converter, but its core logic is written in pure Python, which is very slow.

Coincidentally, there is a library in C called [matio](https://github.com/tbeu/matio), so I wanted to create a binding using ~~pybind11~~ nanobind.

## Installation

```
pip install pymatio
```

## Example

```python
import pymatio as pm

print(pm.get_library_version())
print(pm.loadmat('file.mat'))
```

## For developers

### Standard build process

```bash
git clone https://github.com/myuanz/pymatio && cd pymatio
uv sync --dev --no-install-project
# or use cmake
cmake -S . -B build
cmake --build build

# for test data
git submodule update --init --recursive
```

Basic dependencies such as zlib and hdf5 will be automatically built cross-platform.

### Windows toolchain

Windows usually doesn't come with a built-in build toolchain. You can refer to [this page](https://learn.microsoft.com/en-us/windows/dev-environment/rust/setup#install-visual-studio-recommended-or-the-microsoft-c-build-tools) to download the `Microsoft C++ Build Tools`. Follow the image examples to build the recommended toolchain and click install. Completing this step is sufficient; you don't need to install Rust afterwards.

## Roadmap

- [x] Package as a whl file
- [x] Add basic tests for successful builds
- [x] Add cibuildwheel packaging for whl
- [x] Github Action
- [x] Automatically handle virtual environments when compiling extensions
- [x] Complete loadmat
- [ ] Complete savemat
- [x] Free-threaded whl
- [ ] Import tests from scio
- [x] Import tests from mat73
- [x] Import tests from matio
- [x] Add types
