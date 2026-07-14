#ifndef CDF_DB_ENTITY_ENTITY_H
#define CDF_DB_ENTITY_ENTITY_H

#include "../core/core.h"
#include <stdint.h>

typedef struct _Entity {
	inherits(Object);
	Long * id;

	List * (*_fields)(ObjectPtr);
	List * (*fields)(ObjectPtr, bool);

	String * (*table_name)(ObjectPtr);

	String * (*to_json)(ObjectPtr, List * fields);
	void (*from_json)(ObjectPtr, String *, List * fields);
	Map * (*to_map)(ObjectPtr, List * fields);
	void (*from_map)(ObjectPtr, Map *, List * fields);

} Entity;


Entity * Entity_new(Entity *);
void Entity_delete(ObjectPtr);

typedef struct _FieldMetadata {
	inherits(Object);
	intptr_t offset;
	String * name;
	String * type;
} FieldMetadata;

FieldMetadata * FieldMetadata_new4(FieldMetadata *, Entity *, Object **, String *, String *);
void FieldMetadata_delete(ObjectPtr);
#endif
