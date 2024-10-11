#pragma once

#include <vector>

using ssize_t = std::make_signed_t<size_t>;

// copy from https://github.com/pybind/pybind11/blob/af67e87393b0f867ccffc2702885eea12de063fc/include/pybind11/buffer_info.h#L18-L38
// Default, C-style strides
inline std::vector<ssize_t> c_strides(const std::vector<ssize_t> &shape, ssize_t itemsize) {
    auto ndim = shape.size();
    std::vector<ssize_t> strides(ndim, itemsize);
    if (ndim > 0) {
        for (size_t i = ndim - 1; i > 0; --i) {
            strides[i - 1] = strides[i] * shape[i];
        }
    }
    return strides;
}

// F-style strides; default when constructing an array_t with `ExtraFlags & f_style`
inline std::vector<ssize_t> f_strides(const std::vector<ssize_t> &shape, ssize_t itemsize) {
    auto ndim = shape.size();
    std::vector<ssize_t> strides(ndim, itemsize);
    for (size_t i = 1; i < ndim; ++i) {
        strides[i] = strides[i - 1] * shape[i - 1];
    }
    return strides;
}
