/*
        LUA script wrapper class by J.Harbour.
        Note: LUA does not throw exceptions.
*/

#include <csetjmp>
#include <iostream>
#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_physfs.h>

#include "Game.h"
#include "Script.h"
#include "Util.h"

using namespace std;

ALLEGRO_DEBUG_CHANNEL("Script")

static jmp_buf custom_lua_panic_jump;

static int
custom_lua_atpanic(lua_State * /*lua*/) {
    ALLEGRO_DEBUG("custom_lua_atpanic\n");
    longjmp(custom_lua_panic_jump, 1);
    return 0;
}

Script::Script(const string &dat_file)
    : errorMessage(""), m_dat_file(dat_file) {
    luaState = luaL_newstate();
    luaL_openlibs(luaState);
    lua_register(luaState, "L_LoadScript", Script::L_LoadScript);
    lua_atpanic(luaState, &custom_lua_atpanic);
}

Script::~Script() { lua_close(luaState); }

bool
Script::load(const string &scriptfile) {
    int ret;

    ALLEGRO_FILE *f = al_fopen(scriptfile.c_str(), "rb");
    int64_t size = al_fsize(f);

    char *data = new char[size + 1];
    al_fread(f, static_cast<void *>(data), size);
    data[size] = 0;
    al_fclose(f);

    ret = luaL_dostring(luaState, data);
    delete[] data;

    if (ret > 0) {
        // get lua error message from the stack
        string luaError = lua_tostring(luaState, -1);
        lua_pop(luaState, 1);
        errorMessage = luaError.c_str();
        ALLEGRO_DEBUG(
            "Script load error: return= %d, message= %s.\n",
            ret,
            errorMessage.c_str());
        return false;
    }

    errorMessage = "";
    return true;
}

string
Script::getGlobalString(const string &name) {
    string value = "";

    if (setjmp(custom_lua_panic_jump) == 0) {
        /* interact with Lua VM */
        lua_getglobal(luaState, name.c_str());
    } else {
        /* recovered from panic. log and return */
    }

    value = lua_tostring(luaState, -1);
    lua_pop(luaState, 1);
    return value;
}

void
Script::setGlobalString(const string &name, const string &value) {
    lua_pushstring(luaState, value.c_str());
    lua_setglobal(luaState, name.c_str());
}

double
Script::getGlobalNumber(const string &name) {
    double value = 0.0;
    lua_getglobal(luaState, name.c_str()); // TAG - debug spot
    value = lua_tonumber(luaState, -1);
    lua_pop(luaState, 1);
    return value;
}

void
Script::setGlobalNumber(const string &name, double value) {
    lua_pushnumber(luaState, (int)value);
    lua_setglobal(luaState, name.c_str());
}

bool
Script::getGlobalBoolean(const string &name) {
    bool value = false;
    lua_getglobal(luaState, name.c_str());
    value = lua_toboolean(luaState, -1) != 0;
    lua_pop(luaState, 1);
    return value;
}

void
Script::setGlobalBoolean(const string &name, bool value) {
    lua_pushboolean(luaState, static_cast<int>(value));
    lua_setglobal(luaState, name.c_str());
}

bool
Script::runFunction(const string &name) {
    // call script function, 0 args, 0 retvals
    string luaError;
    lua_getglobal(luaState, name.c_str());
    int result = lua_pcall(luaState, 0, 0, 0);
    if (result != 0) {
        luaError = lua_tostring(luaState, -1);
        ALLEGRO_DEBUG(
            "Script run error:  Lua error message= %s\n", errorMessage.c_str());
        lua_pop(luaState, 1);
        errorMessage = luaError;
        return false;
    }
    errorMessage = "";
    return true;
}

int
Script::L_LoadScript(lua_State *luaVM) {
    int ret;
    string lua_script = lua_tostring(luaVM, -1);
    ALLEGRO_FILE *f = al_fopen(lua_script.c_str(), "rb");
    int64_t size = al_fsize(f);

    char *data = new char[size + 1];
    al_fread(f, static_cast<void *>(data), size);
    data[size] = 0;
    al_fclose(f);

    ret = luaL_dostring(luaVM, data);
    delete[] data;

    if (ret > 0) {
        // get lua error message from the stack
        string luaError = lua_tostring(luaVM, -1);
        lua_pop(luaVM, 1);
        string errorMessage = luaError.c_str();
        ALLEGRO_DEBUG(
            "Script load error: return= %d, message= %s.\n",
            ret,
            errorMessage.c_str());
        return false;
    }
    lua_pop(luaVM, 1);

    return true;
}
