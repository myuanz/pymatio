[中文](./README.md) [English](./README_en.md)

# 背景

Python 中始终没有一个一站式读取 mat 文件的库，mat5 总是依赖 scipy.io, mat7.3 总是依赖 h5py，h5py 直接读取 mat 文件又需要很多手动转换，有一个 mat73 转换，但是核心逻辑是纯 Python 写的，又非常慢。

恰巧 C 中有一个库 [matio](https://github.com/tbeu/matio)，我就想用 pybind11 做一个绑定。

# 路线

- [x] 完成基本函数的绑定
- [x] 使用 xmake 在 Windows 和 Linux 上编译通过
- [ ] 添加更 Pythonic 的调用接口
- [ ] 打包为 whl 文件
- [ ] 添加 benchmark
- [ ] 导入 scio 的测试和 mat73 的测试
