#include "slopjc.h"

#define SLOPJC_Pet SLOPJC_Object                 \
    FIELD    ( String *,name                   ) \
    OVERRIDE ( void    ,Pet,Release  ,(void *) ) \
    OVERRIDE ( String *,Pet,ToString ,(void *) ) \
    VIRTUAL  ( String *,Pet,MakeSound,(void)   )
#define SLOPJC Pet
#include "slopjc.h"

#define SLOPJC_Dog SLOPJC_Pet                    \
    OVERRIDE ( String *,Dog,MakeSound,(void)   )
#define SLOPJC Dog
#include "slopjc.h"

#define SLOPJC_Cat SLOPJC_Pet                    \
    FIELD    ( String *,title                  ) \
    OVERRIDE ( void    ,Cat,Release  ,(void *) ) \
    OVERRIDE ( String *,Cat,ToString ,(void *) ) \
    OVERRIDE ( String *,Cat,MakeSound,(void)   )
#define SLOPJC Cat
#include "slopjc.h"

void Pet_Release(void *_self) {
    const Pet *self = _self;
    slopjcRelease(self->name);
}

String *Pet_ToString(void *_self) {
    const Pet *self = _self;
    return slopjcFormat("Pet '%'", self->name);
}

String *Dog_MakeSound(void) {
    return slopjcAutoreleaseStringWithPtr("Bark"); 
}

void Cat_Release(void *_self) {
    const Cat *self = _self;
    slopjcRelease(self->name);
    slopjcRelease(self->title);
}

String *Cat_ToString(void *_self) {
    const Cat *self = _self;
    return slopjcFormat("% '%'", self->title, self->name);
}

String *Cat_MakeSound(void) {
    return slopjcAutoreleaseStringWithPtr("Meow"); 
}

int main(void) {
    SLOPJC_AUTORELEASEPOOL {
        Dog *dog = Dog_New();
        slopjcAutorelease(dog);
        dog->name = String_NewWithPtr("Jake");
        Pet *pdog = (void *)dog;

        Cat *cat = Cat_New();
        slopjcAutorelease(cat);
        cat->name = String_NewWithPtr("Beef Stroganoff");
        cat->title = String_NewWithPtr("His Majesty");
        Pet *pcat = (void *)cat;

        slopjcLog("% says '%'\n"
                  "% demands '%'\n",
                  pdog, pdog->v->MakeSound(),
                  pcat, pcat->v->MakeSound());
    }
}

