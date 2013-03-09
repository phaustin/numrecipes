numrecipes
==========

An example and tests of a modified version of
http://www.nr.com/nr3python.h for use with cython.

To build
--------

0) Tested witn Macports python 2.7.3 and gcc 4.7.2_2 for osx,
   Enthought 7.3.2 with 32 bit windows (cygwin g++ 4.5.2 under parallels),
   and Ubuntu lucid with gcc 4.7.2.2, all with cython 0.18.  
   Under Ubuntu the preprocessor removes MAX and MIN from the
   header, so I've changed those names to NR_MAX and NR_MIN to compile.
   Any numrecipe header that uses MAX or MIN will need to
   be edited to match that change

1) Edit setup_xxx.py (where xxx is win, osx or linux) and replace nrecipesdir::

     nrecipesdir='/Users/phil/repos/nrecipes/NR_C301/code304/'

   with the path to your numerical recipes directory

2) Copy nr3cython.h to that directory

3) Edit multinormaldev.h in that directory to add the dependencies::

     #include "nr3cython.h"
     #include "ran.h"
     #include "cholesky.h"

     struct Multinormaldev : Ran {
     ...

4)  Exectuing the appropriate setup_xx.py eg::

       python setup_osx.py build_ext  

    in the numrecipes directory 
    should produce an inplace build with many warnings but files named
    multinorm.so and recipes_tests.so

To test
-------

1) On both windows and osx executing::

     python multinorm_example.py 

   should open a scatter plot showing 4000 correlated random deviates

2) On osx (currently crashing for me on windows) executing::

     nosetests

   should produce the following output::

     .....
     -------------------------
     Ran 5 tests in 0.085s

     OK

