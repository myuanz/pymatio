#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>

#include <nanobind/stl/tuple.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <matio.h>
#include <string>
#include <vector>
#include <map>
#include <codecvt>
#include <locale>
#include <stdexcept>
#include <cstring>
#include <fmt/format.h>
#include <iostream>

#include "matio_private.h"
#include "matio.h"
#include "strides_utils.h"

namespace nb = nanobind;


static bool DEBUG_LOG_ENABLED = false;

template<typename... Args>
void debug_log_with_indent(const std::string& fmt, int indent, Args&&... args) {
    if (!DEBUG_LOG_ENABLED) {
        return;
    }
    try {
        std::string formatted = fmt::format(fmt, std::forward<Args>(args)...);
        return fmt::println("{:{}}{}", "", indent, formatted);
    } catch (const fmt::format_error& e) {
        std::cerr << "Format error in debug_log_with_indent: " << e.what() << "\n";
        std::cerr << "Format string: " << fmt << "\n";
        std::cerr << "Indent: " << indent << "\n";
        std::cerr << "Argument types: ";
        int dummy[] = { 0, (std::cerr << typeid(Args).name() << " ", 0)... };
        (void)dummy;
        std::cerr << "\n";
        throw; 
    }
}

template<typename... Args>
void debug_log(const std::string& fmt, Args&&... args) {
    return debug_log_with_indent(fmt, 0, std::forward<Args>(args)...);
}


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
nb::object matvar_to_pyobject(matvar_t* matvar, int indent, bool simplify_cells);

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

nb::object handle_numeric(matvar_t* matvar, bool simplify_cells) {
    if(!matvar->data) {
        return nb::none();
    }

    size_t num_elements = 1;
    for(int i = 0; i < matvar->rank; ++i) {
        num_elements *= matvar->dims[i];
    }

    bool can_simplify = simplify_cells && 
                        (matvar->rank == 2) && 
                        (matvar->dims[0] == 1 || matvar->dims[1] == 1);

    nb::dlpack::dtype np_dtype;
    size_t element_size;

    switch(matvar->data_type) {
        case MAT_T_DOUBLE:
            np_dtype = nb::dtype<double>();
            element_size = sizeof(double);
            break;
        case MAT_T_SINGLE:
            np_dtype = nb::dtype<float>();
            element_size = sizeof(float);
            break;
        case MAT_T_INT8:
            np_dtype = nb::dtype<int8_t>();
            element_size = sizeof(int8_t);
            break;
        case MAT_T_UINT8:
            np_dtype = nb::dtype<uint8_t>();
            element_size = sizeof(uint8_t);
            if (matvar->isLogical) {
                np_dtype = nb::dtype<bool>();
            }
            break;
        case MAT_T_INT16:
            np_dtype = nb::dtype<int16_t>();
            element_size = sizeof(int16_t);
            break;
        case MAT_T_UINT16:
            np_dtype = nb::dtype<uint16_t>();
            element_size = sizeof(uint16_t);
            break;
        case MAT_T_INT32:
            np_dtype = nb::dtype<int32_t>();
            element_size = sizeof(int32_t);
            break;
        case MAT_T_UINT32:
            np_dtype = nb::dtype<uint32_t>();
            element_size = sizeof(uint32_t);
            break;
        case MAT_T_INT64:
            np_dtype = nb::dtype<int64_t>();
            element_size = sizeof(int64_t);
            break;
        case MAT_T_UINT64:
            np_dtype = nb::dtype<uint64_t>();
            element_size = sizeof(uint64_t);
            break;
        default:
            throw std::runtime_error("Unsupported MAT data type: " + std::to_string(matvar->data_type));
    }
    if (can_simplify && num_elements == 1) {
        switch(matvar->data_type) {
            case MAT_T_DOUBLE:
                return nb::cast(static_cast<double*>(matvar->data)[0]);
            case MAT_T_SINGLE:
                return nb::cast(static_cast<float*>(matvar->data)[0]);
            case MAT_T_INT8:
                return nb::cast(static_cast<int8_t*>(matvar->data)[0]);
            case MAT_T_INT16:
                return nb::cast(static_cast<int16_t*>(matvar->data)[0]);
            case MAT_T_INT32:
                return nb::cast(static_cast<int32_t*>(matvar->data)[0]);
            case MAT_T_INT64:
                return nb::cast(static_cast<int64_t*>(matvar->data)[0]);
            case MAT_T_UINT8:
                if (matvar->isLogical) {
                    return nb::cast(static_cast<uint8_t*>(matvar->data)[0] != 0);
                } else {
                    return nb::cast(static_cast<uint8_t*>(matvar->data)[0]);
                }

            case MAT_T_UINT16:
                return nb::cast(static_cast<uint16_t*>(matvar->data)[0]);
            case MAT_T_UINT32:
                return nb::cast(static_cast<uint32_t*>(matvar->data)[0]);
            case MAT_T_UINT64:
                return nb::cast(static_cast<uint64_t*>(matvar->data)[0]);
            default:
                throw std::runtime_error("Unsupported MAT data type: " + std::to_string(matvar->data_type));
        }
    }

    std::vector<ssize_t> shape = {};
    ssize_t rank = matvar->rank;

    for (int i = 0; i < matvar->rank; ++i) {
        if (matvar->dims[i] == 1 && can_simplify) {
            rank --;
            continue;
        }
        shape.push_back(static_cast<ssize_t>(matvar->dims[i]));
    }

    auto arr = nb::ndarray<nb::numpy, nb::f_contig>(
        matvar->data,
        rank, 
        reinterpret_cast<size_t*>(shape.data()), 
        {}, {}, 
        np_dtype
    );
    return arr.cast();
}

