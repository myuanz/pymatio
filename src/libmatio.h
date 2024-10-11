// #ifndef PYMATIO_LIBMATIO_H
// #define PYMATIO_LIBMATIO_H
// // #include <pybind11/stl.h>
// // #include <pybind11/pybind11.h>
// // #include <pybind11/numpy.h>

// #include "matio_private.h"

// namespace matio {
//     /** @brief MAT file access types
//         *
//         * @ingroup MAT
//         * MAT file access types
//         */
//     enum MatAcc {
//         RDONLY = MAT_ACC_RDONLY,   /**< @brief Read only file access                */
//         RDWR   = MAT_ACC_RDWR      /**< @brief Read/Write file access               */
//     };

//     /** @brief Matlab MAT File information
//      * Contains information about a Matlab MAT file
//      * @ingroup MAT
//      */
//     class MatT: public mat_t {
//     public:
//         const std::valarray<const char *> *get_dir();
//         MatAcc get_mode();
//         void set_mode(MatAcc);
//     };

//     /** @brief MAT file versions
//      *
//      * @ingroup MAT
//      * MAT file versions
//      */
//     enum MatFt {
//         MAT7_3    = MAT_FT_MAT73,     /**< @brief Matlab version 7.3 file             */
//         MAT5      = MAT_FT_MAT5,      /**< @brief Matlab version 5 file               */
//         MAT4      = MAT_FT_MAT4,      /**< @brief Matlab version 4 file               */
//         UNDEFINED = MAT_FT_UNDEFINED  /**< @brief Undefined version                   */
//     };

//     /** @brief Complex data type using split storage
//      *
//      * Complex data type using split real/imaginary pointers
//      * @ingroup MAT
//      */
//     class MatComplexSplitT: public mat_complex_split_t {
//     public:
//         /**< Pointer to the real part */
//         const pybind11::object &get_real();
//         void set_real(const pybind11::object &);
//         /**< Pointer to the imaginary part */
//         const pybind11::object &get_imag();
//         void set_imag(const pybind11::object &);
//     };

//     /** @brief Matlab variable information
//      *
//      * Contains information about a Matlab variable
//      * @ingroup MAT
//      */
//     class MatVarT: public matvar_t {
//     public:
//         void *get_data();
//     };

//     /** @brief Matlab data types
//      *
//      * @ingroup MAT
//      * Matlab data types
//      */
//     enum MatioTypes {
//         T_UNKNOWN    = MAT_T_UNKNOWN,      /**< @brief UNKNOWN data type                   */
//         T_INT8       = MAT_T_INT8,         /**< @brief 8-bit signed integer data type      */
//         T_UINT8      = MAT_T_UINT8,        /**< @brief 8-bit unsigned integer data type    */
//         T_INT16      = MAT_T_INT16,        /**< @brief 16-bit signed integer data type     */
//         T_UINT16     = MAT_T_UINT16,       /**< @brief 16-bit unsigned integer data type   */
//         T_INT32      = MAT_T_INT32,        /**< @brief 32-bit signed integer data type     */
//         T_UINT32     = MAT_T_UINT32,       /**< @brief 32-bit unsigned integer data type   */
//         T_SINGLE     = MAT_T_SINGLE,       /**< @brief IEEE 754 single precision data type */
//         T_DOUBLE     = MAT_T_DOUBLE,       /**< @brief IEEE 754 double precision data type */
//         T_INT64      = MAT_T_INT64,        /**< @brief 64-bit signed integer data type     */
//         T_UINT64     = MAT_T_UINT64,       /**< @brief 64-bit unsigned integer data type   */
//         T_MATRIX     = MAT_T_MATRIX,       /**< @brief matrix data type                    */
//         T_COMPRESSED = MAT_T_COMPRESSED,   /**< @brief compressed data type                */
//         T_UTF8       = MAT_T_UTF8,         /**< @brief 8-bit Unicode text data type        */
//         T_UTF16      = MAT_T_UTF16,        /**< @brief 16-bit Unicode text data type       */
//         T_UTF32      = MAT_T_UTF32,        /**< @brief 32-bit Unicode text data type       */
//         T_STRING     = MAT_T_STRING,       /**< @brief String data type                    */
//         T_CELL       = MAT_T_CELL,         /**< @brief Cell array data type                */
//         T_STRUCT     = MAT_T_STRUCT,       /**< @brief Structure data type                 */
//         T_ARRAY      = MAT_T_ARRAY,        /**< @brief Array data type                     */
//         T_FUNCTION   = MAT_T_FUNCTION      /**< @brief Function data type                  */
//     };

