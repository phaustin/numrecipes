/* nr3cython.h */
// based on nr3python.h version 0.4 -- modified
// to work with cython per
// https://github.com/phaustin/numrecipes
//  orig comment
// This file is a version of nr3.h with hooks that
// make it easy to interface to Python
// See http://www.nr.com/nr3_python_tutorial.html
#ifndef _NR3_H_
#define _NR3_H_
#ifndef Py_PYTHON_H
#include "Python.h"
#endif /* Py_PYTHON_H */
#ifndef Py_ARRAYOBJECT_H
#include "numpy/arrayobject.h"
#endif  /* Py_ARRAYOBJECT_H */

#define _CHECKBOUNDS_ 1
#define _TURNONFPES_ 1

// all the system #include's we'll ever need
#include <fstream>
#include <cmath>
#include <stdexcept>
#include <complex>
#include <iostream> 
#include <iomanip>
#include <vector>
#include <limits> 
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

using namespace std;

// basic type names (redefine if your bit lengths don't match)

typedef int Int; // 32 bit integer
typedef unsigned int Uint;

#ifdef _MSC_VER
typedef __int64 Llong; // 64 bit integer
typedef unsigned __int64 Ullong;
#else
typedef long long int Llong; // 64 bit integer
typedef unsigned long long int Ullong;
#endif

typedef char Char; // 8 bit integer
typedef unsigned char Uchar;

typedef double Doub; // default floating type
typedef long double Ldoub;

typedef complex<double> Complex; // default complex type

typedef bool Bool;


// NaN: uncomment one of the following 3 methods of defining a global NaN
// you can test by verifying that (NaN != NaN) is true

static const double NaN = numeric_limits<double>::quiet_NaN();

//Uint proto_nan[2]={0xffffffff, 0x7fffffff};
//double NaN = *( double* )proto_nan;

//Doub NaN = sqrt(-1.);

// templated check of a PyObject's type (used in initpyvec and initpymat below)
template <class T> inline int NRpyTypeOK(PyObject *a) {return 0;}
template <> inline int NRpyTypeOK<double>(PyObject *a) {
      return  PyArray_TYPE(a) == NPY_DOUBLE;
}

template <> inline int NRpyTypeOK<int>(PyObject *a) {
      return  PyArray_TYPE(a) == NPY_INT32;
}



// macro-like inline functions

template<class T>
inline T SQR(const T a) {return a*a;}

//NR_MAX, NR_MIN redefined from nr3python.h because
//linux g++ preprocessor remove MIN and MAX (pha)

template<class T>
inline const T &NR_MAX(const T &a, const T &b)
        {return b > a ? (b) : (a);}

inline float NR_MAX(const double &a, const float &b)
        {return b > a ? (b) : float(a);}

inline float NR_MAX(const float &a, const double &b)
        {return b > a ? float(b) : (a);}

template<class T>
inline const T &NR_MIN(const T &a, const T &b)
        {return b < a ? (b) : (a);}

inline float NR_MIN(const double &a, const float &b)
        {return b < a ? (b) : float(a);}

inline float NR_MIN(const float &a, const double &b)
        {return b < a ? float(b) : (a);}

template<class T>
inline T SIGN(const T &a, const T &b)
	{return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}

inline float SIGN(const float &a, const double &b)
	{return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}

inline float SIGN(const double &a, const float &b)
	{return (float)(b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a));}

template<class T>
inline void SWAP(T &a, T &b)
	{T dum=a; a=b; b=dum;}

// Vector and Matrix Classes

