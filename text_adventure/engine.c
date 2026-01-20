#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Include the Lua C API headers
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define MAX_INPUT 256

/**
 * @brief Helper function to report Lua errors.
 */
void report_lua_error(lua_State *L, int status) {
    if (status != LUA_OK) {
        const char *error = lua_tostring(L, -1);
        fprintf(stderr, "Lua Error: %s\n", error);
        lua_pop(L, 1); // Remove error message from stack
        exit(1);
    }
}

/**
 * @brief Calls a global Lua function.
 * @param L The Lua state.
 * @param func_name The name of the Lua function.
 * @param ... String arguments to pass to the function.
 * @param n_args Number of arguments.
 * @return The string returned by the Lua function. Caller must free.
 */
char* call_lua_string_func(lua_State *L, const char *func_name, int n_args, ...) {
    va_list args;
    
    // 1. Get the function from the global scope
    lua_getglobal(L, func_name);
    
    // 2. Push arguments
    va_start(args, n_args);
    for(int i = 0; i < n_args; i++) {
        const char* arg = va_arg(args, const char*);
        lua_pushstring(L, arg);
    }
    va_end(args);

    // 3. Call the function (n_args, 1 result, 0 error handler)
    int status = lua_pcall(L, n_args, 1, 0);
    report_lua_error(L, status);
    
    // 4. Get the result
    if (!lua_isstring(L, -1)) {
        fprintf(stderr, "Error: Lua function '%s' did not return a string.\n", func_name);
        lua_pop(L, 1);
        return NULL;
    }
    
    const char *result_str = lua_tostring(L, -1);
    char *result = strdup(result_str); // Copy it
    lua_pop(L, 1); // Pop result from stack
    
    return result;
}

int main() {
    char current_room[100] = "start";
    char input[MAX_INPUT];
    
    // 1. Initialize Lua state
    lua_State *L = luaL_newstate();
    if (L == NULL) {
        fprintf(stderr, "Failed to create Lua state.\n");
        return 1;
    }
    luaL_openlibs(L); // Load standard libraries

    // 2. Load the game script
    int status = luaL_dofile(L, "gamedata.lua");
    report_lua_error(L, status);

    printf("--- C/Lua Adventure Engine ---\n");
    printf("Type 'quit' to exit.\n\n");

    // 3. Main Game Loop
    while (1) {
        // 4. Get room description from Lua
        char* description = call_lua_string_func(L, "get_description", 1, current_room);
        printf("%s\n", description);
        free(description);

        // 5. Get user input
        printf("> ");
        if (!fgets(input, MAX_INPUT, stdin)) {
            break; // EOF
        }
        input[strcspn(input, "\n")] = 0; // Remove newline

        if (strcmp(input, "quit") == 0) {
            break;
        }

        // 6. Send command to Lua and get new room
        char* new_room = call_lua_string_func(L, "process_command", 2, current_room, input);
        if (new_room) {
            strcpy(current_room, new_room);
            free(new_room);
        }
    }

    // 7. Clean up
    lua_close(L);
    printf("Goodbye!\n");
    return 0;
}
