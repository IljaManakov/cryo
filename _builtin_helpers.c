#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <object.h>

#include <stdbool.h>


static PyObject*
_set_class_on_builtin(PyObject *module, PyObject *args)
{
    PyObject *object;
    PyObject *new_class;
    if (!PyArg_ParseTuple(args, "OO", &object, &new_class)) {
        PyErr_SetString(PyExc_TypeError,
                        "_set_class_on_builtin must be called with two arguments: object and new_class");
        return NULL;
    }

    if (new_class == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "can't delete __class__ attribute");
        return NULL;
    }
    if (!PyType_Check(new_class)) {
        PyErr_Format(PyExc_TypeError,
          "__class__ must be set to a class, not '%s' object",
          Py_TYPE(new_class)->tp_name);
        return NULL;
    }

    // This should only work on lists, dicts or sets
    PyTypeObject *obj_type = Py_TYPE(object);
    if (!(
    	PyType_IsSubtype(obj_type, &PyList_Type) ||
    	PyType_IsSubtype(obj_type, &PyDict_Type) ||
    	PyType_IsSubtype(obj_type, &PySet_Type)
	))
    {
    	PyErr_Format(PyExc_TypeError,
          "_set_class_on_builtin can only be called on mutable container types (list, set, dict). Got '%s'",
          object->ob_type->tp_name);
    	return NULL;
    }

//    Py_INCREF(new_class);
    Py_SET_TYPE(object, (PyTypeObject*) new_class);
    Py_INCREF(object);
    return object;
}

static PyMethodDef Methods[] = {
    {"_set_class_on_builtin",  _set_class_on_builtin, METH_VARARGS,
     "Changes __class__ on a list, dict or set."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef _builtin_helpers_module = {
    PyModuleDef_HEAD_INIT,
    "_builtin_helpers",   /* name of module */
    NULL,
    -1,
    Methods
};

PyMODINIT_FUNC
PyInit__builtin_helpers(void)
{
    return PyModule_Create(&_builtin_helpers_module);
}