template <class T>
class NRvector {
private:
	npy_intp nn;	// size of array. upper index is nn-1
	T *v;
public:
	int ownsdata; // 1 for normal NRmatrix, 0 if Python owns the data
	PyObject *pyident; // if I don't own my data, who does?
	NRvector(PyObject *a);			// construct from Python array
	NRvector();
	explicit NRvector(int n);		// Zero-based array
	void initpyvec(PyObject *a); // helper function used by above
	NRvector(int n, const T &a);	//initialize to constant value
	NRvector(int n, const T *a);	// Initialize to array
	NRvector(const NRvector &rhs);	// Copy constructor
	NRvector & operator=(const NRvector &rhs);	//assignment
	typedef T value_type; // make T available externally
	inline T & operator[](const int i);	//i'th element
	inline const T & operator[](const int i) const;
	inline int size() const;
	void resize(int newn); // resize (contents not preserved)
	void assign(int newn, const T &a); // resize and assign a constant value
	~NRvector();
};

// NRvector definitions

template <class T>
NRvector<T>::NRvector() : nn(0),v(NULL),ownsdata(1){}

template <class T>
NRvector<T>::NRvector(int n) : nn(n), v(n>0 ? (T*)PyMem_Malloc(n*sizeof(T))  : NULL), ownsdata(1) {}

template <class T>
void NRvector<T>::initpyvec(PyObject *a) {
	ownsdata = 0;
	pyident = a;
	if (! PyArray_CheckExact(a)) throw runtime_error("PyObject is not an Array in NRmatrix constructor.");
	if (! PyArray_ISCARRAY_RO(a)) throw runtime_error("Python Array must be contiguous (e.g., not strided)."); 
	if (PyArray_NDIM(a) != 1) throw runtime_error("Python Array must be 1-dim in NRmatrix constructor.");
	if (! NRpyTypeOK<T>(a)) throw runtime_error("Python Array type does not agree with NRvector type."); 
	npy_intp* dims;
	npy_intp i, ndim = PyArray_NDIM(a);
        dims = PyArray_DIMS(a);
	nn = 1;
	for (i=0;i<ndim;i++) nn *= dims[i];
	v = (nn>0 ? (T*)PyArray_DATA(a) : NULL);
}
template <class T> NRvector<T>::NRvector(PyObject *a) {
	initpyvec(a);
}

template <class T>
NRvector<T>::NRvector(int n, const T& a) : nn(n),  v(n>0 ? (T*)PyMem_Malloc(n*sizeof(T)) : NULL), ownsdata(1)
{
	for(int i=0; i<n; i++) v[i] = a;
}

template <class T>
NRvector<T>::NRvector(int n, const T *a) : nn(n),  v(n>0 ? (T*)PyMem_Malloc(n*sizeof(T)) : NULL), ownsdata(1)
{
	for(int i=0; i<n; i++) v[i] = *a++;
}

template <class T>
NRvector<T>::NRvector(const NRvector<T> &rhs) : nn(rhs.nn),
       v(nn>0 ? (T*)PyMem_Malloc(nn*sizeof(T)) : NULL),  ownsdata(1) {
	for(int i=0; i<nn; i++) v[i] = rhs[i];
}

template <class T>
NRvector<T> & NRvector<T>::operator=(const NRvector<T> &rhs) {
	if (this != &rhs) {
		if (nn != rhs.nn) {
			resize(rhs.nn);
			nn=rhs.nn;
		}
		for (int i=0; i<nn; i++)
			v[i]=rhs[i];
	}
	return *this;
}

template <class T>
inline T & NRvector<T>::operator[](const int i)	//subscripting
{
#ifdef _CHECKBOUNDS_
if (i<0 || i>=nn) {
	throw runtime_error("NRvector subscript out of bounds");
}
#endif
	return v[i];
}

template <class T>
inline const T & NRvector<T>::operator[](const int i) const	//subscripting
{
#ifdef _CHECKBOUNDS_
if (i<0 || i>=nn) {
	throw runtime_error("NRvector subscript out of bounds");
}
#endif
	return v[i];
}

template <class T>
inline int NRvector<T>::size() const
{
	return nn;
}

