#ifndef DB_ENTITY_MANAGER_H
#define DB_ENTITY_MANAGER_H

#include <ooc.h>
#include "entity.h"
#include "database.h"

typedef struct _EntityMetadata {
	inherits(Object);
	String * table_name;
	List * fields;
} EntityMetadata;


EntityMetadata * EntityMetadata_new2(EntityMetadata *, String *, List *);
void EntityMetadata_delete(ObjectPtr);

typedef struct _EntityManager {
	inherits(Object);

	void (*load)(ObjectPtr, Entity *); //only id is set
	void (*remove)(ObjectPtr, Entity *); //only id is set
	void (*save)(ObjectPtr, Entity *); //id is not set
	void (*update)(ObjectPtr, Entity *); //all set

	Database * db;
	DbConnection * conn;
	//SqlGenerator * generator;

	Map * known_entities;


} EntityManager;

EntityManager * EntityManager_new1(EntityManager *, Database *);
void EntityManager_delete(ObjectPtr);

#endif
