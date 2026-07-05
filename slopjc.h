#ifndef SLOPJC_H_
#define SLOPJC_H_

#include <stdatomic.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SLOPJC_CAT2_(A,B) A##B
#define SLOPJC_CAT2(A,B) SLOPJC_CAT2_(A,B)
 
#define SLOPJC_CAT3_(A,B,C) A##B##C
#define SLOPJC_CAT3(A,B,C) SLOPJC_CAT3_(A,B,C)

#define SLOPJC_AUTORELEASEPOOL \
    for (AutoreleasePool *pool = slopjcNewAutoreleasePool() \
       ; pool != NULL \
       ; slopjcRelease(pool), pool = NULL)

typedef struct String String;

#define SLOPJC_Object                                    \
    FIELD   ( _Atomic size_t ,refCount                 ) \
    VIRTUAL ( void           ,Object,Release ,(void *) ) \
    VIRTUAL ( String        *,Object,ToString,(void *) )
#define SLOPJC Object
#include __FILE__

#define SLOPJC_AutoreleasePool SLOPJC_Object                        \
    FIELD    ( Object         **,ptr                              ) \
    FIELD    ( size_t           ,count                            ) \
    FIELD    ( size_t           ,cap                              ) \
    FIELD    ( AutoreleasePool *,prev                             ) \
    OVERRIDE ( void             ,AutoreleasePool,Release,(void *) )
#define SLOPJC AutoreleasePool
#include "slopjc.h"

#define SLOPJC_String SLOPJC_Object                \
    FIELD    ( char   *,ptr                      ) \
    FIELD    ( size_t  ,count                    ) \
    OVERRIDE ( void    ,String,Release ,(void *) ) \
    OVERRIDE ( String *,String,ToString,(void *) )
#define SLOPJC String
#include __FILE__

#define SLOPJC_MutableString SLOPJC_String \
    FIELD (size_t ,cap)
#define SLOPJC MutableString
#include __FILE__

void slopjcAutorelease(void *);
AutoreleasePool *slopjcAutoreleasePool(void);
String *slopjcAutoreleaseStringWithPtr(const char *ptr);
String *slopjcFormat(const char *fmt, ...);
void slopjcLog(const char *fmt, ...);
AutoreleasePool *slopjcNewAutoreleasePool(void);
void slopjcRelease(void *);
void slopjcRetain(void *);
void slopjcSetAutoreleasePool(AutoreleasePool *pool);

void AutoreleasePool_Drain(AutoreleasePool *);
void String_InitWithPtr(String *, const char *ptr);
String *String_NewWithPtr(const char *ptr);
void MutableString_Append(MutableString *, String *s);
void MutableString_Appendf(MutableString *, const char *fmt, ...);

#endif

#ifdef SLOPJC
 
typedef struct SLOPJC SLOPJC;
typedef struct SLOPJC_CAT2(SLOPJC,VTable) SLOPJC_CAT2(SLOPJC,VTable);

struct SLOPJC {
    const SLOPJC_CAT2(SLOPJC,VTable) *v;
#define FIELD(Type, Name) \
    Type Name;
#define OVERRIDE(...)
#define VIRTUAL(...)
    SLOPJC_CAT2(SLOPJC_,SLOPJC)
#undef FIELD
#undef OVERRIDE
#undef VIRTUAL
};
 
#define FIELD(...)
#define OVERRIDE(Type, Class,Method,Args) \
    Type SLOPJC_CAT3(Class,_,Method)Args;
#define VIRTUAL(Type, Class,Method,Args) \
    Type SLOPJC_CAT3(Class,_,Method)Args;
    SLOPJC_CAT2(SLOPJC_,SLOPJC)
#undef FIELD
#undef OVERRIDE
#undef VIRTUAL
 
struct SLOPJC_CAT2(SLOPJC,VTable) {
#define FIELD(...)
#define OVERRIDE(...)
#define VIRTUAL(Type, _,Method,Args) \
    Type (*Method)Args;
    SLOPJC_CAT2(SLOPJC_,SLOPJC)
#undef FIELD
#undef OVERRIDE
#undef VIRTUAL
};

static inline SLOPJC *SLOPJC_CAT3(SLOPJC,_,Alloc)(void)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winitializer-overrides"
    static const SLOPJC_CAT2(SLOPJC,VTable) v = {
#define FIELD(...)
#define OVERRIDE(_1, Class,Method,_2) \
        .Method = SLOPJC_CAT3(Class,_,Method),
#define VIRTUAL(_1, Class,Method,_2) \
        .Method = SLOPJC_CAT3(Class,_,Method),
        SLOPJC_CAT2(SLOPJC_,SLOPJC)
#undef FIELD
#undef OVERRIDE
#undef VIRTUAL
    };
#pragma GCC diagnostic pop
    SLOPJC *self = malloc(sizeof(*self));
    self->refCount = 1;
    self->v = &v;
    return self;
}
 
static inline void SLOPJC_CAT3(SLOPJC,_,Init)(SLOPJC *self)
{
    size_t offset = offsetof(SLOPJC, refCount) + sizeof(self->refCount);
    memset((char *)self + offset, 0, sizeof(*self) - offset);
}

static inline SLOPJC *SLOPJC_CAT3(SLOPJC,_,New)(void)
{
    SLOPJC *self = SLOPJC_CAT3(SLOPJC,_,Alloc)();
    SLOPJC_CAT3(SLOPJC,_,Init)(self);
    return self;
}

#undef SLOPJC
#endif

