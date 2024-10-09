//
// Created by Administrator on 2023/11/1.
//
// #include <pybind11/pybind11.h>
// #include <pybind11/numpy.h>
// #include <pybind11/stl.h>
// #include <matio.h>
// #include <string>
// #include <vector>
// #include <map>
// #include <codecvt>
// #include <locale>
// #include "matio.h"
// #include "libmatio.h"


// namespace py = pybind11;


// std::string gbk_to_utf8(const std::string& input) {
//     try {
//         static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
//         std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>> gbk_conv;
//         std::wstring utf16_str = gbk_conv.from_bytes(input);
//         return utf8_conv.to_bytes(utf16_str);
//     } catch (const std::exception&) {
//         printf("gbk_to_utf8 error: %s\n", input.c_str());
//         // 如果转换失败，返回原始字符串
//         return input;
//     }
// }

// std::string utf8_to_gbk(const std::string& utf8_str) {
//     try {
//         static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
//         std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>> gbk_conv;
//         std::wstring utf16_str = utf8_conv.from_bytes(utf8_str);
//         return gbk_conv.to_bytes(utf16_str);
//     } catch (const std::exception&) {
//         // 如果转换失败，返回原始字符串
//         return utf8_str;
//     }
// }

// py::object read_mat_var(matvar_t* matvar) {
//     if (matvar == nullptr) {
//         return py::none();
//     }

//     switch (matvar->class_type) {
//         case MAT_C_DOUBLE:
//         case MAT_C_SINGLE:
//         case MAT_C_INT8:
//         case MAT_C_UINT8:
//         case MAT_C_INT16:
//         case MAT_C_UINT16:
//         case MAT_C_INT32:
//         case MAT_C_UINT32:
//         case MAT_C_INT64:
//         case MAT_C_UINT64: {
//             std::vector<ssize_t> dims(matvar->dims, matvar->dims + matvar->rank);
//             py::array_t<double> arr(dims);
//             memcpy(arr.mutable_data(), matvar->data, matvar->nbytes);
//             return std::move(arr);
//         }
//         case MAT_C_CHAR: {
//             std::string raw_str(static_cast<char*>(matvar->data), matvar->nbytes);
//             std::string utf8_str = gbk_to_utf8(raw_str);
//             return py::str(utf8_str);
//         }
//         case MAT_C_STRUCT: {
//             py::dict result;
//             size_t nfields = Mat_VarGetNumberOfFields(matvar);
//             char* const* fieldnames = Mat_VarGetStructFieldnames(matvar);
//             for (size_t i = 0; i < nfields; ++i) {
//                 matvar_t* field = Mat_VarGetStructFieldByName(matvar, fieldnames[i], 0);
//                 result[py::str((fieldnames[i]))] = read_mat_var(field);
//             }
//             return std::move(result);
//         }
//         case MAT_C_CELL: {
//             py::list result;
//             for (size_t i = 0; i < matvar->nbytes / sizeof(matvar_t*); ++i) {
//                 matvar_t* cell = Mat_VarGetCell(matvar, i);
//                 result.append(read_mat_var(cell));
//             }
//             return std::move(result);
//         }
//         default:
//             return py::none();
//     }
// }

// py::dict loadmat(const std::string& filename) {
//     mat_t* mat = Mat_Open(filename.c_str(), MAT_ACC_RDONLY);
//     if (mat == nullptr) {
//         throw std::runtime_error("Failed to open MAT file");
//     }

//     py::dict result;
//     matvar_t* matvar;
//     while ((matvar = Mat_VarReadNext(mat)) != nullptr) {
//         result[py::str((matvar->name))] = read_mat_var(matvar);
//         Mat_VarFree(matvar);
//     }

//     Mat_Close(mat);
//     return result;
// }

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <matio.h>
#include <string>
#include <vector>
#include <map>
#include <codecvt>
#include <locale>
#include <stdexcept>
#include <cstring>

#include "matio_private.h" // 添加这行，确保包含了完整的 matvar_internal 定义
#include "matio.h"

namespace py = pybind11;

std::string string_to_utf8(int string_type, const std::string& input) {
    // match MAT_T_UTF8 MAT_T_UTF16 MAT_T_UTF32
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
        std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>> utf16_conv;
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32_conv;

        switch (string_type) {
            case MAT_T_UTF8:
            case MAT_T_UINT8:
                return input;
            case MAT_T_UTF16:
            case MAT_T_UINT16: {
                std::wstring utf16_str = utf16_conv.from_bytes(input);
                return utf8_conv.to_bytes(utf16_str);
            }
            case MAT_T_UTF32: 
            case MAT_T_UINT32: {
                std::u32string utf32_str(reinterpret_cast<const char32_t*>(input.data()), input.length() / sizeof(char32_t));
                return utf32_conv.to_bytes(utf32_str);
            }
            default:
                throw std::runtime_error("Unsupported string type: " + std::to_string(string_type));
        }
    } catch (const std::exception& e) {
        printf("string_to_utf8 error: ");
        for (unsigned char c : input) {
            printf("%02x ", c);
        }
        printf("\n");
        printf("string_to_utf8 error: %s\n", e.what());
        // 如果转换失败，返回原始字符串
        return input;
    }
}
// Helper function to convert matvar_t to Python object
py::object matvar_to_pyobject(matvar_t* matvar, int indent, bool simplify_cells);

