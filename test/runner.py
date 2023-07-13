import sys
import subprocess
from enum import Enum
import numpy as np
import numpy.typing as npt


class Result(Enum):
    SUCCESS = 0
    FAILURE = 1


def evaluate(data_p: npt.NDArray, data_c: npt.NDArray) -> Result:
    result = Result.SUCCESS
    if data_p.ndim != data_c.ndim:
        print(f"ndim mismatch: {data_p.ndim} {data_c.ndim}")
        result = Result.FAILURE
    if data_p.shape != data_c.shape:
        print(f"shape mismatch: {data_p.shape} {data_c.shape}")
        result = Result.FAILURE
    if data_p.dtype != data_c.dtype:
        print(f"dtype mismatch: {data_p.dtype} {data_c.dtype}")
        result = Result.FAILURE
    if not np.array_equal(data_p, data_c):
        print(f"array mismatch: {data_p} {data_c}")
        result = Result.FAILURE
    return result


def main(ndim: int, shape: npt.NDArray, dtype: str) -> Result:
    if "|b1" == dtype:
        data_p = np.random.random_sample(shape)
        data_p = np.array(data_p < 0.5, dtype=dtype)
    elif "<i4" == dtype:
        data_p = np.random.randint(
                low=-1024,
                high=1024,
                size=shape,
                dtype=dtype
        )
    elif "<u8" == dtype:
        data_p = np.random.randint(
                low=0,
                high=1024,
                size=shape,
                dtype=dtype
        )
    elif "<f8" == dtype:
        data_p = np.array(np.random.random_sample(shape), dtype=dtype)
    elif "<c16" == dtype:
        data0 = np.array(np.random.random_sample(shape))
        data1 = np.array(np.random.random_sample(shape))
        data_p = np.array(data0 + 1j * data1, dtype=dtype)
    else:
        print("data type not implemented")
        sys.exit(1)
    pfname = "array-from-p.npy"
    cfname = "array-from-c.npy"
    dsize = data_p.itemsize if 0 == ndim else data_p[0].itemsize
    np.save(pfname, data_p, allow_pickle=False)
    retval = subprocess.call(f"./a.out {pfname} {cfname} {dsize}", shell=True)
    if 0 != retval:
        print("snpyio failed")
        return Result.FAILURE
    else:
        data_c = np.load(cfname)
        return evaluate(data_p, data_c)


if __name__ == "__main__":
    np.random.seed(1)
    ndims = [0, 1, 2, 3]
    dtypes = ["|b1", "<i4", "<u8", "<f8", "<c16"]
    ntests = 100
    nsuccess = 0
    for cntr in range(ntests):
        ndim = np.random.choice(ndims)
        shape = np.random.randint(low=1, high=65, size=ndim, dtype=int)
        dtype = np.random.choice(dtypes)
        if Result.SUCCESS == main(ndim, shape, dtype):
            print(f"success: ndim: {ndim} shape: {shape} dtype: {dtype}")
            nsuccess += 1
        else:
            print(f"failure: ndim: {ndim} shape: {shape} dtype: {dtype}")
    print("{} test, {} success".format(ntests, nsuccess))
    if nsuccess == ntests:
        exit(0)
    else:
        exit(1)

