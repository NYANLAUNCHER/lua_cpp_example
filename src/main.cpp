// std
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <new>
#include <exception>
#include <cassert>
// lua
#include <lua.hpp>
// Local
#include "_generated/sys_paths.h"
typedef unsigned int uint;
inline std::string BLUE(std::string s) { return "\033[34m" + s + "\033[0m"; }
inline std::string GREEN(std::string s) { return "\033[32m" + s + "\033[0m"; }

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

// note: doesn't contain trailing slash
std::string getDirectory(const std::string& path) {
    size_t pos = path.find_last_of("/\\");   // handle both slashes
    if (pos == std::string::npos) return ""; // no directory
    return path.substr(0, pos);          // include trailing slash
}

struct MeshData {
    struct Attrib {
        enum Type {
            GENERIC=0,
            POSITION=1,
            NORMAL=2,
            TEXTURE_COORD=3,
            COLOR=4,
        };
        Type type;
        uint size=0;// number of components (float)
    };
    std::vector<Attrib> attributes;
    std::vector<float> vertex_data;
    std::vector<uint> indices;
    // Zero-initialize vectors
    MeshData():
        attributes(),
        vertex_data(),
        indices()
    {}
    // Populate MeshData
    MeshData(
        std::vector<Attrib> attributes,
        std::vector<float>  vertex_data,
        std::vector<uint>   indices
    ):
        attributes(attributes),
        vertex_data(vertex_data),
        indices(indices)
    {}
};
// Pretty-printing for MeshData
std::ostream& operator<<(std::ostream& os, const MeshData& m) {
    if (m.attributes.size() == 0) {
        os<< "{\n}";
        return os;
    }
    os << "{\n";
    uint vert_size=0;
    for (uint i=0; i < m.attributes.size(); i++) {
        //vert_size += m.attributes[i].size;
        os << "m.attributes[" << i << "].size = " << m.attributes[i].size;
    }
    os << "vert_size = " << vert_size;
    os << "}\n";
    return os;
}

int main() {
    // Initialize Lua state
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    const std::string lua_path=LUA_DIR"/simple.lua";
    std::string lua_src =
        "package.path=package.path..';" + getDirectory(lua_path) + "/?.lua'\n" +
        "package.path=package.path..';" + getDirectory(lua_path) + "/?/init.lua'\n"
    ;// setup package search paths
    luaL_dostring(L, lua_src.c_str());
    readfile(lua_path, lua_src);// note: overwrites lua_src
    std::cout << GREEN("\n```") << BLUE(lua_path) << "\n" << lua_src << GREEN("\n```") << std::endl;

    // Methods for MeshData, invoked by __index
    lua_newtable(L);
    int method_tbl=lua_gettop(L);
    // mesh.draw()
    lua_pushstring(L, "draw");// -2
    lua_pushcfunction(L, [](lua_State* L) -> int {
        // args: self, [indices]
        //assert(lua_isuserdata(L, 1));
        std::cout << "mesh.draw() called with " << lua_gettop(L) << " args." << std::endl;
        if (lua_isuserdata(L, 1))
            std::cout << *(MeshData*)(lua_touserdata(L, 1)) << std::endl;
        return 0;
    });// -1
    lua_settable(L, method_tbl);// ["draw"] = cfunction()

    // Assign method_tbl to __index
    // 'libmesh' to avoid name conflicts in registry, see: https://www.lua.org/manual/5.4/manual.html#4.3
    luaL_newmetatable(L, "libmesh.mt_MeshData");// -3
    lua_pushstring(L, "__index");// -2
    lua_pushvalue(L, method_tbl);// -1
    lua_settable(L, -3);// key: -2, value: -1

    // createMesh() for constructing MeshData
    lua_register(L, "createMesh", [](lua_State* L) -> int {
        // return: MeshData mesh
        MeshData& mesh = *(MeshData*)(lua_newuserdata(L, sizeof(MeshData)));// -2
        new (&mesh) MeshData();
        // attach metatable to mesh
        luaL_getmetatable(L, "libmesh.mt_MeshData");// -1
        lua_setmetatable(L, -2);
        // args: mesh_data={...}
        assert(lua_istable(L, 1));
        return 1;// mesh_id
    });

    // Run Lua Script
    if (luaL_dostring(L, lua_src.c_str()) != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        std::cerr << "Lua error: " << err << std::endl;
        lua_pop(L, 1);
    }
    // Cleanup
    lua_close(L);
    return 0;
}
