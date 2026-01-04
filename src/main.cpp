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

int gMESH_INDEX=0;

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

enum AttribType {
    GENERIC,
    POSITION,
    NORMAL,
    TEXTURE_COORD,
    COLOR,
};

struct Attrib {
    enum AttribType type;
    unsigned int size;
};

struct MeshData {
    float* vertexData;
    unsigned int* indices;
    Attrib* attributes;
};

// Cpp functions to call from lua
// Load mesh data into a cpp struct
// returns mesh_id
auto createMesh = [&gMESH_INDEX](lua_State* L) -> int {
    printf("Hello from createMesh()!\n");
    gMESH_INDEX++;
    lua_pushnumber(L, gMESH_INDEX);
    return 1;// number of return values
};
// Print out mesh data
auto drawMesh = [](lua_State* L) -> int {
    printf("Hello from drawMesh()!");
    return 0;// number of return values
};
// Update mesh data
auto updateMesh = [](lua_State* L) -> int {
    printf("Hello from updateMesh()!");
    return 0;// number of return values
};

int main() {
    std::string lua_src = R"(
    meshIdx = createMesh()
    )";
    //readfile(LUA_DIR"/test.lua", lua_src);
    // Create Lua state
    lua_State* L = luaL_newstate();
    lua_pushcfunction(L, createMesh);
    lua_setglobal(L, "createMesh");
    lua_pushcfunction(L, updateMesh);
    lua_getglobal(L, "meshIdx");
    printf("meshIdx = %d\n", (int)lua_tointeger(L, -1));
    //lua_setglobal(L, "updateMesh");
    //lua_pushcfunction(L, drawMesh);
    //lua_setglobal(L, "drawMesh");
    luaL_dostring(L, lua_src.c_str());
    // Cleanup
    lua_close(L);
    return 0;
}
