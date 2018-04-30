/*====================================================================================
 * Copyright (c) 2015, Jesse K Medley
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of The University of Washington nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <Python.h>
#include "structmember.h"

#include "graphfab/core/SagittariusCore.h"
#include "graphfab/sbml/gf_layout.h"
#include "graphfab/layout/gf_fr.h"
#include "graphfab/util/gf_string.h"
#include "graphfab/draw/gf_tikz.h"

#include <stdlib.h>

#define PySWAP(x, y) if(y) { PyObject* tmp__; tmp = x; Py_INCREF(y); x = y; Py_XDECREF(tmp); } else { Py_XDECREF(x); x = NULL; }

// http://python3porting.com/cextensions.html
#ifndef Py_TYPE
    #define Py_TYPE(ob) (((PyObject*)(ob))->ob_type)
#endif

#define STRINGIFY(x) #x
#define EXPAND_AND_STRINGIFY(x) STRINGIFY(x)

static PyObject *SBNWError;

static char* gfPyString_getString(PyObject* uni) {
    char* str = NULL;
//     #pragma message "PYTHON_API_VER = " EXPAND_AND_STRINGIFY(PYTHON_API_VER)
#if PYTHON_API_VER == 3
    PyObject* bytes = PyUnicode_AsUTF8String(uni);
    str = gf_strclone(PyBytes_AsString(bytes));
    Py_XDECREF(bytes);
#else
    str = gf_strclone(PyString_AsString(uni));
#endif
    return str;
}

static int PyCompareString(PyObject* uni, const char* str) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
    {
//         printf("PyCompareString started\n");
    }
    #endif
    {
        char* s = gfPyString_getString(uni);
        int cmp = !strcmp(s,str);
        gf_strfree(s);
        
        if(cmp)
            return 1;
        else
            return 0;
    }
}

PyObject* gfp_PyString_FromString(const char* s) {
#if PYTHON_API_VER == 3
    return PyUnicode_FromString(s);
#else
    return PyString_FromString(s);
#endif
}

// return: new reference
// static PyObject* gfp_PointToPyPoint(gf_point p) {
//     return Py_BuildValue("dd", p.x, p.y);
// }

static gf_point gfp_UnpackPyPoint(PyObject* v) {
    gf_point p;
    // PySequence_GetItem returns a new reference
    // http://docs.python.org/3.4/c-api/sequence.html
    // http://www.python.org/doc/essays/refcnt/
    PyObject* o;
    o = PySequence_GetItem(v,0);
    p.x = PyFloat_AsDouble(o);
    Py_XDECREF(o);
    o = PySequence_GetItem(v,1);
    p.y = PyFloat_AsDouble(o);
    Py_XDECREF(o);
    return p;
}

/// -- point --
typedef struct {
    PyObject_HEAD
    double x;
    double y;
} gfp_Point;

static int gfp_Point_Check(PyObject* p);

static void gfp_Point_dealloc(gfp_Point* self) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("Point dealloc\n");
    #endif
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* gfp_Point_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    gfp_Point* self;
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Point_new called\n");
    #endif
    
    self = (gfp_Point*)type->tp_alloc(type, 0);
    
    return (PyObject*)self;
}

static int gfp_Point_rawinit(gfp_Point *self, gf_point p) {
    if(!gfp_Point_Check((PyObject*)self))
        return -1;

    self->x = p.x;
    self->y = p.y;

    return 0;
}

static int gfp_Point_init(gfp_Point *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"x", "y", NULL};
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Point_init called\n");
    #endif

    // parse args
    double x,y;
    gf_point p;

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "dd", kwlist,
        &x, &y)) {
        PyErr_SetString(SBNWError, "Invalid argument(s)");
        return 1;
    }
    p.x = x;
    p.y = y;
    gfp_Point_rawinit(self, p);
    
    return 0;
}

static PyMemberDef gfp_Point_members[] = {
    {"x", T_DOUBLE, offsetof(gfp_Point,x), 0,
     "x"},
    {"y",  T_DOUBLE, offsetof(gfp_Point,y) , 0,
     "y"},
    {NULL}  /* Sentinel */
};

Py_ssize_t gfp_Point_SeqLength(PyObject *o) {
    if(!gfp_Point_Check((PyObject*)o))
        return 0;
    return 2;
}

static PyObject* gfp_Point_SeqGetItem(gfp_Point *p, Py_ssize_t i) {
    if(!gfp_Point_Check((PyObject*)p))
        return NULL;
//     printf("Offset is %u\n", i);
    if(i == 0)
        return PyFloat_FromDouble(p->x);
    else if(i == 1)
        return PyFloat_FromDouble(p->y);
    else {
//         return PyFloat_FromDouble(-1);
//         PyErr_SetString(SBNWError, "Index out of range");
        return NULL;
    }
}

int gfp_Point_SeqSetItem(gfp_Point *p, Py_ssize_t i, PyObject *v) {
    if(!gfp_Point_Check((PyObject*)p))
        return 0;
    if(i == 0) {
        p->x = PyFloat_AsDouble(v);
        return PyErr_Occurred() ? -1 : 0;
    } else if (i == 1) {
        p->y = PyFloat_AsDouble(v);
        return PyErr_Occurred() ? -1 : 0;
    } else {
        PyErr_SetString(SBNWError, "Index out of range");
        return -1;
    }
}

PyObject* gfp_PointRepr(gfp_Point *self) {
    char s[256];
    sprintf(s, "(%f, %f)", self->x, self->y);
    return gfp_PyString_FromString(s);
}

static PySequenceMethods gfp_PointSeqMethods = {
    (lenfunc)gfp_Point_SeqLength, /* sq_length */
    0, /* sq_concat */
    0, /* sq_repeat */
    (ssizeargfunc)gfp_Point_SeqGetItem, /* sq_item */
    0, /* sq_slice */
    0, /* sq_ass_item */
    (ssizessizeobjargproc)gfp_Point_SeqSetItem, /* sq_ass_slice */
    0, /* sq_contains */
    0, /* sq_inplace_concat */
    0 /* sq_inplace_repeat */
};

static PyTypeObject gfp_PointType = {
#if PYTHON_API_VER == 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,
#endif
    "sbnw.point",      /* tp_name */
    sizeof(gfp_Point),/* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)gfp_Point_dealloc,/* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    (reprfunc)gfp_PointRepr,             /* tp_repr */
    0,                         /* tp_as_number */
    &gfp_PointSeqMethods,       /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "A set of coordinates in 2D space",             /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    0,//Noddy_methods,             /* tp_methods */
    gfp_Point_members,         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)gfp_Point_init,/* tp_init */
    0,                         /* tp_alloc */
    gfp_Point_new,         /* tp_new */
};

static int gfp_Point_Check(PyObject* p) {
    if(Py_TYPE(p) != (PyTypeObject*)&gfp_PointType) {
        PyErr_SetString(SBNWError, "Not a point object");
        return 0;
    } else
        return 1;
}
static PyObject* gfp_PointToPyPoint(gf_point p) {
//     printf("gfp_PointToPyPoint\n");
    gfp_Point* q = (gfp_Point*)PyObject_Call((PyObject*)&gfp_PointType, Py_BuildValue("dd", p.x, p.y), NULL);
    if(gfp_Point_rawinit(q, p) == -1) {
        gfp_Point_dealloc(q);
        return NULL;
    }
    return (PyObject*)q;
}



/// -- transform --
typedef struct {
    PyObject_HEAD
    gf_transform* tf;
    void* dead;
} gfp_Transform;

static int gfp_Transform_Check(PyObject* p);

static void gfp_Transform_dealloc(gfp_Transform* self) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("Transform dealloc\n");
    #endif
    gf_release_transform(self->tf);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* gfp_Transform_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    gfp_Transform* self;
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Transform_new called\n");
    #endif
    
    self = (gfp_Transform*)type->tp_alloc(type, 0);
    
    return (PyObject*)self;
}

static int gfp_Transform_init(gfp_Transform *self, PyObject *args, PyObject *kwds) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Transform_init called\n");
    #endif
    
    return 0;
}

static int gfp_Transform_rawinit(gfp_Transform *self, gf_transform* tf) {
    if(!gfp_Transform_Check((PyObject*)self))
        return -1;

    self->tf = tf;

    return 0;
}

