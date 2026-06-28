#include "customer.h"
#include "console.h"
#include "entitymanager.h"
#include "sqlite_database.h"
#include <assert.h>
#include <string.h>
#include <math.h>

static void entitymanager_test(void)
{
	Entity * customer = new(Customer, REFCTMP(new(String, "John")), REFCTMP(new(String, "Doe")), REFCTMP(new(Integer, 55)));
	Database * db = new(SQLiteDatabase);

	EntityManager * em = new(EntityManager, db);
	REFCDEC(db);
	call(em, save, customer);

	assert((customer->id->value) == (1L));
	assert(strcmp(call(((Customer*)customer)->first_name, to_cstring), "John") == 0);
	assert(strcmp(call(((Customer*)customer)->last_name, to_cstring), "Doe") == 0);
	assert((((Customer*)customer)->age->value) == (55));

	Entity * loaded_customer = new(Customer);
    loaded_customer->id = new(Long, 1L);
    call(em, load, loaded_customer);

	assert((loaded_customer->id->value) == (1L));
	assert(((Customer*)loaded_customer)->first_name != NULL);
	assert(strcmp(call(((Customer*)loaded_customer)->first_name, to_cstring), "John") == 0);
	assert(((Customer*)loaded_customer)->last_name != NULL);
	assert(strcmp(call(((Customer*)loaded_customer)->last_name, to_cstring), "Doe") == 0);
	assert(((Customer*)loaded_customer)->age != NULL);
	assert((((Customer*)loaded_customer)->age->value) == (55));

    call(em, remove, customer);

    call(em, load, loaded_customer);
	assert((loaded_customer->id->value) == (-1L));

	REFCDEC(em);
	REFCDEC(customer);
	REFCDEC(loaded_customer);
}
int main(void)
{
    entitymanager_test();
    return 0;
}