std::string combine_var_type(matvar_t* matvar) {
    const char *data_type_desc[25] = {"Unknown",
                                        "8-bit, signed integer",
                                        "8-bit, unsigned integer",
                                        "16-bit, signed integer",
                                        "16-bit, unsigned integer",
                                        "32-bit, signed integer",
                                        "32-bit, unsigned integer",
                                        "IEEE 754 single-precision",
                                        "RESERVED",
                                        "IEEE 754 double-precision",
                                        "RESERVED",
                                        "RESERVED",
                                        "64-bit, signed integer",
                                        "64-bit, unsigned integer",
                                        "Matlab Array",
                                        "Compressed Data",
                                        "Unicode UTF-8 Encoded Character Data",
                                        "Unicode UTF-16 Encoded Character Data",
                                        "Unicode UTF-32 Encoded Character Data",
                                        "RESERVED",
                                        "String",
                                        "Cell Array",
                                        "Structure",
                                        "Array",
                                        "Function"};
    const char *class_type_desc[18] = {"Undefined",
                                        "Cell Array",
                                        "Structure",
                                        "Object",
                                        "Character Array",
                                        "Sparse Array",
                                        "Double Precision Array",
                                        "Single Precision Array",
                                        "8-bit, signed integer array",
                                        "8-bit, unsigned integer array",
                                        "16-bit, signed integer array",
                                        "16-bit, unsigned integer array",
                                        "32-bit, signed integer array",
                                        "32-bit, unsigned integer array",
                                        "64-bit, signed integer array",
                                        "64-bit, unsigned integer array",
                                        "Function",
                                        "Opaque"};
    return "class type: " + std::string(class_type_desc[matvar->class_type]) + " | data type: " + std::string(data_type_desc[matvar->data_type]);
}

py::object handle_numeric(matvar_t* matvar, bool simplify_cells) {
    if(!matvar->data) {
        return py::none();
    }

    // Calculate the total number of elements
    size_t num_elements = 1;
    for(int i = 0; i < matvar->rank; ++i) {
        num_elements *= matvar->dims[i];
    }

    // Determine the NumPy dtype and format descriptor
    py::dtype np_dtype;
    size_t element_size;

    switch(matvar->data_type) {
        case MAT_T_DOUBLE:
            np_dtype = py::dtype::of<double>();
            element_size = sizeof(double);
            break;
        case MAT_T_SINGLE:
            np_dtype = py::dtype::of<float>();
            element_size = sizeof(float);
            break;
        case MAT_T_INT8:
            np_dtype = py::dtype::of<int8_t>();
            element_size = sizeof(int8_t);
            break;
        case MAT_T_UINT8:
            np_dtype = py::dtype::of<uint8_t>();
            element_size = sizeof(uint8_t);
            if (matvar->isLogical) {
                np_dtype = py::dtype::of<bool>();
            }
            break;
        case MAT_T_INT16:
            np_dtype = py::dtype::of<int16_t>();
            element_size = sizeof(int16_t);
            break;
        case MAT_T_UINT16:
            np_dtype = py::dtype::of<uint16_t>();
            element_size = sizeof(uint16_t);
            break;
        case MAT_T_INT32:
            np_dtype = py::dtype::of<int32_t>();
            element_size = sizeof(int32_t);
            break;
        case MAT_T_UINT32:
            np_dtype = py::dtype::of<uint32_t>();
            element_size = sizeof(uint32_t);
            break;
        case MAT_T_INT64:
            np_dtype = py::dtype::of<int64_t>();
            element_size = sizeof(int64_t);
            break;
        case MAT_T_UINT64:
            np_dtype = py::dtype::of<uint64_t>();
            element_size = sizeof(uint64_t);
            break;
        default:
            throw std::runtime_error("Unsupported MAT data type: " + std::to_string(matvar->data_type));
    }
    if (simplify_cells) {
        if (num_elements == 1) {
            switch(matvar->data_type) {
                case MAT_T_DOUBLE:
                    return py::cast(static_cast<double*>(matvar->data)[0]);
                case MAT_T_SINGLE:
                    return py::cast(static_cast<float*>(matvar->data)[0]);
                case MAT_T_INT8:
                    return py::cast(static_cast<int8_t*>(matvar->data)[0]);
                case MAT_T_INT16:
                    return py::cast(static_cast<int16_t*>(matvar->data)[0]);
                case MAT_T_INT32:
                    return py::cast(static_cast<int32_t*>(matvar->data)[0]);
                case MAT_T_INT64:
                    return py::cast(static_cast<int64_t*>(matvar->data)[0]);
                case MAT_T_UINT8:
                    if (matvar->isLogical) {
                        return py::cast(static_cast<uint8_t*>(matvar->data)[0] != 0);
                    } else {
                        return py::cast(static_cast<uint8_t*>(matvar->data)[0]);
                    }

                case MAT_T_UINT16:
                    return py::cast(static_cast<uint16_t*>(matvar->data)[0]);
                case MAT_T_UINT32:
                    return py::cast(static_cast<uint32_t*>(matvar->data)[0]);
                case MAT_T_UINT64:
                    return py::cast(static_cast<uint64_t*>(matvar->data)[0]);
                default:
                    throw std::runtime_error("Unsupported MAT data type: " + std::to_string(matvar->data_type));
            }
        }
    }
    // Prepare dimensions and strides
    std::vector<ssize_t> shape(matvar->rank);
    for (int i = 0; i < matvar->rank; ++i) {
        shape[i] = static_cast<ssize_t>(matvar->dims[i]);
    }
    std::vector<ssize_t> strides = py::detail::f_strides(shape, element_size);

    auto arr = py::array(np_dtype, shape, strides, matvar->data);
    // printf("arr.attr(\"flags\"): %s\n", arr.attr("flags").attr("__str__")().cast<std::string>().c_str());

    return arr;
}

