//
// Created by Administrator on 2023/11/1.
//
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <matio.h>
#include <string>
#include <vector>
#include <map>
#include <codecvt>
#include <locale>
#include "matio.h"
#include "libmatio.h"


namespace py = pybind11;


std::string gbk_to_utf8(const std::string& input) {
    try {
        static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
        std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>> gbk_conv;
        std::wstring utf16_str = gbk_conv.from_bytes(input);
        return utf8_conv.to_bytes(utf16_str);
    } catch (const std::exception&) {
        printf("gbk_to_utf8 error: %s\n", input.c_str());
        // 如果转换失败，返回原始字符串
        return input;
    }
}

std::string utf8_to_gbk(const std::string& utf8_str) {
    try {
        static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
        std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>> gbk_conv;
        std::wstring utf16_str = utf8_conv.from_bytes(utf8_str);
        return gbk_conv.to_bytes(utf16_str);
    } catch (const std::exception&) {
        // 如果转换失败，返回原始字符串
        return utf8_str;
    }
}

py::object read_mat_var(matvar_t* matvar) {
    if (matvar == nullptr) {
        return py::none();
    }

    switch (matvar->class_type) {
        case MAT_C_DOUBLE:
        case MAT_C_SINGLE:
        case MAT_C_INT8:
        case MAT_C_UINT8:
        case MAT_C_INT16:
        case MAT_C_UINT16:
        case MAT_C_INT32:
        case MAT_C_UINT32:
        case MAT_C_INT64:
        case MAT_C_UINT64: {
            std::vector<ssize_t> dims(matvar->dims, matvar->dims + matvar->rank);
            py::array_t<double> arr(dims);
            memcpy(arr.mutable_data(), matvar->data, matvar->nbytes);
            return std::move(arr);
        }
        case MAT_C_CHAR: {
            std::string raw_str(static_cast<char*>(matvar->data), matvar->nbytes);
            std::string utf8_str = gbk_to_utf8(raw_str);
            return py::str(utf8_str);
        }
        case MAT_C_STRUCT: {
            py::dict result;
            size_t nfields = Mat_VarGetNumberOfFields(matvar);
            char* const* fieldnames = Mat_VarGetStructFieldnames(matvar);
            for (size_t i = 0; i < nfields; ++i) {
                matvar_t* field = Mat_VarGetStructFieldByName(matvar, fieldnames[i], 0);
                result[py::str((fieldnames[i]))] = read_mat_var(field);
            }
            return std::move(result);
        }
        case MAT_C_CELL: {
            py::list result;
            for (size_t i = 0; i < matvar->nbytes / sizeof(matvar_t*); ++i) {
                matvar_t* cell = Mat_VarGetCell(matvar, i);
                result.append(read_mat_var(cell));
            }
            return std::move(result);
        }
        default:
            return py::none();
    }
}

py::dict loadmat(const std::string& filename) {
    mat_t* mat = Mat_Open(filename.c_str(), MAT_ACC_RDONLY);
    if (mat == nullptr) {
        throw std::runtime_error("Failed to open MAT file");
    }

    py::dict result;
    matvar_t* matvar;
    while ((matvar = Mat_VarReadNext(mat)) != nullptr) {
        result[py::str((matvar->name))] = read_mat_var(matvar);
        Mat_VarFree(matvar);
    }

    Mat_Close(mat);
    return result;
}

