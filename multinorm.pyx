import numpy as np
cimport numpy as np
#
# nrcython.pxd contains the declarations for MatDoub and VecDoub
#
from nrcython cimport *
from libc.stdint cimport uint64_t
#
# nr3cython.h uses PyArray_* functions
# will segfault without import_array()
#
np.import_array()

cdef extern from "multinormaldev.h":
   cdef cppclass Multinormaldev:
      Multinormaldev(uint64_t j, VecDoub &mmean, MatDoub &vvar) except +
      VecDoub& dev()
      
cdef class multi_norm:
   """
       wrapper for multinormaldev.h (NR section 7.4)
       To use from python:

       import multinorm as mn
       #construct an array of means (mean_vec) and a correlation matrix (corray_array)
       #create an instance:
       the_seed=1234
       multi_class=mn.multi_norm(the_seed,meanvec,corr)
       #get an array of 4000 multinormal deviates
       out_vals=multi_class.get_devs(4000)
   """
   cdef Multinormaldev *thisptr
   cdef uint64_t seed 
   cdef int mm
   
   def __cinit__(self, np.uint64_t the_seed, object the_mean, object the_var):
      self.seed=the_seed
      self.thisptr = new Multinormaldev(the_seed, VecDoub(<PyObject*> the_mean), 
                                        MatDoub(<PyObject*> the_var))
      self.mm=the_mean.size

   def get_devs(self,num_devs):
       """
          returns a np.array of shape [mm,num_devs]
       """
       cdef VecDoub the_dev
       cdef int i,j 
       cdef int count=num_devs
       #
       # get a memory view object so that the loop
       # doesn't contain any python calls
       #
       cdef double[:,:] mean_view=np.empty([self.mm,num_devs])
       cdef int mm=self.mm
       for i in range(count):
           the_dev=self.thisptr.dev()
           for j in range(mm):
               mean_view[j,i]=the_dev[j]
       return np.asarray(mean_view)

   def get_seed(self):
       return self.seed
   
