#include "entitymanager.h"
#include <stdio.h>

EntityMetadata * EntityMetadata_new2(EntityMetadata * this, String * table_name, List * fields)  {
	super(Object, EntityMetadata);
	REFCINC(fields);
	this->fields = fields;
	REFCINC(table_name);
	this->table_name = table_name;
	return this;
}
void EntityMetadata_delete(ObjectPtr _this) {
	make_this(EntityMetadata, _this);
	REFCDEC(this->table_name);
	REFCDEC(this->fields);
}

static String * sqlescape(Object * object) {
	if(object == NULL) {
		return new(String, "NULL");
	}
	String * v = call(object, to_string);
	if(type_equal(object, "String")) {
		String * tmp = v;
		v = new(String, "'");
		call(v, append, tmp);
		call(v, append_char, '\'');
		REFCDEC(tmp);
	}
	return v;
}

static bool table_exists(DbConnection * conn, String * tablename) {
	String * sql = new(String, "SELECT COUNT(1) AS CNT FROM ");
	call(sql, append, tablename);
	call(sql, append_cstring, ";");
	List * result = call(conn, query, sql);
	REFCDEC(sql);
	if(result == NULL) {
		String * err = call(conn, last_error_msg);
		fprintf(stderr, "error=%s\n", call(err, to_cstring));
		REFCDEC(err);
		return false;
	}

	String * s = call(result, to_string);

	fprintf(stderr, "result=%s\n", call(s, to_cstring));
	REFCDEC(result);
	REFCDEC(s);
	return true;
}

static void create_table(DbConnection * conn, String * tablename, List * fields) {

	String * sql = new(String, "CREATE TABLE ");


	call(sql, append, tablename);
	call(sql, append_cstring, "(id INTEGER PRIMARY KEY, ");
	for(int i = 0; i < fields->length; ++i) {
		FieldMetadata * field = (FieldMetadata*) call(fields, get, i);

		String * datatype = NULL;
		if(call(field->type, equals_cstring, "String")) {
			datatype = new(String, "TEXT");
		} else if(call(field->type, equals_cstring, "Integer")) {
			datatype = new(String, "INT");
		} else if(call(field->type, equals_cstring, "Long")) {
			datatype = new(String, "INT");
		}
		//TODO add missing datatypes and make it db-specific

		if(datatype != NULL) {
			call(sql, append, field->name);
			call(sql, append_char, ' ');
			call(sql, append, datatype);
			REFCDEC(datatype);
			if(i != fields->length - 1) {
				call(sql, append_cstring, ", ");
			}
		} else {
			fprintf(stderr, "Unsupported datatype: %s\n", call(field->type, to_cstring));
		}

		REFCDEC(field);
	}
	call(sql, append_cstring, ");");

	fprintf(stderr, "About to execute sql=%s\n", call(sql, to_cstring));
	bool ret = call(conn, exec, sql);
	fprintf(stderr, "ret=%d\n", ret);
	REFCDEC(sql);

}


static EntityMetadata * get_metadata(ObjectPtr _this, Entity * e) {
	make_this(EntityManager, _this);
	String * type = new(String, ((Object*)e)->type);
	EntityMetadata * em = call(this->known_entities, get, type);
	if(em == NULL) {
		List * fields = call(e, fields, false);
		String * table_name = call(e, table_name);
		em = new(EntityMetadata, table_name, fields);

		call(this->known_entities, put, type, em);
		if(!table_exists(this->conn, table_name)) {
			create_table(this->conn, table_name, fields);
		}
		REFCDEC(fields);
		REFCDEC(table_name);
	}
	REFCDEC(type);
	return em;
}