py::array matvar_to_numpy_cell(matvar_t* matvar, int indent, bool simplify_cells) {
    if (!matvar || matvar->class_type != MAT_C_CELL) {
        throw std::runtime_error("Invalid matvar or not a cell array");
    }

    // 获取维度信息
    std::vector<ssize_t> shape(matvar->dims, matvar->dims + matvar->rank);

    // 创建一个 NumPy 数组，使用 object 类型
    py::array cell_array = py::array(py::dtype("O"), shape);
    py::buffer_info buf = cell_array.request();
    py::object* ptr = static_cast<py::object*>(buf.ptr);

    // 计算总元素数
    size_t total_elements = 1;
    for (int i = 0; i < matvar->rank; ++i) {
        total_elements *= matvar->dims[i];
    }

    // 填充数组
    matvar_t** cells = static_cast<matvar_t**>(matvar->data);
    for (size_t i = 0; i < total_elements; ++i) {
        if (cells[i]) {
            ptr[i] = matvar_to_pyobject(cells[i], indent + 1, simplify_cells);
        } else {
            ptr[i] = py::none();
        }
    }

    return cell_array;
}

// Function to convert matvar_t to Python object
py::object matvar_to_pyobject(matvar_t* matvar, int indent, bool simplify_cells = false) {
    if(matvar == nullptr) {
        return py::none();
    }

    printf("%*s matvar->class_type, data_type: %d, %d. %d\n", indent, "", matvar->class_type, matvar->data_type, matvar->isLogical);

    switch(matvar->class_type) {
        case MAT_C_DOUBLE:
        case MAT_C_SINGLE:
        case MAT_C_INT8:
        case MAT_C_UINT8:
        case MAT_C_INT16:
        case MAT_C_UINT16:
        case MAT_C_INT32:
        case MAT_C_UINT32:
        case MAT_C_INT64:
        case MAT_C_UINT64:
            return handle_numeric(matvar, simplify_cells);
        case MAT_C_EMPTY:
            return py::none();
        case MAT_C_STRUCT: {
            py::dict struct_dict;
            if(!matvar->internal) {
                throw std::runtime_error("Malformed MAT_C_STRUCT variable: " + std::string(matvar->name));
            }
            printf("%*s matvar->internal->num_fields: %d\n", indent, "", matvar->internal->num_fields);
            for(unsigned i = 0; i < matvar->internal->num_fields; ++i) {
                const char* field_name = matvar->internal->fieldnames[i];
                matvar_t* field_var = static_cast<matvar_t**>(matvar->data)[i];
                printf("%*s field_name: %s, field_var: %p\n", indent, "", field_name, field_var);

                if(field_var) {
                    struct_dict[field_name] = matvar_to_pyobject(field_var, indent + 1, simplify_cells);
                } else {
                    struct_dict[field_name] = py::none();
                }
            }
            return struct_dict;
        }
        case MAT_C_CELL: {
            return matvar_to_numpy_cell(matvar, indent, simplify_cells);

            py::list cell_list;
            matvar_t** cells = static_cast<matvar_t**>(matvar->data);
            printf("%*s matvar->dims[0]: %zu, matvar->dims[1]: %zu\n", indent, "", matvar->dims[0], matvar->dims[1]);
            for(size_t i = 0; i < matvar->dims[0] * matvar->dims[1]; ++i) {
                if(cells[i]) {
                    cell_list.append(matvar_to_pyobject(cells[i], indent + 1, simplify_cells));
                } else {
                    cell_list.append(py::none());
                }
            }
            return cell_list;
        }
        case MAT_C_CHAR: {
            printf("%*s MAT_C_CHAT matvar->data: %s\n", indent, "", static_cast<char*>(matvar->data));

            if(!matvar->data) {
                return py::str("");
            }
            std::string raw_str(static_cast<char*>(matvar->data), matvar->nbytes);
            std::string utf8_str = string_to_utf8(matvar->data_type, raw_str);

            // Trim trailing spaces
            size_t endpos = utf8_str.find_last_not_of(" ");
            if(endpos != std::string::npos) {
                utf8_str = utf8_str.substr(0, endpos + 1);
            }
            return py::str(utf8_str);
        }
        case MAT_C_OPAQUE: {
            throw std::runtime_error("Unsupported MAT class: " + std::to_string(matvar->class_type));
        }
        default:
            throw std::runtime_error("Unsupported MAT class: " + std::to_string(matvar->class_type));
    }
}

