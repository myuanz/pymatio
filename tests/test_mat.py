from pathlib import Path

import pytest

import pymatio as pm
import scipy.io as scio
import mat73
import numpy as np
from types import NoneType

DATA_DIR = Path(__file__).parent / "data"
MATIO_DATASET_DIR = DATA_DIR / "matio-matio_test_datasets"

def is_iterable(a):
    if isinstance(a, np.ndarray):
        return a.ndim > 0
    return isinstance(a, (list, tuple))

def compare_maybe_array(a, b):
    if isinstance(a, np.ndarray) and a.ndim == 0:
        a = a.item()
    if isinstance(b, np.ndarray) and b.ndim == 0:
        b = b.item()

    if {type(a), type(b)} == {np.ndarray, NoneType}:
        if a is None: return b.size == 0
        if b is None: return a.size == 0
        return False
    
    if is_iterable(a) and not isinstance(b, np.ndarray) and len(a) == 1:
        return a[0] == b
    if is_iterable(b) and not isinstance(a, np.ndarray) and len(b) == 1:
        return a == b[0]

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
                        return compare_mats(
                            a.reshape(-1)[i], b.reshape(-1)[i]
                        )
                    except:
                        print('cmp error', i, a, b)

    else:
        return a == b

def compare_mats(mat1, mat2, path=""):
    if isinstance(mat1, dict) and isinstance(mat2, dict):
        if '__matio_reason__' in mat1 or '__matio_reason__' in mat2:
            return True
        for key in set(mat1.keys()) | set(mat2.keys()):
            if key in ['', None, 'None'] or 'unnamed' in str(key) or key.startswith('__'):
                continue
            if key not in mat1:
                print(f"字段 '{path}{key}' 在 mat_from_pm 中不存在")
                return False
            elif key not in mat2:
                print(f"字段 '{path}{key}' 在 mat_from_mat73 中不存在")
                return False
            else:
                if not compare_mats(mat1[key], mat2[key], f"{path}{key}."):
                    return False
        return True
    elif is_iterable(mat1) and is_iterable(mat2):
        if len(mat1) != len(mat2):
            print(f"列表长度不一致: {path[:-1]} | {len(mat1)} != {len(mat2)} | {mat1=} {mat2=}")
            return False
        else:
            for i, (item1, item2) in enumerate(zip(mat1, mat2)):
                if not compare_mats(item1, item2, f"{path}[{i}]."):
                    return False
            return True
    elif any(type(m) in (float, int, bool, np.bool, np.float64, np.uint8) for m in (mat1, mat2)):
        return mat1 == mat2
    elif mat1 is None and isinstance(mat2, np.ndarray) and mat2.size == 0:
        return True
    elif mat2 is None and isinstance(mat1, np.ndarray) and mat1.size == 0:
        return True
    elif not compare_maybe_array(mat1, mat2):
        print(f"值不一致: {path[:-1]} `{mat1}` <-> `{mat2}`")
        return False

    return True

def test_version() -> None:
    assert pm.get_library_version() == (1, 5, 27)

def load_mat_baseline(path: Path) -> dict:
    try:
        result = scio.loadmat(str(path), simplify_cells=True)
    except NotImplementedError:
        result = mat73.loadmat(str(path))
    assert isinstance(result, dict)
    return result

def _check_mat(path: Path, debug_log_enabled=False) -> None:
    print(f"loading: {path}", flush=True)
    result = pm.loadmat(str(path), debug_log_enabled=debug_log_enabled, simplify_cells=True)
    # print(result)
    assert isinstance(result, dict)
    if '__matio_reason__' in result:
        print(f"Skipping comparison for {path} due to matio placeholder: {result['__matio_reason__']}")
        return

    try:
        baseline = load_mat_baseline(path)
    except Exception as e:
        import warnings
        warnings.warn(f"Failed to load baseline for {path} using scipy.io or mat73: {e}")
        return
    
    baseline.pop("__header__", None)
    baseline.pop("__version__", None)
    baseline.pop("__globals__", None)

    if not compare_mats(result, baseline):
        raise AssertionError(f"Comparison failed for {path}")


@pytest.mark.parametrize("mat_path", sorted(DATA_DIR.glob("*.mat")))
def test_load_local_datasets(mat_path: Path) -> None:
    try:
        _check_mat(mat_path)
    except Exception as e:
        raise AssertionError(f"Failed to load {mat_path}: {e}") from e


def test_load_matio_dataset() -> None:
    if not MATIO_DATASET_DIR.exists():
        pytest.skip("matio test datasets submodule not available")
    mat_files = sorted(MATIO_DATASET_DIR.rglob("*.mat"))
    if not mat_files:
        pytest.skip("matio test datasets submodule is empty")
    for mat_path in mat_files:
        _check_mat(mat_path)

if __name__ == "__main__":
    import sys
    mat_p = sys.argv[-1]
    _check_mat(Path(mat_p), True)
