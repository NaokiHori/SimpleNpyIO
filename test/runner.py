import sys
import subprocess
import numpy as np


def evaluate(data_p, data_c):
    val = 0
    val += 0 if data_p.ndim == data_c.ndim else 1
    val += 0 if data_p.shape == data_c.shape else 1
    val += 0 if data_p.dtype == data_c.dtype else 1
    val += 0 if np.array_equal(data_p, data_c) else 1
    return True if val == 0 else False


def main(f, cntr, ndim, shape, dtype):
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
        success = False
    else:
        data_c = np.load(cfname)
        success = evaluate(data_p, data_c)
    cntr = f"``{cntr}``"
    ndim = f"``{ndim}``"
    shape = f"``{shape}``"
    dtype = f"``'{dtype}'``"
    result = "``PASSED``" if success else "``FAILED``"
    f.write(f"   * - {cntr}\n")
    f.write(f"     - {ndim}\n")
    f.write(f"     - {shape}\n")
    f.write(f"     - {dtype}\n")
    f.write(f"     - {result}\n")
    return 1 if success else 0


if __name__ == "__main__":
    np.random.seed(1)
    ndims = [0, 1, 2, 3]
    dtypes = ["|b1", "<i4", "<u8", "<f8", "<c16"]
    ntests = 100
    nsuccess = 0
    with open("test-result.rst", "w") as f:
        f.write(".. list-table:: Test cases and results\n")
        f.write("   :header-rows: 1\n\n")
        f.write("   * - Case\n")
        f.write("     - ndim\n")
        f.write("     - shape\n")
        f.write("     - dtype\n")
        f.write("     - result\n")
        for cntr in range(ntests):
            ndim = np.random.choice(ndims)
            shape = np.random.randint(low=1, high=65, size=ndim, dtype=int)
            dtype = np.random.choice(dtypes)
            nsuccess += main(f, cntr, ndim, shape, dtype)
    print("{} test, {} success".format(ntests, nsuccess))
    color = "green" if nsuccess == ntests else "red"
    url = (
            f"https://img.shields.io/"
            f"badge/Test-{ntests}tests,"
            f"{nsuccess}success-{color}.svg"
    )
    with open("link.txt", "w") as f:
        f.write(url)