// Function to load MAT file
py::dict loadmat(const std::string& filename, bool simplify_cells = false) {
    mat_t* matfp = Mat_Open(filename.c_str(), MAT_ACC_RDONLY);
    if(matfp == nullptr) {
        throw std::runtime_error("Failed to open MAT file: " + filename);
    }

    matvar_t* matvar;
    py::dict mat_dict;

    while((matvar = Mat_VarReadNext(matfp)) != nullptr) {
        try {
            printf("in matvar->name: %s\n", matvar->name);
            mat_dict[matvar->name] = matvar_to_pyobject(matvar, 0, simplify_cells);
            printf("out matvar->name: %s\n", matvar->name);
        } catch(const std::exception& e) {
            printf("Error processing variable '%s': %s\n", matvar->name, e.what());
            Mat_VarFree(matvar);
            Mat_Close(matfp);

            throw std::runtime_error(std::string("Error processing variable"));
        }
        Mat_VarFree(matvar);
    }

    Mat_Close(matfp);
    return mat_dict;
}

// Helper function to convert Python object to matvar_t
matvar_t* pyobject_to_matvar(const std::string& name, py::object obj);

// Helper function to handle numeric data for savemat
matvar_t* handle_numeric_save(const std::string& name, py::array& array) {
    matio_classes class_type;
    matio_types data_type;

    // Determine the MAT class and type based on NumPy dtype
    if(array.dtype().is(py::dtype::of<double>())) {
        class_type = MAT_C_DOUBLE;
        data_type = MAT_T_DOUBLE;
    }
    else if(array.dtype().is(py::dtype::of<float>())) {
        class_type = MAT_C_SINGLE;
        data_type = MAT_T_SINGLE;
    }
    else if(array.dtype().is(py::dtype::of<int8_t>())) {
        class_type = MAT_C_INT8;
        data_type = MAT_T_INT8;
    }
    else if(array.dtype().is(py::dtype::of<uint8_t>())) {
        class_type = MAT_C_UINT8;
        data_type = MAT_T_UINT8;
    }
    else if(array.dtype().is(py::dtype::of<int16_t>())) {
        class_type = MAT_C_INT16;
        data_type = MAT_T_INT16;
    }
    else if(array.dtype().is(py::dtype::of<uint16_t>())) {
        class_type = MAT_C_UINT16;
        data_type = MAT_T_UINT16;
    }
    else if(array.dtype().is(py::dtype::of<int32_t>())) {
        class_type = MAT_C_INT32;
        data_type = MAT_T_INT32;
    }
    else if(array.dtype().is(py::dtype::of<uint32_t>())) {
        class_type = MAT_C_UINT32;
        data_type = MAT_T_UINT32;
    }
    else if(array.dtype().is(py::dtype::of<int64_t>())) {
        class_type = MAT_C_INT64;
        data_type = MAT_T_INT64;
    }
    else if(array.dtype().is(py::dtype::of<uint64_t>())) {
        class_type = MAT_C_UINT64;
        data_type = MAT_T_UINT64;
    }
    else {
        throw std::runtime_error("Unsupported NumPy dtype for numeric array: '" + std::string(1, array.dtype().char_()) + "'");
    }

    // Prepare dimensions
    std::vector<size_t> dims(array.ndim());
    for(size_t i = 0; i < array.ndim(); ++i) {
        dims[i] = array.shape(i);
    }

    matvar_t* matvar = Mat_VarCreate(name.c_str(), class_type, data_type, static_cast<int>(array.ndim()), dims.data(), array.data(), 0);
    if(matvar == nullptr) {
        throw std::runtime_error("Failed to create MAT variable for numeric array: " + name);
    }

    return matvar;
}

