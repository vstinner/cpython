#include "Python.h"
#include "pycore_tuple.h"         // _PyTuple_ITEMS()
#include <stddef.h>               // offsetof()


typedef struct {
    int arg_type;
} PyArgSpecImpl;

typedef struct PyArgParserImpl {
    struct PyArgParserImpl *next;

    // pyargparser_dealloc() clears parser->impl
    PyArgParser *parser;

    PyObject *func_name;  // can be NULL

    Py_ssize_t nspec;
    PyArgSpecImpl specs[1];
} PyArgParserImpl;


// FIXME: make it per-interpreter?
// Or pyargparser_create() must allocate static immortal str objects on the
// heap memory!
static PyArgParserImpl *parser_head = NULL;


static void pyargparser_dealloc(PyArgParserImpl *parser)
{
    if (parser->parser != NULL) {
        parser->parser->impl = NULL;
    }
    Py_XDECREF(parser->func_name);
    PyMem_Free(parser);
}


static PyArgParserImpl* pyargparser_create(PyArgParser *parser)
{
    size_t size = offsetof(PyArgParserImpl, specs);
    if ((size_t)parser->nspec > ((size_t)PY_SSIZE_T_MAX - size)
                                 / sizeof(PyArgSpecImpl)) {
        PyErr_NoMemory();
        return NULL;
    }
    size += sizeof(PyArgSpecImpl) * parser->nspec;

    PyArgParserImpl *impl = PyMem_Malloc(size);
    if (impl == NULL) {
        PyErr_NoMemory();
        return NULL;
    }

    impl->parser = parser;
    if (parser->func_name != NULL) {
        impl->func_name = Py_NewRef(parser->func_name);
    }
    else if (parser->func_name_utf8 != NULL) {
        impl->func_name = PyUnicode_FromString(parser->func_name_utf8);
        if (impl->func_name == NULL) {
            goto error;
        }
    }
    else {
        impl->func_name = NULL;
    }

    impl->nspec = parser->nspec;
    for (Py_ssize_t i=0; i < parser->nspec; i++) {
        PyArgSpecImpl *spec_impl = &impl->specs[i];
        const PyArgSpec *spec = &parser->specs[i];
        spec_impl->arg_type = spec->arg_type;
    }


    PyArgParserImpl *other_impl = NULL;
    if (!_Py_atomic_compare_exchange_ptr(&parser->impl, &other_impl, impl)) {
        // Concurrent call: use the race winner, delete our copy.
        impl->parser = NULL;
        pyargparser_dealloc(impl);
        return other_impl;
    }

    // FIXME: use a regular mutex instead?
    while (1) {
        PyArgParserImpl *next = NULL;
        if (_Py_atomic_compare_exchange_ptr(&parser_head, &next, impl)) {
            impl->next = next;
            break;
        }
        // lost race, retry
    }

    return impl;

error:
    pyargparser_dealloc(impl);
    return NULL;
}


static int pyargparser_get_impl(PyArgParser *parser, PyArgParserImpl **result)
{
    PyArgParserImpl *impl = parser->impl;
    if (impl != NULL) {
        *result = impl;
        return 0;
    }

    impl = pyargparser_create(parser);
    *result = impl;
    if (impl == NULL) {
        return -1;
    }
    return 0;
}


static int
pyargparser_tuple(PyArgParserImpl *parser, Py_ssize_t nargs, PyObject **args,
                  va_list va)
{
    Py_ssize_t spec_index = 0;
    Py_ssize_t arg_index = 0;

    for (; spec_index < parser->nspec; spec_index++) {
        PyArgSpecImpl *spec = &parser->specs[spec_index];

        assert(spec->arg_type == PyArgType_OBJECT);

        if (arg_index >= nargs) {
            PyErr_Format(PyExc_TypeError, "expect X args, got %zd", nargs);
            return -1;
        }

        PyObject **pobj = va_arg(va, PyObject **);

        *pobj = args[arg_index];
        arg_index++;
    }

    if (spec_index != parser->nspec) {
        PyErr_Format(PyExc_TypeError, "expect X args, got %zd", nargs);
        return -1;
    }
    if (arg_index != nargs) {
        PyErr_Format(PyExc_TypeError, "expect X args, got %zd", nargs);
        return -1;
    }
    return 0;
}


int PyArgParser_Tuple(PyArgParser *parser, PyObject *args, ...)
{
    if (!PyTuple_Check(args)) {
        PyErr_Format(PyExc_TypeError, "expect args tuple, got %T", args);
        return -1;
    }

    PyArgParserImpl *impl;
    if (pyargparser_get_impl(parser, &impl) < 0) {
        return -1;
    }

    va_list va;
    va_start(va, args);
    int res = pyargparser_tuple(impl,
                                PyTuple_GET_SIZE(args),
                                _PyTuple_ITEMS(args),
                                va);
    va_end(va);
    return res;
}


void PyArgParser_Fini(void)
{
    PyArgParserImpl *parser = parser_head;
    parser_head = NULL;

    while (parser != NULL) {
        PyArgParserImpl *next = parser->next;
        pyargparser_dealloc(parser);
        parser = next;
    }
}

static PyArgSpec test_specs[] = {
    {
        .arg_kind = PyArgKind_POS_ONLY,
        .arg_type = PyArgType_OBJECT,
    },
    {
        .arg_kind = PyArgKind_POS_ONLY,
        .arg_type = PyArgType_OBJECT,
    },
};

static PyArgParser test_parser = {
    .func_name_utf8 = "PyArgParser_Test",
    .nspec = Py_ARRAY_LENGTH(test_specs),
    .specs = test_specs,
};

PyAPI_FUNC(int) PyArgParser_Test(PyObject *args)
{
    PyObject *x = NULL;
    PyObject *y = NULL;
    if (PyArgParser_Tuple(&test_parser, args, &x, &y) < 0) {
        return -1;
    }
    _PyObject_Dump(x);
    _PyObject_Dump(y);
    return 0;
}
