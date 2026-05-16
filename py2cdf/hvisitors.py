import ast
import visitorcommons

class BaseVisitor(ast.NodeVisitor):
    def __init__(self):
        self.code = ''

class RootVisitor(BaseVisitor):
    def visit_Import(self, node):
        v = ImportVisitor()
        v.visit(node)
        self.code += v.code

    def visit_FunctionDef(self, node):
        v = FunctionDefVisitor()
        v.visit(node)
        self.code += v.code

    def visit_ClassDef(self, node):
        v = ClassDefVisitor()
        v.visit(node)
        self.code += v.code

    def generic_visit(self, node):
        print(type(node).__name__)


class ClassDefVisitor(BaseVisitor):
    def __init__(self):
        super().__init__()
        self.constructors = []

    def visit_ClassDef(self, node):
        name = node.name

        self.code += 'typedef struct _%s {\n' % name
        if not node.bases:
            base_class = 'Object'
        else:
            base_class = node.bases[0]
        self.code += 'inherits(%s);\n' % base_class

        for n in node.body:
            self.visit(n)

        for c in self.constructors:
            for n in c.body:
                target = n.targets[0]
                if target.value.id == 'self':
                    arg = n.value.id
                    type_ = 'ObjectPtr'
                    for a in c.args.args:
                        if arg == a.arg:
                            type_ = a.annotation.id
                    self.code += '%s* %s;\n' % (type_, target.attr)

        self.code += '} %s;\n' % name

        for c in self.constructors:
            args = visitorcommons.arguments_declaration(c.args)
            args_num = len(args.split(',')) - 1
            if args_num > 0:
                args_num = str(args_num)
            else:
                args_num = ''

            self.code += '%s* %s_new%s(%s);\n' % (name, name, args_num, args)

        self.code += 'void %s_delete(ObjectPtr _this);\n' % name

    def visit_FunctionDef(self, node):
        func_name = node.name
        if func_name == '__init__':
            self.constructors.append(node)
        else:
            name = node.name
            self.code += '%s (*%s)(%s);\n' % (visitorcommons.return_type(node), name, visitorcommons.arguments_declaration(node.args))


class ImportVisitor(BaseVisitor):
    CODE_PATTERN = '#include <%s>\n'

    def visit_Import(self, node):
        for alias in node.names:
            self.code += self.CODE_PATTERN % alias.name


class FunctionDefVisitor(BaseVisitor):
    def visit_FunctionDef(self, node):
        if not node.name.startswith('_'):
            self.code += '%s %s(%s);\n' % (visitorcommons.return_type(node), node.name, visitorcommons.arguments_declaration(node.args))

