from nose.tools import assert_raises
import site
site.addsitedir('../.')
import recipes_tests as rt

def test_pyobject_constructorI():
    rt.test_pyobject_constructorI()

def test_pyobject_constructorII():
    rt.test_pyobject_constructorII()
    
def test_NRpyObject_MatDoub():
    rt.test_NRpyObject_MatDoub()

def test_NRpyObject_VecDoub():
    rt.test_NRpyObject_VecDoub()
    
def test_raise_RuntimeError():
    assert_raises(RuntimeError, rt.test_pyobject_constructor32)    



            
