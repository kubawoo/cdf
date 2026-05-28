#ifndef OOC_MACROS_H
#define OOC_MACROS_H

#include <stdlib.h>
#include <string.h>
#include "ooc_memory.h"

#define GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,NAME,...) NAME

#define _new0 #error Invalid constructor call
#define _new1(class)(void*) class##_new(NULL)
#define _new2(class, a) (void*)class##_new1(NULL, a)
#define _new3(class, a, b) (void*)class##_new2(NULL, a, b)
#define _new4(class, a, b, c) (void*)class##_new3(NULL, a, b, c)
#define _new5(class, a, b, c, d) (void*)class##_new4(NULL, a, b, c, d)
#define _new6(class, a, b, c, d, e) (void*)class##_new5(NULL, a, b, c, d, e)
#define _new7(class, a, b, c, d, e, f) (void*)class##_new6(NULL, a, b, c, d, e, f)
#define _new8(class, a, b, c, d, e, f, g) (void*)class##_new7(NULL, a, b, c, d, e, f, g)
#define _new9(class, a, b, c, d, e, f, g, h) (void*)class##_new8(NULL, a, b, c, d, e, f, g, h)
#define _new10(class, a, b, c, d, e, f, g, h, i) (void*)class##_new9(NULL, a, b, c, d, e, f, g, h, i)
#define _new11(class, a, b, c, d, e, f, g, h, i, j) (void*)class##_new10(NULL, a, b, c, d, e, f, g, h, i, j)
#define new(...) GET_MACRO(__VA_ARGS__, _new11, _new10, _new9, _new8, _new7, _new6, _new5, _new4, _new3, _new2, _new1, _new0)(__VA_ARGS__)

#define _call1 #error Invalid method call
#define _call2(this, method) (this)->method(this)
#define _call3(this, method, a) (this)->method(this, a)
#define _call4(this, method, a, b) (this)->method(this, a, b)
#define _call5(this, method, a, b, c) (this)->method(this, a, b, c)
#define _call6(this, method, a, b, c, d) (this)->method(this, a, b, c, d)
#define _call7(this, method, a, b, c, d, e) (this)->method(this, a, b, c, d, e)
#define _call8(this, method, a, b, c, d, e, f) (this)->method(this, a, b, c, d, e, f)
#define _call9(this, method, a, b, c, d, e, f, g) (this)->method(this, a, b, c, d, e, f, g)
#define _call10(this, method, a, b, c, d, e, f, g, h) (this)->method(this, a, b, c, d, e, f, g, h)
#define _call11(this, method, a, b, c, d, e, f, g, h, i) (this)->method(this, a, b, c, d, e, f, g, h, i)
#define call(...) GET_MACRO(__VA_ARGS__, _call11, _call10, _call9, _call8, _call7, _call6, _call5, _call4, _call3, _call2, _call1)(__VA_ARGS__)

#define _this1(class) this = class##_new(this)
#define _this2(class, a) this = class##_new1(this, a)
#define _this3(class, a, b) this = class##_new2(this, a, b)
#define _this4(class, a, b, c) this = class##_new3(this, a, b, c)
#define _this5(class, a, b, c, d) this = class##_new4(this, a, b, c, d)
#define _this6(class, a, b, c, d, e) this = class##_new5(this, a, b, c, d, e)
#define _this7(class, a, b, c, d, e, f) this = class##_new6(this, a, b, c, d, e, f)
#define _this8(class, a, b, c, d, e, f, g) this = class##_new7(this, a, b, c, d, e, f, g)
#define _this9(class, a, b, c, d, e, f, g, h) this = class##_new8(this, a, b, c, d, e, f, g, h)
#define _this10(class, a, b, c, d, e, f, g, h, i) this = class##_new9(this, a, b, c, d, e, f, g, h, i)
#define _this11(class, a, b, c, d, e, f, g, h, i, j) this = class##_new10(this, a, b, c, d, e, f, g, h, i, j)
#define this(...) GET_MACRO(__VA_ARGS__, _this11, _this10, _this9, _this8, _this7, _this6, _this5, _this4, _this3, _this2, _this1)(__VA_ARGS__)

#define delete(x) do { if(!x) { break; } Object * _o = (Object *) x; _o->_ooc_destructor(_o); pool_free(x); x = NULL; } while(0)

#define inherits(class) class _ooc_base

//TODO add arguments to super constructor
#define super(BaseClass, ThisClass) if(!this) this = pool_alloc(sizeof(ThisClass));\
                                    BaseClass##_new((BaseClass *)this);\
                                    ((Object *) this)->_ooc_destructor =  ThisClass##_delete;\
                                    ((Object *) this)->type = #ThisClass


#define super_delete(BaseClass, x) BaseClass##_delete((BaseClass *) x)

#define override(BaseClass, base_method, method) ((BaseClass *) this)->base_method = method; this->base_method = method
#define super_method(BaseClass, method) this->method = ((BaseClass *)this)->method

#define make_this(class, _this) class * this = (class *) _this

#define type_equal(x, _type) (strcmp(((Object *) (x))->type, _type) == 0)
#define types_equal(x, y) (strcmp(((Object *) (x))->type, ((Object *) y)->type) == 0)

#define REFCINC(x) do { if(!x) break; Object * _o = (Object *) x; _o->_refc++; } while(0)
#define REFCDEC(x) do { if(!x) break; Object * _o = (Object *) x; _o->_refc--; if(_o->_refc <= 0)  delete(x); } while(0)
#define REFCTMP(x) _refctmp(x)


#define ObjectPtr void*

#endif

