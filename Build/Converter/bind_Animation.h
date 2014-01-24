#ifndef __bind_ANIMATION__
#define __bind_ANIMATION__

#include "package_SpriteStudio.h"


class Bind_Animation {
public:
    Bind_Animation(){ puts("construct Animation"); }
    char const* init(){
        return "Animation init";
    }
};



#endif