template <class T>
void NRvector<T>::resize(int newn) {
	if (newn != nn) {
		nn = newn;
		if (ownsdata) {
			if (v != NULL) PyMem_Free(v);
			v = nn > 0 ? (T*)PyMem_Malloc(nn*sizeof(T)) : NULL;
		} else { // Python
			npy_intp dm[1];
			dm[0] = newn;
			PyArray_Dims mydims;
			mydims.ptr = dm;
			mydims.len = 1;
			PyArray_Resize((PyArrayObject *)pyident, &mydims, 0, NPY_CORDER);
			// the return value is garbage, or maybe PyNone, contrary to Numpy docs
			// I think it's a Numpy bug, but following is correct
			v = nn>0 ? (T*)PyArray_DATA(pyident) : NULL;
		}
	}
}

template <class T>
void NRvector<T>::assign(int newn, const T& a) {
	resize(newn);
	for (int i=0;i<nn;i++) v[i] = a;
}

template <class T>
NRvector<T>::~NRvector()
{
	if (v != NULL && ownsdata) {
		PyMem_Free(v);
	}
}

// end of NRvector definitions

template <class T>
class NRmatrix {
private:
	npy_intp nn;
	npy_intp mm;
	T **v;
public:
	int ownsdata; // 1 for normal NRmatrix, 0 if Python owns the data
	PyObject *pyident;
	NRmatrix();
	NRmatrix(int n, int m);			// Zero-based array
	NRmatrix(PyObject *a); // construct from Python array
	NRmatrix(char *name, char *dict = NULL); // construct from name in Python scope
	void initpymat(PyObject *a); // helper function used by above
	NRmatrix(int n, int m, const T &a);	//Initialize to constant
	NRmatrix(int n, int m, const T *a);	// Initialize to array
	NRmatrix(const NRmatrix &rhs);		// Copy constructor
	NRmatrix & operator=(const NRmatrix &rhs);	//assignment
	typedef T value_type; // make T available externally
	inline T* operator[](const int i);	//subscripting: pointer to row i
	inline const T* operator[](const int i) const;
	inline int nrows() const;
	inline int ncols() const;
	void resize(int newn, int newm); // resize (contents not preserved)
	void assign(int newn, int newm, const T &a); // resize and assign a constant value
	~NRmatrix();
};

template <class T>
NRmatrix<T>::NRmatrix() : nn(0), mm(0), v(NULL),ownsdata(1)  {}

template <class T>
NRmatrix<T>::NRmatrix(int n, int m) : nn(n), mm(m),  v(n>0 ? new T*[n] : NULL), ownsdata(1)
{
	int i,nel=m*n;
	if (v) v[0] = nel>0 ? (T*)PyMem_Malloc(nel*sizeof(T)) : NULL;
	for (i=1;i<n;i++) v[i] = v[i-1] + m;
}

template <class T>
void NRmatrix<T>::initpymat(PyObject *a) {
	pyident = a;
	ownsdata = 0;
	if (! PyArray_CheckExact(a)) throw runtime_error("PyObject is not an Array in NRmatrix constructor.");
	if (! PyArray_ISCARRAY_RO(a)) throw runtime_error("Python Array must be contiguous (e.g., not strided)."); 
	if (PyArray_NDIM(a) != 2) throw runtime_error("Python Array must be 2-dim in NRmatrix constructor.");
	if (! NRpyTypeOK<T>(a)) throw runtime_error("Python Array type does not agree with NRmatrix type."); 
	npy_intp* dims;
	npy_intp i, nel; 
        dims = PyArray_DIMS(a);
	nn = dims[0];
	mm = dims[1];
	nel = mm*nn;
	v = (nn>0 ? new T*[nn] : NULL);
	if (v) v[0] = nel>0 ? (T*)PyArray_DATA(a) : NULL;
	for (i=1;i<nn;i++) v[i] = v[i-1] + mm;
        //cout << "leaving * constructor " << endl;
}
template <class T> NRmatrix<T>::NRmatrix(PyObject *a) {
	initpymat(a);
}

