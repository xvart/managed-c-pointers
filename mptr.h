#ifndef _MPTR_H_
#define _MPTR_H_

#include <stdlib.h>
#include <stddef.h>
#include <sys/queue.h>


typedef struct MPTRENTRY {
	void *data;
	void (*free_handler)(void*); /* if a complex free is needed */
	LIST_ENTRY(MPTRENTRY) entries; /* Singly-linked List. */
} MPTRENTRY;

LIST_HEAD(MANAGEDPTR_HEAD, MPTRENTRY);


/**
 * The ugly functions
 */
void *managed_pointer_alloc(struct MANAGEDPTR_HEAD *, size_t , void (*)(void*));
void *managed_pointer_calloc(struct MANAGEDPTR_HEAD *, size_t , size_t , void (*)(void*));
void *managed_pointer_realloc(struct MANAGEDPTR_HEAD *, void *, size_t );
void *managed_pointer_reallocarray(struct MANAGEDPTR_HEAD *, void *, size_t , size_t );
void managed_pointer_free(struct MANAGEDPTR_HEAD *, void *);
void managed_pointer_clear(struct MANAGEDPTR_HEAD *);

/**
 * Macro's with the magic
 */
#define mp_init() \
	struct MANAGEDPTR_HEAD managedptr_head = LIST_HEAD_INITIALIZER(managedptr_head)

#define mp_alloc(s, h) managed_pointer_alloc(&managedptr_head, s, h)
#define mp_calloc(n, s, h) managed_pointer_calloc(&managedptr_head, n, s, h)
#define mp_realloc(p, s) managed_pointer_realloc(&managedptr_head, p, s)
#define mp_reallocarray(p, n, s) managed_pointer_reallocarray(&managedptr_head, p, n, s)
#define mp_free(p) managed_pointer_free(&managedptr_head, p)
#define mp_clear() managed_pointer_clear(&managedptr_head)

#define mp_return(v) do { \
	managed_pointer_clear(&managedptr_head); \
	return v; \
} while (0)

#endif /* _MPTR_H_ */