PyObject* gfp_Transform_Call(gfp_Transform *self, PyObject *args, PyObject *kw) {
//     gfp_Point* p = NULL;
    PyObject* o = NULL;
    
    if(!PyArg_ParseTuple(args, "o", o)) {
        PyErr_SetString(SBNWError, "Invalid arguments");
        return NULL;
    }
    
    if(Py_TYPE(o) != &gfp_PointType) {
//         p = (gfp_Point*)o;
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyMemberDef gfp_Transform_members[] = {
    {NULL}  /* Sentinel */
};

PyObject* gfp_Transform_dump(gfp_Transform *self) {
    if(!gfp_Transform_Check((PyObject*)self)) {
        PyErr_SetString(SBNWError, "Not a transform");
        return NULL;
    }
    gf_dump_transform(self->tf);
    return NULL;
}

static PyMethodDef gfp_Transform_methods[] = {
    {"dump", (PyCFunction)gfp_Transform_dump, METH_NOARGS,
     "dump doc"
    },
    {NULL}  /* Sentinel */
};

static PyObject *
gfp_Transform_getScale(gfp_Transform *self, void *closure) {
    return Py_BuildValue("O", gfp_PointToPyPoint(gf_tf_getScale(self->tf)));
}

static int
gfp_Transform_setScale(gfp_Transform *self, PyObject *value, void *closure) {
    return 0;
}

static PyObject *
gfp_Transform_getDisp(gfp_Transform *self, void *closure) {
    return Py_BuildValue("O", gfp_PointToPyPoint(gf_tf_getDisplacement(self->tf)));
}

static int
gfp_Transform_setDisp(gfp_Transform *self, PyObject *value, void *closure) {
    return 0;
}

static PyObject *
gfp_Transform_getPostDisp(gfp_Transform *self, void *closure) {
    return Py_BuildValue("O", gfp_PointToPyPoint(gf_tf_getPostDisplacement(self->tf)));
}

static int
gfp_Transform_setPostDisp(gfp_Transform *self, PyObject *value, void *closure) {
    return 0;
}

static PyGetSetDef gfp_Transform_getseters[] = {
    {"scale",
     (getter)gfp_Transform_getScale, (setter)gfp_Transform_setScale,
     "Get the scale for the transform. ",
     NULL},
    {"disp",
     (getter)gfp_Transform_getDisp, (setter)gfp_Transform_setDisp,
     "Get the displacement of the transform",
     NULL},
    {"postdisp",
     (getter)gfp_Transform_getPostDisp, (setter)gfp_Transform_setPostDisp,
     "Get the displacement of the transform (after transform applied",
     NULL},
    {NULL}  /* Sentinel */
};

static PyTypeObject gfp_TransformType = {
#if PYTHON_API_VER == 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,
#endif
    "sbnw.transform",      /* tp_name */
    sizeof(gfp_Transform),/* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)gfp_Transform_dealloc,/* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    (ternaryfunc)gfp_Transform_Call,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "A 2D affine transformation used for pan/zoom",             /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    gfp_Transform_methods,//Noddy_methods,             /* tp_methods */
    gfp_Transform_members,         /* tp_members */
    gfp_Transform_getseters,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)gfp_Transform_init,/* tp_init */
    0,                         /* tp_alloc */
    gfp_Transform_new,         /* tp_new */
};

static int gfp_Transform_Check(PyObject* p) {
    if(Py_TYPE(p) != (PyTypeObject*)&gfp_TransformType) {
        PyErr_SetString(SBNWError, "Not a transform object");
        return 0;
    } else
        return 1;
}

/// -- compartment --
typedef struct {
    PyObject_HEAD
    gf_compartment c;
    int owning;
    PyObject* dead;
    //TODO: add list of contained elts
} gfp_Compartment;

static void gfp_Compartment_dealloc(gfp_Compartment* self) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("Compartment dealloc\n");
    #endif
    if(self->owning)
      gf_releaseCompartment(&self->c);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* gfp_Compartment_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    gfp_Compartment* self;
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Compartment_new called\n");
    #endif
    
    self = (gfp_Compartment*)type->tp_alloc(type, 0);
    if(self) {
        self->dead = NULL;
    }

    self->owning = 0;
    
    return (PyObject*)self;
}

static int gfp_Compartment_init(gfp_Compartment *self, PyObject *args, PyObject *kwds) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Compartment_init called\n");
    #endif
    
    return 0;
}

static int gfp_Compartment_rawinit(gfp_Compartment *self, gf_compartment c) {
    
    self->c = c;
    
    return 0;
}

static PyObject* gfp_Compartment_getAttro(gfp_Compartment *self, PyObject *attr) {
    if(PyCompareString(attr, "min")) {
        gf_point p = gf_compartment_getMinCorner(&self->c);
        return gfp_PointToPyPoint(p);
    } else if(PyCompareString(attr, "max")) {
        gf_point p = gf_compartment_getMaxCorner(&self->c);
        return gfp_PointToPyPoint(p);
    } else if(PyCompareString(attr, "width")) {
        return Py_BuildValue("d", gf_compartment_getWidth(&self->c));
    } else if(PyCompareString(attr, "height")) {
        return Py_BuildValue("d", gf_compartment_getHeight(&self->c));
    } else if(PyCompareString(attr, "numelt")) {
        return PyLong_FromSize_t(gf_compartment_getNumElt(&self->c));
    } else
        return PyObject_GenericGetAttr((PyObject *)self, attr);
}

static int gfp_Compartment_SetAttro(gfp_Compartment* self, PyObject* attr, PyObject* v) {
    return PyObject_GenericSetAttr((PyObject*)self, attr, v);;
}

static PyMemberDef gfp_Compartment_members[] = {
    {"min", T_OBJECT_EX, offsetof(gfp_Compartment,dead), READONLY,
     "min"},
    {"max",  T_OBJECT_EX, offsetof(gfp_Compartment,dead) , READONLY,
     "max"},
    {"numelt", T_OBJECT_EX, offsetof(gfp_Compartment,dead), READONLY,
     "numelt"},
    {"width", T_OBJECT_EX, offsetof(gfp_Compartment,dead), READONLY,
     "width"},
    {"height", T_OBJECT_EX, offsetof(gfp_Compartment,dead), READONLY,
     "height"},
    {NULL}  /* Sentinel */
};

static PyTypeObject gfp_CompartmentType = {
#if PYTHON_API_VER == 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,
#endif
    "sbnw.compartment",      /* tp_name */
    sizeof(gfp_Compartment),/* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)gfp_Compartment_dealloc,/* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    (getattrofunc)gfp_Compartment_getAttro,                         /* tp_getattro */
    (setattrofunc)gfp_Compartment_SetAttro,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "An SBML compartment which may contain species/reactions etc.",             /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    0,//Noddy_methods,             /* tp_methods */
    gfp_Compartment_members,         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)gfp_Compartment_init,/* tp_init */
    0,                         /* tp_alloc */
    gfp_Compartment_new,         /* tp_new */
};

/// -- Reaction --
typedef struct {
    PyObject_HEAD
    gf_reaction r;
    int owning;
    // all lists
//     PyObject* spec;
    PyObject* curv;
    PyObject* custom;
} gfp_Rxn;

static void gfp_Rxn_dealloc(gfp_Rxn* self) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("Rxn dealloc\n");
    #endif
    if(self->owning)
      gf_releaseRxn(&self->r);
    //FIXME: decref for custom
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* gfp_Rxn_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    gfp_Rxn* self;
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Rxn_new called\n");
    #endif
    
    self = (gfp_Rxn*)type->tp_alloc(type, 0);
    if(self) {
//         self->spec = NULL;
        self->curv  = NULL;
        self->custom = NULL;
    }

    self->owning = 0;
    
    return (PyObject*)self;
}

static int gfp_Rxn_init(gfp_Rxn *self, PyObject *args, PyObject *kwds) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Rxn_init called\n");
    #endif
    
    return 0;
}

static PyObject *
gfp_Rxn_getCurves(gfp_Rxn *self, void *closure);

static int gfp_Rxn_rawinit(gfp_Rxn *self, gf_reaction r, PyObject* speclist) {
    // speclist is list of all nodes in network
//     size_t numspec;
//     size_t i, numcurv;

    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("rxn raw init\n");
    #endif
    
    self->r = r;
//     numspec = gf_reaction_getNumSpec  (&self->r);
//     fprintf(stderr, "  rxn get num curves\n");
//     numcurv = gf_reaction_getNumCurves(&self->r);
//     fprintf(stderr, "  rxn get num curves %zu\n", numcurv);

//     self->spec = PyList_New(numspec);
//     self->curv = PyList_New(numcurv);

//     for(i=0; i<numspec; ++i) {
//         gf_specRole role = gf_reaction_getSpecRole(&r, i);
//         const char* rolestr = gf_roleToStr(role);
//
//         PyObject* spec = PySequence_GetItem(speclist, gf_reaction_specGeti(&r, i));
//
//         PyList_SetItem(self->spec, i, Py_BuildValue("Os", spec, rolestr)); // would need incref
//     }

//     for(i=0; i<numcurv; ++i) {
// //         fprintf(stderr, "  rxn get curve\n");
//         gf_curve c = gf_reaction_getCurve(&r, i);
// //         fprintf(stderr, "  rxn get curve cps\n");
//         gf_curveCP cp = gf_getCurveCPs(&c);
// //         fprintf(stderr, "  rxn get curve cps done\n");
// //         printf("cp %f,%f %f,%f %f,%f %f,%f\n", cp.s.x, cp.s.y, cp.c1.x, cp.c1.y, cp.c2.x, cp.c2.y, cp.e.x, cp.e.y);
//
//         // role
//         const char* role = gf_roleToStr(gf_curve_getRole(&c));
//
//         PyList_SetItem(self->curv, i, Py_BuildValue("OOOO O",
//             gfp_PointToPyPoint(cp.s),
//             gfp_PointToPyPoint(cp.c1),
//             gfp_PointToPyPoint(cp.c2),
//             gfp_PointToPyPoint(cp.e),
//             gfp_PyString_FromString(role)
//         ));
//     }
//     fprintf(stderr, "  rxn raw init done\n");

    self->curv = gfp_Rxn_getCurves(self, NULL);

    return 0;
}

static PyMemberDef gfp_Rxn_members[] = {
//     {"spec",  T_OBJECT_EX, offsetof(gfp_Rxn,spec) , 0,
//      "The species involved in the reaction. "
//      "This attribute is **READ ONLY**. "
//      "If you want to change the reaction species, you must create a new "
//      "reaction and delete the old one\n"
//     },
    {"custom", T_OBJECT_EX, offsetof(gfp_Rxn,custom), 0,
     "Custom user data"},
    {NULL}  /* Sentinel */
};

