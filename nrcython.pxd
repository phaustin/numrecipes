from cpython.ref cimport PyObject

cdef extern from "nr3cython.h":
    cdef cppclass MatDoub:
        MatDoub() 
        MatDoub(PyObject*) except +
        int ownsdata
        int nrows() const
        int ncols() const
        double* operator[](const int i)
        MatDoub(int n, int m, double &a)
        PyObject* pyident

    cdef cppclass VecDoub:
        VecDoub() 
        VecDoub(PyObject*) except +
        int ownsdata
        int size() const
        double& operator[](const int i)
        VecDoub(int n, double &a)
        PyObject* pyident

    cdef PyObject* NRpyObject(VecDoub& nrvector)
    cdef PyObject* NRpyObject(MatDoub& nrmatrix)
