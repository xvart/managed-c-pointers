#include <stdio.h>
#include "mptr.h"


/**
 * Find an empty MPTRENTRY
 */
MPTRENTRY *managed_pointer_findEmpty(struct MANAGEDPTR_HEAD *head) {
	MPTRENTRY *np;
	LIST_FOREACH(np, head, entries)
		if( np->used == 0 ) {
			return np;
		}
	return NULL;
}

/**
 * Allocate a block of LIST entries.
 */
void managed_pointer_listAllocate(struct MANAGEDPTR_HEAD *head, int size) {
	if( head->count == 0 ) {
		for(int i =0; i < size; i++) {
			LIST_INSERT_HEAD(head, (MPTRENTRY*) calloc(1, sizeof(MPTRENTRY)), entries);
		}
	}
}

/**
 * Remove unused entries
 */
void managed_pointer_trim(struct MANAGEDPTR_HEAD *head, int size) {
	MPTRENTRY *entry_ptr;
	MPTRENTRY *last_ptr;
	LIST_FOREACH(entry_ptr, head, entries)
		if( entry_ptr->used == 0 ) {
			last_ptr = *entry_ptr->entries.le_prev;
			LIST_REMOVE(entry_ptr, entries);
			free(entry_ptr);
			entry_ptr = *last_ptr->entries.le_prev;
		}
}

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

/**
 * Show info on limits
 */
void managed_pointer_info(struct MANAGEDPTR_HEAD *head, FILE *fp) {
	fprintf(fp, "Head: min %u, max %u, init %u, count %u\n", head->min, head->max, head->init, head->count);
}

/**
 * Optional.
 * Set the limits, why because people like that sort of thing.
 * When max = 0 min/max allocation limits are ignored.
 */
void managed_pointer_setLimits(struct MANAGEDPTR_HEAD *head, int min, int max, int init) {
	head->min = min;
	head->max = max;
	if( head->count = 0 )
		managed_pointer_listAllocate(head, min);
}

/**
 * Add a poiter to the list
 */
void *managed_pointer_link(struct MANAGEDPTR_HEAD *head, void *vptr, void (*handler)(void*)) {
	MPTRENTRY *eptr = (MPTRENTRY*) calloc(1, sizeof(MPTRENTRY));
	eptr->data = vptr;
	eptr->free_handler = handler;
	LIST_INSERT_HEAD(head, eptr, entries);
	return vptr;
}

/**
 * Remove from list
 */
void *managed_pointer_unlink(struct MANAGEDPTR_HEAD *head, void *vptr) {
	MPTRENTRY *np;
	if( vptr != NULL ) {
		LIST_FOREACH(np, head, entries)
			if( np->data == vptr ) {
				//~ fprintf(stderr, "mp_free: removing\n");
				LIST_REMOVE(np, entries);
				free(np);
				return vptr;
			}
			fprintf(stderr, "mp_free: error attempting to remove unmanaged pointer\n");
	}
	return NULL;
}
