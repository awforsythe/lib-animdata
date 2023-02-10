#pragma once

#ifdef __EMSCRIPTEN__
#   include <emscripten.h>
#   define AD_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#   define AD_EXPORT
#endif
