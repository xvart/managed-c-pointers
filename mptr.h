#ifndef _MPTR_H_
#define _MPTR_H_

#include <stdlib.h>
#include <stddef.h>
#include <sys/queue.h>

#define	LIST_HEADX(name, type)		\
struct name {						\
	/* Limits */					\
	int min, max, init;				\
	/* Number of entries */			\
	int count; 						\
	/* first element */				\
	struct type *lh_first;			\
}

#define	LIST_HEAD_INITIALIZERX(head) { 0,0,0,0,NULL }

typedef struct MPTRENTRY {
	void *data;
	/* Used when a complex free is needed */
	void (*free_handler)(void*);
	/* needed if I add a manager */
	int used;
	/* Doubly-linked List. */
	LIST_ENTRY(MPTRENTRY) entries;
} MPTRENTRY;

//~ LIST_HEAD(MANAGEDPTR_HEAD, MPTRENTRY);
LIST_HEADX(MANAGEDPTR_HEAD, MPTRENTRY);

/**
 * The ugly functions
 */
void *managed_pointer_link(struct MANAGEDPTR_HEAD *head, void *vptr, void (*handler)(void*));
void *managed_pointer_unlink(struct MANAGEDPTR_HEAD *head, void *vptr);
void managed_pointer_clear(struct MANAGEDPTR_HEAD *);
void managed_pointer_free(struct MANAGEDPTR_HEAD *, void *);
void managed_pointer_info(struct MANAGEDPTR_HEAD *, FILE *);
void *managed_pointer_realloc(struct MANAGEDPTR_HEAD *, void *, size_t );
void managed_pointer_setLimits(struct MANAGEDPTR_HEAD *, int , int , int);
void *managed_pointer_alloc(struct MANAGEDPTR_HEAD *, size_t , void (*)(void*));
void *managed_pointer_reallocarray(struct MANAGEDPTR_HEAD *, void *, size_t , size_t );
void *managed_pointer_calloc(struct MANAGEDPTR_HEAD *, size_t , size_t , void (*)(void*));

/**
 * Macro's with the magic, they perform the same as their namesakes.
 * x variants are extended, this just adds the free_handler pointer.
 */
#define mp_link(s) managed_pointer_alloc(&managedptr_head, s, NULL)
#define mp_unlink(s) managed_pointer_alloc(&managedptr_head, s)
#define mp_linkx(s, h) managed_pointer_alloc(&managedptr_head, s, h)

#define mp_alloc(s) managed_pointer_alloc(&managedptr_head, s, NULL)
#define mp_allocx(s, h) managed_pointer_alloc(&managedptr_head, s, h)
#define mp_calloc(n, s) managed_pointer_calloc(&managedptr_head, n, s, NULL)
#define mp_callocx(n, s, h) managed_pointer_calloc(&managedptr_head, n, s, h)

#define mp_realloc(p, s) managed_pointer_realloc(&managedptr_head, p, s)
#define mp_reallocarray(p, n, s) managed_pointer_reallocarray(&managedptr_head, p, n, s)
#define mp_free(p) managed_pointer_free(&managedptr_head, p)
#define mp_clear() managed_pointer_clear(&managedptr_head)
#define mp_info(fp) managed_pointer_info(&managedptr_head, fp)


#define mp_setLimits(min,max,init) managed_pointer_setLimits(&managedptr_head, min, max, init)

#define mp_init() \
	struct MANAGEDPTR_HEAD managedptr_head = LIST_HEAD_INITIALIZERX(managedptr_head)

#define mp_return(v) do { \
	managed_pointer_clear(&managedptr_head); \
	return v; \
} while (0)

#endif /* _MPTR_H_ */
