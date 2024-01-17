#pragma once
#define SIMPLE_FUNC_IMPL(ret, fname, var) \
           ret Get##fname() \
           { \
                     return var; \
           } \
           void Set##fname(ret tmp) \
           { \
                     var = tmp; \
           }
