// #include "libmatio.h"

// #include <iostream>

// namespace matio {
//     pybind11::tuple get_library_version() {
//         int version[3];
//         Mat_GetLibraryVersion(version, version + 1, version + 2);
//         pybind11::tuple v(3);
//         for (int i = 0; i < 3; i++)
//             v[i] = version[i];
//         return v;
//     }

//     int log_init(const char *program) {
//         return Mat_LogInit(program);
//     }

//     void set_debug(int d) {
//         Mat_SetDebug(d);
//     }

//     void critical(const char *msg) {
//         Mat_Critical(msg, nullptr);
//     }

//     void help(const std::valarray<std::string> &helpStr) {
//         size_t size = helpStr.size();
//         const char **lines = new const char *[size + 1];
//         for (int i = 0; i < size; i++)
//             lines[i] = helpStr[i].c_str();
//         lines[size] = nullptr;
//         Mat_Help(lines);
//         delete[] lines;
//     }

//     MatT *create_ver(const char *matName, const char *hdrStr, enum MatFt matFileVer) {
//         return reinterpret_cast<MatT *>(Mat_CreateVer(matName, hdrStr, static_cast<mat_ft>(matFileVer)));
//     }

//     MatT *open(const char *filename, MatAcc mode) {
//         return reinterpret_cast<MatT *>(Mat_Open(filename, mode));
//     }

//     int close(MatT *matFile) {
//         return Mat_Close(matFile);
//     }

//     MatVarT *var_read_next(MatT *matFile) {
//         return reinterpret_cast<MatVarT *>(Mat_VarReadNext(matFile));
//     }

//     MatVarT *var_duplicate(const MatVarT *in, int opt) {
//         return reinterpret_cast<MatVarT *>(Mat_VarDuplicate(in, opt));
//     }

//     void var_free(MatVarT *in) {
//         Mat_VarFree(in);
//     }

//     int var_write(MatT *mat, MatVarT *var, MatioCompression compress) {
//         return Mat_VarWrite(reinterpret_cast<mat_t*>(mat), reinterpret_cast<matvar_t*>(var), static_cast<matio_compression>(compress));
//     }

//     MatVarT *var_read_info(MatT *mat, const char *name) {
//         return reinterpret_cast<MatVarT *>(Mat_VarReadInfo(mat, name));
//     }

//     void var_print(MatVarT *var, int print_data) {
//         Mat_VarPrint(var, print_data);
//     }

//     std::valarray<size_t> *calc_subscripts2(int rank, const std::valarray<size_t> &dims, size_t index) {
//         auto *temp = new size_t[dims.size()];
//         std::copy(begin(dims), end(dims), temp);
//         auto subs = Mat_CalcSubscripts2(rank, temp, index);
//         delete[] temp;
//         auto ret = new std::valarray<size_t>(subs, rank);
//         free(subs);
//         return ret;
//     }

//     void message(const char *msg) {
//         Mat_Message(msg, nullptr);
//     }

//     MatVarT *var_read(MatT *mat, const char *name) {
//         return reinterpret_cast<MatVarT *>(Mat_VarRead(mat, name));
//     }

//     template <typename T>
//     matvar_t *var_create_internal(const char *name, MatioClasses class_type, MatioTypes data_type, int rank, size_t *dims, const pybind11::object &data, int opt, [[maybe_unused]] T *unused) {
//         auto arr = data.cast<std::valarray<T>>();
//         auto temp = new T[arr.size()];
//         std::copy(begin(arr), end(arr), temp);
//         auto ret = Mat_VarCreate(name, static_cast<matio_classes>(class_type), static_cast<matio_types>(data_type), rank, dims, temp, opt);
//         delete[] temp;
//         return ret;
//     }