static PyObject *
gfp_Rxn_getCurves(gfp_Rxn *self, void *closure) {
    size_t numcurv = gf_reaction_getNumCurves(&self->r);
    PyObject* curv = PyList_New(numcurv);
    size_t i;
    unsigned int arrow_n, k;
    gf_point* arrow_v;
    PyObject* arrow;

    
    for(i=0; i<numcurv; ++i) {
        gf_curve c = gf_reaction_getCurve(&self->r, i);
        gf_curveCP cp = gf_getCurveCPs(&c);
//         printf("cp %f,%f %f,%f %f,%f %f,%f\n", cp.s.x, cp.s.y, cp.c1.x, cp.c1.y, cp.c2.x, cp.c2.y, cp.e.x, cp.e.y);
        
        // role
        const char* role = gf_roleToStr(gf_curve_getRole(&c));
//         fprintf(stderr, "gfp_Rxn_getCurves, role = %s\n", role);

        if(gf_curve_hasArrowhead(&c)) {
          // arrowheads
          gf_curve_getArrowheadVerts(&c, &arrow_n, &arrow_v);

          arrow = PyList_New(arrow_n);
//           fprintf(stderr, "gfp_Rxn_getCurves, arrow_n = %u\n", arrow_n);

          for(k=0; k<arrow_n; ++k)
            PyList_SetItem(arrow, k, Py_BuildValue("O", gfp_PointToPyPoint(arrow_v[k])));

          gf_free(arrow_v);

          PyList_SetItem(curv, i, Py_BuildValue("OOOO O O",
              gfp_PointToPyPoint(cp.s),
              gfp_PointToPyPoint(cp.c1),
              gfp_PointToPyPoint(cp.c2),
              gfp_PointToPyPoint(cp.e),
              // role
              gfp_PyString_FromString(role),
              // arrowhead verts
              arrow
          ));
        } else {
          PyList_SetItem(curv, i, Py_BuildValue("OOOO O",
              gfp_PointToPyPoint(cp.s),
              gfp_PointToPyPoint(cp.c1),
              gfp_PointToPyPoint(cp.c2),
              gfp_PointToPyPoint(cp.e),
              // role
              gfp_PyString_FromString(role)
          ));
        }
    }
    
    return curv;
}

static int
gfp_Rxn_setCurves(gfp_Rxn *self, PyObject *value, void *closure) {
    return 0;
}

// reaction.centroid
static PyObject *gfp_Rxn_getCentroid(gfp_Rxn *self, void *closure) {
    return gfp_PointToPyPoint(gf_reaction_getCentroid(&self->r));
}

static int gfp_Rxn_setCentroid(gfp_Rxn *self, PyObject *value, void *closure) {
    gf_reaction_setCentroid(&self->r, gfp_UnpackPyPoint(value));
    return 0;
}

static PyObject* gfp_Rxn_recenter(gfp_Rxn *self, PyObject *args, PyObject *kwds) {
    gf_reaction_recenter(&self->r);

    Py_RETURN_NONE;
}

static PyObject* gfp_Rxn_recalccps(gfp_Rxn *self, PyObject *args, PyObject *kwds) {
    gf_reaction_recalcCurveCPs(&self->r);

    Py_RETURN_NONE;
}

/// Defined after node
static PyObject* gfp_Rxn_has(gfp_Rxn *self, PyObject *args, PyObject *kwds);

static PyGetSetDef gfp_Rxn_getseters[] = {
    {"curves",
     (getter)gfp_Rxn_getCurves, (setter)gfp_Rxn_setCurves,
     "Get the curves for the reaction. This attribute is **READ ONLY**. "
     "If you want to change the reaction species, you must create a new "
     "reaction and delete the old one\n",
     NULL},
    {"centroid",
     (getter)gfp_Rxn_getCentroid, (setter)gfp_Rxn_setCentroid,
     "Reaction centroid",
     NULL},
    {NULL}  /* Sentinel */
};

static PyMethodDef gfp_Rxn_methods[] = {
    {"recenter", (PyCFunction)gfp_Rxn_recenter, METH_NOARGS,
     "Recalculate the reaction centroid"
    },
    {"recalccps", (PyCFunction)gfp_Rxn_recalccps, METH_NOARGS,
     "Recalculate the curve cps, don't recenter"
    },
    {"has", (PyCFunction)gfp_Rxn_has, METH_VARARGS | METH_KEYWORDS,
     "Return true if a given node is in this reaction\n\n"
     ":param node: A node\n"
    },
    {NULL}  /* Sentinel */
};

static PyTypeObject gfp_RxnType = {
#if PYTHON_API_VER == 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,
#endif
    "sbnw.reaction",      /* tp_name */
    sizeof(gfp_Rxn),/* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)gfp_Rxn_dealloc,/* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "A SBML reaction with substrate/product/modifier curves",            /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    gfp_Rxn_methods,           /* tp_methods */
    gfp_Rxn_members,           /* tp_members */
    gfp_Rxn_getseters,         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)gfp_Rxn_init,/* tp_init */
    0,                         /* tp_alloc */
    gfp_Rxn_new,         /* tp_new */
};

/// -- node --
typedef struct {
    PyObject_HEAD
    gf_node n;
    int owning;
    PyObject* dead;
    PyObject* custom;
} gfp_Node;

static void gfp_Node_dealloc(gfp_Node* self) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("Node dealloc %p\n", self->n.n);
    #endif
    if(self->owning)
      gf_releaseNode(&self->n);
    //FIXME: decref for custom
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* gfp_Node_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    gfp_Node* self;
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Node_new called\n");
    #endif
    
    self = (gfp_Node*)type->tp_alloc(type, 0);
    if(self) {
        self->dead = NULL;
        self->custom = NULL;
    } else {
        PyErr_SetString(SBNWError, "Failed to construct node");
        return NULL;
    }

    self->owning = 1;
    
    return (PyObject*)self;
}

static int gfp_Node_init(gfp_Node *self, PyObject *args, PyObject *kwds) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Node_init called\n");
    #endif
    
    return 0;
}

static int gfp_Node_rawinit(gfp_Node *self, gf_node n) {
    self->n = n;
    
    return 0;
}

// static PyObject* gfp_Node_GetAttro(gfp_Node *self, PyObject *attr) {
//     if(PyCompareString(attr, "centroid"))
//         return gfp_PointToPyPoint(gf_node_getCentroid(&self->n));
//     else
//         return PyObject_GenericGetAttr((PyObject *)self, attr);
// }

// static int gfp_Node_SetAttro(gfp_Node* self, PyObject* attr, PyObject* v) {
//     int result;
// 
//     if(PyCompareString(attr, "centroid")) {
//         gf_node_setCentroid(&self->n, gfp_UnpackPyPoint(v));
//         return 0;
//     }
// 
//     result = PyObject_GenericSetAttr((PyObject*)self, attr, v);
// 
//     return result;
// }

// is locked?
static PyObject *gfp_Node_isLocked(gfp_Node *self, void *closure) {
    return PyBool_FromLong(gf_node_isLocked(&self->n));
}

// lock
static PyObject *gfp_Node_Lock(gfp_Node *self, void *closure) {
    gf_node_lock(&self->n);
    Py_RETURN_NONE;
}

// unlock
static PyObject *gfp_Node_Unlock(gfp_Node *self, void *closure) {
    gf_node_unlock(&self->n);
    Py_RETURN_NONE;
}

// is aliased?
static PyObject *gfp_Node_isAliased(gfp_Node *self, void *closure) {
    return PyBool_FromLong(gf_node_isAliased(&self->n));
}

// node.centroid
static PyObject *gfp_Node_getCentroid(gfp_Node *self, void *closure) {
    return gfp_PointToPyPoint(gf_node_getCentroid(&self->n));
}

static int gfp_Node_setCentroid(gfp_Node *self, PyObject *value, void *closure) {
//     printf("What do you think you are doing?\n");
    gf_node_setCentroid(&self->n, gfp_UnpackPyPoint(value));
    return 0;
}

// node.width
static PyObject *gfp_Node_getWidth(gfp_Node *self, void *closure) {
    return PyFloat_FromDouble(gf_node_getWidth(&self->n));
}

static int gfp_Node_setWidth(gfp_Node *self, PyObject *value, void *closure) {
    gf_node_setWidth(&self->n, PyFloat_AsDouble(value));
    return 0;
}

// node.height
static PyObject *gfp_Node_getHeight(gfp_Node *self, void *closure) {
    return PyFloat_FromDouble(gf_node_getHeight(&self->n));
}

static int gfp_Node_setHeight(gfp_Node *self, PyObject *value, void *closure) {
    gf_node_setHeight(&self->n, PyFloat_AsDouble(value));
    return 0;
}

// node.name
static PyObject *gfp_Node_getName(gfp_Node *self, void *closure) {
#if PYTHON_API_VER == 3
    return PyUnicode_FromString(gf_node_getName(&self->n));
#else
    return PyString_FromString(gf_node_getName(&self->n));
#endif
}

static int gfp_Node_setName(gfp_Node *self, PyObject *value, void *closure) {
    printf("Cannot set name\n");
    return 0;
}

// node.id
static PyObject *gfp_Node_getId(gfp_Node *self, void *closure) {
#if PYTHON_API_VER == 3
    return PyUnicode_FromString(gf_node_getID(&self->n));
#else
    return PyString_FromString(gf_node_getID(&self->n));
#endif
}

static int gfp_Node_setId(gfp_Node *self, PyObject *value, void *closure) {
    printf("Cannot set id\n");
    return 0;
}

static PyMethodDef gfp_Node_methods[] = {
    {"islocked", (PyCFunction)gfp_Node_isLocked, METH_NOARGS,
     "Check if the node is locked"
    },
    {"lock", (PyCFunction)gfp_Node_Lock, METH_NOARGS,
     "Lock the node"
    },
    {"unlock", (PyCFunction)gfp_Node_Unlock, METH_NOARGS,
     "Unlock the node"
    },
    {"isaliased", (PyCFunction)gfp_Node_isAliased, METH_NOARGS,
     "Check if the node is aliased"
    },
    {NULL}  /* Sentinel */
};

static PyGetSetDef gfp_Node_getseters[] = {
    {"centroid",
     (getter)gfp_Node_getCentroid, (setter)gfp_Node_setCentroid,
     "Node centroid",
     NULL},
    {"width",
     (getter)gfp_Node_getWidth, (setter)gfp_Node_setWidth,
     "Node width",
     NULL},
    {"height",
     (getter)gfp_Node_getHeight, (setter)gfp_Node_setHeight,
     "Node height",
     NULL},
    {"name",
     (getter)gfp_Node_getName, (setter)gfp_Node_setName,
     "Node name",
     NULL},
    {"id",
     (getter)gfp_Node_getId, (setter)gfp_Node_setId,
     "Node id",
     NULL},
    {NULL}  /* Sentinel */
};

