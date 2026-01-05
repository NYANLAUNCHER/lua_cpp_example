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
    unsigned int vertCount;
    float* vertexData;
    unsigned int* indices;
    unsigned int attribCount;
    struct attributes {
        enum AttribType type;
        unsigned int size;
    };
};

std::ostream& operator<<(std::ostream& os, const MeshData& m) {
    os << "(" << p.x << ", " << p.y << ")";
    for (int i=0; i < m.vertCount; ++i) {
        os << 
    }
    return os; // allow chaining: cout << a << b;
}

static int gMESH_INDEX=0;
static MeshData* gMESH_DATA;

// Cpp functions to call from lua
// Load mesh data into a cpp struct
// returns mesh_id
auto createMesh = [](lua_State* L) -> int {
    // args: Attrib* layout, MeshData mesh
    gMESH_INDEX++;
    MeshData mesh=gMESH_DATA[gMESH_INDEX];
    // return mesh_id
    lua_pushnumber(L, gMESH_INDEX);
    return 1;// number of return values
};
// Print out mesh data
auto drawMesh = [](lua_State* L) -> int {
    return 0;// number of return values
};
// Update mesh data
auto updateMesh = [](lua_State* L) -> int {
    return 0;// number of return values
};

int main() {
    std::string lua_src = R"(
    local layout = {
      {
        type="position",
        size=2
      },
      {
        type="color",
        size=3
      },
    }
    local mesh = {
      --position    color
      {-0.5, 0.5,  1.0, 0.0, 0.0}, -- Top left (1)
      { 0.5, 0.5,  0.0, 1.0, 0.0}, -- Top right (2)
      { 0.5,-0.5,  0.0, 0.0, 1.0}, -- Bottom Right (3)
      {-0.5,-0.5,  0.0, 0.0, 0.0}, -- Bottom Left (4)
      -- EBO Data:
      -- Note index starts with 1
      indices={-- using CW winding order
        1, 2, 3, -- upper-right triangle
        1, 3, 4  -- lower-left triangle
      }
    }
    createMesh(layout, mesh)
    meshIdx = createMesh(layout, mesh)
    )";
    //readfile(LUA_DIR"/test.lua", lua_src);
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
    lua_getglobal(L, "meshIdx");
    printf("meshIdx = %d\n", (int)lua_tointeger(L, -1));
    // Cleanup
    lua_close(L);
    return 0;
}
