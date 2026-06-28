
#ifndef DB_ENTITY_SAMPLES_CUSTOMER_H_
#define DB_ENTITY_SAMPLES_CUSTOMER_H_

#include <ooc.h>
#include <ooc_macros.h>
#include <entity.h>

typedef struct _Customer {
	inherits(Entity);
	List * (*_fields)(ObjectPtr);

	String * first_name;
	String * last_name;
	Integer * age;

} Customer;


List * Customer_fields(ObjectPtr _this) {
	make_this(Customer, _this);
	List * fields = new(List);
	String * name = new(String, "first_name");
	String * type = new(String, "String");
	FieldMetadata * fm = new(FieldMetadata, (Entity *)this, (Object**)&(this->first_name), name, type);
	call(fields, add, fm);
	REFCDEC(name);
	REFCDEC(type);
	REFCDEC(fm);

	name = new(String, "last_name");
	type = new(String, "String");
	fm = new(FieldMetadata, (Entity*)this, (Object**)&(this->last_name), name, type);
	call(fields, add, fm);
	REFCDEC(name);
	REFCDEC(type);
	REFCDEC(fm);

	name = new(String, "age");
	type = new(String, "Integer");
	fm = new(FieldMetadata, (Entity*)this, (Object**)&(this->age), name, type);
	call(fields, add, fm);
	REFCDEC(name);
	REFCDEC(type);
	REFCDEC(fm);


	return fields;
}

void Customer_delete(ObjectPtr _this) {
    make_this(Customer, _this);
    REFCDEC(this->first_name);
    REFCDEC(this->last_name);
    REFCDEC(this->age);
    super_delete(Entity, _this);
}

Customer * Customer_new3(Customer * this, String * first_name, String * last_name, Integer * age) {
    super(Entity, Customer);
    override(Entity, _fields, Customer_fields);
    REFCINC(first_name);
    this->first_name = first_name;
    REFCINC(last_name);
    this->last_name = last_name;
    REFCINC(age);
    this->age = age;
    return this;
}

Customer * Customer_new(Customer * this) {
    this(Customer, NULL, NULL, NULL);
    return this;
}



#endif