//     /** @brief Matlab variable classes
//      *
//      * @ingroup MAT
//      * Matlab variable classes
//      */
//     enum MatioClasses {
//         C_EMPTY    = MAT_C_EMPTY,      /**< @brief Empty array                           */
//         C_CELL     = MAT_C_CELL,       /**< @brief Matlab cell array class               */
//         C_STRUCT   = MAT_C_STRUCT,     /**< @brief Matlab structure class                */
//         C_OBJECT   = MAT_C_OBJECT,     /**< @brief Matlab object class                   */
//         C_CHAR     = MAT_C_CHAR,       /**< @brief Matlab character array class          */
//         C_SPARSE   = MAT_C_SPARSE,     /**< @brief Matlab sparse array class             */
//         C_DOUBLE   = MAT_C_DOUBLE,     /**< @brief Matlab double-precision class         */
//         C_SINGLE   = MAT_C_SINGLE,     /**< @brief Matlab single-precision class         */
//         C_INT8     = MAT_C_INT8,       /**< @brief Matlab signed 8-bit integer class     */
//         C_UINT8    = MAT_C_UINT8,      /**< @brief Matlab unsigned 8-bit integer class   */
//         C_INT16    = MAT_C_INT16,      /**< @brief Matlab signed 16-bit integer class    */
//         C_UINT16   = MAT_C_UINT16,     /**< @brief Matlab unsigned 16-bit integer class  */
//         C_INT32    = MAT_C_INT32,      /**< @brief Matlab signed 32-bit integer class    */
//         C_UINT32   = MAT_C_UINT32,     /**< @brief Matlab unsigned 32-bit integer class  */
//         C_INT64    = MAT_C_INT64,      /**< @brief Matlab signed 64-bit integer class    */
//         C_UINT64   = MAT_C_UINT64,     /**< @brief Matlab unsigned 64-bit integer class  */
//         C_FUNCTION = MAT_C_FUNCTION,   /**< @brief Matlab function class                 */
//         C_OPAQUE   = MAT_C_OPAQUE      /**< @brief Matlab opaque class                   */
//     };

//     /** @brief MAT file compression options
//      *
//      * This option is only used on version 5 MAT files
//      * @ingroup MAT
//      */
//     enum MatioCompression {
//         NONE = MAT_COMPRESSION_NONE, /**< @brief No compression */
//         ZLIB = MAT_COMPRESSION_ZLIB  /**< @brief zlib compression */
//     };

//     /** @brief Matlab array flags
//      *
//      * @ingroup MAT
//      * Matlab array flags
//      */
//     enum MatioFlags {
//         COMPLEX        = MAT_F_COMPLEX,        /**< @brief Complex bit flag */
//         GLOBAL         = MAT_F_GLOBAL,         /**< @brief Global bit flag */
//         LOGICAL        = MAT_F_LOGICAL,        /**< @brief Logical bit flag */
//         DONT_COPY_DATA = MAT_F_DONT_COPY_DATA  /**< Don't copy data, use keep the pointer */
//     };

//     /** @brief Get the version of the library
//      *
//      * Gets the version number of the library
//      * @return major Pointer to store the library major version number
//      * @return minor Pointer to store the library minor version number
//      * @return release Pointer to store the library release version number
//      */
//     pybind11::tuple get_library_version();

//     /** @brief Initializes the logging system
//      *
//      * @ingroup mat_util
//      * @param prog_name Name of the program initializing the logging functions
//      * @return 0 on success
//      */
//     int log_init(const char *);

//     /** @brief Set debug parameter
//      *
//      *  Sets the debug level.  This value is used by
//      *  program to determine what information should be printed to the screen
//      *  @ingroup mat_util
//      *  @param d sets logging debug level
//      */
//     void set_debug(int);

