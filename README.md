[中文](./README.md) [English](./README_en.md)

## 背景

Python 中始终没有一个一站式读取 mat 文件的库，mat5 总是依赖 scipy.io, mat7.3 总是依赖 h5py，h5py 直接读取 mat 文件又需要很多手动转换，有一个 mat73 转换，但是核心逻辑是纯 Python 写的，又非常慢。

恰巧 C 中有一个库 [matio](https://github.com/tbeu/matio)，我就想用 ~~pybind11~~ nanobind 做一个绑定。

## 安装

```
pip install pymatio
```

## 样例

```python
import pymatio as pm

print(pm.get_library_version())
print(pm.loadmat('file.mat'))
```

## 从源代码构建

### 常规构建流程

```bash
git clone https://github.com/myuanz/pymatio
pip install .
```

zlib、hdf5 等基础依赖将自动跨平台构建.

### 对于 Windows

Windows 通常没有自带的构建工具链, 你可以参考 [此页](https://learn.microsoft.com/en-us/windows/dev-environment/rust/setup#install-visual-studio-recommended-or-the-microsoft-c-build-tools), 下载`Microsoft C++ Build Tools`, 按照图片示例构建推荐的工具链, 点击安装. 只要完成这一步就够了, 不需要再往下安装 Rust. 



## 路线

- [x] 打包为 whl 文件
- [x] 添加关于构建成功的基本测试
- [x] 添加 cibuildwheel 打包 whl
- [x] Github Action
- [x] 编译扩展时自动处理虚拟环境
- [x] 完成 loadmat
- [ ] 完成 savemat
- [ ] 自由线程 whl
- [ ] 导入 scio 的测试和 mat73 的测试
- [ ] 添加 types
- [ ] 添加 benchmark
  - [ ] With scio
  - [ ] With mat73
  - [ ] With Free-Thread

