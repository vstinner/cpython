import pyperf
import codecs
import errno
import locale
import os
import _testcapi
import _testlimitedcapi

loc = 'C.utf8'
#loc = 'fr_FR.utf8'
locale.setlocale(locale.LC_ALL, loc)

enc = locale.getencoding()
err = errno.ENOENT
errmsg_unicode = os.strerror(err)
errmsg_bytes = errmsg_unicode.encode(enc)
try:
    unicode_decodelocale = _testlimitedcapi.unicode_decodelocale
except AttributeError:
    unicode_decodelocale = _testcapi.unicode_decodelocale

assert unicode_decodelocale(errmsg_bytes) == errmsg_unicode
s = errmsg_bytes.decode(enc)
assert s == errmsg_unicode, f"{s!a} != {errmsg_unicode!a}"

runner = pyperf.Runner()
runner.bench_func("decode python", errmsg_bytes.decode, enc)
runner.bench_func("decode locale", unicode_decodelocale, errmsg_bytes)
runner.bench_func("strerror", os.strerror, err)
