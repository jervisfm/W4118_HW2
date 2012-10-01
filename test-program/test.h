#ifndef TEST_H
#define TEST_H

#include <ctype.h> /* for is_digit method */

#define DEFAULT_NR_ARG 100 /* Default nr argument parament */



/* Prints all the processes in the prinfo array */
void print_tree(struct prinfo *tree, const int size);

/* Print process details in the given prinfo. Count is the depth
 * to indent the print output.
 */
void print_prinfo(int count, struct prinfo p);
/* Determines if given string is a number or not. Return 1 if true 0 if false*/
int is_number(char *string);
#endif