static PyMemberDef gfp_Node_members[] = {
//     {"centroid", T_OBJECT_EX, offsetof(gfp_Node,dead), READONLY,
//      "Layout attribute: The 2D centroid of the node. It is used by the layout algorithm and is set when layout information is present"},
//     {"width",  T_OBJECT_EX, offsetof(gfp_Node,dead) , READONLY,
//      "Layout attribute: The width of the node's bounding box"},
//     {"height", T_OBJECT_EX, offsetof(gfp_Node,dead), READONLY,
//      "Layout attribute: The height of the node's bounding box"},
    {"custom", T_OBJECT_EX, offsetof(gfp_Node,custom), 0,
     "Custom user data"},
    {NULL}  /* Sentinel */
};

static PyTypeObject gfp_NodeType = {
#if PYTHON_API_VER == 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,
#endif
    "sbnw.node",      /* tp_name */
    sizeof(gfp_Node),/* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)gfp_Node_dealloc,/* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0, //(getattrofunc)gfp_Node_GetAttro,         /* tp_getattro */
    0, //(setattrofunc)gfp_Node_SetAttro,         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "A node in the layout engine represents a (chemical) species "             /* tp_doc */
    "in the SBML model. The nodes will be given initial coordinates if "
    "layout information is present in the original SBML model.",
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    gfp_Node_methods,          /* tp_methods */
    gfp_Node_members,         /* tp_members */
    gfp_Node_getseters,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)gfp_Node_init,/* tp_init */
    0,                         /* tp_alloc */
    gfp_Node_new,         /* tp_new */
};

static PyObject* gfp_Rxn_has(gfp_Rxn *self, PyObject *args, PyObject *kwds) {
//     fr_options opt;
    gfp_Node* node=NULL;
    static char *kwlist[] = {"node", NULL};

    // parse args
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "|O!", kwlist,
        &gfp_NodeType, &node
    )) {
        PyErr_SetString(SBNWError, "Invalid argument(s)");
        return NULL;
    }

    int result = gf_reaction_hasSpec(&self->r, &node->n);
    if(result) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

/// -- canvas --
typedef struct {
    PyObject_HEAD
    gf_canvas c;
    unsigned long width;
    unsigned long height;
} gfp_Canvas;

static void gfp_Canvas_dealloc(gfp_Canvas* self) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("Canvas dealloc\n");
    #endif
    gf_releaseCanvas(&self->c);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* gfp_Canvas_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    gfp_Canvas* self;
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Canvas_new called\n");
    #endif
    
    self = (gfp_Canvas*)type->tp_alloc(type, 0);
    if(self) {
        gf_clearCanvas(&self->c);
    }
    
    return (PyObject*)self;
}

static int gfp_Canvas_init(gfp_Canvas *self, PyObject *args, PyObject *kwds) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Canvas_init called\n");
    #endif
    
    return 0;
}

static int gfp_Canvas_rawinit(gfp_Canvas *self, gf_canvas c) {
    self->c = c;
    self->width  = gf_canvGetWidth (&self->c);
    self->height = gf_canvGetHeight(&self->c);
    
    return 0;
}

// if width/height have changed, propagate
static void gfp_Canvas_UpdateBounds(gfp_Canvas* self) {
    gf_canvSetWidth(&self->c, self->width);
    gf_canvSetHeight(&self->c, self->height);
}

static int gfp_Canvas_SetAttr(gfp_Canvas* self, PyObject* attr, PyObject* v) {
    int result = PyObject_GenericSetAttr((PyObject*)self, attr, v);
    gfp_Canvas_UpdateBounds(self);
    return result;
}

static PyMemberDef gfp_Canvas_members[] = {
    {"width", T_ULONG, offsetof(gfp_Canvas,width), 0,
     "width"},
    {"height", T_ULONG, offsetof(gfp_Canvas,height), 0,
     "height"},
    {NULL}  /* Sentinel */
};

static PyTypeObject gfp_CanvasType = {
#if PYTHON_API_VER == 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,
#endif
    "sbnw.canvas",      /* tp_name */
    sizeof(gfp_Canvas),/* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)gfp_Canvas_dealloc,/* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    (setattrofunc)gfp_Canvas_SetAttr,        /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "The canvas (2D rectangle) on which the layout is applied",                  /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    0,//Noddy_methods,             /* tp_methods */
    gfp_Canvas_members,         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)gfp_Canvas_init,/* tp_init */
    0,                         /* tp_alloc */
    gfp_Canvas_new,         /* tp_new */
};

/// -- network --
typedef struct {
    PyObject_HEAD
    gf_network n;
    // all lists
    PyObject* nodes;
    PyObject* rxns;
    PyObject* comps;

    // added to condense object hierarchy
    gf_layoutInfo* l;
    gfp_Canvas*  canv;
} gfp_Network;

static void gfp_Network_dealloc(gfp_Network* self) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("Network dealloc\n");
    #endif

    gf_releaseNetwork(&self->n);

    Py_XDECREF(self->nodes);
    Py_XDECREF(self->rxns);
    Py_XDECREF(self->comps);

    Py_XDECREF(self->canv);

    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* gfp_Network_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    gfp_Network* self;
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Network_new called\n");
    #endif
    
    self = (gfp_Network*)type->tp_alloc(type, 0);
    if(self) {
        gf_clearNetwork(&self->n);
        self->nodes = NULL;
        self->rxns  = NULL;
        self->comps = NULL;

        self->canv = NULL;
    }
    
    return (PyObject*)self;
}

static int gfp_Network_init(gfp_Network *self, PyObject *args, PyObject *kwds) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Network_init called\n");
    #endif
    
    return 0;
}

static int gfp_Network_rawinit(gfp_Network *self, gf_network n, gf_layoutInfo* l) {
    size_t i, numnodes, numrxns, numcomps;
    
    self->n = n;

    self->l = l;
    
    numnodes = gf_nw_getNumNodes(&self->n);
    numrxns  = gf_nw_getNumRxns (&self->n);
    numcomps = gf_nw_getNumComps(&self->n);
    
    if(self->nodes)
      Py_XDECREF(self->nodes);
    if(self->rxns)
      Py_XDECREF(self->rxns);
    if(self->comps)
      Py_XDECREF(self->comps);

    self->nodes = PyTuple_New(numnodes);
    self->rxns  = PyTuple_New(numrxns);
    self->comps = PyTuple_New(numcomps);
    
//     fprintf(stderr, "layout raw init nodes\n");
    for(i=0; i<numnodes; ++i) {
        gfp_Node* o = (gfp_Node*)PyObject_Call((PyObject*)&gfp_NodeType, PyTuple_New(0), NULL);
        PyTuple_SetItem(self->nodes, i, (PyObject*)o);
        if(gfp_Node_rawinit(o, gf_nw_getNode(&self->n, i)))
            return 1;
    }
    
//     fprintf(stderr, "layout raw init rxns\n");
    for(i=0; i<numrxns; ++i) {
        gfp_Rxn* o = (gfp_Rxn*)PyObject_Call((PyObject*)&gfp_RxnType, PyTuple_New(0), NULL);
        PyTuple_SetItem(self->rxns, i, (PyObject*)o);
        if(gfp_Rxn_rawinit(o, gf_nw_getRxn(&self->n, i), self->nodes))
            return 1;
    }
    
//     fprintf(stderr, "layout raw init comps\n");
    for(i=0; i<numcomps; ++i) {
        gfp_Compartment* o = (gfp_Compartment*)PyObject_Call((PyObject*)&gfp_CompartmentType, PyTuple_New(0), NULL);
        PyTuple_SetItem(self->comps, i, (PyObject*)o);
        if(gfp_Compartment_rawinit(o, gf_nw_getCompartment(&self->n, i)))
            return 1;
    }

//     fprintf(stderr, "layout raw init done\n");
    
    return 0;
}

static int gfp_Network_SetAttr(gfp_Network* self, PyObject* attr, PyObject* v) {
    int result = PyObject_GenericSetAttr((PyObject*)self, attr, v);
    return result;
}

static PyObject* gfp_NetworkRandomizeLayout(gfp_Network *self, PyObject *args, PyObject *kwds) {
    gfp_Canvas* canvas=NULL;
    static char *kwlist[] = {"canvas", NULL};
    static char *kwlist_coords[] = {"left", "top", "right", "bottom", NULL};

    double left=0., top=0., right=1000., bottom=1000.;
    int use_coords = 0; // if false use canvas, else use coords

    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_NetworkRandomizeLayout called\n");
    #endif
    
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "O!", kwlist, &gfp_CanvasType, &canvas)) {
      PyErr_Clear();
      if(!PyArg_ParseTupleAndKeywords(args, kwds, "|dddd", kwlist_coords, &left, &top, &right, &bottom)) {
//         printf("could not parse the dubs\n");
        PyErr_SetString(SBNWError, "Invalid arguments");
        return NULL;
      } else {
//         printf("parsed the dubs\n");
        use_coords = 1;
        // PyArg_ParseTupleAndKeywords sets an error whether you want it to or not
        PyErr_Clear();
      }
    }
    if(!use_coords) {

      AN(canvas, "No canvas");
      Py_INCREF(canvas);
      // this is rendered unnecessary by "O!" above
      if(Py_TYPE(canvas) != &gfp_CanvasType) {
          PyErr_SetString(PyExc_TypeError, "Expected sbnw.canvas type");
          return NULL;
      }
      gf_randomizeLayout2(&self->n, &canvas->c);

      Py_XDECREF(canvas);

    } else {

      gf_randomizeLayout_fromExtents(&self->n, left, top, right, bottom);

    }
    
    Py_RETURN_NONE;
}

