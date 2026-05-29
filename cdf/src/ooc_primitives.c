#include "ooc_primitives.h"
#include <stdio.h>

static String * Boolean_to_string(ObjectPtr _this) {
    make_this(Boolean, _this);
    if(this->value) {
        return new(String, "true");
    } else {
        return new(String, "false");
    }
}

static bool Boolean_equals(ObjectPtr _this, ObjectPtr _other) {
    if(_this == _other) {
        return true;
    }

    if(_other == NULL) {
        return false;
    }

    make_this(Boolean, _this);

    if(!type_equal(_other, "Boolean")) {
        return false;
    }

    Boolean * other = (Boolean *) _other;
    return this->value == other->value;
}
static bool Boolean_from_string(ObjectPtr _this, String * string) {
    make_this(Boolean, _this);
    if(call(string, equals_cstring, "true")) {
        this->value = true;
    } else if(call(string, equals_cstring, "false")) {
        this->value = false;
    } else {
        return false;
    }
    return true;
}

Boolean * Boolean_new1(Boolean * this, bool value) {
    super(Object, Boolean);
    override(Object, to_string, Boolean_to_string);
    override(Object, equals, Boolean_equals);
    this->from_string = Boolean_from_string;
    this->value = value;
    return this;
}

Boolean * Boolean_new(Boolean * this) {
    this(Boolean, false);
    return this;
}

void Boolean_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}

static Integer * Integer_add_int(ObjectPtr _this, int i) {
    make_this(Integer, _this);
    return new(Integer, this->value + i);
}

static Integer * Integer_add(ObjectPtr _this, Integer * i) {
    return Integer_add_int(_this, i->value);
}

static void Integer_increment_int(ObjectPtr _this, int i) {
    make_this(Integer, _this);
    this->value += i;
}

static void Integer_increment(ObjectPtr _this, Integer * i) {
    Integer_increment_int(_this, i->value);
}

static Integer * Integer_subtract_int(ObjectPtr _this, int i) {
    make_this(Integer, _this);
    return new(Integer, this->value - i);
}

static Integer * Integer_subtract(ObjectPtr _this, Integer * i) {
    return Integer_subtract_int(_this, i->value);
}

static void Integer_decrement_int(ObjectPtr _this, int i) {
    make_this(Integer, _this);
    this->value -= i;
}

static void Integer_decrement(ObjectPtr _this, Integer * i) {
    Integer_decrement_int(_this, i->value);
}

static Integer * Integer_multiply_int(ObjectPtr _this, int i) {
    make_this(Integer, _this);
    return new(Integer, this->value * i);
}

static Integer * Integer_multiply(ObjectPtr _this, Integer * i) {
    return Integer_multiply_int(_this, i->value);
}

static void Integer_multiply_inplace_int(ObjectPtr _this, int i) {
    make_this(Integer, _this);
    this->value *= i;
}

static void Integer_multiply_inplace(ObjectPtr _this, Integer * i) {
    Integer_multiply_inplace_int(_this, i->value);
}

static Integer * Integer_divide_int(ObjectPtr _this, int i) {
    make_this(Integer, _this);
    return new(Integer, this->value / i);
}

static Integer * Integer_divide(ObjectPtr _this, Integer * i) {
    return Integer_divide_int(_this, i->value);
}

static void Integer_divide_inplace_int(ObjectPtr _this, int i) {
    make_this(Integer, _this);
    this->value /= i;
}

static void Integer_divide_inplace(ObjectPtr _this, Integer * i) {
    Integer_divide_inplace_int(_this, i->value);
}

static String * Integer_to_string(ObjectPtr _this) {
    make_this(Integer, _this);
    char buf[15];
    sprintf(buf, "%d", this->value);
    return new(String, buf);
}

static bool Integer_from_string(ObjectPtr _this, String * string) {
    int v = (int) strtol(call(string, to_cstring), NULL, 10);
    if(v == 0 && !call(string, equals_cstring, "0")) {
        return false;
    }
    make_this(Integer, _this);
    this->value = v;
    return true;
}

static bool Integer_equals(ObjectPtr _this, void * _other) {
    if(_this == _other) {
        return true;
    }

    if(_other == NULL) {
        return false;
    }

    make_this(Integer, _this);;

    if(!type_equal(_other, "Integer")) {
        return false;
    }

    Integer * other = (Integer *) _other;
    return this->value == other->value;
}