static void load(ObjectPtr _this, Entity * e) {
	make_this(EntityManager, _this);
	EntityMetadata * metadata = get_metadata(this, e);

    String * sql = new(String, "SELECT * FROM ");
    call(sql, append, metadata->table_name);
    call(sql, append_cstring, " WHERE id=");
    String * id_string = call(e->id, to_string);
    call(sql, append, id_string);
    call(sql, append_cstring, ";");
    REFCDEC(id_string);
	
	
	fprintf(stderr, "About to execute sql=%s\n", call(sql, to_cstring));
	List * result = call(this->conn, query, sql);
	if(result == NULL) { 
		String *s = call(this->conn, last_error_msg);
        fprintf(stderr, "errormsg=%s\n", call(s, to_cstring));
        REFCDEC(s);
    }

    String *s = call(result, to_string);
    fprintf(stderr, "result=%s\n", call(s, to_cstring));
	REFCDEC(s);

    if(result->length == 1) {
		Map * m = call(result, get, 0);
		call(e, from_map, m, metadata->fields);
    	REFCDEC(m);
    } else {
		//TODO: error, expected exactly one entity
		e->id->value = -1L;
    }

    REFCDEC(result);
    REFCDEC(metadata);
	REFCDEC(sql);
}

static void _remove(ObjectPtr _this, Entity * e) {
	make_this(EntityManager, _this);
    EntityMetadata * metadata = get_metadata(this, e);
	String * sql = new(String, "DELETE FROM ");
	call(sql, append, metadata->table_name);
    call(sql, append_cstring, " WHERE id=");

	String * id_string = call(e->id, to_string);
	call(sql, append, id_string);
    call(sql, append_cstring, ";");
    REFCDEC(id_string);
	REFCDEC(metadata);

    fprintf(stderr, "About to execute sql=%s\n", call(sql, to_cstring));
    bool ret = call(this->conn, exec, sql);
    if(!ret) {
        String *s = call(this->conn, last_error_msg);
        fprintf(stderr, "errormsg=%s\n", call(s, to_cstring));
        REFCDEC(s);
    }

	REFCDEC(sql);

}

static void save(ObjectPtr _this, Entity * e) {
	make_this(EntityManager, _this);
	EntityMetadata * em = get_metadata(this, e);

	String * sql = new(String, "INSERT INTO ");
	call(sql, append, em->table_name);
	call(sql, append_cstring, " (");

	String * sql_values = new(String);

	Map * map = call(e, to_map, em->fields);
	for(int i = 0; i < em->fields->length; ++i) {
		FieldMetadata * fm = (FieldMetadata*) call(em->fields, get, i);

		call(sql, append, fm->name);

		Object * value = call(map, get, fm->name);
		String * sql_value = sqlescape(value);
		REFCDEC(value);
		call(sql_values, append, sql_value);
		REFCDEC(sql_value);

		REFCDEC(fm);

		if(i < em->fields->length - 1) {
			call(sql, append_cstring, ", ");
			call(sql_values, append_cstring, ", ");
		} else {
			call(sql, append_cstring, ") VALUES (");
			call(sql, append, sql_values);
			call(sql, append_cstring, ");");
		}
	}
	REFCDEC(sql_values);
	REFCDEC(map);

	fprintf(stderr, "About to execute sql=%s\n", call(sql, to_cstring));
	bool ret = call(this->conn, exec, sql);
	if(!ret) {
		String *s = call(this->conn, last_error_msg);
		fprintf(stderr, "errormsg=%s\n", call(s, to_cstring));
		REFCDEC(s);
	}

	long id = call(this->conn, last_inserted_id);
	e->id = new(Long, id);

	REFCDEC(sql);
	REFCDEC(em);
}

static void update(ObjectPtr _this, Entity * e) {
	//TODO
}



EntityManager * EntityManager_new1(EntityManager * this, Database * db) {
	super(Object, EntityManager);
	this->known_entities = new(Map);
	REFCINC(db);
	this->db = db;
	this->conn = call(db, open);

	this->load = load;
	this->remove = _remove;
	this->save = save;
	this->update = update;

	return this;
}

void EntityManager_delete(ObjectPtr _this) {
	make_this(EntityManager, _this);
	REFCDEC(this->conn);
	REFCDEC(this->db);
	REFCDEC(this->known_entities);

	super_delete(Object, _this);
}