// Function to convert Python object to matvar_t
matvar_t* pyobject_to_matvar(const std::string& name, py::object obj) {
    if(py::isinstance<py::array>(obj)) {
        py::array array = py::cast<py::array>(obj);
        if(array.dtype().is(py::dtype::of<std::complex<double>>())) {
            throw std::runtime_error("Complex type is not supported");
        }
        else {
            // Handle real numeric arrays
            return handle_numeric_save(name, array);
        }
    }
    else if(py::isinstance<py::dict>(obj)) {
        py::dict dict = py::cast<py::dict>(obj);
        std::vector<const char*> field_names;
        std::vector<matvar_t*> fields;

        for(auto item : dict) {
            std::string field_name = py::str(item.first);
            field_names.push_back(strdup(field_name.c_str())); // To be freed later
            matvar_t* field_var = pyobject_to_matvar(field_name, pybind11::reinterpret_borrow<pybind11::object>(item.second));

            if(field_var == nullptr) {
                // Free previously allocated memory
                for(auto fn : field_names) free((void*)fn);
                for(auto fv : fields) Mat_VarFree(fv);
                throw std::runtime_error("Unsupported Python object type in struct field: " + field_name);
            }
            fields.push_back(field_var);
        }

        size_t num_fields = field_names.size();
        std::vector<const char*> c_field_names(num_fields);
        for(size_t i = 0; i < num_fields; ++i) {
            c_field_names[i] = field_names[i];
        }

        size_t dims[2] = {1, 1};
        matvar_t* matvar = Mat_VarCreateStruct(name.c_str(), 2, dims, c_field_names.data(), num_fields);
        if(matvar == nullptr) {
            for(auto fn : field_names) free((void*)fn);
            for(auto fv : fields) Mat_VarFree(fv);
            throw std::runtime_error("Failed to create MAT struct variable: " + name);
        }

        for(size_t i = 0; i < num_fields; ++i) {
            if(Mat_VarSetStructFieldByName(matvar, c_field_names[i], 0, fields[i]) != 0) {
                Mat_VarFree(matvar);
                for(auto fn : field_names) free((void*)fn);
                for(auto fv : fields) Mat_VarFree(fv);
                throw std::runtime_error("Failed to set struct field: " + std::string(c_field_names[i]));
            }
            // Ownership of fields[i] is transferred to matvar
        }

        // Free allocated field names
        for(auto fn : field_names) free((void*)fn);

        return matvar;
    }
    else if(py::isinstance<py::list>(obj)) {
        py::list list = py::cast<py::list>(obj);
        size_t num_elements = list.size();
        size_t dims[2] = {1, num_elements};
        matvar_t* matvar = Mat_VarCreate(name.c_str(), MAT_C_CELL, MAT_T_CELL, 2, dims, NULL, 0);
        if(matvar == nullptr) {
            throw std::runtime_error("Failed to create MAT cell array variable: " + name);
        }

        for(size_t i = 0; i < num_elements; ++i) {
            py::object item = list[i];
            matvar_t* cell_var = pyobject_to_matvar("", item); // Empty name for cell
            if(cell_var == nullptr) {
                Mat_VarFree(matvar);
                throw std::runtime_error("Unsupported Python object type in cell array.");
            }
            if(Mat_VarSetCell(matvar, i, cell_var) != 0) {
                Mat_VarFree(cell_var);
                Mat_VarFree(matvar);
                throw std::runtime_error("Failed to set cell array element at index: " + std::to_string(i));
            }
        }

        return matvar;
    }
    else if(py::isinstance<py::str>(obj)) {
        std::string str = py::str(obj);
        size_t dims[2] = {1, str.size()};
        matvar_t* matvar = Mat_VarCreate(name.c_str(), MAT_C_CHAR, MAT_T_UINT8, 2, dims, (void*)str.c_str(), 0);
        if(matvar == nullptr) {
            throw std::runtime_error("Failed to create MAT char variable: " + name);
        }
        return matvar;
    }
    else {
        throw std::runtime_error("Unsupported Python object type for MAT variable: " + name);
    }
}

// Function to save MAT file
void savemat(const std::string& filename, py::dict dict) {
    mat_t* matfp = Mat_CreateVer(filename.c_str(), NULL, MAT_FT_DEFAULT);
    if(matfp == nullptr) {
        throw std::runtime_error("Failed to create MAT file: " + filename);
    }

    for(auto item : dict) {
        std::string var_name = py::str(item.first);
        py::object obj = py::reinterpret_borrow<py::object>(item.second);
        matvar_t* matvar = nullptr;

        try {
            matvar = pyobject_to_matvar(var_name, obj);
        } catch(const std::exception& e) {
            Mat_Close(matfp);
            throw std::runtime_error(std::string("Error converting Python object to MAT variable for '") + var_name + "': " + e.what());
        }

        if(Mat_VarWrite(matfp, matvar, MAT_COMPRESSION_ZLIB) != 0) {
            Mat_VarFree(matvar);
            Mat_Close(matfp);
            throw std::runtime_error("Failed to write MAT variable: " + var_name);
        }

        Mat_VarFree(matvar);
    }

    Mat_Close(matfp);
    return;
}

