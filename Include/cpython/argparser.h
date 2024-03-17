#ifndef Py_LIMITED_API
#ifndef Py_ARGPARSER_H
#define Py_ARGPARSER_H
#ifdef __cplusplus
extern "C" {
#endif

// PyArgSpec.arg_kind values
#define PyArgKind_POS_ONLY 0
#define PyArgKind_POS_OR_KW 1
#define PyArgKind_KW_ONLY 2
#define PyArgKind_VARARGS 3
#define PyArgKind_KWARGS 4

#define PyArgType_OBJECT 0
// FIXME: support more types
// FIXME: support converter API

typedef struct PyArgSpec {
    // FIXME: add name for keyword argument

    int arg_kind;
    int arg_type;

    // FIXME: default value for optional argument
} PyArgSpec;


typedef struct PyArgParser {
    // Name used in error message
    const char *func_name_utf8;
    PyObject *func_name;

    Py_ssize_t nspec;
    PyArgSpec *specs;
    // FIXME: keyword names
    void *impl;
} PyArgParser;

#ifdef __cplusplus
}
#endif
#endif /* !Py_ARGPARSER_H */
#endif /* !Py_LIMITED_API */
