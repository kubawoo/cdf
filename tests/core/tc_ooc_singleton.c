#include "ooc.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void singleton_test(void)
{
	String * a = _cdf_get_singleton("String");
	assert((a) == (NULL));
	String * b = _cdf_set_singleton("String", new(String));
	assert((b) != (NULL));
}

static void singleton_test2(void)
{
	Integer * i = singleton(Integer);
	Integer * j = singleton(Integer);
	Integer * k = singleton(Integer);

	assert(i != NULL);
	assert(j != NULL);
	assert(k != NULL);
	assert((void*)(i) == (void*)(j));
	assert((void*)(i) == (void*)(k));
	assert((void*)(j) == (void*)(k));
}
int main(void)
{
    singleton_test();
    singleton_test2();
    return 0;
}


