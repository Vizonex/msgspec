#ifndef MSGSPEC_H
#define MSGSPEC_H

#include <Python.h>
#include <datetime.h>
#include <assert.h>


#ifdef __cplusplus
extern "C" {
#endif

/* This code is inspired by multidict's concept for a C-API Capsule written by Vizonex and Avestlov 
* Written by Vizonex */

#define MSGSPEC_MODULE_NAME "msgspec._core"
#define MSGSPEC_CAPI_NAME "CAPI"
#define MSGSPEC_CAPSULE_NAME MSGSPEC_MODULE_NAME "." MSGSPEC_CAPI_NAME


/* TODO consider Moving EncoderState over to another header file and let it have some iteraction here... */
typedef struct EncoderState EncoderState;




typedef struct _msgspec_capi {
    /* module_state (private) */
    void* _state;

    PyTypeObject* Factory_Type;
    PyTypeObject* Field_Type;



    /* Kept things in alphabetical order for the sake of neatness 
    let me know if this is not the order you want these in... */

    /* EXT */

    // PyObject * (*Ext_New)(long code, PyObject* data);

    /* XXX: Didn't know if Ext's data should be altered by the end user or not so I made a few gettypes */
    
    // Might move Ext Structure to a new neighboring header file for Recasting to Cython maybe :/
    /* Returns data and code from extension type, Returns -1 if Type is not an Ext type */
    // int (*Ext_GetData)(PyObject* self, PyObject** data, long *code);


    /* Factory */

    /* Creates a new object from factory Type, returns NULL on exception*/
    PyObject* (*Factory_New)(PyObject* factory);
    PyObject* (*Factory_Create)(PyObject* self);


    /* Fields */
    
    PyObject* (*Field_New)(PyObject* name, PyObject* value, PyObject* factory);

    /* In Cython there is no need for these due to safely having checks of it's own to grab attributes
    however, in other projects (CPython or Rust) this it may be nessesary to have here... */

    /* Gets the name of the field, returns -1 if type is not a msgspec field 
    Field name can be NULL if it wasn't set by a msgspec structure yet... */
    int (*Field_GetName)(PyObject* self, PyObject** name);

    /* Obtains default attribute, 
    returns -1 if type is not a msgspec field value could appear as NULL if field was a NODEFAULT */
    int (*Field_GetDefault)(PyObject* self, PyObject** value);

    /* Obtains factory attribute, returns -1 if type is not a msgspec field */
    int (*Field_GetFactory)(PyObject* self, PyObject** factory);

} Msgspec_CAPI;

/* Capsule */

/* imports msgspec capsule 
returns NULL if import fails. */
static inline Msgspec_CAPI*
Msgspec_Import()
{
    return (Msgspec_CAPI*)PyCapsule_Import(MSGSPEC_CAPSULE_NAME, 0);
}

#ifdef MSGSPEC_USE_CAPSULE_API // Define if your not using Cython or want to use your own capsule

/*************************************************************************
 * Factory                                                               *
 *************************************************************************/

static inline int Factory_Check(Msgspec_CAPI* api, PyObject* ob){
    return Py_IS_TYPE(ob, api->Factory_Type);
}

static inline int Factory_CheckExact(Msgspec_CAPI* api, PyObject* ob){
    return Py_IS_TYPE(ob, api->Factory_Type) || PyObject_TypeCheck(ob, api->Factory_Type);
}

/*************************************************************************
 * Field                                                                 *
 *************************************************************************/

static inline int Field_Check(Msgspec_CAPI* api, PyObject* ob){
    return Py_IS_TYPE(ob, api->Field_Type);
}

static inline int Field_CheckExact(Msgspec_CAPI* api, PyObject* ob){
    return Py_IS_TYPE(ob, api->Field_Type) || PyObject_TypeCheck(ob, api->Field_Type);
}

static inline int Field_GetName(Msgspec_CAPI* api, PyObject* self, PyObject** name){
    return api->Field_GetName(self, name);
};

static inline int Field_GetDefault(Msgspec_CAPI* api, PyObject* self, PyObject** value){
    return api->Field_GetDefault(self, value);
};

static inline int Field_GetFactory(Msgspec_CAPI* api, PyObject* self, PyObject** factory){
    return api->Field_GetFactory(self, factory);
}
#endif /* MSGSPEC_USE_CAPSULE_API */


#ifdef __cplusplus
}
#endif

#endif // MSGSPEC_H