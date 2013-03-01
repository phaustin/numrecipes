from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import numpy
import sys,os,glob,shutil


#
# for windows cleanall
#
def onerror(func, path, exc_info):
    """
    Error handler for ``shutil.rmtree``.

    If the error is due to an access error (read only file)
    it attempts to add write permission and then retries.

    If the error is for another reason it re-raises the error.

    Usage : ``shutil.rmtree(path, onerror=onerror)``
    http://stackoverflow.com/questions/2656322/python-shutil-rmtree-fails-on-windows-with-access-is-denied
    """
    import stat
    if not os.access(path, os.W_OK):
        # Is the error an access error ?
        os.chmod(path, stat.S_IWUSR)
        func(path)
    else:
        raise

args = sys.argv[1:]

#--- adapted from 
# http://www.cs.dartmouth.edu/~nfoti/blog/blog/2013/02/07/cleaning-cython-build-files/
# Make a `cleanall` rule to get rid of intermediate and library files
if "cleanall" in args:
    print "Deleting cython files..."
    the_files=glob.glob('*.so') + ['multinorm.cpp','recipes_tests.cpp']
    [os.remove(a_file) for a_file in the_files]
    shutil.rmtree('build', onerror=onerror)
    # Now do a normal clean
    sys.argv[1] = "clean"

# We want to always use build_ext --inplace
if args.count("build_ext") > 0 and args.count("--inplace") == 0:
    sys.argv.insert(sys.argv.index("build_ext")+1, "--inplace")

# Only build for 64-bit target
#os.environ['ARCHFLAGS'] = "-arch x86_64"
os.environ['CXX'] = "g++"
os.environ['CC'] = "g++"
#----

nrecipesdir='/Users/phil/repos/nrecipes/NR_C301/code304/'
#nrecipesdir= r'z:\repos\nrecipes\NR_C301\code304'
nrecipesdir='/home/phil/repos/nrecipes/NR_C301/code304/'

recipes=Extension('recipes_tests', ['recipes_tests.pyx'],
            include_dirs = [numpy.get_include(),nrecipesdir],
            extra_compile_args=['-O3', '-fPIC'],
            library_dirs=['.'],
            language="c++")

multinorm=Extension('multinorm', ['multinorm.pyx'],
            include_dirs = [numpy.get_include(),nrecipesdir],
            extra_compile_args=['-O3', '-fPIC'],
            library_dirs=['.'],
            language="c++")

ext_modules = [recipes,multinorm]

setup(name        = 'recipes',
      cmdclass    = {'build_ext': build_ext},
      ext_modules = ext_modules
      )
