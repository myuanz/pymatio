[中文](./README.md) [English](./README_en.md)

# 背景

Python 中始终没有一个一站式读取 mat 文件的库，mat5 总是依赖 scipy.io, mat7.3 总是依赖 h5py，h5py 直接读取 mat 文件又需要很多手动转换，有一个 mat73 转换，但是核心逻辑是纯 Python 写的，又非常慢。

恰巧 C 中有一个库 [matio](https://github.com/tbeu/matio)，我就想用 pybind11 做一个绑定。

# 路线

- [x] 完成基本函数的绑定
- [x] 使用 xmake 在 Windows 和 Linux 上编译通过
- [x] 打包为 whl 文件
- [x] 添加关于构建成功的基本测试
- [ ] 添加 cibuildwheel 和 Github Action
- [ ] 吸取 https://github.com/pybind/scikit_build_example 中的优势
- [x] 编译扩展时自动处理虚拟环境
- [ ] 添加更 Pythonic 的调用接口
- [ ] 添加 benchmark
- [ ] 导入 scio 的测试和 mat73 的测试

# 使用

当前还没有打包到 whl，如需试用需安装 [xmake](https://github.com/xmake-io/xmake/)，这是一个基于 lua 的构建系统，非常轻量。

```bash
git clone https://github.com/myuanz/pymatio
xmake
```

之后即可在`build`目录下见对应平台的 Python 扩展，hdf5 和 matio 之依赖会自动处理。

# 样例

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

输出：

```
(1, 5, 26)
0
None
-E- abcdefg234,456
: abcdefg234,456

test.mat 512 <capsule object NULL at 0x7f85197b3fc0> MATLAB 7.3 MAT-file, Platform: x86_64-pc-Linux, Created by: libmatio v1.5.26 on Tue Dec 26 15:49:00 2023
 HDF5 sche 0 MatAcc.RDWR 128 0 1 -1 []
 ```