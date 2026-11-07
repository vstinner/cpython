/*[clinic input]
preserve
[clinic start generated code]*/

#if defined(Py_BUILD_CORE) && !defined(Py_BUILD_CORE_MODULE)
#  include "pycore_gc.h"          // PyGC_Head
#  include "pycore_runtime.h"     // _Py_ID()
#endif
#include "pycore_modsupport.h"    // _PyArg_UnpackKeywords()
#include "pycore_unicodeobject.h" // _PyUnicode_AsUTF8NoNUL()

PyDoc_STRVAR(_testcapi_unicode_decodelocale__doc__,
"unicode_decodelocale($module, /, data, errors=<unrepresentable>)\n"
"--\n"
"\n"
"Test PyUnicode_DecodeLocale().");

#define _TESTCAPI_UNICODE_DECODELOCALE_METHODDEF    \
    {"unicode_decodelocale", _PyCFunction_CAST(_testcapi_unicode_decodelocale), METH_FASTCALL|METH_KEYWORDS, _testcapi_unicode_decodelocale__doc__},

static PyObject *
_testcapi_unicode_decodelocale_impl(PyObject *module, const char *data,
                                    Py_ssize_t data_length,
                                    const char *errors);

static PyObject *
_testcapi_unicode_decodelocale(PyObject *module, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    PyObject *return_value = NULL;
    #if defined(Py_BUILD_CORE) && !defined(Py_BUILD_CORE_MODULE)

    #define NUM_KEYWORDS 2
    static struct {
        PyGC_Head _this_is_not_used;
        PyObject_VAR_HEAD
        Py_hash_t ob_hash;
        PyObject *ob_item[NUM_KEYWORDS];
    } _kwtuple = {
        .ob_base = PyVarObject_HEAD_INIT(&PyTuple_Type, NUM_KEYWORDS)
        .ob_hash = -1,
        .ob_item = { &_Py_ID(data), &_Py_ID(errors), },
    };
    #undef NUM_KEYWORDS
    #define KWTUPLE (&_kwtuple.ob_base.ob_base)

    #else  // !Py_BUILD_CORE
    #  define KWTUPLE NULL
    #endif  // !Py_BUILD_CORE

    static const char * const _keywords[] = {"data", "errors", NULL};
    static _PyArg_Parser _parser = {
        .keywords = _keywords,
        .fname = "unicode_decodelocale",
        .kwtuple = KWTUPLE,
    };
    #undef KWTUPLE
    PyObject *argsbuf[2];
    Py_ssize_t noptargs = nargs + (kwnames ? PyTuple_GET_SIZE(kwnames) : 0) - 1;
    const char *data;
    Py_ssize_t data_length;
    const char *errors = NULL;

    args = _PyArg_UnpackKeywords(args, nargs, NULL, kwnames, &_parser,
            /*minpos*/ 1, /*maxpos*/ 2, /*minkw*/ 0, /*varpos*/ 0, argsbuf);
    if (!args) {
        goto exit;
    }
    extern Py_ssize_t _Py_ConvertBuffer(PyObject *arg, const char **buf);

    data_length = _Py_ConvertBuffer(args[0], &data);
    if (data_length < 0) {
        goto exit;
    }
    if (!noptargs) {
        goto skip_optional_pos;
    }
    if (args[1] == Py_None) {
        errors = NULL;
    }
    else if (PyUnicode_Check(args[1])) {
        errors = _PyUnicode_AsUTF8NoNUL(args[1]);
        if (errors == NULL) {
            goto exit;
        }
    }
    else {
        _PyArg_BadArgument("unicode_decodelocale", "argument 'errors'", "str or None", args[1]);
        goto exit;
    }
skip_optional_pos:
    return_value = _testcapi_unicode_decodelocale_impl(module, data, data_length, errors);

exit:
    return return_value;
}
/*[clinic end generated code: output=13321698267d80c3 input=a9049054013a1b77]*/
