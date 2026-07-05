#include <inttypes.h>
#include <stdarg.h>
#include <threads.h>
#include "slopjc.h"

thread_local static AutoreleasePool *_slopjcAutoreleasePool;

void slopjcAutorelease(void *self)
{
    AutoreleasePool *pool = _slopjcAutoreleasePool;

    if (pool->count + 1 > pool->cap) {
        size_t cap = pool->cap < 4 ? 4 : pool->cap * 13 >> 3;
        pool->ptr = realloc(pool->ptr, cap * sizeof(*pool->ptr));
        pool->cap = cap;
    }

    pool->ptr[pool->count++] = self;
}

String *slopjcAutoreleaseStringWithPtr(const char *ptr)
{
    String *result = String_NewWithPtr(ptr);
    slopjcAutorelease(result);
    return result;
}

String *slopjcFormat(const char *fmt, ...)
{
    MutableString *result = MutableString_New();
    slopjcAutorelease(result);
    va_list arg;
    va_start(arg, fmt);
    
    while (*fmt != '\0') {
        if (*fmt != '%') {
            MutableString_Appendf(result, "%c", *fmt);
            ++fmt;
            continue;
        }

        if (fmt[1] == '%') {
            MutableString_Appendf(result, "%%");
            fmt += 2;
            continue;
        }

        Object *obj = va_arg(arg, Object *);
        if (obj != NULL) {
            MutableString_Append(result, obj->v->ToString(obj));
        } else {
            MutableString_Appendf(result, "(nil)");
        }

        ++fmt;
    }

    va_end(arg);
    return (String *)result;
}

void slopjcLog(const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    while (*fmt != '\0') {
        if (*fmt != '%') {
            printf("%c", *fmt);
            ++fmt;
            continue;
        }

        if (fmt[1] == '%') {
            printf("%%");
            fmt += 2;
            continue;
        }

        Object *obj = va_arg(arg, Object *);
        if (obj != NULL) {
            String *s = obj->v->ToString(obj);
            fwrite(s->ptr, 1, s->count, stdout);
        } else {
            printf("(nil)");
        }

        ++fmt;
    }

    va_end(arg);
}

AutoreleasePool *slopjcNewAutoreleasePool(void)
{
    AutoreleasePool *pool = AutoreleasePool_New();
    pool->prev = _slopjcAutoreleasePool;
    _slopjcAutoreleasePool = pool;
    return pool;
}

void slopjcRelease(void *_obj)
{
    Object *obj = _obj;

    if (atomic_fetch_sub(&obj->refCount, 1) == 1) {
        obj->v->Release(obj);
        free(obj);
    }
}

void slopjcRetain(void *_obj)
{
    Object *obj = _obj;
    atomic_fetch_add(&obj->refCount, 1);
}

void Object_Release(void *self)
{
    printf("Released Object@%"PRIxPTR"\n", (uintptr_t)self);
}

String *Object_ToString(void *self)
{
    String *result = String_Alloc();
    const char *fmt = "Object@%"PRIxPTR;
    result->count = snprintf(NULL, 0, fmt, (uintptr_t)self);
    result->ptr = malloc(result->count + 1);
    snprintf(result->ptr, result->count + 1, fmt, (uintptr_t)self);
    slopjcAutorelease(result);
    return result;
}

void AutoreleasePool_Drain(AutoreleasePool *self)
{
    for (size_t i = 0; i != self->count; ++i) {
        slopjcRelease(self->ptr[i]);
    }

    self->count = 0;
}

void AutoreleasePool_Release(void *_self)
{
    AutoreleasePool *self = _self;
    _slopjcAutoreleasePool = self->prev;
    AutoreleasePool_Drain(self);
}

void String_InitWithPtr(String *self, const char *ptr)
{
    self->count = strlen(ptr);
    self->ptr = malloc(self->count);
    memcpy(self->ptr, ptr, self->count);
}

String *String_NewWithPtr(const char *ptr)
{
    String *result = String_Alloc();
    String_InitWithPtr(result, ptr);
    return result;
}

void String_Release(void *_self)
{
    String *self = _self;
    printf("Released \"");
    fwrite(self->ptr, 1, self->count, stdout);
    printf("\"\n");
    free(self->ptr);
}

String *String_ToString(void *self)
{
    slopjcRetain(self);
    slopjcAutorelease(self);
    return self;
}

void MutableString_Append(MutableString *self, String *s)
{
    size_t newCount = self->count + s->count;
    if (newCount > self->cap) {
        size_t cap = self->cap > 4 ? self->cap : 4;
        while (cap < newCount) {
            cap = cap * 13 >> 3;
        }

        self->ptr = realloc(self->ptr, cap);
        self->cap = cap;
    }

    memcpy(self->ptr + self->count, s->ptr, s->count);
    self->count = newCount;
}

void MutableString_Appendf(MutableString *self, const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    int count = vsnprintf(NULL, 0, fmt, arg);
    va_end(arg);

    size_t newCount = self->count + count;
    if (newCount + 1 > self->cap) {
        size_t cap = self->cap > 4 ? self->cap : 4;
        while (cap < newCount + 1) {
            cap = cap * 13 >> 3;
        }

        self->ptr = realloc(self->ptr, cap);
        self->cap = cap;
    }

    va_start(arg, fmt);
    vsnprintf(self->ptr + self->count, count + 1, fmt, arg);
    va_end(arg);
    self->count = newCount;
}

