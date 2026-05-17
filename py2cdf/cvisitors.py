import ast
import visitorcommons
import ooctypes


__ctr = 0
def _nexttmpvar():
    global __ctr
    __ctr += 1
    return '_tmp_%d' % __ctr

def _string_literal(arg):
    nodename = type(arg).__name__
    if nodename == 'Str':
        return arg.s
    if nodename == 'Constant' and isinstance(arg.value, str):
        return arg.value
    return None


class BaseVisitor(ast.NodeVisitor):
    def __init__(self, scope, parentscope):
        self.code = ''
        self.scope = scope
        self.parentscope = parentscope

    def create_parentscope(self):
        d = {}
        if self.scope:
            d.update(self.scope)
        if self.parentscope:
            d.update(self.parentscope)
        return d

    def _nodename(self, node):
        return type(node).__name__


class RootVisitor(BaseVisitor):
    def visit_Import(self, node):
        pass

    def visit_FunctionDef(self, node):
        v = FunctionDefVisitor({}, self.create_parentscope())
        v.visit(node)
        self.code += v.code

    def visit_Assign(self, node):
        v = AssignVisitor(self.scope, self.parentscope)
        v.visit(node)
        self.code += v.code

    def visit_Call(self, node):
        v = CallVisitor(self.scope, self.parentscope)
        v.visit(node)
        self.code += v.init_code
        self.code += v.code
        self.code += ';\n'

    def visit_Expr(self, node):
        self._do_visit(ExprVisitor(self.scope, self.parentscope), node)

    def visit_ClassDef(self, node):
        self._do_visit(ClassDefVisitor(), node)

    def _do_visit(self, visitor, node):
        visitor.visit(node)
        self.code += visitor.code

    def generic_visit(self, node):
        print("RootVisitor.generic_visit", self._nodename(node))


class ClassDefVisitor(BaseVisitor):

    def __init__(self):
        super().__init__({}, {})
        self.constructors = []

    def visit_ClassDef(self, node):
        for n in node.body:
            self.visit(n)

        pass

    def visit_FunctionDef(self, node):
        func_name = node.name
        if func_name == '__init__':
            self.constructors.append(node)
        else:
            name = node.name
           # self.code += '%s (*%s)(%s);\n' % (visitorcommons.return_type(node), name, visitorcommons.arguments_declaration(node.args))

class FunctionDefVisitor(BaseVisitor):
    def visit_FunctionDef(self, node):
        self.code += '%s %s(%s) {\n' % (visitorcommons.return_type(node), node.name, visitorcommons.arguments_declaration(node.args))
        code = ''
        for n in node.body:
            v = RootVisitor(self.scope, self.parentscope)
            v.visit(n)
            code += v.code

        for var in self.scope:
            if self.scope[var] == 'ObjectPtr':
                self.code += 'ObjectPtr %s;\n' % var
            else:
                self.code += '%s* %s;\n' % (self.scope[var], var)

        self.code += code

        for var in self.scope:
            self.code += 'REFCDEC(%s);\n' % var
        self.code += '}\n'


class CallVisitor(BaseVisitor):

    def __init__(self, scope, parentscope):
        super().__init__(scope, parentscope)
        self.rettype = None
        self.init_code = ''

    def visit_Call(self, node):
        if self._nodename(node.func) == 'Attribute':
            obj = node.func.value.id
            func = node.func.attr
            self.rettype = ooctypes.get_return_type(self.scope[obj], func)
            clazz = ooctypes.get_class_for_method(self.scope[obj], func)
            args = visitorcommons.arguments_call(node)
            if args:
                self.code += 'call((%s *)%s, %s, %s)' % (clazz, obj, func, args)
            else:
                self.code += 'call((%s *)%s, %s)' % (clazz, obj, func)

        elif self._nodename(node.func) == 'Name':
            f = node.func.id
            if ooctypes.is_known_type(f):
                self.rettype = f
                init_code = ''
                call_args = ''
                for i, arg in enumerate(node.args):
                    s = _string_literal(arg)
                    if s is not None:
                        tmp = _nexttmpvar()
                        self.scope[tmp] = 'String'
                        init_code += '%s = new(String, "%s");\n' % (tmp, s)
                        arg_expr = tmp
                    else:
                        arg_expr = visitorcommons._node_to_expr(arg)
                    if i > 0:
                        call_args += ', '
                    call_args += arg_expr
                self.init_code = init_code
                if call_args:
                    self.code = 'new(%s, %s)' % (f, call_args)
                else:
                    self.code = 'new(%s)' % f
            else:
                raise Exception('Unsupported method call: %s' % f)


