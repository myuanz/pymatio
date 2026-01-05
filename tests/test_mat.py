from pathlib import Path

import pytest

import pymatio as pm
import scipy.io as scio
import mat73
import numpy as np
from types import NoneType

DATA_DIR = Path(__file__).parent / "data"
MATIO_DATASET_DIR = DATA_DIR / "matio-matio_test_datasets"

HDF5_MAGIC = b"\x89HDF\r\n\x1a\n"

def is_mat73_file(path: Path) -> bool:
    with path.open("rb") as f:
        if f.read(8) == HDF5_MAGIC:
            return True
        f.seek(512)
        return f.read(8) == HDF5_MAGIC

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

    if {type(a), type(b)} == {np.ndarray, str}:
        if isinstance(a, np.ndarray):
            s, arr = b, a
        else:
            s, arr = a, b
        assert arr.dtype.char == 'S' or arr.dtype.char == 'U', f"Cannot compare array of dtype {arr.dtype} with string"
        arr_str = ''.join([x.decode('utf-8') if isinstance(x, bytes) else str(x) for x in arr.reshape(-1)])
        return arr_str == s

    if is_iterable(a) and not isinstance(b, np.ndarray) and len(a) == 1:
        return a[0] == b
    if is_iterable(b) and not isinstance(a, np.ndarray) and len(b) == 1:
        return a == b[0]

    if isinstance(a, np.ndarray) and isinstance(b, np.ndarray):
        # print('cmp', a, b)
        if a.dtype == np.bool_ and b.dtype == np.uint8:
            return np.array_equal(a.reshape(-1), b.astype(np.bool_).reshape(-1))
        if a.dtype == np.uint8 and b.dtype == np.bool_:
            return np.array_equal(a.astype(np.bool_).reshape(-1), b.reshape(-1))

        if a.dtype == object or b.dtype == object:
            if a.size != b.size:
                return False
            for i, (x, y) in enumerate(zip(a.reshape(-1), b.reshape(-1))):
                if not compare_mats(x, y, f"[{i}]."):
                    return False
            return True

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
    for m in (mat1, mat2):
        if isinstance(m, dict) and '__matio_reason__' in m:
            return True

    types = {type(mat1), type(mat2)}

    # print(f"{mat1=} | {mat2=}")
    mat1 = squeeze(mat1)
    mat2 = squeeze(mat2)
    types = {type(mat1), type(mat2)}
    print(f"After squeeze: {mat1=} | {mat2=}")

    # print(type(mat1), mat1.items())
    if types == {np.ndarray}:
        return compare_maybe_array(mat1, mat2)

    if types == {list, np.ndarray}:
        if isinstance(mat1, np.ndarray):
            lst, arr = mat2, mat1
        else:
            lst, arr = mat1, mat2

        seq1 = arr
        if len(seq1) != len(lst):
            print(f"列表长度不一致: {path[:-1]} | {len(seq1)} != {len(lst)} | {arr=} {lst=}")
            return False
        for i, (item1, item2) in enumerate(zip(seq1, lst)):
            # print(f"\tComparing list/array item {i} at {path}[{i}] | {item1=} | {item2=}")
            if not compare_mats(item1, item2, f"{path}[{i}]."):
                return False
        return True

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
                print(f"comp '{path}{key}'")
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

def load_mat5_baseline(path: Path, simplify_cells: bool) -> dict:
    result = scio.loadmat(str(path), simplify_cells=simplify_cells)
    assert isinstance(result, dict)
    return result

def load_mat73_baseline(path: Path) -> dict:
    result = mat73.loadmat(str(path))
    assert isinstance(result, dict)
    return result

def squeeze(obj):
    if isinstance(obj, np.void) and obj.dtype.fields is not None:
        return {k: squeeze(obj[k]) for k in obj.dtype.fields}
    if isinstance(obj, np.ndarray) and obj.dtype.names is not None and obj.ndim == 1:
        return dict(obj)

    if isinstance(obj, np.ndarray) and obj.dtype.fields is not None:
        flat = obj.reshape(-1)
        if flat.size == 1:
            return squeeze(flat[0])
        return [squeeze(x) for x in flat]

    if hasattr(obj, "_fieldnames") and isinstance(getattr(obj, "_fieldnames"), (list, tuple)):
        return {k: squeeze(getattr(obj, k)) for k in obj._fieldnames}

    if isinstance(obj, (np.ndarray, list, tuple)):
        if isinstance(obj, np.ndarray):
            if obj.ndim == 0:
                return obj.item()
            if obj.dtype.kind in ("S", "U"):
                flat = obj.reshape(-1)
                chars_per_elem = obj.dtype.itemsize // 4 if obj.dtype.kind == "U" else obj.dtype.itemsize
                if chars_per_elem == 1:
                    return "".join([x.decode("utf-8") if isinstance(x, bytes) else str(x) for x in flat])
                return [x.decode("utf-8") if isinstance(x, bytes) else str(x) for x in flat]

        if len(obj) == 1:
            return squeeze(obj[0])
        elif len(obj) == 0:
            return None
    return obj

def _check_mat(path: Path, debug_log_enabled=False) -> None:
    print(f"loading: {path}", flush=True)

    if is_mat73_file(path):
        result = pm.loadmat(str(path), debug_log_enabled=debug_log_enabled, simplify_cells=True)
        assert isinstance(result, dict)
        baseline = load_mat73_baseline(path)
        baseline.pop("__header__", None)
        baseline.pop("__version__", None)
        baseline.pop("__globals__", None)
        # print(f'{result=}\n{baseline=}')


        if not compare_mats(result, baseline):
            raise AssertionError(f"Comparison failed for {path}")
        return
    else:
        for simplify_cells in (True, False):
            result = pm.loadmat(str(path), debug_log_enabled=debug_log_enabled, simplify_cells=simplify_cells)
            assert isinstance(result, dict)

            baseline = load_mat5_baseline(path, simplify_cells=simplify_cells)
            baseline.pop("__header__", None)
            baseline.pop("__version__", None)
            baseline.pop("__globals__", None)
            # print(f'{simplify_cells=}\n{result=}\n{baseline=}')

            if not compare_mats(result, baseline):
                raise AssertionError(f"Comparison failed for {path} (simplify_cells={simplify_cells})")


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
