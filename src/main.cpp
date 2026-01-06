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
typedef unsigned int uint;

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
    AttribType type;
    uint components;
};
struct MeshData {
    // vertices
    float* vertexData;
    uint* indices;
    uint vertCount;
    // attributes
    Attrib* attributes;
    uint attribCount;
};
// Pretty-printing for MeshData
std::ostream& operator<<(std::ostream& os, const MeshData& m) {
    uint vertComponents=0;
    for (uint i=0; i < m.attribCount; ++i) {
        vertComponents += m.attributes[i].components;
    }
    for (uint i=0; i < m.vertCount; ++i) {
        os << "{ ";
        for (uint d=0; d < vertComponents; ++d)
            os << m.vertexData[i+d] << ", ";
        os << " }\n";
    }
    return os;
}

static int gMESH_INDEX=0;
static MeshData* gMESH_DATA;

// Cpp functions to call from lua
// Load mesh data into a cpp struct
auto createMesh = [](lua_State* L) -> int {
    // args: Attrib* layout, MeshData mesh
    gMESH_INDEX++;
    // userdata
    MeshData mesh=gMESH_DATA[gMESH_INDEX];
    // return mesh_id
    lua_pushnumber(L, gMESH_INDEX);
    return 1;// number of return values
};
// Print out mesh data from cpp
auto drawMesh = [](lua_State* L) -> int {
    std::cout << gMESH_DATA << std::endl;
    return 0;// number of return values
};
// Update cpp mesh data
auto updateMesh = [](lua_State* L) -> int {
    return 0;// number of return values
};

int main() {
    std::string lua_src;
    readfile(LUA_DIR"/simple.lua", lua_src);
    std::cout << "```lua\n" << lua_src << "```" << std::endl;
    // Create Lua state
    lua_State* L = luaL_newstate();
    // Push globals
    lua_pushcfunction(L, createMesh);
    lua_setglobal(L, "createMesh");
    lua_pushcfunction(L, updateMesh);
    lua_setglobal(L, "updateMesh");
    lua_pushcfunction(L, drawMesh);
    lua_setglobal(L, "drawMesh");
    // Run Lua File
    luaL_dostring(L, lua_src.c_str());
    lua_getglobal(L, "MeshIdx");
    std::cout << "MeshIdx = " << (int)lua_tointeger(L, -1) << std::endl;
    // Cleanup
    lua_close(L);
    return 0;
}
