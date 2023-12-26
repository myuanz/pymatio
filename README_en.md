[中文](./README.md) [English](./README_en.md)

# Background

There has always been a lack of a one-stop library for reading mat files in Python. Mat5 files always depend on scipy.io, and mat7.3 files always depend on h5py. Reading mat files directly with h5py requires many manual conversions. There is a conversion tool, mat73, but its core logic is written in pure Python, making it very slow.

Coincidentally, there is a library in C, [matio](https://github.com/tbeu/matio), and I thought of using pybind11 to create a binding.

# Roadmap

- [x] Complete the binding of basic functions
- [x] Compile successfully on Windows and Linux using xmake
- [ ] Add more Pythonic interfaces
- [ ] Package as a whl file
- [ ] Add benchmarking
- [ ] Import tests from scio and mat73
