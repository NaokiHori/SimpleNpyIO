import numpy as np


def test_0d():
    data_p = np.load("npyfiles/created-by-c-0d.npy")
    data_c = np.load("npyfiles/created-by-python-0d.npy")
    assert np.array_equal(data_p, data_c) == True

def test_1d():
    data_p = np.load("npyfiles/created-by-c-1d.npy")
    data_c = np.load("npyfiles/created-by-python-1d.npy")
    assert np.array_equal(data_p, data_c) == True

def test_2d():
    data_p = np.load("npyfiles/created-by-c-2d.npy")
    data_c = np.load("npyfiles/created-by-python-2d.npy")
    assert np.array_equal(data_p, data_c) == True

def test_3d():
    data_p = np.load("npyfiles/created-by-c-3d.npy")
    data_c = np.load("npyfiles/created-by-python-3d.npy")
    assert np.array_equal(data_p, data_c) == True

