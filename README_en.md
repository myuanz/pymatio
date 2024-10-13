[中文](./README.md) [English](./README_en.md)

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

## Building from source

### Standard build process

```bash
git clone https://github.com/myuanz/pymatio
pip install .
```

Basic dependencies such as zlib and hdf5 will be automatically built cross-platform.

### For Windows

Windows usually doesn't come with a built-in build toolchain. You can refer to [this page](https://learn.microsoft.com/en-us/windows/dev-environment/rust/setup#install-visual-studio-recommended-or-the-microsoft-c-build-tools) to download the `Microsoft C++ Build Tools`. Follow the image examples to build the recommended toolchain and click install. Completing this step is sufficient; you don't need to install Rust afterwards.

## Roadmap

- [x] Package as a whl file
- [x] Add basic tests for successful builds
- [x] Add cibuildwheel packaging for whl
- [x] Github Action
- [x] Automatically handle virtual environments when compiling extensions
- [x] Complete loadmat
- [ ] Complete savemat
- [ ] Free-threaded whl
- [ ] Import tests from scio and mat73
- [ ] Add types
- [ ] Add benchmarks
  - [ ] With scio
  - [ ] With mat73
  - [ ] With Free-Thread