template <class T>
NRmatrix<T>::NRmatrix(int n, int m, const T &a) : nn(n), mm(m),  v(n>0 ? new T*[n] : NULL), ownsdata(1)
{
	int i, j, nel=m*n;
	if (v) v[0] = nel>0 ? (T*)PyMem_Malloc(nel*sizeof(T)) : NULL;
	for (i=1; i< n; i++) v[i] = v[i-1] + m;
	for (i=0; i< n; i++) for (j=0; j<m; j++) v[i][j] = a;
}

template <class T>
NRmatrix<T>::NRmatrix(int n, int m, const T *a) : nn(n), mm(m),  v(n>0 ? new T*[n] : NULL), ownsdata(1)
{
	int i,j,nel=m*n;
	if (v) v[0] = nel>0 ? (T*)PyMem_Malloc(nel*sizeof(T)) : NULL;
	for (i=1; i< n; i++) v[i] = v[i-1] + m;
	for (i=0; i< n; i++) for (j=0; j<m; j++) v[i][j] = *a++;
}

template <class T>
NRmatrix<T>::NRmatrix(const NRmatrix &rhs) : nn(rhs.nn), mm(rhs.mm),  v(nn>0 ? new T*[nn] : NULL), ownsdata(1)
{
	int i,j,nel=mm*nn;
	if (v) v[0] = nel>0 ? (T*)PyMem_Malloc(nel*sizeof(T)) : NULL;
	for (i=1; i< nn; i++) v[i] = v[i-1] + mm;
	for (i=0; i< nn; i++) for (j=0; j<mm; j++) v[i][j] = rhs[i][j];
        //cout << "leaving copy constructor" << endl;
}

template <class T>
NRmatrix<T> & NRmatrix<T>::operator=(const NRmatrix<T> &rhs) {
	if (this != &rhs) {
		int i,j;
		if (nn != rhs.nn || mm != rhs.mm) {
			resize(rhs.nn,rhs.mm);
			nn=rhs.nn;
			mm=rhs.mm;
		}
		for (i=0; i<nn; i++) for (j=0; j<mm; j++) v[i][j] = rhs[i][j];
	}
	return *this;
}

template <class T>
inline T* NRmatrix<T>::operator[](const int i)	//subscripting: pointer to row i
{
#ifdef _CHECKBOUNDS_
if (i<0 || i>=nn) {
	throw runtime_error("NRmatrix subscript out of bounds");
}
#endif
	return v[i];
}

template <class T>
inline const T* NRmatrix<T>::operator[](const int i) const
{
#ifdef _CHECKBOUNDS_
if (i<0 || i>=nn) {
	throw runtime_error("NRmatrix subscript out of bounds");
}
#endif
	return v[i];
}

template <class T>
inline int NRmatrix<T>::nrows() const
{
	return nn;
}

template <class T>
inline int NRmatrix<T>::ncols() const
{
	return mm;
}

template <class T>
void NRmatrix<T>::resize(int newn, int newm) {
	int i,nel;
	if (newn != nn || newm != mm) {
		nn = newn;
		mm = newm;
		nel = mm*nn;
		if (ownsdata) {
			if (v != NULL) {
				PyMem_Free(v[0]);
				delete[] (v);
			}
			v = nn>0 ? new T*[nn] : NULL;
			if (v) v[0] = nel>0 ? (T*)PyMem_Malloc(nel*sizeof(T)) : NULL;
		} else {
			if (v != NULL) delete[] (v);
			npy_intp dm[2];
			dm[0] = newn; dm[1] = newm;
			PyArray_Dims mydims;
			mydims.ptr = dm;
			mydims.len = 2;
			PyArray_Resize((PyArrayObject *)pyident, &mydims, 0, NPY_CORDER);
			// the return value is garbage, or maybe PyNone, contrary to Numpy docs
			// I think it's a Numpy bug, but following is correct
			v = (nn>0 ? new T*[nn] : NULL);
			if (v) v[0] = nel>0 ? (T*)PyArray_DATA(pyident) : NULL;
		}
		for (i=1; i<nn; i++) v[i] = v[i-1] + mm;
	}
}