static PyObject* gfp_NetworkAutolayout(gfp_Network *self, PyObject *args, PyObject *kwds) {
    fr_options opt;
    gfp_Canvas* canvas=NULL;
    gf_canvas* c = NULL;
    //PyObject *k, *boundary, *mag, *grav, *bary, *autobary, *enablecomps, *prerandomize;
    PyObject* bary=NULL;
    static char *kwlist[] = {"canvas", "k", "boundary", "mag", "grav", "bary", 
        "autobary", "enablecomps", "prerandomize", NULL};
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_NetworkAutolayout called\n");
    #endif
    // set defaults
    gf_getLayoutOptDefaults(&opt);
    
    // parse args
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "|O!" GF_PYREALFMT "ii" GF_PYREALFMT "Oiii", kwlist, 
        &gfp_CanvasType, &canvas, &opt.k, &opt.boundary, &opt.mag, &opt.grav, &bary, &opt.autobary, &opt.enable_comps, &opt.prerandomize
    )) {
        PyErr_SetString(SBNWError, "Invalid argument(s)");
        return NULL;
    }
    // unpack bary point object
    if(bary) {
        opt.baryx = gfp_UnpackPyPoint(bary).x;
        opt.baryy = gfp_UnpackPyPoint(bary).y;
    }
    
    if(canvas)
        c = &canvas->c;
    
    gf_doLayoutAlgorithm2(opt, &self->n, c);
    
    Py_RETURN_NONE;
}

static PyObject* gfp_NetworkRebuildCurves(gfp_Network *self, PyObject *args, PyObject *kwds) {
    gf_nw_rebuildCurves(&self->n);
    
    Py_RETURN_NONE;
}

static PyObject* gfp_NetworkRecenterJunctions(gfp_Network *self, PyObject *args, PyObject *kwds) {
    gf_nw_recenterJunctions(&self->n);
    
    Py_RETURN_NONE;
}

// steals a reference to value
static PyObject* gfp_ExtendPyTuple(PyObject *tuple, PyObject *value) {
    size_t i, size = PyTuple_Size(tuple);
    PyObject* newtuple = PyTuple_New(size+1);
    if(!newtuple)
        return NULL;
    
    for(i=0; i<size; ++i) {
        PyObject* x = PyTuple_GetItem(tuple, i);
        Py_INCREF(x);
        if(PyTuple_SetItem(newtuple, i, x)) {
            Py_XDECREF(newtuple);
            return NULL;
        }
    }
    
    if(PyTuple_SetItem(newtuple, size, value)) {
        Py_XDECREF(newtuple);
        return NULL;
    }
    
    return newtuple;
}

// Truncates the tuple by removing one element
static PyObject* gfp_TruncatePyTuple1(PyObject *tuple, PyObject *value) {
    size_t i, j, size = PyTuple_Size(tuple);
    PyObject* newtuple = PyTuple_New(size-1);
    if(!newtuple)
        return NULL;
    
    for(i=0,j=0; i<size; ++i) {
        PyObject* x = PyTuple_GetItem(tuple, i);
        AN(j == i || j+1 == i, "Invariant broken");
        if(x != value) {
            Py_INCREF(x);
            if(PyTuple_SetItem(newtuple, j++, x)) {
                Py_XDECREF(newtuple);
                return NULL;
            }
        }
    }
    
    return newtuple;
}

static PyObject* gfp_NetworkNewNode(gfp_Network *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"name", "id", "compartment", NULL};
    const char* id=NULL;
    const char* name=NULL;
    gfp_Compartment* comp=NULL;
    gf_compartment* c=NULL;
	gf_node node;

    #if SAGITTARIUS_DEBUG_LEVEL >= 2
    printf("gfp_NetworkNewNode called\n");
    #endif
    
    // parse args
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "s|sO!", kwlist, 
        &name, &id, &gfp_CompartmentType, &comp)) {
        PyErr_SetString(SBNWError, "Invalid argument(s)");
        return NULL;
    }
    
    if(comp)
        c = &comp->c;
    
    node = gf_nw_newNode(&self->n, id, name, c);
    printf("gf_nw_newNode returned\n");
    if(node.n) {
        gfp_Node* o = (gfp_Node*)PyObject_Call((PyObject*)&gfp_NodeType, PyTuple_New(0), NULL);
        Py_INCREF(o); // because we are returning it
        if(!gfp_Node_rawinit(o, node)) {
            PyObject* newnodes = gfp_ExtendPyTuple(self->nodes, (PyObject*)o); // steals a reference to o
            if(newnodes) {
                Py_XDECREF(self->nodes);
                self->nodes = newnodes;
                printf("new node refcnt: %lu\n", ((PyObject*)o)->ob_refcnt);
                return (PyObject*)o;
            }
        }
        // failed
        Py_XDECREF(o);
    }
    
    PyErr_SetString(SBNWError, "Failed to create node");
    return NULL;
}

void gfp_Network_RemoveReaction(gfp_Network *self, gfp_Rxn* rxn) {
    gf_nw_removeRxn(&self->n, &rxn->r);
    
    PyObject* newrxns = gfp_TruncatePyTuple1(self->rxns, (PyObject*)rxn); // steals a reference to o
    if(newrxns) {
        Py_XDECREF(self->rxns);
        self->rxns = newrxns;
        
        printf("Done removing reaction.\n");
    }
}

void gfp_Network_TrimReactions(gfp_Network *self) {
    size_t numrxns, i;
    numrxns  = gf_nw_getNumRxns (&self->n);
    AT(numrxns == PyTuple_Size(self->rxns), "Synchronization error");
    
    for(i=0; i<PyTuple_Size(self->rxns); ++i) {
        gfp_Rxn* rxn = (gfp_Rxn*)PyTuple_GetItem(self->rxns, i);
        printf("num spec: %lu\n", gf_reaction_getNumSpec(&rxn->r));
        if(gf_reaction_getNumSpec(&rxn->r) <= 1) {
            gfp_Network_RemoveReaction(self, rxn);
        }
    }
}

static PyObject* gfp_NetworkRemoveNode(gfp_Network *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"node", NULL};
    gfp_Node* node=NULL;
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
    printf("gfp_NetworkRemoveNode called\n");
    #endif
    
    // parse args
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "O!", kwlist, &gfp_NodeType, &node)) {
        PyErr_SetString(SBNWError, "Invalid argument(s)");
        return NULL;
    }
    
    printf("Trying to remove node...\n");
    
    if(gf_nw_removeNode(&self->n, &node->n)) {
        PyErr_SetString(SBNWError, "Unable to remove node (may not be member of network)");
        return NULL;
    }
    
    // truncate nodes
    PyObject* newnodes = gfp_TruncatePyTuple1(self->nodes, (PyObject*)node); // steals a reference to o
    if(newnodes) {
        Py_XDECREF(self->nodes);
        self->nodes = newnodes;
        
        gfp_Network_TrimReactions(self);
        
        printf("node %p refcnt: %lu\n", node->n.n, ((PyObject*)node)->ob_refcnt);
//         Py_XDECREF(node);
        
        printf("Done removing node.\n");
        
        Py_RETURN_NONE;
    } else {
        PyErr_SetString(SBNWError, "Unable to create new consistent state after removing node");
        return NULL;
    }
}

static PyObject* gfp_NetworkAliasNode(gfp_Network *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"node", NULL};
    gfp_Node* node=NULL;
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
    printf("gfp_NetworkAliasNode called\n");
    #endif

    // parse args
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "O!", kwlist, &gfp_NodeType, &node)) {
        PyErr_SetString(SBNWError, "Invalid argument(s)");
        return NULL;
    }

    printf("Trying to alias node...\n");

    if(gf_node_alias(&node->n, &self->n)) {
        PyErr_SetString(SBNWError, "Unable to alias node (may not be member of network)");
        return NULL;
    }

    gfp_Network_rawinit(self, self->n, self->l);

    Py_RETURN_NONE;
}

// is locked?
static PyObject *gfp_Network_isLayoutSpecified(gfp_Network *self, void *closure) {
    return PyBool_FromLong(gf_nw_isLayoutSpecified(&self->n));
}

PyObject* gfp_Network_FitToWindow(gfp_Network *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"left", "top", "right", "bottom", NULL};
    double left, top, right, bottom;

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "dddd", kwlist, &left, &top, &right, &bottom)) {
        return NULL;
    }

    gf_fit_to_window(self->l, left, top, right, bottom);

    Py_RETURN_NONE;
}

static PyMethodDef Network_methods[] = {
    {"randomize", (PyCFunction)gfp_NetworkRandomizeLayout, METH_VARARGS | METH_KEYWORDS,
     "Randomize the layout\n\n"
//      ":param canvas: The layout canvas\n"
     ":param float left:The minimum X coord of the bounding box\n"
     ":param float top: The minimum Y coord of the bounding box\n"
     ":param float right: The maximum X coord of the bounding box\n"
     ":param float bottom: The maximum Y coord of the bounding box\n"
    },
    {"autolayout", (PyCFunction)gfp_NetworkAutolayout, METH_VARARGS | METH_KEYWORDS,
     "Run the FR algorithm\n\n"
     ":param canvas: The layout canvas\n"
     ":param float k: The stiffness\n"
     ":param int boundary: Use boundary\n"
     ":param int mag: Use magnetism\n"
     ":param float grav: Gravity value\n"
     ":param int bary: Use barycenter\n"
     ":param int autobary: Use autobary\n"
     ":param int comps: Enable compartments (leave off)\n"
     ":param int prerand: Pre-randomize\n"
    },
    {"rebuildcurves", (PyCFunction)gfp_NetworkRebuildCurves, METH_NOARGS,
     "Rebuild the curves for changed node positions"
    },
    {"recenterjunct", (PyCFunction)gfp_NetworkRecenterJunctions, METH_NOARGS,
     "Recenter reaction junctions for changed node positions (you do not have to also call rebuildcurves)"
    },
    {"newnode", (PyCFunction)gfp_NetworkNewNode, METH_VARARGS | METH_KEYWORDS,
     "Add a node to the network\n\n"
     ":param str id: The node name\n"
     ":param str id: The node id\n"
     ":param comp: The compartment to use (optional)\n"
    },
    {"removenode", (PyCFunction)gfp_NetworkRemoveNode, METH_VARARGS | METH_KEYWORDS,
     "Remove a node from the network\n\n"
     ":param node: The node to remove\n"
    },
    {"aliasnode", (PyCFunction)gfp_NetworkAliasNode, METH_VARARGS | METH_KEYWORDS,
     "Alias a node\n\n"
     ":param node: The node to alias\n"
    },
    {"haslayout", (PyCFunction)gfp_Network_isLayoutSpecified, METH_NOARGS,
     "Return whether the SBML model included layout information or not"
    },
    {"fitwindow", (PyCFunction)gfp_Network_FitToWindow, METH_VARARGS | METH_KEYWORDS,
     "Pan & scale the network so it fits in the given window\n\n"
     ":param float xmin: The start of the window in X\n"
     ":param float ymin: The start of the window in Y\n"
     ":param float xmax: The end of the window in X\n"
     ":param float ymax: The end of the window in Y\n"
    },
    {NULL}  /* Sentinel */
};