//     /** @brief Logs a Critical message and returns to the user
//      *
//      * Logs a Critical message and returns to the user.  If the program should
//      * stop running, use @ref Mat_Error
//      * @ingroup mat_util
//      * @param msg formatted string
//      */
//     void critical(const char *);

//     /** @brief Log a message unless silent
//      *
//      * Logs the message unless the silent option is set (See @ref Mat_SetVerbose).
//      * To log a message based on the verbose level, use @ref Mat_VerbMessage
//      * @ingroup mat_util
//      * @param msg formatted message
//      */
//     void message(const char *);

//     /** @brief Prints a help string to stdout and exits with status EXIT_SUCCESS (typically 0)
//      *
//      * Prints the array of strings to stdout and exits with status EXIT_SUCCESS.
//      * @ingroup mat_util
//      * @param helpStr array of strings with NULL as its last element
//      */
//     void help(const std::valarray<std::string> &);

//     /** @brief Creates a new Matlab MAT file
//      *
//      * Tries to create a new Matlab MAT file with the given name and optional
//      * header string.  If no header string is given, the default string
//      * is used containing the software, version, and date in it.  If a header
//      * string is given, at most the first 116 characters is written to the file.
//      * The given header string need not be the full 116 characters, but MUST be
//      * NULL terminated.
//      * @ingroup MAT
//      * @param matName Name of MAT file to create
//      * @param hdrStr Optional header string, NULL to use default
//      * @param matFileVer MAT file version to create
//      * @return A pointer to the MAT file or NULL if it failed.  This is not a
//      * simple FILE * and should not be used as one.
//      */
//     MatT *create_ver(const char *, const char *, enum MatFt);

//     /** @brief Opens an existing Matlab MAT file
//      *
//      * Tries to open a Matlab MAT file with the given name
//      * @ingroup MAT
//      * @param matName Name of MAT file to open
//      * @param mode File access mode (MAT_ACC_RDONLY,MAT_ACC_RDWR,etc).
//      * @return A pointer to the MAT file or NULL if it failed.  This is not a
//      * simple FILE * and should not be used as one.
//      */
//     MatT *open(const char *filename, MatAcc mode);

//     /** @brief Closes an open Matlab MAT file
//      *
//      * Closes the given Matlab MAT file and frees any memory with it.
//      * @ingroup MAT
//      * @param mat Pointer to the MAT file
//      * @retval 0 on success
//      */
//     int close(MatT *);

//     /** @brief Reads the next variable in a MAT file
//      *
//      * Reads the next variable in the Matlab MAT file
//      * @ingroup MAT
//      * @param mat Pointer to the MAT file
//      * @return Pointer to the @ref MatVarT structure containing the MAT
//      * variable information
//      */
//     MatVarT *var_read_next(MatT *);

//     /** @brief Duplicates a MatVarT structure
//      *
//      * Provides a clean function for duplicating a MatVarT structure.
//      * @ingroup MAT
//      * @param in pointer to the MatVarT structure to be duplicated
//      * @param opt 0 does a shallow duplicate and only assigns the data pointer to
//      *            the duplicated array.  1 will do a deep duplicate and actually
//      *            duplicate the contents of the data.  Warning: If you do a shallow
//      *            copy and free both structures, the data will be freed twice and
//      *            memory will be corrupted.  This may be fixed in a later release.
//      * @returns Pointer to the duplicated MatVarT structure.
//      */
//     MatVarT *var_duplicate(const MatVarT *, int);

//     /** @brief Frees all the allocated memory associated with the structure
//      *
//      * Frees memory used by a MAT variable.  Frees the data associated with a
//      * MAT variable if it's non-NULL and MAT_F_DONT_COPY_DATA was not used.
//      * @ingroup MAT
//      * @param in Pointer to the MatVarT structure
//      */
//     void var_free(MatVarT *);

//     /** @brief Writes the given MAT variable to a MAT file
//      *
//      * Writes the MAT variable information stored in var to the given MAT file.
//      * The variable will be written to the end of the file.
//      * @ingroup MAT
//      * @param mat MAT file to write to
//      * @param var MAT variable information to write
//      * @param compress Whether or not to compress the data
//      *        (Only valid for version 5 and 7.3 MAT files and variables with numeric data)
//      * @retval 0 on success
//      */
//     int var_write(MatT *, MatVarT *, MatioCompression);

