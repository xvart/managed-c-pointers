# managed-c-pointers

A simple managed auto pointer like system for C.

The system works like every other whith a linked list that tracks allocations, uses unix `sys/queue.h LIST`.

This is a wrapper that creates a back end list to track pointers created and to free them on demand by using mp_return it
provides similar function C++ auto pointers.

You need to call `mp_return()` or `mp_clear()` otherwise you will not free the pointers created, it's still C so no compiler magic is in play.

The library creates the following as wrappers around the system variants.
```
mp_init()			- initializes the back end linked list, must be called first

mp_link(void *)						- Add an unmanaged pointer to the list
mp_unlink(void *)					- Remove a pointer from the link
mp_link(void *, void (*)(void*))	- Add a pointer to the list with free

mp_alloc(size_t)
mp_calloc(size_t, size_t)

- These are used to add a function pointer to free the data when
- vanilla free just won't cut it.

mp_allocx(size_t , void (*)(void*))
mp_callocx(size_t, size_t , void (*)(void*))

mp_realloc(void *, size_t)
mp_reallocarray(void *, size_t, size_t)

mp_free(void *)		- free a managed pointer.
mp_clear(void) 		- This frees all managed pointers.

mp_return			- functions as a normal return with the addition of running mp_clear().

```

Example code,

```
#include <stdio.h>
#include <string.h>
#include "mptr.h"

int sub();

int  main() {
	mp_init();
	char *cp = mp_calloc(1, strlen("test me") + 1, NULL);
	printf("sub %u\n", sub());
	mp_return(5);
}

int sub() {
	mp_init();
	char *cp = mp_alloc(strlen("hello") + 1, NULL);
	sprintf(cp, "hello");
	printf("%s ", cp);
	mp_free(cp);
	mp_free(cp);
	mp_return(1);
}
```
