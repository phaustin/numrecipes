numrecipes
==========

An example and tests of a modified version of
http://www.nr.com/nr3python.h for use with cython.

To build
--------

1) Edit setup.py and replace::

     nrecipesdir='/Users/phil/repos/nrecipes/NR_C301/code304/'

   with the path to your numerical recipes directory

2) Edit multinormaldev.h in that directory to add the dependencies::

     #include "nr3.h"
     #include "ran.h"
     #include "cholesky.h"

     struct Multinormaldev : Ran {
     ...
