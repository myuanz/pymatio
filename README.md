## 简介
pymatio是用于python读写mat数据文件的库，基于matio开元项目（https://github.com/tbeu/matio）。


## 构建

### cmake方式
matio依赖HDF5,在这里下载所需文件并安装：https://www.hdfgroup.org/. Matio。
配置环境变量HDF5_DIR指向HDF5的安装目录。
```sh
git clone https://gitcode.net/mzdk100/pymatio
git submodule update --init
cd pymatio
cmake -DCMAKE_BUILD_TYPE=Release -DMATIO_SHARED=OFF -S . -B cmake-build-release
cmake --build cmake-build-release
```

### cibuildwheel
```sh
git clone https://gitcode.net/mzdk100/pymatio
git submodule update --init
cd pymatio
pip install cibuildwheel
cibuildwheel --platform windows|linux --archs AMD64
```
其中--platform参数的值可选windows或linux。
--archs参数使用AMD64，因为x86的构建暂时没通过。
默认构建出用于python3.11及以上版本的所有轮子，如果要更改最低版本，可以修改setup.py中python_requires=">=3.11"的配置。
构建成功后会运行必要的测试，同时轮子将保存在wheelhouse目录，更多信息可以运行cibuildwheel -h进行查看。
