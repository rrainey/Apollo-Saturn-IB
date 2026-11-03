/* MSVC "secure" string function shims for POSIX.
 *
 * Purpose: SaturnIBStack.cpp was written against MSVC's Annex-K string
 * functions (strncpy_s, strcpy_s, strcat_s).  On POSIX these do not
 * exist; the ordinary str{n,cpy,cat} functions have compatible enough
 * semantics for the bounded uses in this codebase (all callers pass a
 * static buffer size that is strictly larger than the source).
 *
 * Not a full Annex-K conformance layer -- if new MSVC-only string
 * primitives appear, add matching macros here.
 */
#ifndef LIB_SATURNIB_SHIM_MSVC_STR_H
#define LIB_SATURNIB_SHIM_MSVC_STR_H

#if !defined(_WIN32) && !defined(_MSC_VER)

#include <string.h>

/* dest, dest_size, src, count -- POSIX strncpy plus explicit termination. */
static inline int _shim_strncpy_s(char *dst, size_t dst_size,
                                  const char *src, size_t count) {
    if (!dst || dst_size == 0) return 22 /* EINVAL */;
    size_t n = (count < dst_size - 1) ? count : dst_size - 1;
    strncpy(dst, src ? src : "", n);
    dst[n] = '\0';
    return 0;
}

static inline int _shim_strcpy_s(char *dst, size_t dst_size, const char *src) {
    if (!dst || dst_size == 0) return 22;
    strncpy(dst, src ? src : "", dst_size - 1);
    dst[dst_size - 1] = '\0';
    return 0;
}

static inline int _shim_strcat_s(char *dst, size_t dst_size, const char *src) {
    if (!dst || dst_size == 0) return 22;
    size_t used = strnlen(dst, dst_size);
    if (used >= dst_size) return 34 /* ERANGE */;
    strncpy(dst + used, src ? src : "", dst_size - used - 1);
    dst[dst_size - 1] = '\0';
    return 0;
}

#define strncpy_s(d, sz, s, n) _shim_strncpy_s((d), (sz), (s), (n))
#define strcpy_s(d, sz, s)     _shim_strcpy_s((d), (sz), (s))
#define strcat_s(d, sz, s)     _shim_strcat_s((d), (sz), (s))

#endif  /* !_WIN32 && !_MSC_VER */

#endif  /* LIB_SATURNIB_SHIM_MSVC_STR_H */
