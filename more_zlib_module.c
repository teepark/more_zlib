#include "Python.h"
#include "zlib.h"


/* copied from Modules/zlibmodule.c in the CPython distribution */
static PyTypeObject *Comptype;
static PyTypeObject *Decomptype;
static PyObject *ZlibError;
typedef struct {
    PyObject_HEAD
    z_stream zst;
    PyObject *unused_data;
    PyObject *unconsumed_tail;
    int is_initialised;
} compobject;


static PyObject *
python_deflateSetDictionary(PyObject *self, PyObject *args) {
    uint dictlen;
    Bytef *dictionary;
    compobject *compressor;
    int result;

    if (!PyArg_ParseTuple(args, "O!s#", Comptype, (PyObject *)&compressor,
            (char **)&dictionary, (Py_ssize_t *)&dictlen))
        return NULL;

    result = deflateSetDictionary(&(compressor->zst), dictionary, dictlen);
    switch(result) {
        case (Z_OK):
            Py_INCREF(Py_None);
            return Py_None;
        case (Z_MEM_ERROR):
            PyErr_SetString(PyExc_MemoryError, "out of memory");
            return NULL;
        case (Z_STREAM_ERROR):
            PyErr_SetString(ZlibError, "bad zlib stream");
            return NULL;
        default:
            PyErr_SetString(PyExc_Exception, "unknown zlib error");
            return NULL;
    }
}

static PyObject *
python_inflateSetDictionary(PyObject *self, PyObject *args) {
    uint dictlen;
    Bytef *dictionary;
    compobject *decompressor;
    int result;

    if (!PyArg_ParseTuple(args, "O!s#", Decomptype, (PyObject *)&decompressor,
            (char **)&dictionary, (Py_ssize_t *)&dictlen))
        return NULL;

    result = inflateSetDictionary(&(decompressor->zst), dictionary, dictlen);
    switch(result) {
        case (Z_OK):
            Py_INCREF(Py_None);
            return Py_None;
        case (Z_MEM_ERROR):
            PyErr_SetString(PyExc_MemoryError, "out of memory");
            return NULL;
        case (Z_STREAM_ERROR):
            PyErr_SetString(ZlibError, "bad zlib stream");
            return NULL;
        default:
            PyErr_SetString(PyExc_Exception, "unknown zlib error");
            return NULL;
    }
    puts("seven");
}


static int
populate_types(void) {
    PyObject *zlib_mod, *func, *args, *obj;

    if (!(args = PyTuple_New(0)))
        return -1;

    if (!(zlib_mod = PyImport_ImportModule("zlib")))
        return -1;

    if (!(ZlibError = PyObject_GetAttrString(zlib_mod, "error")))
        return -1;

    if (!(func = PyObject_GetAttrString(zlib_mod, "compressobj")))
        return -1;

    if (!(obj = PyObject_Call(func, args, NULL)))
        return -1;
    Comptype = obj->ob_type;

    Py_DECREF(func);
    Py_DECREF(obj);

    if (!(func = PyObject_GetAttrString(zlib_mod, "decompressobj")))
        return -1;

    if (!(obj = PyObject_Call(func, args, NULL)))
        return -1;
    Decomptype = obj->ob_type;

    Py_DECREF(func);
    Py_DECREF(obj);
    Py_DECREF(args);
    return 0;
}

static PyMethodDef methods[] = {
    {"deflateSetDictionary", python_deflateSetDictionary, METH_VARARGS, ""},
    {"inflateSetDictionary", python_inflateSetDictionary, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef more_zlib_module = {
    PyModuleDef_HEAD_INIT,
    "more_zlib", "", -1, methods,
    NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_more_zlib(void) {
    PyObject *module;

    if (populate_types()) return NULL;
        
    module = PyModule_Create(&more_zlib_module);
    return module;
}
#else
PyMODINIT_FUNC
initmore_zlib(void) {
    /* TODO: raise if this fails */
    populate_types();

    Py_InitModule("more_zlib", methods);
}
#endif