class AssignVisitor(BaseVisitor):
    def visit_Assign(self, node):
        varname = node.targets[0].id
        if varname in self.scope:
            self.code += 'REFCDEC(%s);\n' % varname
        if self._nodename(node.value) == 'Call':
            visitor = CallVisitor(self.scope, self.parentscope)
            visitor.visit(node.value)
            self.scope[varname] = visitor.rettype
            self.code += visitor.init_code
            self.code += '%s = %s;\n' % (varname, visitor.code)


        elif self._nodename(node.value) == 'Constant':
            value = node.value.value
            if isinstance(value, str):
                self.scope[varname] = 'String'
                self.code += '%s = new(String, "%s");\n' % (varname, value)
            elif isinstance(value, bool):
                self.scope[varname] = 'Boolean'
                self.code += '%s = new(%s, %s);\n' % (varname, self.scope[varname], str(value).lower())
            elif isinstance(value, int):
                self.scope[varname] = 'Integer'
                self.code += '%s = new(%s, %s);\n' % (varname, self.scope[varname], str(value))
            elif isinstance(value, float):
                self.scope[varname] = 'Double'
                self.code += '%s = new(%s, %s);\n' % (varname, self.scope[varname], str(value))
            elif value is None:
                self.scope[varname] = 'ObjectPtr'
                self.code += '%s = NULL;\n' % varname
            else:
                raise Exception('Unsupported Constant type: %s' % type(value))
        elif self._nodename(node.value) == 'Num':
            value = node.value.n
            if isinstance(value, int):
                self.scope[varname] = 'Integer'
            elif isinstance(value, float):
                self.scope[varname] = 'Double'
            else:
                raise Exception('Unsupported type: %s' % type(value))
            self.code += '%s = new(%s, %s);\n' % (varname, self.scope[varname], str(value))
        elif self._nodename(node.value) == 'NameConstant':
            value = node.value.value
            if value is False or value is True:
                self.scope[varname] = 'Boolean'
                self.code += '%s = new(%s, %s);\n' % (varname, self.scope[varname], str(value).lower())
            else:
                self.scope[varname] = 'ObjectPtr'
                self.code += '%s = NULL;\n' % varname
        elif self._nodename(node.value) == 'Str':
            value = node.value.s
            self.scope[varname] = 'String'
            self.code += '%s = new(String, "%s");\n' %(varname, value)
        elif self._nodename(node.value) == 'List':
            self.scope[varname] = 'List'
            self.code += '%s = new(List);\n' % varname

            for e in node.value.elts:
                if self._nodename(e) == 'Name':
                    var = e.id
                elif self._nodename(e) == 'Constant':
                    var = _nexttmpvar()
                    val = e.value
                    if isinstance(val, str):
                        self.scope[var] = 'String'
                        self.code += '%s = new(String, "%s");\n' % (var, val)
                    elif isinstance(val, bool):
                        self.scope[var] = 'Boolean'
                        self.code += '%s = new(Boolean, %s);\n' % (var, str(val).lower())
                    elif isinstance(val, int):
                        self.scope[var] = 'Integer'
                        self.code += '%s = new(Integer, %s);\n' % (var, val)
                    elif isinstance(val, float):
                        self.scope[var] = 'Double'
                        self.code += '%s = new(Double, %s);\n' % (var, val)
                    else:
                        continue
                elif self._nodename(e) == 'Str':
                    var = _nexttmpvar()
                    self.code += '%s = new(String, "%s");\n' % (var, e.s)
                    self.scope[var] = 'String'
                elif self._nodename(e) == 'Num':
                    var = _nexttmpvar()
                    value = e.n
                    if isinstance(value, int):
                        self.scope[var] = 'Integer'
                    elif isinstance(value, float):
                        self.scope[var] = 'Double'
                    else:
                        raise Exception('Unsupported type: %s' % type(value))
                    self.code += '%s = new(%s, %s);\n' % (var, self.scope[var], value)
                elif self._nodename(e) == 'UnaryOp' and self._nodename(e.op) == 'USub':
                    var = _nexttmpvar()
                    inner = e.operand
                    innername = self._nodename(inner)
                    if innername == 'Constant' and isinstance(inner.value, (int, float)):
                        val = inner.value
                        cls = 'Integer' if isinstance(val, int) else 'Double'
                        self.scope[var] = cls
                        self.code += '%s = new(%s, -%s);\n' % (var, cls, val)
                    elif innername == 'Num':
                        val = inner.n
                        cls = 'Integer' if isinstance(val, int) else 'Double'
                        self.scope[var] = cls
                        self.code += '%s = new(%s, -%s);\n' % (var, cls, val)
                    else:
                        continue
                else:
                    continue
                self.code += 'call(%s, add, %s);\n' % (varname, var)
        else:
            self.code += '?'


class ExprVisitor(BaseVisitor):

    def visit_Expr(self, node):
        # inline code
        val = node.value
        if self._nodename(val) == 'Bytes':
            raw = val.s
        elif self._nodename(val) == 'Constant' and isinstance(val.value, bytes):
            raw = val.value
        else:
            v = RootVisitor(self.scope, self.parentscope)
            v.visit(node.value)
            self.code += v.code
            return

        unescaped = raw.decode('utf-8')
        escaped = ''
        quotes = 0
        for c in unescaped:
            if c == '"':
                quotes += 1
                escaped += c
            elif c == '\n' and quotes % 2 == 1:
                escaped += '\\n'
            else:
                escaped += c
        self.code += escaped