static PyMemberDef gfp_Network_members[] = {
    {"nodes", T_OBJECT_EX, offsetof(gfp_Network,nodes), READONLY,
     "nodes"},
    {"rxns",  T_OBJECT_EX, offsetof(gfp_Network,rxns) , READONLY,
     "rxns"},
    {"compartments", T_OBJECT_EX, offsetof(gfp_Network,comps), READONLY,
     "compartments"},
    {"canvas", T_OBJECT_EX, offsetof(gfp_Network, canv), 0,
     "canvas"},
    {NULL}  /* Sentinel */
};

static PyTypeObject gfp_NetworkType = {
#if PYTHON_API_VER == 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,
#endif
    "sbnw.network",      /* tp_name */
    sizeof(gfp_Network),/* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)gfp_Network_dealloc,/* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    (setattrofunc)gfp_Network_SetAttr,       /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "Represents a model of the network including species and reactions",             /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    Network_methods,             /* tp_methods */
    gfp_Network_members,         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)gfp_Network_init,/* tp_init */
    0,                         /* tp_alloc */
    gfp_Network_new,         /* tp_new */
};

/// -- cubicintersec --

typedef struct {
    PyObject_HEAD
} gfp_Cubicintersec;

// static int gfp_Cubicintersec_Check(PyObject* p);

static void gfp_Cubicintersec_dealloc(gfp_Cubicintersec* self) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
    printf("Cubicintersec dealloc\n");
    #endif
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* gfp_Cubicintersec_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    gfp_Cubicintersec* self;
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
    printf("gfp_Cubicintersec_new called\n");
    #endif

    self = (gfp_Cubicintersec*)type->tp_alloc(type, 0);

    return (PyObject*)self;
}

static int gfp_Cubicintersec_init(gfp_Cubicintersec *self, PyObject *args, PyObject *kwds) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
    printf("gfp_Cubicintersec_init called\n");
    #endif

    return 0;
}

PyObject* gfp_Cubicintersec_GetPoints(gfp_Cubicintersec *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"p0", "p1", "p2", "p3", "l0", "l1", NULL};
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
    fprintf(stderr, "gfp_Cubicintersec_GetPoints called\n");
    #endif

    gfp_Point* p0 = NULL;
    gfp_Point* p1 = NULL;
    gfp_Point* p2 = NULL;
    gfp_Point* p3 = NULL;

    gfp_Point* l0 = NULL;
    gfp_Point* l1 = NULL;

    gf_point* pts;

    gf_curveCP cp;

    gf_point lstart;
    gf_point lend;

    size_t count=0, i;

    PyObject* result;

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "O!O!O!O!O!O!", kwlist, &gfp_PointType, &p0, &gfp_PointType, &p1, &gfp_PointType, &p2, &gfp_PointType, &p3, &gfp_PointType, &l0, &gfp_PointType, &l1)) {
        PyErr_SetString(SBNWError, "Invalid arguments");
        return NULL;
    }

    cp.s.x = p0->x;
    cp.s.y = p0->y;

    cp.c1.x = p1->x;
    cp.c1.y = p1->y;

    cp.c2.x = p2->x;
    cp.c2.y = p2->y;

    cp.e.x = p3->x;
    cp.e.y = p3->y;

    lstart.x = l0->x;
    lstart.y = l0->y;

    lend.x = l1->x;
    lend.y = l1->y;

    pts = gf_computeCubicBezierLineIntersec(&cp, &lstart, &lend);

    while(pts[count].x != 0 && pts[count].y != 0)
      ++count;

    result = PyList_New(count);

    for(i=0; i<count; ++i) {
      PyList_SetItem(result, i, Py_BuildValue("O",
            gfp_PointToPyPoint(pts[i])
        ));
    }

    return result;
}

static PyMethodDef gfp_Cubicintersec_methods[] = {
    {"getpoints", (PyCFunction)gfp_Cubicintersec_GetPoints, METH_VARARGS | METH_KEYWORDS,
     "Get the intersection points of a cubic curve & line"
    },
    {NULL}  /* Sentinel */
};

static PyTypeObject gfp_CubicintersecType = {
#if PYTHON_API_VER == 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,
#endif
    "sbnw.cubicintersec",      /* tp_name */
    sizeof(gfp_Cubicintersec),/* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)gfp_Cubicintersec_dealloc,/* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "Cubic Bezier intersection. ",                  /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    gfp_Cubicintersec_methods,//Noddy_methods,             /* tp_methods */
    0,         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)gfp_Cubicintersec_init,/* tp_init */
    0,                         /* tp_alloc */
    gfp_Cubicintersec_new,         /* tp_new */
};

// static int gfp_Cubicintersec_Check(PyObject* p) {
//     if(Py_TYPE(p) != (PyTypeObject*)&gfp_CubicintersecType) {
//         PyErr_SetString(SBNWError, "Not a cubicintersec object");
//         return 0;
//     } else
//         return 1;
// }

/// -- layout --
typedef struct {
    PyObject_HEAD
    gf_layoutInfo* l;
    gfp_Canvas*  canv;
    gfp_Network* network;
} gfp_Layout;

static int gfp_Layout_Check(PyObject* p);

static void gfp_Layout_dealloc(gfp_Layout* self) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("Layout dealloc\n");
    #endif
    gf_freeLayoutInfo(self->l);
    Py_XDECREF(self->canv);
    Py_XDECREF(self->network);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* gfp_Layout_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    gfp_Layout* self;
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Layout_new called\n");
    #endif
    
    self = (gfp_Layout*)type->tp_alloc(type, 0);
    if(self) {
        self->l = NULL;
        self->canv = NULL;
        self->network = NULL;
    }
    
    return (PyObject*)self;
}

static int gfp_Layout_init(gfp_Layout *self, PyObject *args, PyObject *kwds) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_Layout_init called\n");
    #endif
    
    return 0;
}

static int gfp_Layout_rawinit(gfp_Layout* self, gf_layoutInfo* l) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("layout raw init\n");
    #endif
    self->l = l;
    // canvas
    self->canv = (gfp_Canvas*)PyObject_Call((PyObject*)&gfp_CanvasType, PyTuple_New(0), NULL);
    if(gfp_Canvas_rawinit(self->canv, gf_getCanvas(self->l)))
        return 1;

    // network
    self->network = (gfp_Network*)PyObject_Call((PyObject*)&gfp_NetworkType, PyTuple_New(0), NULL);
    if(gfp_Network_rawinit(self->network, gf_getNetwork(self->l), self->l))
        return 1;

    // set canvas object
    Py_INCREF(self->canv);
    self->network->canv = self->canv;
    
    return 0;
}

PyObject* gfp_Layout_FitToWindow(gfp_Layout *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"left", "top", "right", "bottom", NULL};
    double left, top, right, bottom;
    
    // unnecessary?
    if(!gfp_Layout_Check((PyObject*)self))
        return NULL;
    
//     printf("PyTuple_Size: %u\n", PyTuple_Size(args));
    
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "dddd", kwlist, &left, &top, &right, &bottom)) {
//     if(!PyArg_ParseTuple(args, "o", &left, &top, &right, &bottom)) {
//         PyErr_SetString(SBNWError, "Invalid args");
//         printf("%f %f %f %f\n", left, top, right, bottom);
        return NULL;
    }
    
//     printf("%f %f %f %f\n", left, top, right, bottom);
    
    gf_fit_to_window(self->l, left, top, right, bottom);
    
    Py_RETURN_NONE;
}

PyObject* gfp_Layout_TF_FitToWindow(gfp_Layout *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"left", "top", "right", "bottom", NULL};
    double left, top, right, bottom;
    gf_transform* tf;
    gfp_Transform* t;

    // unnecessary?
    if(!gfp_Layout_Check((PyObject*)self))
        return NULL;

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "dddd", kwlist, &left, &top, &right, &bottom)) {
        return NULL;
    }

//     printf("%f %f %f %f\n", left, top, right, bottom);

    t = (gfp_Transform*)PyObject_Call((PyObject*)&gfp_TransformType, PyTuple_New(0), NULL);

    tf = gf_tf_fitToWindow(self->l, left, top, right, bottom);

    if(gfp_Transform_rawinit(t, tf)) {
        Py_RETURN_NONE;
    }

    return (PyObject*)t;
}

static PyMethodDef gfp_Layout_methods[] = {
    {"fitwindow", (PyCFunction)gfp_Layout_FitToWindow, METH_VARARGS | METH_KEYWORDS,
     "Pan & scale the network so it fits in the given window\n\n"
     ":param float xmin: The start of the window in X\n"
     ":param float ymin: The start of the window in Y\n"
     ":param float xmax: The end of the window in X\n"
     ":param float ymax: The end of the window in Y\n"
    },
    {"tf_fitwindow", (PyCFunction)gfp_Layout_TF_FitToWindow, METH_VARARGS | METH_KEYWORDS,
     "Like fitwindow but just returns the transformation, does not apply"
    },
    {NULL}  /* Sentinel */
};

static PyMemberDef gfp_Layout_members[] = {
    {"canvas", T_OBJECT_EX, offsetof(gfp_Layout, canv), 0,
     "canvas name"},
    {"network", T_OBJECT_EX, offsetof(gfp_Layout, network), 0,
     "network name"},
    {NULL}  /* Sentinel */
};