template <class T>
void NRmatrix<T>::assign(int newn, int newm, const T& a) {
	int i,j;
	resize(newn,newm);
	for (i=0; i< nn; i++) for (j=0; j<mm; j++) v[i][j] = a;
}

template <class T>
NRmatrix<T>::~NRmatrix()
{
	if (v != NULL) {
		if (ownsdata) PyMem_Free(v[0]); // pointer to the data
		delete[] (v); // pointer to the pointers
	}
}

template <class T>
class NRMat3d {
private:
	int nn;
	int mm;
	int kk;
	T ***v;
public:
	NRMat3d();
	NRMat3d(int n, int m, int k);
	inline T** operator[](const int i);	//subscripting: pointer to row i
	inline const T* const * operator[](const int i) const;
	inline int dim1() const;
	inline int dim2() const;
	inline int dim3() const;
	~NRMat3d();
};

template <class T>
NRMat3d<T>::NRMat3d(): nn(0), mm(0), kk(0), v(NULL) {}

template <class T>
NRMat3d<T>::NRMat3d(int n, int m, int k) : nn(n), mm(m), kk(k), v(new T**[n])
{
	int i,j;
	v[0] = new T*[n*m];
	v[0][0] = new T[n*m*k];
	for(j=1; j<m; j++) v[0][j] = v[0][j-1] + k;
	for(i=1; i<n; i++) {
		v[i] = v[i-1] + m;
		v[i][0] = v[i-1][0] + m*k;
		for(j=1; j<m; j++) v[i][j] = v[i][j-1] + k;
	}
}

template <class T>
inline T** NRMat3d<T>::operator[](const int i) //subscripting: pointer to row i
{
	return v[i];
}

template <class T>
inline const T* const * NRMat3d<T>::operator[](const int i) const
{
	return v[i];
}

template <class T>
inline int NRMat3d<T>::dim1() const
{
	return nn;
}

template <class T>
inline int NRMat3d<T>::dim2() const
{
	return mm;
}

template <class T>
inline int NRMat3d<T>::dim3() const
{
	return kk;
}

template <class T>
NRMat3d<T>::~NRMat3d()
{
	if (v != NULL) {
		delete[] (v[0][0]);
		delete[] (v[0]);
		delete[] (v);
	}
}

// templated return a PyObject's type (used in NRpyObject on vector and matrix args)
template <class T> inline int NRpyDataType() {return PyArray_INT;}
template <> inline int NRpyDataType<double>() {return PyArray_DOUBLE;}
template <> inline int NRpyDataType<int>() {return PyArray_INT;}

template<class T> PyObject* NRpyObject(NRvector<T> &a) {
	if (a.ownsdata == 0) {Py_INCREF(a.pyident); return a.pyident;}
	npy_intp nd=1, dims[1];
	dims[0] = a.size();
	PyObject *thing;
	if (dims[0] > 0) {
		thing = PyArray_SimpleNewFromData(nd, dims, NRpyDataType<T>(), &a[0]);
	} else {
		thing = PyArray_SimpleNew(nd, dims, NRpyDataType<T>()); // zero size
	}
	PyArray_FLAGS(thing) |= NPY_OWNDATA;
	a.ownsdata = 0;
	a.pyident = thing;
	return thing;
}

template<class T> PyObject* NRpyObject(NRmatrix<T> &a) {
	if (a.ownsdata == 0) {Py_INCREF(a.pyident); return a.pyident;}
	npy_intp nd=2, dims[2];
	dims[0] = a.nrows(); dims[1] = a.ncols();
	PyObject *thing;
	if (dims[0]*dims[1] > 0) {
		thing = PyArray_SimpleNewFromData(nd, dims, NRpyDataType<T>(), &a[0][0]);
	} else {
		thing = PyArray_SimpleNew(nd, dims, NRpyDataType<T>()); // zero size
	}
	PyArray_FLAGS(thing) |= NPY_OWNDATA;
	a.ownsdata = 0;
	a.pyident = thing;
	return thing;
}


