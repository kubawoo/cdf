import ooc.h
import stdio.h
import io.h

def hello():
    i = 5
    d = 1.2
    b = False
    x = None
    s = "hello ooc world!"
    l = ['a', 12, True, -5.5, x]
#    m = {}
    ls = l.to_string()
    ss = s.to_string()
    c = Console()
    c.print_object(s)
    c.print_object(l)


#def hello2(s : String) -> String:
#    b'''
#    printf("%s\n", call(s, to_cstring));
#    return call(s, substring, 0, 1);
#    '''
#
#def _hello3() -> None:
#    pass
#
#def addints(a: int, b: int) -> int:
#   b'return a + b;'
#
#
# class NString(String):
#     def __init__(self, s):
#         self.s = s
#
#     @override(String)
#     def foo():
#         pass
#
#     def __del__(self):
#         pass

