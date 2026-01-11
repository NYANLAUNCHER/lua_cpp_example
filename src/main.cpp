// std
#include <string>
#include <format>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstddef>
#include <new>
#include <exception>
#include <cassert>
// lua
#include "lua.hpp"
// Local
#include "_generated/sys_paths.h"
typedef unsigned int uint;
inline std::string RED(std::string s) { return "\032[31m" + s + "\033[0m"; }
inline std::string GREEN(std::string s) { return "\033[32m" + s + "\033[0m"; }
inline std::string BLUE(std::string s) { return "\033[34m" + s + "\033[0m"; }

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
        uint length=0;// number of components (float)
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
    uint vert_count=0;
    for (uint i=0; i < m.attributes.size(); i++) {
        os << "m.attributes[" << i << "].count = " << m.attributes[i].length;
    }
    os << "vert_size = " << vert_count;
    os << "}\n";
    return os;
}

int main() {
    // Initialize Lua state
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    const std::string lua_path=LUA_DIR"/simple.lua";
    // setup package search paths
    std::string lua_src =
        "package.path=package.path..';" + getDirectory(lua_path) + "/?.lua'\n" +
        "package.path=package.path..';" + getDirectory(lua_path) + "/?/init.lua'\n";
    luaL_dostring(L, lua_src.c_str());
    // load lua script
    readfile(lua_path, lua_src);// note: overwrites lua_src
    std::cout << GREEN("\n```") << BLUE(lua_path) << "\n" << lua_src << GREEN("\n```") << std::endl;

    // Methods for MeshData, invoked by __index
    lua_newtable(L);
    int method_tbl=lua_gettop(L);

    // mesh:draw([indices])
    lua_pushstring(L, "draw");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        if (lua_gettop(L) > 2) {
            std::cout << RED("Error: ") <<
                "Method \"mesh:draw()\" cannot handle more than 2 arguments. " <<
                lua_gettop(L) << " were passed." << std::endl;
            return 0;
        }

        // arg 1: self
        if (lua_isuserdata(L, 1)) {
            std::cout << *(MeshData*)(lua_touserdata(L, 1)) << std::endl;
        } else {
            std::cout << RED("Error: ") <<
                "Method \"mesh:draw()\" requires parameter 1 (aka \"self\") to be of type `userdata`." << std::endl;
        }

        // arg 2: [indices]
        if (lua_gettop(L) == 1)
            return 0;// only process indices if it exists
        if (lua_istable(L, 2)) {
            lua_getglobal(L, "print");
            lua_pushvalue(L, 2);
            lua_call(L, 1, LUA_MULTRET);// equivalent to `print(indices)` in lua
        } else {
            std::cout << RED("Error: ") <<
                "Method \"mesh:draw()\" requires parameter 2 (aka \"indices\") to be of type `table`." << std::endl;
        }
        return 0;
    });
    lua_settable(L, method_tbl);

    // mesh:size()
    lua_pushstring(L, "dump");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        // return: raw bytes of MeshData as a string of hexidecimal
        MeshData& mesh = *(MeshData*)(lua_touserdata(L, 1));
        std::string buffer;
        for (size_t i=0; i < sizeof(mesh.vertex_data); ++i)
            buffer += std::format("{:#04x} ", ((uint8_t*)&(mesh.vertex_data))[i]);
        lua_pushstring(L, buffer.c_str());
        return 1;
    });
    lua_settable(L, method_tbl);


    // Assign method_tbl to __index
    // see: https://www.lua.org/pil/27.3.1.html
    static const void* mt_MeshData;// use light userdata to index metatable in registry
    // initialize mt_MeshData table
    lua_newtable(L);// [+1]
    lua_pushstring(L, "__index");// [+1]
    lua_pushvalue(L, method_tbl);// [+1]
    lua_settable(L, -3);// [-2]
    // push mt_MeshData table to registry
    lua_pushlightuserdata(L, &mt_MeshData);// [+1]
    lua_pushvalue(L, -2);// [+1]
    lua_settable(L, LUA_REGISTRYINDEX);// [-2]

    // construct MeshData in lua
    auto createMesh = [](lua_State* L) -> int {
        // return: MeshData mesh
        MeshData& mesh = *(MeshData*)(lua_newuserdata(L, sizeof(MeshData)));
        new (&mesh) MeshData();// "placement new" to construct MeshData in-place
        // upvalue: metatable @ REGISTRY[&mt_MeshData]
        lua_pushvalue(L, lua_upvalueindex(1));
        // assign mt_MeshData to userdata `mesh`
        lua_setmetatable(L, -2);

        // args: mesh_data={...}
        assert(lua_istable(L, 1));
        return 1;
    };
    lua_pushvalue(L, LUA_REGISTRYINDEX);
    lua_pushlightuserdata(L, &mt_MeshData);
    lua_rawget(L, -2);// pass metatable as upvalue
    lua_pushcclosure(L, createMesh, 1);
    // provide "createMesh()" for lua
    lua_setglobal(L, "createMesh");

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
