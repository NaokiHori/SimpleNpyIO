import sys
import numpy as np


def create_0d():
    # create 0d array with data type float64
    dtype = np.float64
    data = 3.1415
    return data

def create_1d():
    # create 1d array with data type bool
    dtype = bool
    nx = 1024
    data = np.random.randint(low=0, high=2, size=nx, dtype=dtype)
    return data

def create_2d():
    # create 2d array with data type complex128
    dtype = np.complex128
    nx = 13
    ny = 7
    data = np.random.rand( ny, nx).astype(dtype)
    return data

def create_3d():
    # create 3d array with data type int32
    dtype = np.int32
    nx = 9
    ny = 12
    nz = 3
    data = np.arange(start=0, stop=nx*ny*nz, step=1, dtype=dtype)
    data = np.reshape(a=data, newshape=(nz, ny, nx), order="C")
    return data

if __name__ == "__main__":
    np.random.seed(1)
    np.save(file="npyfiles/created-by-python-0d.npy", arr=create_0d(), allow_pickle=True, fix_imports=True)
    np.save(file="npyfiles/created-by-python-1d.npy", arr=create_1d(), allow_pickle=True, fix_imports=True)
    np.save(file="npyfiles/created-by-python-2d.npy", arr=create_2d(), allow_pickle=True, fix_imports=True)
    np.save(file="npyfiles/created-by-python-3d.npy", arr=create_3d(), allow_pickle=True, fix_imports=True)
    sys.exit(0)
