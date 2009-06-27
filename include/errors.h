#ifndef ERROR_H
#define ERROR_H


#define die(...)   __die__( __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )
#define vdie(...)  __vdie__( __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )
#define debug(...) __debug__( __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )

void __die__   (const char *file, const int line, const char *function, const char *fmt, ...);
void __vdie__  (const char *file, const int line, const char *function, const char *fmt, va_list argp);
void __debug__ (const char *file, const int line, const char *function, const char *fmt, ...);
void print_trace();

//void die(const char *fmt, ...);
//void vdie(const char *fmt, va_list argp);


#endif /* ERROR_H */