// vector types

typedef const NRvector<Int> VecInt_I;
typedef NRvector<Int> VecInt, VecInt_O, VecInt_IO;

typedef const NRvector<Uint> VecUint_I;
typedef NRvector<Uint> VecUint, VecUint_O, VecUint_IO;

typedef const NRvector<Llong> VecLlong_I;
typedef NRvector<Llong> VecLlong, VecLlong_O, VecLlong_IO;

typedef const NRvector<Ullong> VecUllong_I;
typedef NRvector<Ullong> VecUllong, VecUllong_O, VecUllong_IO;

typedef const NRvector<Char> VecChar_I;
typedef NRvector<Char> VecChar, VecChar_O, VecChar_IO;

typedef const NRvector<Char*> VecCharp_I;
typedef NRvector<Char*> VecCharp, VecCharp_O, VecCharp_IO;

typedef const NRvector<Uchar> VecUchar_I;
typedef NRvector<Uchar> VecUchar, VecUchar_O, VecUchar_IO;

typedef const NRvector<Doub> VecDoub_I;
typedef NRvector<Doub> VecDoub, VecDoub_O, VecDoub_IO;

typedef const NRvector<Doub*> VecDoubp_I;
typedef NRvector<Doub*> VecDoubp, VecDoubp_O, VecDoubp_IO;

typedef const NRvector<Complex> VecComplex_I;
typedef NRvector<Complex> VecComplex, VecComplex_O, VecComplex_IO;

typedef const NRvector<Bool> VecBool_I;
typedef NRvector<Bool> VecBool, VecBool_O, VecBool_IO;

// matrix types

typedef const NRmatrix<Int> MatInt_I;
typedef NRmatrix<Int> MatInt, MatInt_O, MatInt_IO;

typedef const NRmatrix<Uint> MatUint_I;
typedef NRmatrix<Uint> MatUint, MatUint_O, MatUint_IO;

typedef const NRmatrix<Llong> MatLlong_I;
typedef NRmatrix<Llong> MatLlong, MatLlong_O, MatLlong_IO;

typedef const NRmatrix<Ullong> MatUllong_I;
typedef NRmatrix<Ullong> MatUllong, MatUllong_O, MatUllong_IO;

typedef const NRmatrix<Char> MatChar_I;
typedef NRmatrix<Char> MatChar, MatChar_O, MatChar_IO;

typedef const NRmatrix<Uchar> MatUchar_I;
typedef NRmatrix<Uchar> MatUchar, MatUchar_O, MatUchar_IO;

typedef const NRmatrix<Doub> MatDoub_I;
typedef NRmatrix<Doub> MatDoub, MatDoub_O, MatDoub_IO;

typedef const NRmatrix<Bool> MatBool_I;
typedef NRmatrix<Bool> MatBool, MatBool_O, MatBool_IO;

// 3D matrix types

typedef const NRMat3d<Doub> Mat3DDoub_I;
typedef NRMat3d<Doub> Mat3DDoub, Mat3DDoub_O, Mat3DDoub_IO;


// Floating Point Exceptions for Microsoft compilers


#ifdef _TURNONFPES_
#ifdef _MSC_VER
struct turn_on_floating_exceptions {
	turn_on_floating_exceptions() {
		int cw = _controlfp( 0, 0 );
		cw &=~(EM_INVALID | EM_OVERFLOW | EM_ZERODIVIDE );
		_controlfp( cw, MCW_EM );
	}
};
turn_on_floating_exceptions yes_turn_on_floating_exceptions;
#endif /* _MSC_VER */
#endif /* _TURNONFPES */

#endif /* _NR3_H_ */