//     /** @brief Reads the information of a variable with the given name from a MAT file
//      *
//      * Reads the named variable (or the next variable if name is NULL) information
//      * (class,flags-complex/global/logical,rank,dimensions,and name) from the
//      * Matlab MAT file
//      * @ingroup MAT
//      * @param mat Pointer to the MAT file
//      * @param name Name of the variable to read
//      * @return Pointer to the @ref MatVarT structure containing the MAT
//      * variable information
//      */
//     MatVarT *var_read_info(MatT *, const char *name);

//     /** @brief Prints the variable information
//      *
//      * Prints to stdout the values of the @ref MatVarT structure
//      * @ingroup MAT
//      * @param var Pointer to the MatVarT structure
//      * @param print_data set to 1 if the Variables data should be printed, else 0
//      */
//     void var_print(MatVarT *, int);

//     /** @brief Calculate a set of subscript values from a single(linear) subscript
//      *
//      * Calculates 1-relative subscripts for each dimension given a 0-relative
//      * linear index.  Subscripts are calculated as follows where s is the array
//      * of dimension subscripts, D is the array of dimensions, and index is the
//      * linear index.
//      * \f[
//      *   s_k = \lfloor\frac{1}{L} \prod\limits_{l = 0}^{k} D_l\rfloor + 1
//      * \f]
//      * \f[
//      *   L = index - \sum\limits_{l = k}^{RANK - 1} s_k \prod\limits_{m = 0}^{k} D_m
//      * \f]
//      * @ingroup MAT
//      * @param rank Rank of the variable
//      * @param dims Dimensions of the variable
//      * @param index Linear index
//      * @return Array of dimension subscripts
//      */
//     std::valarray<size_t> *calc_subscripts2(int, const std::valarray<size_t> &, size_t);

//     /** @brief Calculate a single subscript from a set of subscript values
//      *
//      * Calculates a single linear subscript (0-relative) given a 1-relative
//      * subscript for each dimension.  The calculation uses the formula below where
//      * index is the linear index, s is an array of length RANK where each element
//      * is the subscript for the corresponding dimension, D is an array whose
//      * elements are the dimensions of the variable.
//      * \f[
//      *   index = \sum\limits_{k=0}^{RANK-1} [(s_k - 1) \prod\limits_{l=0}^{k} D_l ]
//      * \f]
//      * @ingroup MAT
//      * @param rank Rank of the variable
//      * @param dims Dimensions of the variable
//      * @param subs Array of dimension subscripts
//      * @return Single (linear) subscript
//      * @retval 0 on success
//      */
//     std::pair<int, size_t> calc_single_subscript2(int, const std::valarray<size_t> &, const std::valarray<size_t> &);

//     /** @brief Reads the variable with the given name from a MAT file
//      *
//      * Reads the next variable in the Matlab MAT file
//      * @ingroup MAT
//      * @param mat Pointer to the MAT file
//      * @param name Name of the variable to read
//      * @return Pointer to the @ref MatVarT structure containing the MAT
//      * variable information
//      */
//     MatVarT *var_read(MatT *, const char *);