static PyTypeObject gfp_LayoutType = {
#if PYTHON_API_VER == 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,
#endif
    "sbnw.layout",      /* tp_name */
    sizeof(gfp_Layout),/* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)gfp_Layout_dealloc,/* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "The layout information for an SBML model. "                  /* tp_doc */
    "When the SBML model does not provide layout information, "
    "an initial configuration can be generated by calling "
    ":meth:`sbnw.network.randomize`.",
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    gfp_Layout_methods,//Noddy_methods,             /* tp_methods */
    gfp_Layout_members,         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)gfp_Layout_init,/* tp_init */
    0,                         /* tp_alloc */
    gfp_Layout_new,         /* tp_new */
};

static int gfp_Layout_Check(PyObject* p) {
    if(Py_TYPE(p) != (PyTypeObject*)&gfp_LayoutType) {
        PyErr_SetString(SBNWError, "Not a layout object");
        return 0;
    } else
        return 1;
}

/// -- sbmlmodel --

typedef struct {
    PyObject_HEAD
    gf_SBMLModel* m;
    gfp_Layout* layout;
    gfp_Network* network;
} gfp_SBMLModel;

// "Useful" function (Exported)
static PyObject *
gfp_sbnw_system(PyObject *self, PyObject *args)
{
    const char *command;
    int sts;

    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;
    sts = system(command);
    if (sts < 0) {
        PyErr_SetString(SBNWError, "System command failed");
        return NULL;
    }
    return PyLong_FromLong(sts);
}

static void
gfp_SBMLModel_dealloc(gfp_SBMLModel* self) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("SBMLModel dealloc\n");
    #endif

    Py_XDECREF(self->layout);
    Py_XDECREF(self->network);

    if(self->m)
        gf_freeSBMLModel(self->m);

    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* gfp_SBMLModel_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    gfp_SBMLModel* self;
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
    printf("gfp_SBMLModel_new called\n");
    #endif
    
    self = (gfp_SBMLModel*)type->tp_alloc(type, 0);
    if(self) {
        self->m = NULL;
//         self->layout = Py_None;
//         Py_INCREF(Py_None);

        self->layout = NULL;
        self->network = NULL;
    }
    
    return (PyObject*)self;
}

static int gfp_SBMLModel_init(gfp_SBMLModel *self, PyObject *args, PyObject *kwds) {
    const char *sbml;
    // gf_SBMLModel* m = NULL;
    static char *kwlist[] = {"sbml", NULL};
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_SBMLModel_init called\n");
    #endif
    
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &sbml)) {
        PyErr_SetString(SBNWError, "Invalid SBML");
        return -1;
    }
    if(!sbml)
      return -1;

    while(*sbml == ' ' || *sbml == '\t' || *sbml == '\n' || *sbml == '\r')
      ++sbml;

//     fprintf(stderr, "strncmp = %d\n", strncmp(sbml, "<?xml", 5));
//     fprintf(stderr, " first 5 chars: %c%c%c%c%c\n", sbml[0], sbml[1], sbml[2], sbml[3], sbml[4]);
//     fprintf(stderr, " first 5 chars hex: 0x%x 0x%x 0x%x 0x%x 0x%x\n", sbml[0], sbml[1], sbml[2], sbml[3], sbml[4]);
    
    // Overflow safe: null char will stop strncmp
    if(!strncmp(sbml, "<?xml", 5)) {
        // Arg is raw xml
        #if SAGITTARIUS_DEBUG_LEVEL >= 2
//             fprintf(stderr, "raw xml\n");
        #endif
        self->m = gf_loadSBMLbuf(sbml);
    } else {
        // Arg is filepath
        #if SAGITTARIUS_DEBUG_LEVEL >= 2
//             fprintf(stderr, "filepath\n");
        #endif
        self->m = gf_loadSBMLfile(sbml);
    }
    
    if(!self->m) {
        PyErr_SetString(SBNWError, "Failed to open file (check spelling)");
        return -1;
    } else
        return 0;
}

PyObject* gfp_SBMLModel_save(gfp_SBMLModel *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"filepath", NULL};
    const char* outfile = NULL;
    int error;
    
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_SBMLModel_save started\n");
    #endif
    
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &outfile)) {
        PyErr_SetString(SBNWError, "Invalid arguments to sbnw.model.save; expected filepath string");
        return NULL;
    }
    
    if(self->layout)
        error = gf_writeSBMLwithLayout(outfile, self->m, self->layout->l);
    else
        error = gf_writeSBML(outfile, self->m);
    
    if(error) {
        PyErr_Format(SBNWError, "Unable to write file; write access may be disabled");
        return NULL;
    }
    
    Py_RETURN_NONE;
}

PyObject* gfp_SBMLModel_getsbml(gfp_SBMLModel *self, PyObject *args, PyObject *kwds) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_SBMLModel_getsbml started\n");
    #endif

    if(self->layout) {
        const char* sbml = gf_getSBMLwithLayoutStr(self->m, self->layout->l);
        PyObject* result = gfp_PyString_FromString(sbml);
        free((void*)sbml);
        return result;
    } else {
        PyErr_Format(SBNWError, "Cannot get SBML - no layout information");
        return NULL;
    }
}



static int gfp_SBMLModel_processLayout(gfp_SBMLModel *self) {
    // construct the layout
    self->layout = (gfp_Layout*)PyObject_Call((PyObject*)&gfp_LayoutType, PyTuple_New(0), NULL);
    gfp_Layout_rawinit(self->layout, gf_processLayout(self->m));

    // construct the network
//             self->network = (gfp_Network*)PyObject_Call((PyObject*)&gfp_NetworkType, PyTuple_New(0), NULL);
//             gfp_Network_rawinit(self->network, gf_getNetwork(self->layout->l), self->layout->l);
    Py_INCREF(self->layout->network);
    self->network = self->layout->network;

    return 0;
}

static PyObject* gfp_SBMLModel_getAttro(PyObject *self_, PyObject *attr) {
    gfp_SBMLModel* self = (gfp_SBMLModel*)self_;
	
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
    {
// 		char* attrname = gfPyString_getString(attr);
// 		printf("attr name = %s\n", attrname);
// 		gf_strfree(attrname);
    }
    #endif
    
    // hook to compute layout information if not already present
    if(PyCompareString(attr, "layout") || PyCompareString(attr, "network")) {
        #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("positive hit\n");
    #endif
        if(!self->layout) {
            gfp_SBMLModel_processLayout(self);
        }
    }
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("done\n");
    #endif
    
    return PyObject_GenericGetAttr((PyObject *)self, attr);
}

PyObject* gfp_SBMLModel_renderTikZ_file(gfp_SBMLModel *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"filepath", NULL};
    const char* outfile = NULL;
    int error;

    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("gfp_SBMLModel_renderTikZ_file started\n");
    #endif

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &outfile)) {
        PyErr_SetString(SBNWError, "Invalid arguments to sbnw.model.save; expected filepath string");
        return NULL;
    }

    // try to add layout info if not present
    if(!self->layout)
        gfp_SBMLModel_processLayout(self);

    if(self->layout)
        error = gf_renderTikZFile(self->layout->l, outfile);
    else {
        // failed to add layout info
        PyErr_Format(SBNWError, "No layout information");
        return NULL;
    }

    if(error) {
        PyErr_Format(SBNWError, "Unable to write file; write access may be disabled");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *
gfp_SBMLModel_getLevel(gfp_SBMLModel *self, void *closure) {
    return PyLong_FromLong(self->layout->l->level);
}

static PyObject *
gfp_SBMLModel_setLevel(gfp_SBMLModel *self, PyObject *value, void *closure) {
    self->layout->l->level = PyLong_AsLong(value);
    if(PyErr_Occurred) {
      return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *
gfp_SBMLModel_getVersion(gfp_SBMLModel *self, void *closure) {
    return PyLong_FromLong(self->layout->l->version);
}

static PyObject *
gfp_SBMLModel_setVersion(gfp_SBMLModel *self, PyObject *value, void *closure) {
    self->layout->l->version = PyLong_AsLong(value);
    if(PyErr_Occurred) {
      return NULL;
    }
    Py_RETURN_NONE;
}

static PyGetSetDef gfp_SBMLModel_getseters[] = {
    {"level",
     (getter)gfp_SBMLModel_getLevel, (setter)gfp_SBMLModel_setLevel,
     "The SBML level for the model (integer)",
     NULL},
    {"version",
     (getter)gfp_SBMLModel_getVersion, (setter)gfp_SBMLModel_setVersion,
     "The SBML version for the model (integer)",
     NULL},
    {NULL}  /* Sentinel */
};

static PyMethodDef gfp_SBMLModel_methods[] = {
    {"save", (PyCFunction)gfp_SBMLModel_save, METH_VARARGS | METH_KEYWORDS,
     "Save an SBML model to the filesystem\n\n"
     ":param str path: The path to write the file to\n"
    },
    {"getsbml", (PyCFunction)gfp_SBMLModel_getsbml, METH_NOARGS,
     "Get the raw SBML/XML for the current model\n\n"
    },
    {"savetikz", (PyCFunction)gfp_SBMLModel_renderTikZ_file, METH_VARARGS | METH_KEYWORDS,
     "Render the current model to TikZ\n\n"
     ":param str path: The path to write the output file to\n"
    },
    {NULL}  /* Sentinel */
};

static PyMemberDef SBMLModel_members[] = {
    {"layout", T_OBJECT_EX, offsetof(gfp_SBMLModel, layout), 0,
     "Layout structure for this model. Since not all models have "
     "associated layout information, it is loaded on-demand when "
     "the attribute is requested."
    },
    {"network", T_OBJECT_EX, offsetof(gfp_SBMLModel, network), 0,
     "The reaction network for this model."
    },
    {NULL}  /* Sentinel */
};

static PyTypeObject gfp_SBMLModelType = {
#if PYTHON_API_VER == 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,
#endif
    "sbnw.sbmlmodel",      /* tp_name */
    sizeof(gfp_SBMLModel),/* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)gfp_SBMLModel_dealloc,/* tp_dealloc */
    0,                         /* tp_print */
    0,    /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    gfp_SBMLModel_getAttro,    /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "A model which may be serialized using the SBML XML format",              /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    gfp_SBMLModel_methods,     /* tp_methods */
    SBMLModel_members,         /* tp_members */
    gfp_SBMLModel_getseters,   /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)gfp_SBMLModel_init,/* tp_init */
    0,                         /* tp_alloc */
    gfp_SBMLModel_new,         /* tp_new */
};

// static PyObject* gfp_makePySBMLModel(gf_SBMLModel* m) {
//     return NULL;
// }

/// Load SBML (Exported)
static PyObject *
gfp_loadsbml(PyObject* self, PyObject *args, PyObject *kwds) {
    return PyObject_Call((PyObject*)&gfp_SBMLModelType, args, kwds);
}

/// Parametric cubic
static PyObject *
gfp_paramcubic(PyObject* self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"p0", "p1", "p2", "p3", "t", NULL};

    double t;
    gfp_Point* p0 = NULL;
    gfp_Point* p1 = NULL;
    gfp_Point* p2 = NULL;
    gfp_Point* p3 = NULL;

    gf_point result;
    gf_curveCP cp;

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "O!O!O!O!d", kwlist, &gfp_PointType, &p0, &gfp_PointType, &p1, &gfp_PointType, &p2, &gfp_PointType, &p3, &t)) {
        PyErr_SetString(SBNWError, "Invalid arguments");
        return NULL;
    }

    cp.s.x = p0->x;
    cp.s.y = p0->y;

    cp.c1.x = p1->x;
    cp.c1.y = p1->y;

    cp.c2.x = p2->x;
    cp.c2.y = p2->y;

    cp.e.x = p3->x;
    cp.e.y = p3->y;

    result = gf_computeCubicBezierPoint(&cp, t);

    gfp_Point* q = (gfp_Point*)PyObject_Call((PyObject*)&gfp_PointType, Py_BuildValue("dd", result.x, result.y), NULL);

    return (PyObject*)q;
}

