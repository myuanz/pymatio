[中文](./README.md) [English](./README_en.md)

# Background

In Python, there has never been a one-stop library for reading mat files. mat5 always relies on scipy.io, mat7.3 always depends on h5py, and directly reading mat files with h5py requires many manual conversions. There's a conversion tool called mat73, but its core logic is written in pure Python, making it very slow.

Coincidentally, there's a library in C called [matio](https://github.com/tbeu/matio), and I wanted to create a binding using pybind11.

# Roadmap

- [x] Complete binding of basic functions
- [x] Compile successfully on Windows and Linux using xmake
- [ ] Automatically handle virtual environments during extension compilation
- [ ] Add more Pythonic interfaces
- [ ] Package as a whl file
- [ ] Add benchmarking
- [ ] Import tests from scio and mat73

# Usage

It's not yet packaged as a whl, so if you want to try it, you need to install [xmake](https://github.com/xmake-io/xmake/), a lua-based build system that's very lightweight.

```bash
git clone https://github.com/myuanz/pymatio
xmake
```

Afterwards, you can see the Python extension for your platform in the `build` directory. Dependencies on hdf5 and matio are handled automatically.

# Example

```python
import pymatio as pm

print(pm.get_library_version())
print(pm.log_init('pymatio'))
print(pm.set_debug(1))
pm.critical("abcdefg%d,%d\n" % (234, 456,))
mat = pm.create_ver('test.mat', None, pm.MatFt.MAT73)

var1 = pm.var_create('var1', pm.MatioClasses.DOUBLE, pm.MatioTypes.DOUBLE, 2, (2, 3,), (1, 2, 3, 4, 5, 6,), 0)
pm.var_write(mat, var1, pm.MatioCompression.NONE)
pm.var_free(var1)
print(mat.filename, mat.version, mat.fp, mat.header, mat.byte_swap, mat.mode, mat.bof, mat.next_index, mat.num_datasets, mat.refs_id, mat.dir)
```

Output:

```
(1, 5, 26)
0
None
-E- abcdefg234,456
: abcdefg234,456

test.mat 512 <capsule object NULL at 0x7f85197b3fc0> MATLAB 7.3 MAT-file, Platform: x86_64-pc-Linux, Created by: libmatio v1.5.26 on Tue Dec 26 15:49:00 2023
 HDF5 sche 0 MatAcc.RDWR 128 0 1 -1 []
```