//     /** @brief Creates a MAT Variable with the given name and (optionally) data
//      *
//      * Creates a MAT variable that can be written to a Matlab MAT file with the
//      * given name, data type, dimensions and data.  Rank should always be 2 or more.
//      * i.e. Scalar values would have rank=2 and dims[2] = {1,1}.  Data type is
//      * one of the MatioTypes.  MAT adds MatioTypes.STRUCT and MatioTypes.CELL to create
//      * Structures and Cell Arrays respectively.  For MatioTypes.STRUCT, data should be a
//      * NULL terminated array of MatVarT * variables (i.e. for a 3x2 structure with
//      * 10 fields, there should be 61 MatVarT * variables where the last one is
//      * NULL).  For cell arrays, the NULL termination isn't necessary.  So to create
//      * a cell array of size 3x2, data would be the address of an array of 6
//      * MatVarT * variables.
//      *
//      * EXAMPLE:
//      *   To create a struct of size 3x2 with 3 fields:
//      * @code
//      *     int rank=2, dims[2] = {3,2}, num_fields = 3;
//      *     MatVarT **vars;
//      *
//      *     vars = malloc((3*2*num_fields+1)*sizeof(MatVarT *));
//      *     vars[0]             = Mat_VarCreate(...);
//      *        :
//      *     vars[3*2*num_fields-1] = Mat_VarCreate(...);
//      *     vars[3*2*num_fields]   = NULL;
//      * @endcode
//      *
//      * EXAMPLE:
//      *   To create a cell array of size 3x2:
//      * @code
//      *     int rank=2, dims[2] = {3,2};
//      *     MatVarT **vars;
//      *
//      *     vars = malloc(3*2*sizeof(MatVarT *));
//      *     vars[0]             = Mat_VarCreate(...);
//      *        :
//      *     vars[5] = Mat_VarCreate(...);
//      * @endcode
//      *
//      * @ingroup MAT
//      * @param name Name of the variable to create
//      * @param class_type class type of the variable in Matlab(one of the mx Classes)
//      * @param data_type data type of the variable (one of the MAT_T_ Types)
//      * @param rank Rank of the variable
//      * @param dims array of dimensions of the variable of size rank
//      * @param data pointer to the data
//      * @param opt 0, or bitwise or of the following options:
//      * - MatioFlags.DONT_COPY_DATA to just use the pointer to the data and not copy the
//      *       data itself. Note that the pointer should not be freed until you are
//      *       done with the mat variable.  The var_free function will NOT free
//      *       data that was created with MatioFlags.DONT_COPY_DATA, so free it yourself.
//      * - MatioFlags.COMPLEX to specify that the data is complex.  The data variable
//      *       should be a pointer to a mat_complex_split_t type.
//      * - MatioFlags.GLOBAL to assign the variable as a global variable
//      * - MatioFlags.LOGICAL to specify that it is a logical variable
//      * @return A MAT variable that can be written to a file or otherwise used
//      */
//     MatVarT *var_create(const char *, MatioClasses, MatioTypes, int, const std::valarray<size_t> &, const pybind11::object &, int);

//     /** @brief Gets the file access mode of the given MAT file
//      *
//      * Gets the file access mode of the given MAT file
//      * @ingroup MAT
//      * @param mat Pointer to the MAT file
//      * @return MAT file access mode
//      */
//     MatAcc get_file_access_mode(MatT *);

//     /** @brief Writes/appends the given MAT variable to a version 7.3 MAT file
//      *
//      * Writes the numeric data of the MAT variable stored in var to the given
//      * MAT file. The variable will be written to the end of the file if it does
//      * not yet exist or appended to the existing variable.
//      * @ingroup MAT
//      * @param mat MAT file to write to
//      * @param var MAT variable information to write
//      * @param compress Whether or not to compress the data
//      *        (Only valid for version 7.3 MAT files and variables with numeric data)
//      * @param dim dimension to append data
//      *        (Only valid for version 7.3 MAT files and variables with numeric data)
//      * @retval 0 on success
//      */
//     int var_write_append(MatT *, MatVarT *, MatioCompression, int);

//     /** @brief Creates a structure MATLAB variable with the given name and fields
//      *
//      * @ingroup MAT
//      * @param name Name of the structure variable to create
//      * @param rank Rank of the variable
//      * @param dims array of dimensions of the variable of size rank
//      * @param fields Array of fieldnames
//      * @return Pointer to the new structure MATLAB variable on success, NULL on error
//      */
//     MatVarT *var_create_struct(const char *, int, const std::valarray<size_t> &, const std::valarray<std::string> &);

//     /** @brief Sets the structure field to the given variable
//      *
//      * Sets the specified structure fieldName at the given 0-relative @c index to
//      * @c field.
//      * @ingroup MAT
//      * @param var Pointer to the Structure MAT variable
//      * @param fieldName Name of the structure field
//      * @param index linear index of the structure array
//      * @param field New field variable
//      * @return Pointer to the previous field (NULL if no previous field)
//      */
//     MatVarT *var_set_struct_field_by_name(MatVarT *, const char *, size_t, MatVarT *);

