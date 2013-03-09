from cpython.ref cimport PyObject

cdef extern from "nr3cython.h":
    cdef cppclass NRmatrix[T]:
        NRmatrix() 
        NRmatrix(PyObject*) except +
        int ownsdata
        int nrows() const
        int ncols() const
        T* operator[](const int i)
        NRmatrix(int n, int m, T &a)
        PyObject* pyident

    cdef cppclass NRvector[T]:
        NRvector() 
        NRvector(PyObject*) except +
        int ownsdata
        int size() const
        T& operator[](const int i)
        NRvector(int n, T &a)
        PyObject* pyident

    cdef PyObject* NRpyObject(NRvector[double]& nrvector)
    cdef PyObject* NRpyObject(NRmatrix[double]& nrmatrix)

ctypedef NRvector[double] VecDoub
ctypedef NRmatrix[double] MatDoub
