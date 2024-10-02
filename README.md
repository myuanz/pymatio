[中文](./README.md) [English](./README_en.md)

## 背景

Python 中始终没有一个一站式读取 mat 文件的库，mat5 总是依赖 scipy.io, mat7.3 总是依赖 h5py，h5py 直接读取 mat 文件又需要很多手动转换，有一个 mat73 转换，但是核心逻辑是纯 Python 写的，又非常慢。

恰巧 C 中有一个库 [matio](https://github.com/tbeu/matio)，我就想用 pybind11 做一个绑定。

## 安装

```
pip install pymatio
```

### 对于苹果用户

我没有苹果设备, 经过若干次尝试, 我也无法正确在 Github Actions 里构建 whl 文件. 你可

1. 安装 [xmake](https://xmake.io/#/getting_started?id=installation)
2. 运行`pip install pymatio`, xmake 会帮助你从源代码构建出 Python whl 文件. 

xmake 开发者在苹果设备做过详尽的测试, 理论上构建流程可以正常运行. 

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

# 从源代码构建

## 常规构建流程

1. 安装 xmake: https://xmake.io/#/getting_started?id=installation
2. `git clone https://github.com/myuanz/pymatio`
3. 运行 `poetry build`, xmake 会处理 hdf5 和 zlib 依赖. 


## cibuildwheel 构建

注意: 如果你不需要多 Python 版本的 whl, 你不需要这样做

1. 安装 xmake: https://xmake.io/#/getting_started?id=installation . Linux 用户不需要这一步, 但需要 Podman 引用 Docker 镜像
2. 运行 `cibuildwheel`, 构建结果将会出现在 wheelhouse 目录

# Docker 镜像

cibuildwheel 在 Linux 上会基于 musllinux 和 manylinux 的 Docker 容器构建, 因此需要两个预备镜像. 

## 构建本地测试的镜像

```bash
$ cd dockerfiles
$ podman build -f ./Dockerfile.musllinux -t pymatio-base-musllinux:latest .
$ podman build -f ./Dockerfile.manylinux -t pymatio-base-manylinux:latest .
```

构建本地镜像后, 可修改`pyproject.toml`中`linux.manylinux-x86_64-image`和`linux.musllinux-x86_64-image`字段为`pymatio-base-manylinux`和`pymatio-base-musllinux`来测试.

## 发布

```bash
$ podman tag localhost/pymatio-base-musllinux docker.io/myuanz/pymatio-base-musllinux:latest
$ podman tag localhost/pymatio-base-manylinux docker.io/myuanz/pymatio-base-manylinux:latest
$ podman push docker.io/myuanz/pymatio-base-musllinux:latest
$ podman push docker.io/myuanz/pymatio-base-manylinux:latest
```


# 路线

- [x] 完成 matio 中基本函数的绑定
- [x] 使用 xmake 在 Windows 和 Linux 上编译通过
- [x] 打包为 whl 文件
- [x] 添加关于构建成功的基本测试
- [x] 添加 cibuildwheel 打包 whl
- [x] Github Action
- [x] 编译扩展时自动处理虚拟环境

- [ ] 完成 loadmat 和 savemat
- [ ] 导入 scio 的测试和 mat73 的测试
- [ ] 添加更 Pythonic 的调用接口
- [ ] 添加 types
- [ ] 添加 benchmark
- [ ] 吸取 https://github.com/pybind/scikit_build_example 中的优势