Integer * Integer_new1(Integer * this, int value) {
    super(Object, Integer);
    override(Object, to_string, Integer_to_string);
    override(Object, equals, Integer_equals);
    this->value = value;
    this->add = Integer_add;
    this->add_int = Integer_add_int;
    this->increment = Integer_increment;
    this->increment_int = Integer_increment_int;
    this->subtract = Integer_subtract;
    this->subtract_int = Integer_subtract_int;
    this->decrement = Integer_decrement;
    this->decrement_int = Integer_decrement_int;
    this->multiply = Integer_multiply;
    this->multiply_int = Integer_multiply_int;
    this->multiply_inplace = Integer_multiply_inplace;
    this->multiply_inplace_int = Integer_multiply_inplace_int;
    this->divide = Integer_divide;
    this->divide_int = Integer_divide_int;
    this->divide_inplace = Integer_divide_inplace;
    this->divide_inplace_int = Integer_divide_inplace_int;
    this->from_string = Integer_from_string;
    return this;
}

Integer * Integer_new(Integer * this) {
    this(Integer, 0);
    return this;
}

void Integer_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}

static Long * Long_add_long(ObjectPtr _this, long i) {
    make_this(Long, _this);
    return new(Long, this->value + i);
}

static Long * Long_add(ObjectPtr _this, Long * i) {
    return Long_add_long(_this, i->value);
}

static void Long_increment_long(ObjectPtr _this, long i) {
    make_this(Long, _this);
    this->value += i;
}

static void Long_increment(ObjectPtr _this, Long * i) {
    Long_increment_long(_this, i->value);
}

static Long * Long_subtract_long(ObjectPtr _this, long i) {
    make_this(Long, _this);
    return new(Long, this->value - i);
}

static Long * Long_subtract(ObjectPtr _this, Long * i) {
    return Long_subtract_long(_this, i->value);
}

static void Long_decrement_long(ObjectPtr _this, long i) {
    make_this(Long, _this);
    this->value -= i;
}

static void Long_decrement(ObjectPtr _this, Long * i) {
    Long_decrement_long(_this, i->value);
}

static Long * Long_multiply_long(ObjectPtr _this, long i) {
    make_this(Long, _this);
    return new(Long, this->value * i);
}

static Long * Long_multiply(ObjectPtr _this, Long * i) {
    return Long_multiply_long(_this, i->value);
}

static void Long_multiply_inplace_long(ObjectPtr _this, long i) {
    make_this(Long, _this);
    this->value *= i;
}

static void Long_multiply_inplace(ObjectPtr _this, Long * i) {
    Long_multiply_inplace_long(_this, i->value);
}

static Long * Long_divide_long(ObjectPtr _this, long i) {
    make_this(Long, _this);
    return new(Long, this->value / i);
}

static Long * Long_divide(ObjectPtr _this, Long * i) {
    return Long_divide_long(_this, i->value);
}

static void Long_divide_inplace_long(ObjectPtr _this, long i) {
    make_this(Long, _this);
    this->value /= i;
}

static void Long_divide_inplace(ObjectPtr _this, Long * i) {
    Long_divide_inplace_long(_this, i->value);
}

static String * Long_to_string(ObjectPtr _this) {
    make_this(Long, _this);
    char buf[20];
    sprintf(buf, "%ld", this->value);
    return new(String, buf);
}

static bool Long_from_string(ObjectPtr _this, String * string) {
    long v = strtol(call(string, to_cstring), NULL, 10);
    if(v == 0 && !call(string, equals_cstring, "0")) {
        return false;
    }
    make_this(Long, _this);
    this->value = v;
    return true;
}

static bool Long_equals(ObjectPtr _this, void * _other) {
    if(_this == _other) {
        return true;
    }

    if(_other == NULL) {
        return false;
    }

    make_this(Long, _this);

    if(!type_equal(_other, "Long")) {
        return false;
    }

    Long * other = (Long *) _other;
    return this->value == other->value;
}

Long * Long_new1(Long * this, long value) {
    super(Object, Long);
    override(Object, to_string, Long_to_string);
    override(Object, equals, Long_equals);
    this->value = value;
    this->add = Long_add;
    this->add_long = Long_add_long;
    this->increment = Long_increment;
    this->increment_long = Long_increment_long;
    this->subtract = Long_subtract;
    this->subtract_long = Long_subtract_long;
    this->decrement = Long_decrement;
    this->decrement_long = Long_decrement_long;
    this->multiply = Long_multiply;
    this->multiply_long = Long_multiply_long;
    this->multiply_inplace = Long_multiply_inplace;
    this->multiply_inplace_long = Long_multiply_inplace_long;
    this->divide = Long_divide;
    this->divide_long = Long_divide_long;
    this->divide_inplace = Long_divide_inplace;
    this->divide_inplace_long = Long_divide_inplace_long;
    this->from_string = Long_from_string;
    return this;
}

