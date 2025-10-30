import os
import time
import pymatio as pm

# 获取测试数据目录
test_data_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'data', 'matio-matio_test_datasets')

# 测试文件列表
mat_files = [
    # MAT7.2及以下文件
    'matio_test_cases_uncompressed_le.mat',
    'matio_test_cases_uncompressed_be.mat',
    'matio_test_cases_compressed_le.mat',
    'matio_test_cases_compressed_be.mat',
    'small_v4_le.mat',
    'small_v4_be.mat',
    'matio_test_cases_v4_le.mat',
    'matio_test_cases_v4_be.mat',
    'large_struct_compressed_le.mat',
    'packed_field_name_uncompressed_le.mat',
    'packed_field_name_compressed_le.mat',
    # MAT7.3文件
    'matio_test_cases_uncompressed_hdf_le.mat',
    'matio_test_cases_compressed_hdf_le.mat',
    'matio_test_cases_hdf_be.mat',
    'struct_nullpad_class_name_hdf_le.mat',
]


print("测试每个文件的读取情况：")
print("="*60)

for mat_file in mat_files:
    file_path = os.path.join(test_data_dir, mat_file)
    
    if not os.path.exists(file_path):
        print(f"{mat_file:<60} 文件不存在")
        continue
        
    try:
        start_time = time.time()
        result = pm.loadmat(file_path)
        end_time = time.time()
        print(f"{mat_file:<60} 成功，用时: {((end_time - start_time) * 1000):.2f}ms")
    except Exception as e:
        print(f"{mat_file:<60} 失败: {e}")

print("="*60)