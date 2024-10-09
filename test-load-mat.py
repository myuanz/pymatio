# %%
from types import NoneType
import pymatio as pm
import mat73
import numpy as np
import os
# %%
os.system("clear")

def compare_maybe_array(a, b):
    if {type(a), type(b)} == {np.ndarray, NoneType}:
        if a is None: return b.size == 0
        if b is None: return a.size == 0
        return False

    if isinstance(a, np.ndarray) and isinstance(b, np.ndarray):
        # print('cmp', a, b)
        try:
            return np.array_equal(a.reshape(-1), b.reshape(-1))
        except Exception as e:
            if a.size != b.size or a.dtype != b.dtype:
                # print(f"形状不一致: {a.shape} - {b.shape}")
                return False
            else:
                for i in range(a.size):
                    try:
                        eq = compare_mats(
                            a.reshape(-1)[i], b.reshape(-1)[i]
                        )
                        if not eq: return False
                    except:
                        print('cmp error', i, a, b)

    else:
        return a == b

def compare_mats(mat1, mat2, path=""):
    if isinstance(mat1, dict) and isinstance(mat2, dict):
        for key in set(mat1.keys()) | set(mat2.keys()):
            if key not in mat1:
                print(f"字段 '{path}{key}' 在 mat_from_pm 中不存在")
            elif key not in mat2:
                print(f"字段 '{path}{key}' 在 mat_from_mat73 中不存在")
            else:
                compare_mats(mat1[key], mat2[key], f"{path}{key}.")
    elif isinstance(mat1, (list, tuple, np.ndarray)) and isinstance(mat2, (list, tuple, np.ndarray)):
        if len(mat1) != len(mat2):
            print(f"列表长度不一致: {path[:-1]}")
        else:
            for i, (item1, item2) in enumerate(zip(mat1, mat2)):
                compare_mats(item1, item2, f"{path}[{i}].")
    elif not isinstance(mat1, type(mat2)):
        if any(type(m) in (float, int, bool, np.bool, np.float64, np.uint8) for m in (mat1, mat2)):
            return mat1 == mat2
        elif mat1 is None and isinstance(mat2, np.ndarray) and mat2.size == 0:
            return True
        elif mat2 is None and isinstance(mat1, np.ndarray) and mat1.size == 0:
            return True
        else:
            print(f"类型不一致: {path[:-1]}, `{mat1}` <-> `{mat2}` | {type(mat1)} - {type(mat2)}")
    elif not compare_maybe_array(mat1, mat2):
        print(f"值不一致: {path[:-1]} `{mat1}` <-> `{mat2}`")
        
        
p = '/home/myuan/C052-inj/C052-217-P0.ntp'
# p = '/mnt/90-connectome/test_data.mat'
# p = '/mnt/90-connectome/test_data_73.mat'
mat_from_pm = pm.loadmat(p, simplify_cells=True)

try:
    mat_from_mat73 = mat73.loadmat(p)
    # mat_from_mat73 = hdf5storage.loadmat(p, simplify_cells=True, chars_as_strings=True)

except Exception as e:
    import scipy.io as scio
    mat_from_mat73 = (scio.loadmat(p, simplify_cells=True, chars_as_strings=True))

compare_mats(mat_from_pm, mat_from_mat73)
# print(mat_from_pm['testStruct']['cellArrayMul'])
# print(mat_from_mat73['testStruct']['cellArrayMul'])
# print(mat_from_pm, mat_from_mat73, sep='\n\n')
# %%
# %%
def show_arr_and_flags(arr):
    # print(arr)
    print(arr.flags)
    print(arr.strides)
    print(arr.shape)
    print('-'*40)

# show_arr_and_flags(mat_from_pm['testStruct']['cellArrayMul'])
# show_arr_and_flags(mat_from_mat73['testStruct']['cellArrayMul'])
# %%