//     MatVarT *var_create(const char *name, MatioClasses class_type, MatioTypes data_type, int rank, const std::valarray<size_t> &dims, const pybind11::object &data, int opt) {
//         auto *temp = new size_t[dims.size()];
//         std::copy(begin(dims), end(dims), temp);
//         matvar_t *ret;
//         if (data.is_none()) {
//             ret = Mat_VarCreate(name, static_cast<matio_classes>(class_type), static_cast<matio_types>(data_type), rank, temp,
//                                 nullptr, opt);
//         } else
//             switch (class_type) {
//                 case C_CHAR: {
//                     auto str = data.cast<std::string>();
//                     ret = Mat_VarCreate(name, static_cast<matio_classes>(class_type), static_cast<matio_types>(data_type), rank, temp, str.data(), opt);
//                     break;
//                 }
//                 case C_DOUBLE:
//                     ret = var_create_internal<double>(name, class_type, data_type, rank, temp, data, opt, nullptr);
//                     break;
//                 case C_SINGLE:
//                     ret = var_create_internal<float>(name, class_type, data_type, rank, temp, data, opt, nullptr);
//                     break;
//                 case C_INT8:
//                     ret = var_create_internal<mat_int8_t>(name, class_type, data_type, rank, temp, data, opt, nullptr);
//                     break;
//                 case C_UINT8:
//                     ret = var_create_internal<mat_uint8_t>(name, class_type, data_type, rank, temp, data, opt, nullptr);
//                     break;
//                 case C_INT16:
//                     ret = var_create_internal<mat_int16_t>(name, class_type, data_type, rank, temp, data, opt, nullptr);
//                     break;
//                 case C_UINT16:
//                     ret = var_create_internal<mat_uint16_t>(name, class_type, data_type, rank, temp, data, opt, nullptr);
//                     break;
//                 case C_INT32:
//                     ret = var_create_internal<mat_int32_t>(name, class_type, data_type, rank, temp, data, opt, nullptr);
//                     break;
//                 case C_UINT32:
//                     ret = var_create_internal<mat_uint32_t>(name, class_type, data_type, rank, temp, data, opt, nullptr);
//                     break;
//                 case C_INT64:
//                     ret = var_create_internal<mat_int64_t>(name, class_type, data_type, rank, temp, data, opt, nullptr);
//                     break;
//                 case C_UINT64:
//                     ret = var_create_internal<mat_uint64_t>(name, class_type, data_type, rank, temp, data, opt, nullptr);
//                     break;
//                 default:
//                     ret = Mat_VarCreate(name, static_cast<matio_classes>(class_type), static_cast<matio_types>(data_type), rank, temp, nullptr, opt);
//                     break;
//         }
//         delete[] temp;
//         return reinterpret_cast<MatVarT *>(ret);
//     }

//     std::pair<int, size_t> calc_single_subscript2(int rank, const std::valarray<size_t> &dims, const std::valarray<size_t> &subs) {
//         auto temp = new size_t[dims.size()];
//         std::copy(begin(dims), end(dims), temp);
//         auto temp2 = new size_t[subs.size()];
//         std::copy(begin(subs), end(subs), temp2);
//         size_t linear_index = 0;
//         int error = Mat_CalcSingleSubscript2(rank, temp, temp2, &linear_index);
//         delete[] temp;
//         delete[] temp2;
//         return std::make_pair(error, linear_index);
//     }

//     MatAcc get_file_access_mode(MatT *mat) {
//         return static_cast<MatAcc>(Mat_GetFileAccessMode(mat));
//     }

//     int var_write_append(MatT *mat, MatVarT *var, MatioCompression compression, int dim) {
//         return Mat_VarWriteAppend(mat, var, static_cast<matio_compression>(compression), dim);
//     }

//     MatVarT *var_create_struct(const char *name, int rank, const std::valarray<size_t> &dims, const std::valarray<std::string> &fields) {
//         auto *temp = new size_t[dims.size()];
//         std::copy(begin(dims), end(dims), temp);
//         size_t num_fields = fields.size();
//         auto *temp2 = new const char *[num_fields + 1];
//         for (int i = 0; i < num_fields; i++)
//             temp2[i] = fields[i].c_str();
//         temp2[num_fields] = nullptr;
//         auto ret = Mat_VarCreateStruct(name, rank, temp, temp2, num_fields);
//         delete[] temp;
//         delete[] temp2;
//         return reinterpret_cast<MatVarT *>(ret);
//     }

//     MatVarT *var_set_struct_field_by_name(MatVarT *var, const char *fieldName, size_t index, MatVarT *field) {
//         return reinterpret_cast<MatVarT *>(Mat_VarSetStructFieldByName(var, fieldName, index, field));
//     }

//     MatVarT *var_set_cell(MatVarT *var, int index, MatVarT *cell) {
//         return reinterpret_cast<MatVarT *>(Mat_VarSetCell(var, index, cell));
//     }

//     MatVarT *var_set_struct_field_by_index(MatVarT *var, size_t fieldIndex, size_t index, MatVarT *field) {
//         return reinterpret_cast<MatVarT *>(Mat_VarSetStructFieldByIndex(var, fieldIndex, index, field));
//     }

//     unsigned int var_get_number_of_fields(MatVarT *var) {
//         return Mat_VarGetNumberOfFields(var);
//     }

