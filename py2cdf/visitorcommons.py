def return_type(node):
    if node.returns is None:
        return 'void'

    rt = node.returns
    nodename = type(rt).__name__
    if nodename == 'NameConstant' and rt.value is None:
        return 'void'
    if nodename == 'Constant' and rt.value is None:
        return 'void'
    if nodename == 'Name':
        return map_type(rt.id)

    return '?'


def arguments_declaration(node):
    i = len(node.args)
    if not i:
        return 'void'
    code = ''
    for a in node.args:
        i -= 1
        typename = a.annotation.id if a.annotation else 'ObjectPtr'
        typename = map_type(typename)
        name = a.arg
        if i:
            code += '%s %s, ' % (typename, name)
        else:
            code += '%s %s' % (typename, name)
    return code

def _node_to_expr(node):
    nodename = type(node).__name__
    if nodename == 'Name':
        return node.id
    elif nodename == 'Str':
        return '"%s"' % node.s
    elif nodename == 'Num':
        return str(node.n)
    elif nodename == 'NameConstant':
        v = node.value
        if v is True:
            return 'true'
        elif v is False:
            return 'false'
        elif v is None:
            return 'NULL'
    elif nodename == 'Constant':
        v = node.value
        if isinstance(v, str):
            return '"%s"' % v
        elif isinstance(v, bool):
            return 'true' if v else 'false'
        elif isinstance(v, (int, float)):
            return str(v)
        elif v is None:
            return 'NULL'
    elif nodename == 'UnaryOp' and type(node.op).__name__ == 'USub':
        return '-%s' % _node_to_expr(node.operand)
    return '?'

def arguments_call(node):
    i = len(node.args)
    code = ''
    for a in node.args:
        i -= 1
        arg = _node_to_expr(a)
        if i:
            code += '%s, ' % arg
        else:
            code += '%s' % arg
    return code


def map_type(name):
    if name == 'ObjectPtr':
        return 'void*'

    if name[0].isupper():
        return '%s*' % name

    return name