void write_mat_var(mat_t* mat, const std::string& name, const py::object& data, int indent = 0) {
    if (py::isinstance<py::array>(data)) {
        py::array arr = data.cast<py::array>();
        std::vector<size_t> dims(arr.ndim());
        for (size_t i = 0; i < arr.ndim(); ++i) {
            dims[i] = arr.shape(i);
        }
        
        matio_classes class_type;
        matio_types data_type;
        
        if (arr.dtype().kind() == 'f') {
            class_type = MAT_C_DOUBLE;
            data_type = MAT_T_DOUBLE;
        } else if (arr.dtype().kind() == 'i') {
            class_type = MAT_C_INT32;
            data_type = MAT_T_INT32;
        } else {
            throw std::runtime_error("Unsupported array data type");
        }

        printf("%*s write mat var py::array: %s, %d, %d, dim: %d\n", indent, "", name.c_str(), class_type, data_type, arr.ndim());

        matvar_t* matvar = Mat_VarCreate(name.c_str(), class_type, data_type, arr.ndim(), dims.data(), arr.data(), 0);
        Mat_VarWrite(mat, matvar, MAT_COMPRESSION_NONE);
        Mat_VarFree(matvar);
    }
    else if (py::isinstance<py::str>(data)) {
        printf("%*s write mat var py::str: %s, %d, %d, %d\n", indent, "", name.c_str(), MAT_C_CHAR, MAT_T_UTF8, 2);
        std::string str = utf8_to_gbk(data.cast<std::string>());
        size_t dims[2] = {1, str.length()};
        matvar_t* matvar = Mat_VarCreate(name.c_str(), MAT_C_CHAR, MAT_T_UTF8, 2, dims, const_cast<char*>(str.c_str()), 0);
        Mat_VarWrite(mat, matvar, MAT_COMPRESSION_NONE);
        Mat_VarFree(matvar);
    }
    else if (py::isinstance<py::dict>(data)) {
        py::dict dict = data.cast<py::dict>();
        std::vector<const char*> fieldnames;
        std::vector<matvar_t*> fields;
        
        for (auto item : dict) {
            std::string key = py::str(item.first).cast<std::string>();
            fieldnames.push_back(strdup(key.c_str()));
            printf("%*s write mat var py::dict: %s, %d, %d, %d\n", indent, "", key.c_str(), MAT_C_STRUCT, MAT_T_STRUCT, 2);
            // 创建一个临时变量来存储字段值
            matvar_t* field = Mat_VarCreate(key.c_str(), MAT_C_DOUBLE, MAT_T_DOUBLE, 0, nullptr, nullptr, 0);
            write_mat_var(mat, key, item.second.cast<py::object>(), indent + 2);
            fields.push_back(field);
        }
        
        size_t dims[2] = {1, 1};
        matvar_t* matvar = Mat_VarCreate(name.c_str(), MAT_C_STRUCT, MAT_T_STRUCT, 2, dims, nullptr, 0);
        
        // 使用 Mat_VarAddStructField 来添加字段
        for (size_t i = 0; i < fieldnames.size(); ++i) {
            Mat_VarAddStructField(matvar, fieldnames[i]);
            Mat_VarSetStructFieldByName(matvar, fieldnames[i], 0, fields[i]);
        }
        
        Mat_VarWrite(mat, matvar, MAT_COMPRESSION_NONE);
        Mat_VarFree(matvar);
        
        // 清理内存
        for (auto field : fields) {
            Mat_VarFree(field);
        }
        for (auto fieldname : fieldnames) {
            free(const_cast<char*>(fieldname));
        }
    }
    else if (py::isinstance<py::list>(data)) {
        py::list list = data.cast<py::list>();
        size_t dims[2] = {1, list.size()};
        matvar_t* matvar = Mat_VarCreate(name.c_str(), MAT_C_CELL, MAT_T_CELL, 2, dims, nullptr, 0);
        printf("%*s write mat var py::list: %s, %d, %d, dim: %d\n", indent, "", name.c_str(), MAT_C_CELL, MAT_T_CELL, list.size());
        for (size_t i = 0; i < list.size(); ++i) {
            matvar_t* cell = Mat_VarCreate(nullptr, MAT_C_DOUBLE, MAT_T_DOUBLE, 0, nullptr, nullptr, 0);
            write_mat_var(mat, "", list[i], indent + 2);
            Mat_VarSetCell(matvar, i, cell);
            Mat_VarFree(cell);
        }
        
        Mat_VarWrite(mat, matvar, MAT_COMPRESSION_NONE);
        Mat_VarFree(matvar);
    }
    else if (py::isinstance<py::none>(data)) {
        printf("%*s write mat var py::none: %s, %d, %d, %d\n", indent, "", name.c_str(), MAT_C_EMPTY, MAT_T_DOUBLE, 2);
        size_t dims[2] = {0, 0};  // 创建一个 0x0 的空矩阵
        matvar_t* matvar = Mat_VarCreate(name.c_str(), MAT_C_EMPTY, MAT_T_DOUBLE, 2, dims, nullptr, 0);
        Mat_VarWrite(mat, matvar, MAT_COMPRESSION_NONE);
        Mat_VarFree(matvar);
    }
    else {
        printf("%*s write mat var unsupported data type: %s, %s\n", indent, "", name.c_str(), py::str(data.get_type()).cast<std::string>().c_str());
        throw std::runtime_error("Unsupported data type");
    }
}

void savemat(const std::string& filename, const py::dict& data) {
    printf("save mat filename: %s\n", filename.c_str());
    mat_t* mat = Mat_CreateVer(filename.c_str(), nullptr, MAT_FT_MAT73);
    if (mat == nullptr) {
        throw std::runtime_error("Failed to create MAT file");
    }
    
    for (auto item : data) {
        std::string key = py::str(item.first);
        printf("save mat key: %s\n", key.c_str());
        write_mat_var(mat, key, item.second.cast<py::object>(), 0);
    }
    
    Mat_Close(mat);
}
PYBIND11_MODULE(libpymatio, m) {
    
    m.def("loadmat", &loadmat, "Load a MAT file",
          py::arg("filename"));

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
    .def("get_library_version", &matio::get_library_version, pybind11::return_value_policy::move, "Get the version of the library.");
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