//     /** @brief Sets the element of the cell array at the specific index
//      *
//      * Sets the element of the cell array at the given 0-relative index to @c cell.
//      * @ingroup MAT
//      * @param var Pointer to the cell array variable
//      * @param index 0-relative linear index of the cell to set
//      * @param cell Pointer to the cell to set
//      * @return Pointer to the previous cell element, or NULL if there was no
//      *          previous cell element or error.
//      */
//     MatVarT *var_set_cell(MatVarT *, int, MatVarT *);

//     /** @brief Sets the structure field to the given variable
//      *
//      * Sets the structure field specified by the 0-relative field index
//      * @c field_index for the given 0-relative structure index @c index to
//      * @c field.
//      * @ingroup MAT
//      * @param var Pointer to the structure MAT variable
//      * @param fieldIndex 0-relative index of the field.
//      * @param index linear index of the structure array
//      * @param field New field variable
//      * @return Pointer to the previous field (NULL if no previous field)
//      */
//     MatVarT *var_set_struct_field_by_index(MatVarT *, size_t, size_t, MatVarT *);

//     /** @brief Returns the number of fields in a structure variable
//      *
//      * Returns the number of fields in the given structure.
//      * @ingroup MAT
//      * @param var Structure matlab variable
//      * @returns Number of fields
//      */
//     unsigned int var_get_number_of_fields(MatVarT *);

//     /** @brief Returns the fieldnames of a structure variable
//      *
//      * Returns the fieldnames for the given structure.
//      * @ingroup MAT
//      * @param var Structure matlab variable
//      * @returns Array of fieldnames
//      */
//     std::valarray<const char *> *var_get_struct_field_names(MatVarT *);

//     /** @brief Adds a field to a structure
//      *
//      * Adds the given field to the structure. fields should be an array of MatVarT
//      * pointers of the same size as the structure (i.e. 1 field per structure
//      * element).
//      * @ingroup MAT
//      * @param var Pointer to the Structure MAT variable
//      * @param fieldName Name of field to be added
//      * @retval 0 on success
//      */
//     int var_add_struct_field(MatVarT *, const char *);

//     /** @brief Indexes a structure
//      *
//      * Finds structures of a structure array given a single (linear)start, stride,
//      * and edge.  The structures are placed in a new structure array.  If
//      * copy_fields is non-zero, the indexed structures are copied and should be
//      * freed, but if copy_fields is zero, the indexed structures are pointers to
//      * the original, but should still be freed since the mem_conserve flag is set
//      * so that the structures are not freed.
//      * MAT file version must be 5.
//      * @ingroup MAT
//      * @param var Structure matlab variable
//      * @param start starting index (0-relative)
//      * @param stride stride (1 reads consecutive elements)
//      * @param edge Number of elements to read
//      * @param copyFields 1 to copy the fields, 0 to just set pointers to them.
//      * @returns A new structure with fields indexed from var
//      */
//     MatVarT *var_get_structs_linear(MatVarT *, int, int, int, int);

//     /** @brief Indexes a structure
//      *
//      * Finds structures of a structure array given a start, stride, and edge for
//      * each dimension.  The structures are placed in a new structure array.  If
//      * copy_fields is non-zero, the indexed structures are copied and should be
//      * freed, but if copy_fields is zero, the indexed structures are pointers to
//      * the original, but should still be freed. The structures have a flag set
//      * so that the structure fields are not freed.
//      *
//      * Note that this function is limited to structure arrays with a rank less than
//      * 10.
//      *
//      * @ingroup MAT
//      * @param var Structure matlab variable
//      * @param start vector of length rank with 0-relative starting coordinates for
//      *              each dimension.
//      * @param stride vector of length rank with strides for each dimension.
//      * @param edge vector of length rank with the number of elements to read in
//      *              each dimension.
//      * @param copyFields 1 to copy the fields, 0 to just set pointers to them.
//      * @returns A new structure array with fields indexed from @c var.
//      */
//     MatVarT *var_get_structs(MatVarT *, const std::valarray<int> &, const std::valarray<int> &, const std::valarray<int> &, int);