nb::object matvar_to_numpy_cell(matvar_t* matvar, int indent, bool simplify_cells) {
    if (!matvar || matvar->class_type != MAT_C_CELL) {
        throw std::runtime_error("Invalid matvar or not a cell array");
    }

    // 获取维度信息
    std::vector<ssize_t> shape(matvar->dims, matvar->dims + matvar->rank);

    // 检查是否可以简化为一维数组
    bool can_simplify = simplify_cells && 
                        (matvar->rank == 2) && 
                        (shape[0] == 1 || shape[1] == 1);

    // 如果可以简化，调整 shape
    if (can_simplify) {
        shape = {static_cast<ssize_t>(shape[0] * shape[1])};
    }

    // 计算总元素数
    size_t total_elements = 1;
    for (const auto& dim : shape) {
        total_elements *= dim;
    }
    debug_log_with_indent("cell total_elements: {:d}", indent, total_elements);
    debug_log_with_indent("rank: {:d}", indent, matvar->rank);

    std::string shape_str = "shape: (";
    for (const auto& s : shape) {
        shape_str += std::to_string(s) + ", ";
    }
    debug_log_with_indent(shape_str + ")", indent);

    if (total_elements == 1 && simplify_cells) {
        return matvar_to_pyobject(Mat_VarGetCell(matvar, 0), indent + 2, simplify_cells);
    }

    nb::module_ np = nb::module_::import_("numpy");

    nb::object cell_array = np.attr("empty")(
        shape,
        nb::arg("dtype") = np.attr("object_"),
        nb::arg("order") = "F"
    );

    matvar_t** cells = Mat_VarGetCellsLinear(matvar, 0, 1, total_elements);
    auto cell_array_reshaped = cell_array.attr("ravel")("F");

    for (size_t i = 0; i < total_elements; ++i) {
        nb::object obj;
        debug_log_with_indent("set item {:d}", indent, i);

        if (cells[i]) {
            obj = matvar_to_pyobject(cells[i], indent + 2, simplify_cells);
        } else {
            // obj = nb::none();
            // the default value of cell is empty array, so set it to empty is not necessary
        }
        cell_array_reshaped.attr("__setitem__")(i, obj);
    }
    return cell_array;
}

// Function to convert matvar_t to Python object
nb::object matvar_to_pyobject(matvar_t* matvar, int indent, bool simplify_cells = false) {
    if(matvar == nullptr) {
        return nb::none();
    }

    debug_log_with_indent("matvar {:s}", indent, combine_var_type(matvar).c_str());

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
            return nb::none();
        case MAT_C_STRUCT: {
            nb::dict struct_dict;
            if(!matvar->internal) {
                throw std::runtime_error("Malformed MAT_C_STRUCT variable: " + std::string(matvar->name));
            }
            debug_log_with_indent("matvar->internal->num_fields: {:d}", indent, matvar->internal->num_fields);
            for(unsigned i = 0; i < matvar->internal->num_fields; ++i) {
                const char* field_name = matvar->internal->fieldnames[i];
                matvar_t* field_var = static_cast<matvar_t**>(matvar->data)[i];
                debug_log_with_indent("field_name: {:s}", indent, field_name);

                if(field_var) {
                    struct_dict[field_name] = matvar_to_pyobject(field_var, indent + 2, simplify_cells);
                } else {
                    struct_dict[field_name] = nb::none();
                }
            }
            return struct_dict;
        }
        case MAT_C_CELL: {
            return matvar_to_numpy_cell(matvar, indent, simplify_cells);
        }
        case MAT_C_CHAR: {
            if(!matvar->data) {
                return nb::str("");
            }

            std::string raw_str(static_cast<char*>(matvar->data), matvar->nbytes);
            std::string utf8_str = string_to_utf8(matvar->data_type, raw_str);
            debug_log_with_indent("MAT_C_CHAT matvar->data: `{:s}`", indent, utf8_str);

            // Trim trailing spaces
            size_t endpos = utf8_str.find_last_not_of(" ");
            if(endpos != std::string::npos) {
                utf8_str = utf8_str.substr(0, endpos + 1);
            }
            return nb::str(utf8_str.c_str());
        }
        case MAT_C_OPAQUE: {
            throw std::runtime_error("Unsupported MAT class: " + std::to_string(matvar->class_type));
        }
        default:
            throw std::runtime_error("Unsupported MAT class: " + std::to_string(matvar->class_type));
    }
}

