def return_type(node):
    if node.returns is None:
        return 'void'

    if type(node.returns).__name__ == 'NameConstant' and node.returns.value is None:
        return 'void'

    if type(node.returns).__name__ == 'Name':
        return map_type(node.returns.id)

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

def arguments_call(node):
    i = len(node.args)
    code = ''
    for a in node.args:
        i -= 1
        name = a.id
        if i:
            code += '%s, ' % name
        else:
            code += '%s' % name
    return code


def map_type(name):
    if name == 'ObjectPtr':
        return 'void*'

    if name[0].isupper():
        return '%s*' % name

    return name