//     std::valarray<const char *> *var_get_struct_field_names(MatVarT *var) {
//         unsigned int num = Mat_VarGetNumberOfFields(var);
//         auto p = Mat_VarGetStructFieldnames(var);
//         return new std::valarray<const char *>(p, num);
//     }

//     int var_add_struct_field(MatVarT *var, const char *fieldName) {
//         return Mat_VarAddStructField(var, fieldName);
//     }

//     MatVarT *var_get_structs_linear(MatVarT *var, int start, int stride, int edge, int copyFields) {
//         return reinterpret_cast<MatVarT *>(Mat_VarGetStructsLinear(var, start, stride, edge, copyFields));
//     }

//     MatVarT *var_get_structs(MatVarT *var, const std::valarray<int> &start, const std::valarray<int> &stride, const std::valarray<int> &edge, int copyFields) {
//         auto temp = new int[start.size()];
//         std::copy(begin(start), end(start), temp);
//         auto temp2 = new int[stride.size()];
//         std::copy(begin(stride), end(stride), temp2);
//         auto temp3 = new int[edge.size()];
//         std::copy(begin(edge), end(edge), temp3);
//         auto ret = Mat_VarGetStructs(var, temp, temp2, temp3, copyFields);
//         delete[] temp;
//         delete[] temp2;
//         delete[] temp3;
//         return reinterpret_cast<MatVarT *>(ret);
//     }

//     MatVarT *var_get_cells_linear(MatVarT *var, int start, int stride, int edge) {
//         return reinterpret_cast<MatVarT *>(Mat_VarGetCellsLinear(var, start, stride, edge));
//     }

//     MatVarT *
//     var_get_cells(MatVarT *var, const std::valarray<int> &start, const std::valarray<int> &stride, const std::valarray<int> &edge) {
//         auto temp = new int[start.size()];
//         std::copy(begin(start), end(start), temp);
//         auto temp2 = new int[stride.size()];
//         std::copy(begin(stride), end(stride), temp2);
//         auto temp3 = new int[edge.size()];
//         std::copy(begin(edge), end(edge), temp3);
//         auto ret = Mat_VarGetCells(var, temp, temp2, temp3);
//         delete[] temp;
//         delete[] temp2;
//         delete[] temp3;
//         return reinterpret_cast<MatVarT *>(ret);
//     }

//     MatVarT *var_get_struct_field(MatVarT *var, const pybind11::object &nameOrIndex, int index) {
//         if (pybind11::isinstance<pybind11::int_>(nameOrIndex)) {
//             int in = nameOrIndex.cast<int>();
//             return reinterpret_cast<MatVarT *>(Mat_VarGetStructField(var, &in, MAT_BY_INDEX, index));
//         }
//         if (pybind11::isinstance<pybind11::str>(nameOrIndex)) {
//             auto name = nameOrIndex.cast<std::string>();
//             return reinterpret_cast<MatVarT *>(Mat_VarGetStructField(var, name.data(), MAT_BY_NAME, index));
//         }
//         throw pybind11::type_error("Unsupported type, obtaining a field requires providing a name or index value.");
//     }

