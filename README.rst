numrecipes
==========

An example and tests of a modified version of
http://www.nr.com/nr3python.h for use with cython.

To build
--------

0) Tested witn Macports python 2.7.3 and gcc47 4.7.2_2 for osx and
   Enthought 7.3.2 with 32 bit windows (cygwin g++ 4.5.2 under parallels),
   both with cython 0.18

1) Edit setup.py and replace::

     nrecipesdir='/Users/phil/repos/nrecipes/NR_C301/code304/'

   with the path to your numerical recipes directory

2) Edit multinormaldev.h in that directory to add the dependencies::

     #include "nr3.h"
     #include "ran.h"
     #include "cholesky.h"

     struct Multinormaldev : Ran {
     ...

3)  Exectuing::

       python setup.py build_ext  

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

