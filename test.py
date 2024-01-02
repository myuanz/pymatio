# %%
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

pm.close(mat)
exit(0)

# %%
mat2 = pm.open('test.mat', pm.MatAcc.RDONLY)
print(mat2)
print(pm.get_file_access_mode(mat2))
print(pm.var_read_next(mat))
print(pm.close(mat2))

dims = (256, 256, 124,)
subs = pm.calc_subscripts2(3, dims, 18921 - 1)
pm.message('subs: %s' % subs)
error, index = pm.calc_single_subscript2(3, dims, subs)
pm.message('error, index: %d, %d' % (error, index,))
dims = [5, 10]
l8 = [i % 2 for i in range(0,50)]
var = pm.var_create('l8', pm.MatioClasses.UINT64, pm.MatioTypes.UINT64, 2, dims, l8, pm.MatioFlags.LOGICAL)
print(var)
pm.var_free(var)
dims[0] = 0
var = pm.var_create('l0', pm.MatioClasses.UINT8, pm.MatioTypes.UINT8, 2, dims, None, pm.MatioFlags.LOGICAL)
pm.var_print(var, 1)
pm.var_free(var)
cell = pm.var_create('var1', pm.MatioClasses.CELL, pm.MatioTypes.CELL, 2, (1, 3,), None, 0)
struct = pm.var_create_struct('a', 2, (2,1,), ('field1', 'field2',))
str_data = "aA1[bB2{cC3]dD4}eE5\\fF6|gG7;hH8:iI9'jJ0\"kK!,lL@<mM#.nN$>oO%/pP^?qQ& rR* sS( tT) uU- vV_ wW= xX+ yY` zZ~ "
var = pm.var_create('field2', pm.MatioClasses.CHAR, pm.MatioTypes.UTF8, 2, (4, 26,), str_data, 0)
pm.var_set_struct_field_by_name(struct, 'field2', 1, var)
pm.var_add_struct_field(struct, 'test')
pm.var_set_cell(cell, 1, struct)
pm.var_print(cell, 1)
print(pm.var_get_number_of_fields(struct), pm.var_get_struct_field_names(struct))
var2 = pm.var_get_structs_linear(cell, 0, 1, 3, 0)
pm.var_print(var2, 1)
var3 = pm.var_get_cells_linear(cell, 0, 1, 1)
pm.var_print(var3, 1)

pm.var_write(mat, var, pm.MatioCompression.NONE)
pm.var_write(mat, cell, pm.MatioCompression.NONE)

