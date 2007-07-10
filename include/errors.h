#ifndef ERROR_H
#define ERROR_H

#define BD_LOAD_ERROR  1
#define BD_PARSE_ERROR 2
#define COLORS_ERROR   3

void do_error(int code, char *item, char *filename, int line);

#endif /* ERROR_H */
