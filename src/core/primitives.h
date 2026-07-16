#ifndef CDF_CORE_PRIMITIVES_H
#define CDF_CORE_PRIMITIVES_H

#include "object.h"

typedef struct _Boolean {
    inherits(Object);
    String * (*to_string)(ObjectPtr);
    bool (*equals)(ObjectPtr, ObjectPtr);
    bool (*from_string)(ObjectPtr, String *);
    bool value;
} Boolean;

Boolean * Boolean_new(Boolean * this);
Boolean * Boolean_new1(Boolean * this, bool);
void Boolean_delete(ObjectPtr);

typedef struct _Integer {
    inherits(Object);
    String * (*to_string)(ObjectPtr);
    bool (*equals)(ObjectPtr, ObjectPtr);
    int value;
    bool (*from_string)(ObjectPtr, String *);
    struct _Integer * (*add)(ObjectPtr, struct _Integer *);
    struct _Integer * (*add_int)(ObjectPtr, int);
    void (*increment)(ObjectPtr, struct _Integer *);
    void (*increment_int)(ObjectPtr, int);
    struct _Integer * (*subtract)(ObjectPtr, struct _Integer *);
    struct _Integer * (*subtract_int)(ObjectPtr, int);
    void (*decrement)(ObjectPtr, struct _Integer *);
    void (*decrement_int)(ObjectPtr, int);
    struct _Integer * (*multiply)(ObjectPtr, struct _Integer *);
    struct _Integer * (*multiply_int)(ObjectPtr, int);
    void (*multiply_inplace)(ObjectPtr, struct _Integer *);
    void (*multiply_inplace_int)(ObjectPtr, int);
    struct _Integer * (*divide)(ObjectPtr, struct _Integer *);
    struct _Integer * (*divide_int)(ObjectPtr, int);
    void (*divide_inplace)(ObjectPtr, struct _Integer *);
    void (*divide_inplace_int)(ObjectPtr, int);

} Integer;

Integer * Integer_new(Integer * this);
Integer * Integer_new1(Integer * this, int);
void Integer_delete(ObjectPtr);


typedef struct _Long {
    inherits(Object);
    String * (*to_string)(ObjectPtr);
    bool (*equals)(ObjectPtr, ObjectPtr);
    long value;
    bool (*from_string)(ObjectPtr, String *);
    struct _Long * (*add)(ObjectPtr, struct _Long *);
    struct _Long * (*add_long)(ObjectPtr, long);
    void (*increment)(ObjectPtr, struct _Long *);
    void (*increment_long)(ObjectPtr, long);
    struct _Long * (*subtract)(ObjectPtr, struct _Long *);
    struct _Long * (*subtract_long)(ObjectPtr, long);
    void (*decrement)(ObjectPtr, struct _Long *);
    void (*decrement_long)(ObjectPtr, long);
    struct _Long * (*multiply)(ObjectPtr, struct _Long *);
    struct _Long * (*multiply_long)(ObjectPtr, long);
    void (*multiply_inplace)(ObjectPtr, struct _Long *);
    void (*multiply_inplace_long)(ObjectPtr, long);
    struct _Long * (*divide)(ObjectPtr, struct _Long *);
    struct _Long * (*divide_long)(ObjectPtr, long);
    void (*divide_inplace)(ObjectPtr, struct _Long *);
    void (*divide_inplace_long)(ObjectPtr, long);

} Long;

Long * Long_new(Long * this);
Long * Long_new1(Long * this, long);
void Long_delete(ObjectPtr);

typedef struct _Double {
    inherits(Object);
    String * (*to_string)(ObjectPtr);
    bool (*equals)(ObjectPtr, ObjectPtr);
    double value;
    bool (*from_string)(ObjectPtr, String *);
    struct _Double * (*add)(ObjectPtr, struct _Double *);
    struct _Double * (*add_double)(ObjectPtr, double);
    void (*increment)(ObjectPtr, struct _Double *);
    void (*increment_double)(ObjectPtr, double);
    struct _Double * (*subtract)(ObjectPtr, struct _Double *);
    struct _Double * (*subtract_double)(ObjectPtr, double);
    void (*decrement)(ObjectPtr, struct _Double *);
    void (*decrement_double)(ObjectPtr, double);
    struct _Double * (*multiply)(ObjectPtr, struct _Double *);
    struct _Double * (*multiply_double)(ObjectPtr, double);
    void (*multiply_inplace)(ObjectPtr, struct _Double *);
    void (*multiply_inplace_double)(ObjectPtr, double);
    struct _Double * (*divide)(ObjectPtr, struct _Double *);
    struct _Double * (*divide_double)(ObjectPtr, double);
    void (*divide_inplace)(ObjectPtr, struct _Double *);
    void (*divide_inplace_double)(ObjectPtr, double);

} Double;

Double * Double_new(Double * this);
Double * Double_new1(Double * this, double);
void Double_delete(ObjectPtr);

#endif // OOC_PRIMITIVES_H
