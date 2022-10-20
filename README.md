# managed-c-pointers

A simple managed auto pointer like system for C for when it all seems like too much work.

The system works like every other whith a linked list that tracks allocations, uses unix `sys/queue.h LIST` and "under the hood" it uses the variable managedptr_head created by mp_init() to create a local list needed by everything.

This is a wrapper that creates a back end list to track pointers created and to free them on demand by using mp_return it
provides similar function C++ auto pointers.

You need to call `mp_return()` or `mp_clear()` otherwise you will not free the pointers created, it's still C so no compiler magic is in play.

The library creates the following as wrappers around the system variants.
```
mp_init()			- initializes the back end linked list, must be called first in a function this creates a variable on the stack.

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

Building a library with libtool
`libtool --mode=compile gcc -g -O -c mptr.c`
`libtool --mode=link gcc -g -O -o libmptr.la mptr.lo`
`cp ./libs/mptr.a <somewhere useful>`
`cp mptr.h <also somewhere useful>`


One of the use cases mp_allocx is intended to solve is the following and while it doesn't save much by way of code size for a single allocation do it 10 times and it becomes useful.

The shorter "I don't care what I've allocated" way.
```
/* Nothing is free so while this all works with just regfree to stop the compilier screeching wrap regfree in rfree. */
void rfree(void *v) {
	regfree((regex_t *)v);
}

void fn() {
	mp_init();
	regex_t *re1_ptr = mp_allocx(sizeof(regex_t), rfree), *re2_ptr = mp_allocx(sizeof(regex_t), rfree);
	regex_t *re3_ptr = mp_allocx(sizeof(regex_t), rfree), *re4_ptr = mp_allocx(sizeof(regex_t), rfree);
	regex_t *re5_ptr = mp_allocx(sizeof(regex_t), rfree), *re6_ptr = mp_allocx(sizeof(regex_t), rfree);

	regcomp(re1_ptr, "^\\(.*\\) .*", REG_NEWLINE);
	regcomp(re2_ptr, "^\\(.*\\) .*", REG_NEWLINE);
	regcomp(re3_ptr, "^\\(.*\\) .*", REG_NEWLINE);
	regcomp(re4_ptr, "^\\(.*\\) .*", REG_NEWLINE);
	regcomp(re5_ptr, "^\\(.*\\) .*", REG_NEWLINE);
	regcomp(re6_ptr, "^\\(.*\\) .*", REG_NEWLINE);

	/* the work bit */

	mp_return();
}
```

 The "well thats annoying" way
```
void fn() {
	regex_t *re1_ptr = alloc(sizeof(regex_t)), *re2_ptr = alloc(sizeof(regex_t));
	regex_t *re3_ptr = alloc(sizeof(regex_t)), *re4_ptr = alloc(sizeof(regex_t));
	regex_t *re5_ptr = alloc(sizeof(regex_t)), *re6_ptr = alloc(sizeof(regex_t));

	regcomp(re1_ptr, "^\\(.*\\) .*", REG_NEWLINE);
	regcomp(re2_ptr, "^\\(.*\\) .*", REG_NEWLINE);
	regcomp(re3_ptr, "^\\(.*\\) .*", REG_NEWLINE);
	regcomp(re4_ptr, "^\\(.*\\) .*", REG_NEWLINE);
	regcomp(re5_ptr, "^\\(.*\\) .*", REG_NEWLINE);
	regcomp(re6_ptr, "^\\(.*\\) .*", REG_NEWLINE);

	/* the work bit */

	regfree(re1_ptr);
	free(re1_ptr);
	regfree(re2_ptr);
	free(re2_ptr);
	regfree(re3_ptr);
	free(re3_ptr);
	regfree(re4_ptr);
	free(re4_ptr);
	regfree(re5_ptr);
	free(re5_ptr);
	regfree(re6_ptr);
	free(re6_ptr);
	return();
}
```

Another use case is adaptation.

Building a managed strdup and strndup

```
/*
	Create this somewhere and allocations are now tracked and auto deleted.
*/
#include "mptr.h"
/**
 * alias: mp_strndup
 * strdup is an alias for strndup(ptr, strlen(ptr))
 */
char *managed_pointer_strndup(struct MANAGEDPTR_HEAD *head, void *vptr, size_t size) {
	/* using calloc sets all bytes to zero */
	void *ptr = managed_pointer_calloc(head, 1, size + 1, NULL);
	strncpy(ptr, vptr, size);
	return (char *)ptr;
}
/* have to use macro's and not functions */
#define mp_strndup(p, l) managed_pointer_strndup(&managedptr_head, p, l)
#define mp_strdup(p, l) managed_pointer_strndup(&managedptr_head, p, strlen(p))

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
