#include <stdio.h>
#include "mptr.h"

/**
 * free all the endtries in a list
 *
 * @param head
 * @param handler
 */
void managed_pointer_clear(struct MANAGEDPTR_HEAD *head) {
	MPTRENTRY *np;
	while (!LIST_EMPTY(head)) { /* List Deletion. */
		np = LIST_FIRST(head);
		LIST_REMOVE(np, entries);
		if (np->free_handler == NULL) {
			free(np->data);
		} else {
			np->free_handler(np->data);
		}
		free(np);
	}
}

/**
 *
 * @param head
 * @param ep
 */
static void managed_pointer_append(struct MANAGEDPTR_HEAD *head, struct MPTRENTRY *ep) {
	struct MPTRENTRY *np;
	if (LIST_EMPTY(head)) {
		LIST_INSERT_HEAD(head, ep, entries);
	} else {
		LIST_FOREACH(np, head, entries)
			if (np->entries.le_next == NULL) {
				LIST_INSERT_AFTER(np, ep, entries);
				return;
			}
	}
}

/**
 * Use calloc to zero data
 */
void *managed_pointer_alloc(struct MANAGEDPTR_HEAD *head, size_t size, void (*handler)(void*)) {
	MPTRENTRY *eptr = (MPTRENTRY*) calloc(1, sizeof(MPTRENTRY));
	eptr->data = calloc(1, size);
	eptr->free_handler = handler;
	LIST_INSERT_HEAD(head, eptr, entries);
	return eptr->data;
}

/**
 * wrapper
 */
void *managed_pointer_calloc(struct MANAGEDPTR_HEAD *head, size_t num, size_t size, void (*handler)(void*)) {
	MPTRENTRY *eptr = (MPTRENTRY*) calloc(1, sizeof(MPTRENTRY));
	eptr->data = calloc(num, size);
	eptr->free_handler = handler;
	LIST_INSERT_HEAD(head, eptr, entries);
	return eptr->data;
}

/**
 * wrapper
 */
void *managed_pointer_reallocarray(struct MANAGEDPTR_HEAD *head, void *vptr, size_t num, size_t size) {
	MPTRENTRY *np;
	LIST_FOREACH(np, head, entries)
		if( np->data == vptr ) {
			if( reallocarray(np->data, num, size) != NULL )
				return np->data;
			else
				return NULL;
		}
	return vptr;
}

/**
 * wrapper
 */
void *managed_pointer_realloc(struct MANAGEDPTR_HEAD *head, void *vptr, size_t size) {
	MPTRENTRY *np;
	LIST_FOREACH(np, head, entries)
		if( np->data == vptr ) {
			if( realloc(np->data, size) != NULL )
				return np->data;
			else
				return NULL;
		}
	return vptr;
}

/**
 * Remove and free data with no side effects.
 * If data is not managed report and do nothing.
 */
void managed_pointer_free(struct MANAGEDPTR_HEAD *head, void *vptr) {
	MPTRENTRY *np;
	if( vptr != NULL ) {
		LIST_FOREACH(np, head, entries)
			if( np->data == vptr ) {
				//~ fprintf(stderr, "mp_free: removing\n");
				LIST_REMOVE(np, entries);
				if (np->free_handler == NULL) {
					free(np->data);
				} else {
					np->free_handler(np->data);
				}
				free(np);
				return;
			}
			fprintf(stderr, "mp_free: error attempting to remove unmanaged pointer\n");
	}
	return;
}