//     template <typename T>
//     int var_read_data_internal(MatT *mat, MatVarT *var, int size, int *start, int *stride, int *edge, const pybind11::tuple &data,
//                                [[maybe_unused]] T *unused) {
//         int rank = var->rank;
//         int ret;
//         if ( var->isComplex ) {
//             mat_complex_split_t c;
//             auto *ptr = new T[size], *pti = new T[size];
//             c.Re = ptr;
//             c.Im = pti;
//             ret = Mat_VarReadData(mat, var, &c, start, stride, edge);
//             if ( MAT_FT_MAT73 != mat->version ) {
//                 auto *tmp = static_cast<size_t *>(realloc(var->dims, ++var->rank * sizeof(size_t)));
//                 if (nullptr != tmp) {
//                     var->dims[rank] = 1;
//                     var->dims = tmp;
//                     ret = Mat_VarReadData(mat, var, &c, start, stride, edge);
//                 }
//             }
//             for (int i = 0; i < size; i++)
//                 data[i] = pybind11::make_tuple(ptr[i], pti[i]);
//             delete[] ptr;
//             delete[] pti;
//         } else {
//             auto *ptr = new T[size];
//             ret = Mat_VarReadData(mat, var, ptr, start, stride, edge);
//             if ( MAT_FT_MAT73 != mat->version ) {
//                 auto *tmp = static_cast<size_t *>(realloc(var->dims, ++var->rank * sizeof(size_t)));
//                 if ( nullptr != tmp ) {
//                     var->dims[rank] = 1;
//                     var->dims = tmp;
//                     ret = Mat_VarReadData(mat, var, ptr, start, stride, edge);
//                 }
//             }
//             for (int i = 0; i < size; i++)
//                 data[i] = ptr[i];
//             delete[] ptr;
//         }
//         return ret;
//     }
//     std::pair<int, pybind11::object>
//     var_read_data(MatT *mat, MatVarT *var, const std::valarray<int> &start, const std::valarray<int> &stride, const std::valarray<int> &edge) {
//         auto temp = new int[start.size()];
//         std::copy(begin(start), end(start), temp);
//         auto temp2 = new int[stride.size()];
//         std::copy(begin(stride), end(stride), temp2);
//         auto temp3 = new int[edge.size()];
//         std::copy(begin(edge), end(edge), temp3);
//         int size = edge[0];
//         for (int i = 1;i < var->rank; i++)
//             size *= edge[i];
//         pybind11::tuple data(size);
//         int ret;
//         switch (var->class_type) {
//             case MAT_C_DOUBLE:
//                 ret = var_read_data_internal<double>(mat, var, size, temp, temp2, temp3, data, nullptr);
//                 break;
//             case MAT_C_SINGLE:
//                 ret = var_read_data_internal<float>(mat, var, size, temp, temp2, temp3, data, nullptr);
//                 break;
//             case MAT_C_INT8:
//                 ret = var_read_data_internal<mat_int8_t>(mat, var, size, temp, temp2, temp3, data, nullptr);
//                 break;
//             case MAT_C_UINT8:
//                 ret = var_read_data_internal<mat_uint8_t>(mat, var, size, temp, temp2, temp3, data, nullptr);
//                 break;
//             case MAT_C_INT16:
//                 ret = var_read_data_internal<mat_int16_t>(mat, var, size, temp, temp2, temp3, data, nullptr);
//                 break;
//             case MAT_C_UINT16:
//                 ret = var_read_data_internal<mat_uint16_t>(mat, var, size, temp, temp2, temp3, data, nullptr);
//                 break;
//             case MAT_C_INT32:
//                 ret = var_read_data_internal<mat_int32_t>(mat, var, size, temp, temp2, temp3, data, nullptr);
//                 break;
//             case MAT_C_UINT32:
//                 ret = var_read_data_internal<mat_uint32_t>(mat, var, size, temp, temp2, temp3, data, nullptr);
//                 break;
//             case MAT_C_INT64:
//                 ret = var_read_data_internal<mat_int64_t>(mat, var, size, temp, temp2, temp3, data, nullptr);
//                 break;
//             case MAT_C_UINT64:
//                 ret = var_read_data_internal<mat_uint64_t>(mat, var, size, temp, temp2, temp3, data, nullptr);
//                 break;
//             default:
//                 break;
//         }
//         delete[] temp;
//         delete[] temp2;
//         delete[] temp3;
//         return std::pair(ret, data);
//     }

//     int var_delete(MatT *mat, const char *name) {
//         return Mat_VarDelete(mat, name);
//     }

//     std::valarray<const char *> *get_dir(MatT *mat) {
//         size_t cbSize;
//         auto p = Mat_GetDir(mat, &cbSize);
//         return new std::valarray<const char *>(p, cbSize);
//     }

//     const char *get_filename(MatT *mat) {
//         return Mat_GetFilename(mat);
//     }

//     MatFt get_version(MatT *mat) {
//         return static_cast<MatFt>(Mat_GetVersion(mat));
//     }

//     const char *get_header(MatT *mat) {
//         return Mat_GetHeader(mat);
//     }

//     const std::valarray<const char *> *MatT::get_dir() {
//         auto *temp = new std::valarray<const char *>;
//         char **p = dir;
//         int i = 0;
//         while(p && *p++)
//             temp[i++] = *p;
//         return temp;
//     }

//     MatAcc MatT::get_mode() {
//         return static_cast<MatAcc>(mode);
//     }

//     void MatT::set_mode(MatAcc value) {
//         mode = value;
//     }

//     void *MatVarT::get_data() {
//         return nullptr;
//     }

//     const pybind11::object &MatComplexSplitT::get_real() {
//         return (const pybind11::object &)Re;
//     }

//     void MatComplexSplitT::set_real(const pybind11::object &value) {
//         Re = (void *)&value;
//     }

//     const pybind11::object &MatComplexSplitT::get_imag() {
//         return (const pybind11::object &)Im;
//     }

//     void MatComplexSplitT::set_imag(const pybind11::object &imag) {
//         Im = (void *)&imag;
//     }
// }
