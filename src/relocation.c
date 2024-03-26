/**
 * @file relocation.c
 */
#include "relocation.h"
#include "str.h"

/**
 * Replace all occurrences of `target` with `replacement` in `original`
 *
 * ~~~{.c}
 * char *str = (char *)calloc(100, sizeof(char));
 * strcpy(str, "This are a test.");
 * if (replace_text(str, "are", "is")) {
 *     fprintf(stderr, "string replacement failed\n");
 *     exit(1);
 * }
 * // str is: "This is a test."
 * free(str);
 * ~~~
 *
 * @param original string to modify
 * @param target string value to replace
 * @param replacement string value
 * @return 0 on success, -1 on error
 */
int replace_text(char *original, const char *target, const char *replacement) {
    char buffer[OMC_BUFSIZ];
    char *pos = original;
    char *match = NULL;
    size_t original_len = strlen(original);
    size_t target_len = strlen(target);
    size_t rep_len = strlen(replacement);
    size_t buffer_len = 0;

    if (original_len > sizeof(buffer)) {
        errno = EINVAL;
        SYSERROR("The original string is larger than buffer: %zu > %zu\n", original_len, sizeof(buffer));
        return -1;
    }

    memset(buffer, 0, sizeof(buffer));
    if ((match = strstr(pos, target))) {
        while (*pos != '\0') {
            // append to buffer the bytes leading up to the match
            strncat(buffer, pos, match - pos);
            // position in the string jump ahead to the beginning of the match
            pos = match;

            // replacement is shorter than the target
            if (rep_len < target_len) {
                // shrink the string
                strcat(buffer, replacement);
                memmove(pos, pos + target_len, strlen(pos) - target_len);
                memset(pos + (strlen(pos) - target_len), 0, target_len);
            } else { // replacement is longer than the target
                // write the replacement value to the buffer
                strcat(buffer, replacement);
                // target consumed. jump to the end of the substring.
                pos += target_len;
            }
            // find more matches
            if (!(match = strstr(pos, target))) {
                // no more matches
                // append whatever remains to the buffer
                strcat(buffer, pos);
                // stop
                break;
            }
        }
    }

    buffer_len = strlen(buffer);
    if (buffer_len < original_len) {
        // truncate whatever remains of the original buffer
        memset(original + buffer_len, 0, original_len - buffer_len);
    }
    // replace original with contents of buffer
    strcpy(original, buffer);
    return 0;
}

/**
 * Replace `target` with `replacement` in `filename`
 *
 * ~~~{.c}
 * if (file_replace_text("/path/to/file.txt, "are", "is")) {
 *     fprintf(stderr, "failed to replace strings in file\n");
 *     exit(1);
 * }
 * ~~~
 *
 * @param filename path to file
 * @param target string value to replace
 * @param replacement string
 * @return 0 on success, -1 on error
 */
int file_replace_text(const char* filename, const char* target, const char* replacement) {
    int result;
    char buffer[OMC_BUFSIZ];
    char tempfilename[] = "tempfileXXXXXX";
    FILE *fp;
    FILE *tfp;

    fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "unable to open for reading: %s\n", filename);
        return -1;
    }

    tfp = fopen(tempfilename, "w");
    if (!tfp) {
        SYSERROR("unable to open temporary fp for writing: %s", tempfilename);
        fclose(fp);
        return -1;
    }

    // Write modified strings to temporary file
    result = 0;
    while (fgets(buffer, sizeof(buffer), fp)) {
        if (strstr(buffer, target)) {
            if (replace_text(buffer, target, replacement)) {
                result = -1;
            }
        }
        fputs(buffer, tfp);
    }

    fclose(fp);
    fclose(tfp);

    // Replace original with modified copy
    remove(filename);
    rename(tempfilename, filename);
    return result;
}