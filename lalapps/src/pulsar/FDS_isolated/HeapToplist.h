/* This module keeps a "toplist", i.e. a list of the top n elements (accoding to an externally
   supplied comparison function) in a standard heap structure.

   Author (of this implementation): Bernd Machenschalk
 */

#ifndef HEAPTOPLIST_H
#define HEAPTOPLIST_H

static volatile const char *HEAPTOPLISTHID  = "$Id$";

#ifndef UINT8
#define UINT8 unsigned long long
#endif

/* toplist structure */
typedef struct {
  UINT8  length;   /* the length (maximal number of entries) of the toplist */
  UINT8  elems;    /* number of elements currently in the toplist */
  size_t size;     /* size of an element */
  char   *data;    /* the actual data array of 'length'*'size' chars */
  char   **heap;   /* array of 'length' pointers into data */
  int    (*smaller)(const void *, const void *); /* comparison function */
} toplist_t;


/* creates a toplist with 'length' elements of size 'size', with
   odering based on comparison function 'smaller'.
   returns -1 on error (out of memory), else 0 */
extern int create_toplist(toplist_t**list, UINT8 length, size_t size,
			  int (*smaller)(const void *, const void *));


/* frees the space occupied by the toplist */
extern void free_toplist(toplist_t**list);


/* Inserts an element in to the toplist either if there is space left
   or the element is larger than the smallest element in the toplist.
   In the latter case, remove the smallest element from the toplist.
   Returns 1 if the element was actually inserted, 0 if not. */
extern int insert_into_toplist(toplist_t*list, void *element);


/* apply a function to all elements of the list in the current order
   (possibly after calling qsort_toplist(), e.g. for writing out */
extern void go_through_toplist(toplist_t*list, void (*handle)(const void *));


/* sort the toplist with an arbitrary sorting function
   (potentially) destroying the heap property.

   note that a (q-)sorted list is a heap, but due to the interface of qsort
   the same comparison function will give the reverse order than the heap.
   in order to restore a heap with qsort_toplist() (e.g. to add more elements) you
   must qsort_toplist() with the inverse function of the "smaller" function of the heap.
*/
extern void qsort_toplist(toplist_t*list, int (*compare)(const void *, const void *));


/* therefore we provide a qsort_toplist_r() function that gives the reverse ordering of
   qsort_toplist(), which restores the heap property with the same comparison function */
extern void qsort_toplist_r(toplist_t*list, int (*compare)(const void *, const void *));


#endif /* HEAPTOPLIST_H - double inclusion protection */
