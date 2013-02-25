import numpy as np
cimport numpy as np
from nrcython cimport *
from nose.tools import assert_equal
from nose.tools import assert_not_equal

np.import_array()

def make_array():
    py_mat=np.arange(24,dtype=np.float64)
    py_mat=py_mat.reshape([3,8])
    return py_mat

def make_array32():
    py_mat=np.arange(24,dtype=np.float32)
    py_mat=py_mat.reshape([3,8])
    return py_mat

def test_pyobject_constructorI():
    py_mat=make_array()
    cdef MatDoub nr_mat = MatDoub(<PyObject*> py_mat)
    #
    # assignment operater creates new object
    #
    assert_not_equal(id(py_mat),id(<object> nr_mat.pyident))

def test_pyobject_constructorII():
    cdef MatDoub nr_mat=MatDoub(3,3,88.)
    py_mat=<object>  NRpyObject(nr_mat)
    assert_equal(id(py_mat),id(<object> nr_mat.pyident))
    
def test_NRpyObject_MatDoub():
    cdef MatDoub nr_mat=MatDoub(3,3,88.)
    py_mat=<object>  NRpyObject(nr_mat)
    assert_equal(py_mat[0,0],88.)

def test_NRpyObject_VecDoub():
    cdef VecDoub nr_vec = VecDoub(5,33.)
    cdef object py_vec=<object> NRpyObject(nr_vec)
    nr_vec[0]=77.
    assert_equal(py_vec[0],77.)
    
def test_pyobject_constructor32():
    py_mat=make_array32()
    cdef MatDoub nr_mat = MatDoub(<PyObject*> py_mat)
    