// Function to load MAT file
nb::dict loadmat(const std::string& filename, bool simplify_cells = false, bool debug_log_enabled = false) {
    DEBUG_LOG_ENABLED = debug_log_enabled || std::getenv("PYMATIO_DEBUG") != nullptr;

    debug_log("loadmat: {}", filename);

    mat_t* matfp = Mat_Open(filename.c_str(), MAT_ACC_RDONLY);
    if(matfp == nullptr) {
        throw std::runtime_error("Failed to open MAT file: " + filename);
    }

    matvar_t* matvar;
    nb::dict mat_dict;

    while((matvar = Mat_VarReadNext(matfp)) != nullptr) {
        try {
            debug_log("in matvar->name: {:s}", matvar->name);
            mat_dict[matvar->name] = matvar_to_pyobject(matvar, 0, simplify_cells);
            debug_log("out matvar->name: {:s}", matvar->name);
        } catch(const std::exception& e) {
            debug_log("Error processing variable '{:s}': {:s}", matvar->name, e.what());
            Mat_VarFree(matvar);
            Mat_Close(matfp);
            
            // backward::StackTrace st;
            // st.load_here(32);
            // backward::Printer p;
            // p.print(st);

            throw std::runtime_error(std::string("Error processing variable"));
        }
        Mat_VarFree(matvar);
    }

    Mat_Close(matfp);
    return mat_dict;
}

nb::tuple get_library_version() {
    int version[3];
    Mat_GetLibraryVersion(version, version + 1, version + 2);
    nb::tuple v = nb::make_tuple(version[0], version[1], version[2]);
    return v;
}

NB_MODULE(libpymatio, m) {
    
    m.def("loadmat", &loadmat, "Load a MAT file",
          nb::arg("filename"),
          nb::kw_only(),
          nb::arg("simplify_cells") = false,
          nb::arg("debug_log_enabled") = false
        );

    // m.def("savemat", &savemat, "Save variables to a MAT file",
    //       nb::arg("filename"),
    //       nb::arg("dict"));
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
    .def("get_library_version", &get_library_version, "Get the version of the library.")
    // .def("log_init", &Mat_LogInit, "Initializes the logging system.")
    // .def("set_debug", &Mat_SetDebug, "Set debug parameter.")
    // .def("critical", &Mat_Critical, "Logs a Critical message and returns to the user.")
    // .def("message", &Mat_Message, "Log a message unless silent.")
    // .def("help", &Mat_Help, "Prints a help string to stdout and exits with status EXIT_SUCCESS (typically 0).")
//     .def("create_ver", &matio::create_ver, pybind11::return_value_policy::automatic_reference, "Creates a new Matlab MAT file.")
//     .def("open", &matio::open, pybind11::return_value_policy::automatic_reference, "Opens an existing Matlab MAT file.")
//     .def("close", &matio::close, "Closes an open Matlab MAT file.")
//     .def("var_read_next", &matio::var_read_next, pybind11::return_value_policy::automatic_reference, "Reads the next variable in a MAT file.")
//     .def("var_duplicate", &matio::var_duplicate, pybind11::return_value_policy::automatic_reference, "Duplicates a MatVarT structure.")
//     .def("var_free", &matio::var_free, "Frees all the allocated memory associated with the structure.")
//     .def("var_write", &matio::var_write, "Writes the given MAT variable to a MAT file.")
//     .def("var_read_info", &matio::var_read_info, pybind11::return_value_policy::automatic_reference, "Reads the information of a variable with the given name from a MAT file.")
    // .def("var_print", &Mat_VarPrint, "Prints the variable information.")
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
;
}
