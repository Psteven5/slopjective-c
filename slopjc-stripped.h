#ifndef SLOPJC_H_
#define SLOPJC_H_

#define SLOPJC_CAT2_(A,B) A##B
#define SLOPJC_CAT2(A,B) SLOPJC_CAT2_(A,B)
 
#define SLOPJC_CAT3_(A,B,C) A##B##C
#define SLOPJC_CAT3(A,B,C) SLOPJC_CAT3_(A,B,C)

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
 
static inline void SLOPJC_CAT3(SLOPJC,_,Init)(SLOPJC *self)
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
    self->v = &v;
}

static inline SLOPJC SLOPJC_CAT3(SLOPJC,_,New)(void)
{
    SLOPJC self;
    SLOPJC_CAT3(SLOPJC,_,Init)(&self);
    return self;
}

#undef SLOPJC
#endif