static PyObject *
gfp_arrowpoly(PyObject* self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"style", NULL};

    int style;
    int i, n;
    gf_point p;
    PyObject* arrow;

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "i", kwlist, &style)) {
        PyErr_SetString(SBNWError, "Invalid arguments");
        return NULL;
    }

    n = gf_arrowheadStyleGetNumVerts(style);

    arrow = PyList_New(n);

    for(i=0; i<n; ++i) {
      p = gf_arrowheadStyleGetVert(style, i);
      PyList_SetItem(arrow, i, Py_BuildValue("O", gfp_PointToPyPoint(p)));
    }

    return (PyObject*)arrow;
}

static PyObject *
gfp_arrowpolyfilled(PyObject* self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"style", NULL};

    int style;
    int filled;

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "i", kwlist, &style)) {
        PyErr_SetString(SBNWError, "Invalid arguments");
        return NULL;
    }

    filled = gf_arrowheadStyleIsFilled(style);

    return PyBool_FromLong(filled);
}

static PyObject *
gfp_narrow_styles(PyObject* self) {
    return PyLong_FromLong(gf_arrowheadNumStyles());
}

static PyObject *
gfp_arrow_get_style(PyObject* self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"role", NULL};

    const char* str;

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &str)) {
        PyErr_SetString(SBNWError, "Invalid arguments");
        return NULL;
    }

    return PyLong_FromLong(gf_arrowheadGetStyle(gf_strToRole(str)));
}

static PyObject *
gfp_arrow_set_style(PyObject* self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"role", "style", NULL};

    int style;
    const char* str;

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "si", kwlist, &str, &style)) {
        PyErr_SetString(SBNWError, "Invalid arguments");
        return NULL;
    }

    gf_arrowheadSetStyle(gf_strToRole(str), style);

    Py_RETURN_NONE;
}

// Module method table
static PyMethodDef SBNWMethods[] = {
    {"system",  gfp_sbnw_system, METH_VARARGS,
     "Execute a shell command."},
    {"loadsbml",  (PyCFunction)gfp_loadsbml, METH_VARARGS | METH_KEYWORDS,
     "Pass either a filepath or raw SBML to load a model\n\n"
     ">>> model = sbnw.loadsbml('path/to/file/sbmlmodel.xml')\n\n"
     ":param sbml: Either a filepath or raw SBML content to load\n"
     ":type sbml: str\n"
     ":returns: :class:`sbnw.sbmlmodel` -- The SBML moodel\n"
     ":raises: SBNWError\n"
    },
    {"paramcubic",  (PyCFunction)gfp_paramcubic, METH_VARARGS | METH_KEYWORDS,
     "Evaluate a parametric cubic Bezier\n\n"
    },
    {"arrowpoly",  (PyCFunction)gfp_arrowpoly, METH_VARARGS | METH_KEYWORDS,
     "Get the vertices for an arrow polygon\n\n"
     ":param style: The index of the arrowhead style\n"
     ":type style: int\n"
    },
    {"arrowpoly_filled",  (PyCFunction)gfp_arrowpolyfilled, METH_VARARGS | METH_KEYWORDS,
     "Return true if the poly should be filled\n\n"
     ":param style: The index of the arrowhead style\n"
     ":type style: int\n"
    },
    {"narrow_styles",  (PyCFunction)gfp_narrow_styles, METH_NOARGS,
     "Get the number of arrow styles\n\n"
    },
    {"get_arrow_style",  (PyCFunction)gfp_arrow_get_style, METH_VARARGS | METH_KEYWORDS,
     "Set the arrow style for a role. Returns an integer.\n\n"
     ":param role: The role (SUBSTRATE,PRODUCT,MODIFIER,ACTIVATOR,INHIBITOR)\n"
     ":type role: str\n"
    },
    {"set_arrow_style",  (PyCFunction)gfp_arrow_set_style, METH_VARARGS | METH_KEYWORDS,
     "Set the arrow style for a role\n\n"
     ":param role: The role (SUBSTRATE,PRODUCT,MODIFIER,ACTIVATOR,INHIBITOR)\n"
     ":type role: str\n"
     ":param style: The style to assign\n"
     ":type style: int\n"
    },
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

#if PYTHON_API_VER == 3
    // Module def
    static struct PyModuleDef sbnwmodule = {
    PyModuleDef_HEAD_INIT,
    "sbnw",   /* name of module */
    "SBML autolayout", /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                    or -1 if the module keeps state in global variables. */
    SBNWMethods
    };
#endif
    
#if PYTHON_API_VER == 3
    #define MODINIT_ABORT return NULL;
    #define MODINIT_SUCCESS(mod) return mod;
#else
    #define MODINIT_ABORT return;
    #define MODINIT_SUCCESS(mod) return;
    #ifndef PyMODINIT_FUNC
        #define PyMODINIT_FUNC void
    #endif
#endif

// Module init
PyMODINIT_FUNC
#if PYTHON_API_VER == 3
PyInit_sbnw(void)
#else
initsbnw(void)
#endif
{
    PyObject *m;
    
    // point
    gfp_PointType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&gfp_PointType) < 0)
        MODINIT_ABORT
    // transform
    gfp_TransformType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&gfp_TransformType) < 0)
        MODINIT_ABORT
    // compartment
    gfp_CompartmentType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&gfp_CompartmentType) < 0)
        MODINIT_ABORT
    // rxn
    gfp_RxnType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&gfp_RxnType) < 0)
        MODINIT_ABORT
    // node
    gfp_NodeType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&gfp_NodeType) < 0)
        MODINIT_ABORT
    // network
    gfp_NetworkType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&gfp_NetworkType) < 0)
        MODINIT_ABORT
    // canvas
    gfp_CanvasType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&gfp_CanvasType) < 0)
        MODINIT_ABORT
    // layout
    gfp_LayoutType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&gfp_LayoutType) < 0)
        MODINIT_ABORT
    // cubicintersec
    gfp_CubicintersecType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&gfp_CubicintersecType) < 0)
        MODINIT_ABORT
    // sbmlmodel
    gfp_SBMLModelType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&gfp_SBMLModelType) < 0)
        MODINIT_ABORT

#if PYTHON_API_VER == 3
    m = PyModule_Create(&sbnwmodule);
#else
    m = Py_InitModule("sbnw", SBNWMethods);
#endif
    if (m == NULL)
        MODINIT_ABORT

    SBNWError = PyErr_NewException("sbnw.error", NULL, NULL);
    Py_INCREF(SBNWError);
    PyModule_AddObject(m, "error", SBNWError);
    
    
    // point
    Py_INCREF(&gfp_PointType);
    PyModule_AddObject(m, "point", (PyObject *)&gfp_PointType);
    // transform
    Py_INCREF(&gfp_TransformType);
    PyModule_AddObject(m, "transform", (PyObject *)&gfp_TransformType);
    // compartment
    Py_INCREF(&gfp_CompartmentType);
    PyModule_AddObject(m, "compartment", (PyObject *)&gfp_CompartmentType);
    // rxn
    Py_INCREF(&gfp_RxnType);
    PyModule_AddObject(m, "reaction", (PyObject *)&gfp_RxnType);
    // node
    Py_INCREF(&gfp_NodeType);
    PyModule_AddObject(m, "node", (PyObject *)&gfp_NodeType);
    // network
    Py_INCREF(&gfp_NetworkType);
    PyModule_AddObject(m, "network", (PyObject *)&gfp_NetworkType);
    // canvas
    Py_INCREF(&gfp_CanvasType);
    PyModule_AddObject(m, "canvas", (PyObject *)&gfp_CanvasType);
    // cubicintersec
    Py_INCREF(&gfp_CubicintersecType);
    PyModule_AddObject(m, "cubicintersec", (PyObject *)&gfp_CubicintersecType);
    // layout
    Py_INCREF(&gfp_LayoutType);
    PyModule_AddObject(m, "layout", (PyObject *)&gfp_LayoutType);
    // sbmlmodel
    Py_INCREF(&gfp_SBMLModelType);
    PyModule_AddObject(m, "sbmlmodel", (PyObject *)&gfp_SBMLModelType);

    // add version info
    PyModule_AddStringConstant(m, "__version__", gf_getCurrentLibraryVersion());
    
    MODINIT_SUCCESS(m);
}