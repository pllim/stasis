//
// Created by jhunk on 12/17/23.
//

#ifndef OMC_TEMPLATE_H
#define OMC_TEMPLATE_H

#include "omc.h"

/**
 * Map a text value to a pointer in memory
 *
 * @param key in-text variable name
 * @param ptr pointer to string
 */
void tpl_register(char *key, char **ptr);

/**
 * Free the template engine
 */
void tpl_free();

/**
 * Retrieve the value of a key mapped by the template engine
 * @param key string registered by `tpl_register`
 * @return a pointer to value, or NULL if the key is not present
 */
char *tpl_getval(char *key);

/**
 * Replaces occurrences of all registered key value pairs in `str`
 * @param str the text data to render
 * @return a rendered copy of `str`, or NULL.
 * The caller is responsible for free()ing memory allocated by this function
 */
char *tpl_render(char *str);

/**
 * Write tpl_render() output to a file
 * @param str the text to render
 * @param filename the output file name
 * @return 0 on success, <0 on error
 */
int tpl_render_to_file(char *str, const char *filename);

#endif //OMC_TEMPLATE_H