Long * Long_new(Long * this) {
    this(Long, 0L);
    return this;
}

void Long_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}

static Double * Double_add_double(ObjectPtr _this, double i) {
    make_this(Double, _this);
    return new(Double, this->value + i);
}

static Double * Double_add(ObjectPtr _this, Double * i) {
    return Double_add_double(_this, i->value);
}

static void Double_increment_double(ObjectPtr _this, double i) {
    make_this(Double, _this);
    this->value += i;
}

static void Double_increment(ObjectPtr _this, Double * i) {
    Double_increment_double(_this, i->value);
}

static Double * Double_subtract_double(ObjectPtr _this, double i) {
    make_this(Double, _this);
    return new(Double, this->value - i);
}

static Double * Double_subtract(ObjectPtr _this, Double * i) {
    return Double_subtract_double(_this, i->value);
}

static void Double_decrement_double(ObjectPtr _this, double i) {
    make_this(Double, _this);
    this->value -= i;
}

static void Double_decrement(ObjectPtr _this, Double * i) {
    Double_decrement_double(_this, i->value);
}

static Double * Double_multiply_double(ObjectPtr _this, double i) {
    make_this(Double, _this);
    return new(Double, this->value * i);
}

static Double * Double_multiply(ObjectPtr _this, Double * i) {
    return Double_multiply_double(_this, i->value);
}

static void Double_multiply_inplace_double(ObjectPtr _this, double i) {
    make_this(Double, _this);
    this->value *= i;
}

static void Double_multiply_inplace(ObjectPtr _this, Double * i) {
    Double_multiply_inplace_double(_this, i->value);
}

static Double * Double_divide_double(ObjectPtr _this, double i) {
    make_this(Double, _this);
    return new(Double, this->value / i);
}

static Double * Double_divide(ObjectPtr _this, Double * i) {
    return Double_divide_double(_this, i->value);
}

static void Double_divide_inplace_double(ObjectPtr _this, double i) {
    make_this(Double, _this);
    this->value /= i;
}

static void Double_divide_inplace(ObjectPtr _this, Double * i) {
    Double_divide_inplace_double(_this, i->value);
}

static String * Double_to_string(ObjectPtr _this) {
    make_this(Double, _this);
    char buf[20];
    sprintf(buf, "%.4f", this->value);
    return new(String, buf);
}

static bool Double_from_string(ObjectPtr _this, String * string) {
    double v = strtod(call(string, to_cstring), NULL);
    if(v == 0.0 && !call(string, equals_cstring, "0.0")) {
        return false;
    }
    make_this(Double, _this);
    this->value = v;
    return true;
}

static bool Double_equals(ObjectPtr _this, void * _other) {
    if(_this == _other) {
        return true;
    }

    if(_other == NULL) {
        return false;
    }

    make_this(Double, _this);

    if(!type_equal(_other, "Double")) {
        return false;
    }

    Double * other = (Double *) _other;
    return this->value == other->value;
}

Double * Double_new1(Double * this, double value) {
    super(Object, Double);
    override(Object, to_string, Double_to_string);
    override(Object, equals, Double_equals);
    this->value = value;
    this->add = Double_add;
    this->add_double = Double_add_double;
    this->increment = Double_increment;
    this->increment_double = Double_increment_double;
    this->subtract = Double_subtract;
    this->subtract_double = Double_subtract_double;
    this->decrement = Double_decrement;
    this->decrement_double = Double_decrement_double;
    this->multiply = Double_multiply;
    this->multiply_double = Double_multiply_double;
    this->multiply_inplace = Double_multiply_inplace;
    this->multiply_inplace_double = Double_multiply_inplace_double;
    this->divide = Double_divide;
    this->divide_double = Double_divide_double;
    this->divide_inplace = Double_divide_inplace;
    this->divide_inplace_double = Double_divide_inplace_double;
    this->from_string = Double_from_string;
    return this;
}

Double * Double_new(Double * this) {
    this(Double, 0.0);
    return this;
}

void Double_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}
