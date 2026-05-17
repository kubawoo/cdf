import argparse
import os
import pprint
import re


__BASE_TYPE = '__base_type'
__known_types = {}


def get_return_type(clazz, method):
    while True:
        if method in __known_types[clazz]:
            return __known_types[clazz][method]
        clazz = get_base_class(clazz)

def get_class_for_method(clazz, method):
    while True:
        if method in __known_types[clazz]:
            return clazz
        clazz = get_base_class(clazz)

def get_base_class(clazz):
    return __known_types[clazz][__BASE_TYPE]

def is_known_type(clazz):
    return clazz in __known_types


def process_dirs(dirs, debug=False):
    global __known_types
    for d in dirs:
        for root, dirs, files in os.walk(d):
            for file in files:
                if file.endswith('.h') or file.endswith('.csh'):
                    path = os.path.join(root, file)
                    _process_file(path, __known_types, debug)
    if debug:
        print('Known types:')
        pprint.pprint(__known_types)


def _process_file(path, types, debug):
    if debug:
        print('Processing header file %s' % path)
    with open(path) as f:
        content = f.read()

    # collect forward typedefs: typedef struct _X Y;
    aliases = {}
    for m in re.finditer(r'\s*typedef\s+struct\s+_(\w+)\s+(\w+)\s*;', content):
        struct_tag = '_' + m.group(1)
        alias = m.group(2)
        aliases[struct_tag] = alias

    while True:
        m = re.search(r'\s*typedef\s+struct[^;]*\{(?P<body>.*?)\}\s*(?P<name>\w+)\s*;', content, flags=re.DOTALL)
        if m:
            content = content[m.end():]
            body = m.group('body')
            name = m.group('name')

            if name.startswith('_'):
                continue
            if name in types:
                raise Exception('%s is redefined' % name)

            types[name] = _process_body(body)
            if debug:
                print('Found: %s=%s' % (name, str(types[name])))
            continue

        m = re.search(r'\s*struct\s+_(\w+)\s*\{(?P<body>.*?)\}\s*;', content, flags=re.DOTALL)
        if m:
            content = content[m.end():]
            body = m.group('body')
            tag = '_' + m.group(1)
            name = aliases.get(tag, m.group(1))

            if name.startswith('_'):
                continue
            if name in types:
                raise Exception('%s is redefined' % name)

            types[name] = _process_body(body)
            if debug:
                print('Found: %s=%s' % (name, str(types[name])))
            continue

        break


def _process_body(body):
    ret = {__BASE_TYPE: None}
    for stmt in body.split(';'):
        stmt = stmt.strip()
        m = re.search(r'inherits\s*\(\s*(?P<base>\w+)\s*\)', stmt)
        if m:
            ret[__BASE_TYPE] = m.group('base')
            continue

        m = re.search(r'(?P<rettype>.*?)\s*\(\s*\*\s*(?P<method>\w+)\s*\)', stmt)
        if m:
            method = m.group('method')
            if _add_method(method):
                rettype = m.group('rettype')
                ret[method] = _process_rettype(rettype)
    return ret


def _add_method(m):
    if m in ['_ooc_destructor', 'to_cstring']:
        return False
    return True


def _process_rettype(rt):
    if '*' in rt:
        rt = rt.replace('*', '')
        rt = rt.strip()
    return rt


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Reads *.h files and builds types info')
    parser.add_argument('--include', '-I', nargs='+', required=True, help='Include dirs')
    parser.add_argument('--debug', '-d', action='store_true', required=False, help='Use debug output')
    parser.add_argument('--verbose', '-v', action='store_true', required=False, help='Use verbose output')

    args = parser.parse_args()
    process_dirs(args.include)
    pprint.pprint(__known_types)





