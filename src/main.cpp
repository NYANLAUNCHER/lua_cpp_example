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
    } catch (const std::ifstream::failure& e) {
        std::cout << "Exception thrown: " << e.what() << std::endl;
    }
}

struct meshDataLayout {
    std::string type;
    unsigned int size;
};

struct meshData {
    float* vertexData;// offsets are looked-up in layout
    unsigned int indices;
    meshDataLayout* layout;
};

int main() {
    std::string LUA_SRC;
    readfile(LUA_DIR"/test.lua", LUA_SRC);
    // Create Lua state
    lua_State* L = luaL_newstate();
    luaL_dostring(L, LUA_SRC.c_str());
    lua_setglobal(L, "createMesh");// Loads lua mesh data into cpp struct
    lua_setglobal(L, "updateMesh");// Updates cpp mesh struct
    lua_setglobal(L, "drawMesh");// Prints cpp mesh struct
    // Cleanup
    lua_close(L);
    return 0;
}
