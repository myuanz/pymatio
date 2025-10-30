import os
import time
import pytest
import pymatio as pm

# 尝试导入scipy和mat73
try:
    from scipy.io import loadmat as scipy_loadmat
    scipy_available = True
except ImportError:
    scipy_available = False


try:
    import mat73
    mat73_available = True
except ImportError:
    mat73_available = False


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


def test_performance_comparison():
    """测试pymatio与scipy和mat73的性能对比"""
    # 统计各库的总用时
    pymatio_total_time = 0.0
    scipy_total_time = 0.0
    mat73_total_time = 0.0
    
    # 统计各库成功读取的文件数
    pymatio_success = 0
    scipy_success = 0
    mat73_success = 0
    
    print("\n" + "="*60)
    print("性能测试结果对比")
    print("="*60)
    print(f"{'文件':<60} {'pymatio(ms)':>15} {'scipy(ms)':>15} {'mat73(ms)':>15}")
    print("-"*115)
    
    for mat_file in mat_files:
        file_path = os.path.join(test_data_dir, mat_file)
        
        if not os.path.exists(file_path):
            print(f"{mat_file:<60} {'文件不存在':>15} {'文件不存在':>15} {'文件不存在':>15}")
            continue
            
        # 测试pymatio
        try:
            start_time = time.time()
            pm_result = pm.loadmat(file_path, simplify_cells=True)
            pymatio_time = (time.time() - start_time) * 1000
            pymatio_total_time += pymatio_time
            pymatio_success += 1
        except Exception as e:
            pymatio_time = "失败"
            
        # 测试scipy
        try:
            if not scipy_available:
                scipy_time = "未安装"
            else:
                start_time = time.time()
                scipy_result = scipy_loadmat(file_path)
                scipy_time = (time.time() - start_time) * 1000
                scipy_total_time += scipy_time
                scipy_success += 1
        except Exception as e:
            scipy_time = "失败"
            
        # 测试mat73
        try:
            if not mat73_available:
                mat73_time = "未安装"
            else:
                start_time = time.time()
                mat73_result = mat73.loadmat(file_path)
                mat73_time = (time.time() - start_time) * 1000
                mat73_total_time += mat73_time
                mat73_success += 1
        except Exception as e:
            mat73_time = "失败"
            
        # 打印结果
        print(f"{mat_file:<60} {pymatio_time:>15.2f} {scipy_time:>15} {mat73_time:>15}")
    
    print("-"*115)
    print(f"{'总计':<60} {pymatio_total_time:>15.2f} {scipy_total_time:>15.2f} {mat73_total_time:>15.2f}")
    print(f"{'成功读取数':<60} {pymatio_success:>15} {scipy_success:>15} {mat73_success:>15}")
    print("="*60)
    
    # 验证至少有一个文件成功读取
    assert pymatio_success > 0, "pymatio没有成功读取任何文件"
    
    # 对于scipy和mat73，如果安装了，也应该至少成功读取一个文件
    if scipy_available:
        assert scipy_success > 0, "scipy没有成功读取任何文件"
    
    if mat73_available:
        assert mat73_success > 0, "mat73没有成功读取任何文件"


if __name__ == "__main__":
    test_performance_comparison()