#ifdef __EMSCRIPTEN__

#include <emscripten/bind.h>

#include "ad_buffer.h"

EMSCRIPTEN_BINDINGS(animdata) {
	emscripten::class_<ad_buffer>("ad_buffer")
        .constructor()
        .property("size", &ad_buffer::size)
        .property("capacity", &ad_buffer::capacity)
        .function("init", &ad_buffer::init)
    ;
}

#endif
