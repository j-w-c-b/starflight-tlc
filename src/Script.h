/*
        STARFLIGHT - THE LOST COLONY
        Script.h
        Author: J.Harbour
        Date: 2007

        This wrapper helps with repetitive use of LUA library.
        Add new functionality as needed.
*/

#pragma once

#include <string>
#include <vector>

#include "lua.hpp"

class Script {
  private:
    lua_State *luaState;

  public:
    Script();
    virtual ~Script();

    bool load(const std::string &scriptfile);

    std::string getGlobalString(const std::string &name);
    void setGlobalString(const std::string &name, const std::string &value);

    double getGlobalNumber(const std::string &name);
    void setGlobalNumber(const std::string &name, double value);

    bool getGlobalBoolean(const std::string &name);

    bool runFunction(const std::string &name);

    static int L_LoadScript(lua_State *luaVM);

    std::string errorMessage;
    lua_State *
    getState() {
        return luaState;
    }
};