pybind11::tuple get_library_version() {
    int version[3];
    Mat_GetLibraryVersion(version, version + 1, version + 2);
    pybind11::tuple v(3);
    for (int i = 0; i < 3; i++)
        v[i] = version[i];
    return v;
}

PYBIND11_MODULE(libpymatio, m) {
    
    m.def("loadmat", &loadmat, "Load a MAT file",
          py::arg("filename"),
          py::arg("simplify_cells") = false
    );

    m.def("savemat", &savemat, "Save variables to a MAT file",
          py::arg("filename"),
          py::arg("dict"));
// pybind11::enum_<matio::MatAcc>(m, "MatAcc", "MAT file access types.")
//     .value("RDONLY", matio::MatAcc::RDONLY, "Read only file access.")
//     .value("RDWR", matio::MatAcc::RDWR, "Read/Write file access.")
//     .export_values();
// pybind11::enum_<matio::MatFt>(m, "MatFt", "MAT file versions.")
//     .value("MAT73", matio::MatFt::MAT7_3, "Matlab version 7.3 file.")
//     .value("MAT5", matio::MatFt::MAT5, "Matlab version 5 file.")
//     .value("MAT4", matio::MatFt::MAT4, "Matlab version 4 file.")
//     .value("UNDEFINED", matio::MatFt::UNDEFINED, "Undefined version.")
//     .export_values();
// pybind11::enum_<matio::MatioTypes>(m, "MatioTypes", "Matlab data types.")
//     .value("UNKNOWN", matio::MatioTypes::T_UNKNOWN, "UNKNOWN data type.")
//     .value("INT8", matio::MatioTypes::T_INT8, "8-bit signed integer data type.")
//     .value("UINT8", matio::MatioTypes::T_UINT8, "8-bit unsigned integer data type.")
//     .value("INT16", matio::MatioTypes::T_INT16, "16-bit signed integer data type.")
//     .value("UINT16", matio::MatioTypes::T_UINT16, "16-bit unsigned integer data type.")
//     .value("INT32", matio::MatioTypes::T_INT32, "32-bit signed integer data type.")
//     .value("UINT32", matio::MatioTypes::T_UINT32, "32-bit unsigned integer data type.")
//     .value("SINGLE", matio::MatioTypes::T_SINGLE, "IEEE 754 single precision data type.")
//     .value("DOUBLE", matio::MatioTypes::T_DOUBLE, "IEEE 754 double precision data type.")
//     .value("INT64", matio::MatioTypes::T_INT64, "64-bit signed integer data type.")
//     .value("UINT64", matio::MatioTypes::T_UINT64, "64-bit unsigned integer data type.")
//     .value("MATRIX", matio::MatioTypes::T_MATRIX, "matrix data type.")
//     .value("COMPRESSED", matio::MatioTypes::T_COMPRESSED, "compressed data type.")
//     .value("UTF8", matio::MatioTypes::T_UTF8, "8-bit Unicode text data type.")
//     .value("UTF16", matio::MatioTypes::T_UTF16, "16-bit Unicode text data type.")
//     .value("UTF32", matio::MatioTypes::T_UTF32, "32-bit Unicode text data type.")
//     .value("STRING", matio::MatioTypes::T_STRING, "String data type.")
//     .value("CELL", matio::MatioTypes::T_CELL, "Cell array data type.")
//     .value("STRUCT", matio::MatioTypes::T_STRUCT, "Structure data type.")
//     .value("ARRAY", matio::MatioTypes::T_ARRAY, "Array data type.")
//     .value("FUNCTION", matio::MatioTypes::T_FUNCTION, "Function data type.")
//     .export_values();
// pybind11::enum_<matio::MatioClasses>(m, "MatioClasses", "Matlab variable classes.")
//     .value("EMPTY", matio::C_EMPTY, "Empty array.")
//     .value("CELL", matio::C_CELL, "Matlab cell array class.")
//     .value("STRUCT", matio::C_STRUCT, "Matlab structure class.")
//     .value("OBJECT", matio::C_OBJECT, "Matlab object class.")
//     .value("CHAR", matio::C_CHAR, "Matlab character array class.")
//     .value("SPARSE", matio::C_SPARSE, "Matlab sparse array class.")
//     .value("DOUBLE", matio::C_DOUBLE, "Matlab double-precision class.")
//     .value("SINGLE", matio::C_SINGLE, "Matlab single-precision class.")
//     .value("INT8", matio::C_INT8, "Matlab signed 8-bit integer class.")
//     .value("UINT8", matio::C_UINT8, "Matlab unsigned 8-bit integer class.")
//     .value("INT16", matio::C_INT16, "Matlab signed 16-bit integer class.")
//     .value("UINT16", matio::C_UINT16, "Matlab unsigned 16-bit integer class.")
//     .value("INT32", matio::C_INT32, "Matlab signed 32-bit integer class.")
//     .value("UINT32", matio::C_UINT32, "Matlab unsigned 32-bit integer class.")
//     .value("INT64", matio::C_INT64, "Matlab signed 64-bit integer class.")
//     .value("UINT64", matio::C_UINT64, "Matlab unsigned 64-bit integer class.")
//     .value("FUNCTION", matio::C_FUNCTION, "Matlab function class.")
//     .value("OPAQUE", matio::C_OPAQUE, "Matlab opaque class.")
//     .export_values();
// pybind11::enum_<matio::MatioCompression>(m, "MatioCompression", "MAT file compression options.")
//     .value("NONE", matio::NONE, "No compression.")
//     .value("ZLIB", matio::ZLIB, "zlib compression.")
//     .export_values();
// pybind11::enum_<matio::MatioFlags>(m, "MatioFlags", "Matlab array flags")
//         .value("COMPLEX", matio::MatioFlags::COMPLEX, "Complex bit flag.")
//         .value("GLOBAL", matio::MatioFlags::GLOBAL, "Global bit flag.")
//         .value("LOGICAL", matio::MatioFlags::LOGICAL, "Logical bit flag.")
//         .value("DONT_COPY_DATA", matio::MatioFlags::DONT_COPY_DATA, "Don't copy data, use keep the pointer.")
//         .export_values();
// pybind11::class_<matio::MatT>(m, "MatT", "Matlab MAT File information.")
//     .def(pybind11::init())
//     .def_readwrite("fp", &matio::MatT::fp, "File pointer for the MAT file.")
//     .def_readwrite("header", &matio::MatT::header, "MAT file header string.")
//     .def_readwrite("subsys_offset", &matio::MatT::subsys_offset, "Offset.")
//     .def_readwrite("version", &matio::MatT::version, "MAT file version.")
//     .def_readwrite("filename", &matio::MatT::filename, "Filename of the MAT file.")
//     .def_readwrite("byte_swap", &matio::MatT::byteswap, "1 if byte swapping is required, 0 otherwise.")
//     .def_readwrite("bof", &matio::MatT::bof, "Beginning of file not including any header.")
//     .def_readwrite("next_index", &matio::MatT::next_index, "Index/File position of next variable to read.")
//     .def_readwrite("num_datasets", &matio::MatT::num_datasets, "Number of datasets in the file.")
// #if defined(MAT73) && MAT73
//     .def_readwrite("refs_id", &matio::MatT::refs_id, "Id of the /#refs# group in HDF5.")
// #else
//     .def_readwrite("refs_id", [](const matio::MatT&) -> int { PyErr_SetString(PyExc_RuntimeError, "refs_id is not available without HDF5(mat73) support."); throw pybind11::error_already_set(); }, "Id of the /#refs# group in HDF5.")
// #endif
//     .def_property_readonly("dir", &matio::MatT::get_dir, pybind11::return_value_policy::move, "Names of the datasets in the file.")
//     .def_property("mode", &matio::MatT::get_mode, &matio::MatT::set_mode, "Access mode.");
// pybind11::class_<matio::MatVarT>(m, "MatVarT", "Matlab variable information.")
//     .def(pybind11::init())
//     .def_readwrite("num_bytes", &matio::MatVarT::nbytes, "Number of bytes for the MAT variable.")
//     .def_readwrite("rank", &matio::MatVarT::rank, "Rank (Number of dimensions) of the data.")
//     .def_readwrite("data_size", &matio::MatVarT::data_size, "Bytes / element for the data.")
//     .def_readwrite("data_type", &matio::MatVarT::data_type, "Data type (MatioTypes.*).")
//     .def_property("class_type", 
//         [](const matio::MatVarT& self) { return static_cast<matio::MatioClasses>(self.class_type); },
//         [](matio::MatVarT& self, matio::MatioClasses value) { self.class_type = static_cast<matio_classes>(value); },
//         "Class type (MatioClasses.*)")

//     .def_readwrite("is_complex", &matio::MatVarT::isComplex, "non-zero if the data is complex, 0 if real.")
//     .def_readwrite("is_global", &matio::MatVarT::isGlobal, "non-zero if the variable is global.")
//     .def_readwrite("is_logical", &matio::MatVarT::isLogical, "non-zero if the variable is logical.")
//     .def_property_readonly("dims", [](const matio::MatVarT& var) {
//         return pybind11::array_t<size_t>(var.rank, var.dims);
//     }, "Dimensions of the variable.")
//     .def_readwrite("name", &matio::MatVarT::name, "Name of the variable.")
//     .def_readwrite("mem_conserve", &matio::MatVarT::mem_conserve, "1 if Memory was conserved with data.")
//     .def_readwrite("internal", &matio::MatVarT::internal, "matio internal data.");
// pybind11::class_<matio::MatComplexSplitT>(m, "MatComplexSplitT", "Complex data type using split storage.")
//     .def(pybind11::init())
//     .def_property("real", &matio::MatComplexSplitT::get_real, &matio::MatComplexSplitT::set_real, "Pointer to the real part.")
//     .def_property("imag", &matio::MatComplexSplitT::get_imag, &matio::MatComplexSplitT::set_imag, "Pointer to the imaginary part.");
m
    .def("get_library_version", &get_library_version, pybind11::return_value_policy::move, "Get the version of the library.");
//     .def("log_init", &matio::log_init, "Initializes the logging system.")
//     .def("set_debug", &matio::set_debug, "Set debug parameter.")
//     .def("critical", &matio::critical, "Logs a Critical message and returns to the user.")
//     .def("message", &matio::message, "Log a message unless silent.")
//     .def("help", &matio::help, "Prints a help string to stdout and exits with status EXIT_SUCCESS (typically 0).")
//     .def("create_ver", &matio::create_ver, pybind11::return_value_policy::automatic_reference, "Creates a new Matlab MAT file.")
//     .def("open", &matio::open, pybind11::return_value_policy::automatic_reference, "Opens an existing Matlab MAT file.")
//     .def("close", &matio::close, "Closes an open Matlab MAT file.")
//     .def("var_read_next", &matio::var_read_next, pybind11::return_value_policy::automatic_reference, "Reads the next variable in a MAT file.")
//     .def("var_duplicate", &matio::var_duplicate, pybind11::return_value_policy::automatic_reference, "Duplicates a MatVarT structure.")
//     .def("var_free", &matio::var_free, "Frees all the allocated memory associated with the structure.")
//     .def("var_write", &matio::var_write, "Writes the given MAT variable to a MAT file.")
//     .def("var_read_info", &matio::var_read_info, pybind11::return_value_policy::automatic_reference, "Reads the information of a variable with the given name from a MAT file.")
//     .def("var_print", &matio::var_print, "Prints the variable information.")
//     .def("calc_subscripts2", &matio::calc_subscripts2, pybind11::return_value_policy::move, "Calculate a set of subscript values from a single(linear) subscript.")
//     .def("calc_single_subscript2", &matio::calc_single_subscript2, pybind11::return_value_policy::move, "Calculate a single subscript from a set of subscript values.")
//     .def("var_read", &matio::var_read, pybind11::return_value_policy::automatic_reference, "Reads the variable with the given name from a MAT file.")
//     .def("var_create", &matio::var_create, pybind11::return_value_policy::automatic_reference, "Creates a MAT Variable with the given name and (optionally) data.")
//     .def("var_create_struct", &matio::var_create_struct, pybind11::return_value_policy::automatic_reference, "Creates a structure MATLAB variable with the given name and fields.")
//     .def("get_file_access_mode", &matio::get_file_access_mode, "Gets the file access mode of the given MAT file.")
//     .def("var_write_append", &matio::var_write_append, "Writes/appends the given MAT variable to a version 7.3 MAT file.")
//     .def("var_set_struct_field_by_name", &matio::var_set_struct_field_by_name, pybind11::return_value_policy::automatic_reference, "Sets the structure field to the given variable.")
//     .def("var_set_cell", &matio::var_set_cell, pybind11::return_value_policy::automatic_reference, "Sets the element of the cell array at the specific index.")
//     .def("var_set_struct_field_by_index", &matio::var_set_struct_field_by_index, pybind11::return_value_policy::automatic_reference, "Sets the structure field to the given variable.")
//     .def("var_get_number_of_fields", &matio::var_get_number_of_fields, "Returns the number of fields in a structure variable.")
//     .def("var_get_struct_field_names", &matio::var_get_struct_field_names, pybind11::return_value_policy::move, "Returns the fieldnames of a structure variable.")
//     .def("var_add_struct_field", &matio::var_add_struct_field, "Adds a field to a structure.")
//     .def("var_get_structs_linear", &matio::var_get_structs_linear, pybind11::return_value_policy::automatic_reference, "Indexes a structure.")
//     .def("var_get_structs", &matio::var_get_structs, pybind11::return_value_policy::automatic_reference, "Indexes a structure.")
//     .def("var_get_cells_linear", &matio::var_get_cells_linear, pybind11::return_value_policy::automatic_reference, "Indexes a cell array.")
//     .def("var_get_cells", &matio::var_get_cells, pybind11::return_value_policy::automatic_reference, "Indexes a cell array.")
//     .def("var_get_struct_field", &matio::var_get_struct_field, pybind11::return_value_policy::automatic_reference, "Finds a field of a structure.")
//     .def("var_read_data", &matio::var_read_data, "Reads MAT variable data from a file.")
//     .def("var_delete", &matio::var_delete, "Deletes a variable from a file.")
//     .def("get_dir", &matio::get_dir, pybind11::return_value_policy::move, "Gets a list of the variables of a MAT file.")
//     .def("get_filename", &matio::get_filename, "Gets the filename for the given MAT file.")
//     .def("get_version", &matio::get_version, "Gets the version of the given MAT file.")
//     .def("get_header", &matio::get_header, "Gets the header for the given MAT file.");
}
