#include "scripting.h"

Script create_script(Game* game, const char* code) {
    Janet result;
    janet_dostring(game->env, code, "main", &result);

    if (janet_checktype(result, JANET_FUNCTION)) {
        JanetFunction* func = janet_unwrap_function(result);
        return (Script){.func = func};
    }

    return (Script){.func = NULL};
}