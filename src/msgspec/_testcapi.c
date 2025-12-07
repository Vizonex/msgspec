#define PY_SSIZE_T_CLEAN
#include <Python.h>
#define MSGSPEC_USE_CAPSULE_API
#include "msgspec.h"

/* Modeled after multidict's C-API tests written by the same author Vizonex */

/* TODO: (Vizonex) 
    I think I could make a template github repo 
    for users who want a quick and dirty template to work off of for 
    something like this... 
*/

typedef struct _mod_state {
    Msgspec_CAPI* capi;
} mod_state;

static inline mod_state *
get_mod_state(PyObject *mod)
{
    mod_state *state = (mod_state *)PyModule_GetState(mod);
    assert(state != NULL);
    return state;
}

static inline Msgspec_CAPI*
get_capi(PyObject *mod)
{
    return get_mod_state(mod)->capi;
}

static int
check_nargs(const char *name, const Py_ssize_t nargs, const Py_ssize_t required)
{
    if (nargs != required) {
        PyErr_Format(PyExc_TypeError,
                     "%s should be called with %d arguments, got %d",
                     name,
                     required,
                     nargs);
        return -1;
    }
    return 0;
}

/* Factory Objects */
static PyObject * factory_type(PyObject* mod, PyObject* Py_UNUSED(unused)){
    return Py_NewRef((PyObject*)get_capi(mod)->Factory_Type);
}

//  PyObject *const *args, Py_ssize_t nargs

static PyObject* factory_check(PyObject* mod, PyObject* arg){
    if (Factory_Check(get_capi(mod), arg)){
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

// TBD
// static PyObject* factory_check_exact(PyObject* mod, PyObject* arg){
//     if (Factory_CheckExact(get_capi(mod), arg)){
//         Py_RETURN_TRUE;
//     }
//     Py_RETURN_FALSE;
// }

static PyObject* factory_new(PyObject* mod, PyObject* arg){
    return get_capi(mod)->Factory_New(arg);
}

static PyObject* factory_create(PyObject* mod, PyObject* arg){
    return get_capi(mod)->Factory_Create(arg);
}


/* Field_Type Objects... */

static PyObject* field_check(PyObject* mod, PyObject* arg){
    if (Field_Check(get_capi(mod), arg)){
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

static PyObject * field_type(PyObject* mod, PyObject* Py_UNUSED(unused)){
    return Py_NewRef((PyObject*)get_capi(mod)->Field_Type);
}

static PyObject* field_new(PyObject* mod, PyObject *const *args, Py_ssize_t nargs){
    if (check_nargs("field_new", nargs, 3) < 0){
        return NULL;
    }
    // Will simulate None as NULL since it's kind of difficult to simulate a Null Pointer...
    PyObject* _default = Py_IsNone(args[1]) ? NULL: Py_NewRef(args[1]);
    PyObject* _factory = Py_IsNone(args[2]) ? NULL: Py_NewRef(args[2]);
    PyObject* ret = get_capi(mod)->Field_New(args[0], _default, _factory);
    /* Py_CLEAR has null checks of it's own before it clears out a value... */
    Py_CLEAR(_default);
    Py_CLEAR(_factory);
    return ret;
}

typedef int (*capi_getter_func)(PyObject* self, PyObject** value);

static PyObject* handle_getter_capi_func(capi_getter_func func, PyObject* self){
    PyObject* value = NULL;
    if (func(self, &value) < 0){
        return NULL;
    }
    return value;
}

static PyObject* handle_getter_capi_func_can_be_null(capi_getter_func func, PyObject* self){
    PyObject* value = NULL;
    if (func(self, &value) < 0){
        return NULL;
    }
    if (value == NULL){
        Py_RETURN_NONE;
    }
    return value;
}


static PyObject* field_get_name(PyObject* mod, PyObject* arg){
    return handle_getter_capi_func(get_capi(mod)->Field_GetName, arg);
}
static PyObject* field_get_default(PyObject* mod, PyObject* arg){
    return handle_getter_capi_func_can_be_null(get_capi(mod)->Field_GetDefault, arg);
}
static PyObject* field_get_factory(PyObject* mod, PyObject* arg){
    return handle_getter_capi_func_can_be_null(get_capi(mod)->Field_GetFactory, arg);
}


/* module slots */

static int
module_traverse(PyObject *mod, visitproc visit, void *arg)
{
    return 0;
}

static int
module_clear(PyObject *mod)
{
    return 0;
}

static void
module_free(void *mod)
{
    (void)module_clear((PyObject *)mod);
}

/* These can get annoying to configure so I made simple macros - Vizonex */
#define MM_O(name) \
    {#name, (PyCFunction)(name), METH_O}

#define MM_NOARGS(name) \
    {#name, (PyCFunction)(name), METH_NOARGS}

#define MM_FASTCALL(name) \
    {#name, (PyCFunction)(name), METH_FASTCALL}


static PyMethodDef module_methods[] = {
    MM_O(factory_check),
    MM_O(factory_new),
    MM_O(factory_create),
    MM_NOARGS(factory_type),
    MM_O(field_check),
    MM_O(field_get_default),
    MM_O(field_get_factory),
    MM_O(field_get_name),
    MM_FASTCALL(field_new),
    MM_NOARGS(field_type),
    {NULL, NULL}
};

static int
module_exec(PyObject *mod)
{
    mod_state *state = get_mod_state(mod);
    state->capi = Msgspec_Import();
    if (state->capi == NULL) {
        return -1;
    }
    return 0;
}

static struct PyModuleDef_Slot module_slots[] = {
    {Py_mod_exec, module_exec},
#if PY_VERSION_HEX >= 0x030c00f0
    {Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED},
#endif
#if PY_VERSION_HEX >= 0x030d00f0
    {Py_mod_gil, Py_MOD_GIL_NOT_USED},
#endif
    {0, NULL},
};

static PyModuleDef _testcapi_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "msgspec._testcapi",
    .m_size = sizeof(mod_state),
    .m_methods = module_methods,
    .m_slots = module_slots,
    .m_traverse = module_traverse,
    .m_clear = module_clear,
    .m_free = (freefunc)module_free,
};

PyMODINIT_FUNC
PyInit__testcapi(void)
{
    return PyModuleDef_Init(&_testcapi_module);
}
