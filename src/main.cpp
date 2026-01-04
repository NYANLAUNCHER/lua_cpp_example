// std
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
// lua
#include <lua.hpp>
// Local
#include "_generated/sys_paths.h"

void readfile(const std::string path, std::string& src) {
    std::ifstream file;
    std::stringstream ss;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(path);
        file.seekg(0, std::ios::end);
        src.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&src[0], src.size());
        //ss << file.rdbuf();
        //file.close();
        //src = ss.str();
    } catch (const std::ifstream::failure& e) {
        std::cout << "Exception thrown: " << e.what() << std::endl;
    }
}

int main() {
    std::string LUA_SRC;
    readfile(LUA_DIR"/test.lua", LUA_SRC);
    lua_State* L = luaL_newstate();
    lua_close(L);
    return 0;
}
