"""
Tests on PyConfig API (PEP 587).
"""
import os
import sys
import unittest
from test import support
try:
    import _testcapi
except ImportError:
    _testcapi = None


@unittest.skipIf(_testcapi is None, 'need _testcapi')
class CAPITests(unittest.TestCase):
    def check_config_get(self, get_func):
        # write_bytecode is read from sys.dont_write_bytecode as int
        with support.swap_attr(sys, "dont_write_bytecode", 0):
            self.assertEqual(get_func('write_bytecode'), 1)
        with support.swap_attr(sys, "dont_write_bytecode", "yes"):
            self.assertEqual(get_func('write_bytecode'), 0)
        with support.swap_attr(sys, "dont_write_bytecode", []):
            self.assertEqual(get_func('write_bytecode'), 1)

        # non-existent config option name
        NONEXISTENT_KEY = 'NONEXISTENT_KEY'
        err_msg = f'unknown config option name: {NONEXISTENT_KEY}'
        with self.assertRaisesRegex(ValueError, err_msg):
            get_func('NONEXISTENT_KEY')

    def test_config_get(self):
        config_get = _testcapi.config_get

        self.check_config_get(config_get)

        for name, config_type, expected in (
            ('verbose', int, sys.flags.verbose),    # PyConfig_MEMBER_UINT
            ('isolated', bool, sys.flags.isolated), # PyConfig_MEMBER_BOOL
            ('platlibdir', str, sys.platlibdir),    # PyConfig_MEMBER_WSTR
            ('argv', tuple, tuple(sys.argv)),       # PyConfig_MEMBER_WSTR_LIST
            ('xoptions', dict, sys._xoptions),      # xoptions dict
        ):
            with self.subTest(name=name):
                value = config_get(name)
                self.assertEqual(type(value), config_type)
                self.assertEqual(value, expected)

        # PyConfig_MEMBER_ULONG type
        hash_seed = config_get('hash_seed')
        self.assertIsInstance(hash_seed, int)
        self.assertGreaterEqual(hash_seed, 0)

        # attributes read from sys
        value_str = "TEST_MARKER_STR"
        value_tuple = ("TEST_MARKER_STR_LIST",)
        value_dict = {"x": "value", "y": True}
        for name, sys_name, value in (
            ("base_exec_prefix", None, value_str),
            ("base_prefix", None, value_str),
            ("exec_prefix", None, value_str),
            ("executable", None, value_str),
            ("platlibdir", None, value_str),
            ("prefix", None, value_str),
            ("pycache_prefix", None, value_str),
            ("base_executable", "_base_executable", value_str),
            ("stdlib_dir", "_stdlib_dir", value_str),
            ("argv", None, value_tuple),
            ("orig_argv", None, value_tuple),
            ("warnoptions", None, value_tuple),
            ("module_search_paths", "path", value_tuple),
            ("xoptions", "_xoptions", value_dict),
        ):
            with self.subTest(name=name):
                if sys_name is None:
                    sys_name = name
                with support.swap_attr(sys, sys_name, value):
                    self.assertEqual(config_get(name), value)

    def test_config_getint(self):
        config_getint = _testcapi.config_getint

        self.check_config_get(config_getint)

        # PyConfig_MEMBER_INT type
        self.assertEqual(config_getint('verbose'), sys.flags.verbose)

        # PyConfig_MEMBER_UINT type
        self.assertEqual(config_getint('isolated'), sys.flags.isolated)

        # PyConfig_MEMBER_ULONG type
        hash_seed = config_getint('hash_seed')
        self.assertIsInstance(hash_seed, int)
        self.assertGreaterEqual(hash_seed, 0)

        # platlibdir is a str
        with self.assertRaises(TypeError):
            config_getint('platlibdir')


if __name__ == "__main__":
    unittest.main()