//     /** @brief Indexes a cell array
//      *
//      * Finds cells of a cell array given a linear indexed start, stride, and edge.
//      * The cells are placed in a pointer array.  The cells themself should not
//      * be freed as they are part of the original cell array, but the pointer array
//      * should be.  If copies are needed, use Mat_VarDuplicate on each of the cells.
//      * MAT file version must be 5.
//      * @ingroup MAT
//      * @param var Cell Array matlab variable
//      * @param start starting index
//      * @param stride stride
//      * @param edge Number of cells to get
//      * @returns an array of pointers to the cells
//      */
//     MatVarT *var_get_cells_linear(MatVarT *, int, int, int);

//     /** @brief Indexes a cell array
//      *
//      * Finds cells of a cell array given a start, stride, and edge for each.
//      * dimension.  The cells are placed in a pointer array.  The cells should not
//      * be freed, but the array of pointers should be.  If copies are needed,
//      * use Mat_VarDuplicate on each cell.
//      *
//      * Note that this function is limited to structure arrays with a rank less than
//      * 10.
//      *
//      * @ingroup MAT
//      * @param var Cell Array matlab variable
//      * @param start vector of length rank with 0-relative starting coordinates for
//      *              each dimension.
//      * @param stride vector of length rank with strides for each dimension.
//      * @param edge vector of length rank with the number of elements to read in
//      *              each dimension.
//      * @returns an array of pointers to the cells
//      */
//     MatVarT *var_get_cells(MatVarT *, const std::valarray<int> &, const std::valarray<int> &, const std::valarray<int> &);

//     /** @brief Finds a field of a structure
//      *
//      * Returns a pointer to the structure field at the given 0-relative index.
//      * @ingroup MAT
//      * @param var Pointer to the Structure MAT variable
//      * @param nameOrIndex Name of the field, or the 1-relative index of the field
//      * @param index linear index of the structure to find the field of
//      * @return Pointer to the Structure Field on success, NULL on error
//      */
//     MatVarT *var_get_struct_field(MatVarT *, const pybind11::object &, int);

//     /** @brief Reads MAT variable data from a file
//      *
//      * Reads data from a MAT variable.  The variable must have been read by
//      * Mat_VarReadInfo.
//      * @ingroup MAT
//      * @param mat MAT file to read data from
//      * @param var MAT variable information
//      * @param start array of starting indices
//      * @param stride stride of data
//      * @param edge array specifying the number to read in each direction
//      * @return data to store data in (must be pre-allocated)
//      * @retval 0 on success
//      */
//     std::pair<int, pybind11::object> var_read_data(MatT *, MatVarT *, const std::valarray<int> &, const std::valarray<int> &, const std::valarray<int> &);

//     /** @brief Deletes a variable from a file
//      *
//      * @ingroup MAT
//      * @param mat Pointer to the mat_t file structure
//      * @param name Name of the variable to delete
//      * @returns 0 on success
//      */
//     int var_delete(MatT *, const char *);

//     /** @brief Gets a list of the variables of a MAT file
//      *
//      * Gets a list of the variables of a MAT file
//      * @ingroup MAT
//      * @param mat Pointer to the MAT file
//      * @param[out] n Number of variables in the given MAT file
//      * @return Array of variable names
//      */
//     std::valarray<const char *> *get_dir(MatT *);

//     /** @brief Gets the filename for the given MAT file
//      *
//      * Gets the filename for the given MAT file
//      * @ingroup MAT
//      * @param mat Pointer to the MAT file
//      * @return MAT filename
//      */
//     const char *get_filename(MatT *);

//     /** @brief Gets the version of the given MAT file
//      *
//      * Gets the version of the given MAT file
//      * @ingroup MAT
//      * @param mat Pointer to the MAT file
//      * @return MAT file version
//      */
//     MatFt get_version(MatT *);

//     /** @brief Gets the header for the given MAT file
//      *
//      * Gets the header for the given MAT file
//      * @ingroup MAT
//      * @param mat Pointer to the MAT file
//      * @return MAT header
//      */
//     const char *get_header(MatT *);
// }
// #endif //PYMATIO_LIBMATIO_H
