// std
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <exception>
#include <cassert>
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
    GENERIC=0,
    POSITION=1,
    NORMAL=2,
    TEXTURE_COORD=3,
    COLOR=4,
};
struct Attrib {
    uint type;// see: enum AttribType
    uint size;// number of components
};
struct MeshData {
    // vertices
    std::vector<float> vertexData;
    std::vector<uint> indices;
    uint vert_count=0;
    // attributes
    std::vector<Attrib> attributes;
    uint attrib_count=0;
};
// Pretty-printing for MeshData
std::ostream& operator<<(std::ostream& os, const MeshData& m) {
    os << "Hello from MeshData";
    return os;
}

// Cpp MeshData for lua to manipulate
static std::vector<MeshData> gMESH_DATA;

// args: Attrib* layout, MeshData mesh
auto createMesh = [](lua_State* L) -> int {
    // return userdata
    //MeshData* mesh = (MeshData*)lua_newuserdata(L, sizeof(MeshData));
    return 1;
};
// args: userdata MeshData
auto drawMesh = [](lua_State* L) -> int {
    assert(lua_isuserdata(L, 1));
    std::cout << *(MeshData*)(lua_touserdata(L, 1)) << std::endl;
    return 0;
};
auto updateMesh = [](lua_State* L) -> int {
    return 0;
};

int main() {
    std::string lua_src;
    readfile(LUA_DIR"/simple.lua", lua_src);
    std::cout << "```lua\n" << lua_src << "\n```" << std::endl;
    // Create Lua state
    lua_State* L = luaL_newstate();
    // Create new MeshData for Cpp
    lua_pushcfunction(L, createMesh);
    lua_setglobal(L, "createMesh");
    // Update existing MeshData in Cpp
    lua_pushcfunction(L, updateMesh);
    lua_setglobal(L, "updateMesh");
    // Print MeshData
    lua_pushcfunction(L, drawMesh);
    lua_setglobal(L, "drawMesh");
    // Run Lua Script
    luaL_dostring(L, lua_src.c_str());
    // Cleanup
    lua_close(L);
    return 0;
}
