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

    PyTypeObject* Ext_Type;
    PyTypeObject* Factory_Type;
    PyTypeObject* Field_Type;

    // Coming Soon...
    // PyTypeObject* Raw_Type;

    PyTypeObject* StructMeta_Type;


    /* Kept things in alphabetical order for the sake of neatness 
    let me know if this is not the order you want these in... */

    /* EXT */

    /* Creates a new Extension Type, returns NULL if it fails, 
    this will raise a ValueError if code 
    is not between -128 and 127 and TypeError 
    if data is not a bytes or bytearray object */
    PyObject * (*Ext_New)(long code, PyObject* data);

    /* Returns data from extension type, Returns -1 if Type is not an Ext type */
    int (*Ext_GetData)(PyObject* self, PyObject** data);

    /* Returns code from extension type, Returns -1 if Type is not an Ext type */
    int (*Ext_GetCode)(PyObject* self, long* code);

    

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

    /* Vizonex Note: (I'm deleting this note when the PR is done...)
        Although rather big some developers have asked to subclass StructMeta 
        And with cython being unable to implement metaclasses in a feasable way
        to do what msgspec does it made more sense to me to just shove it in here

        I have need to subclass this object for when I go to make a new library
        called specsql which will have a TableMeta class for adding a tablename but
        also to ensure that it has an attribute __tablename__ or __table__ to identify 
        itself.
        
        SQLAlchemy and SQLModel are both slow and do not have the same serlization capabilities
        and performace speeds that this library can achieve to begin with, hence my desire to 
        add it.
    */

    /* Creates a new StructMeta structure, this can also be used to help subclass 
    StructMeta for other lower-level projects that need StructMeta for Structure 
    creation */
    PyObject* (*StructMeta_New)(
        PyTypeObject *type, 
        PyObject *name, 
        PyObject *bases, 
        PyObject *namespace,
        PyObject *arg_tag_field, 
        PyObject *arg_tag, 
        PyObject *arg_rename,
        int arg_omit_defaults, 
        int arg_forbid_unknown_fields,
        int arg_frozen, 
        int arg_eq, 
        int arg_order, 
        bool arg_kw_only,
        int arg_repr_omit_defaults, 
        int arg_array_like,
        int arg_gc, 
        int arg_weakref, 
        int arg_dict, 
        int arg_cache_hash
    );

    /* Obtains field information good for debugging or other use-cases 
    Returns NULL and raises TypeError if self is not inherited from 
    StructMeta */
    PyObject* (*StructMeta_GetFieldName)(PyObject* self, Py_ssize_t index);
    /* Obtains field information good for debugging or other use-cases returns -1 and raises TypeError if self is not inherited from StructMeta */
    Py_ssize_t (*StuctMeta_GetFieldIndex)(PyObject* self, const char* key, Py_ssize_t key_size);


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
 * Ext                                                                   *
 *************************************************************************/




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


/*************************************************************************
 * StructMeta                                                            *
 *************************************************************************/

static inline int StructMeta_Check(Msgspec_CAPI* api, PyObject* ob){
    return Py_IS_TYPE(ob, api->StructMeta_Type);
}

static inline int StructMeta_CheckExact(Msgspec_CAPI* api, PyObject* ob){
    return Py_IS_TYPE(ob, api->StructMeta_Type) || PyObject_TypeCheck(ob, api->StructMeta_Type);
}



#endif /* MSGSPEC_USE_CAPSULE_API */


#ifdef __cplusplus
}
#endif

#endif // MSGSPEC_H