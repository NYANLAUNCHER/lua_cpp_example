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
        uint size=0;// number of components (float)
    };
    // vertices
    std::vector<float> vertexData;
    std::vector<uint> indices;
    // attributes
    std::vector<Attrib> attributes;
    MeshData() {
        vertexData.clear();
        indices.clear();
        attributes.clear();
    }
    // called by createMesh()
    MeshData(
        std::vector<float> vertexData,
        std::vector<uint> indices,
        std::vector<Attrib> attributes
    ):
        vertexData(vertexData),
        indices(indices),
        attributes(attributes)
    {}
};
// Pretty-printing for MeshData
std::ostream& operator<<(std::ostream& os, const MeshData& m) {
    if (m.attributes.size() == 0)
        return os;
    //uint vert_size=0;
    //for (uint i=0; i < m.attributes.size(); i++) {
    //    //vert_size += m.attributes[i].size;
    //    os << "m.attributes[" << i << "].size = " << m.attributes[i].size;
    //}
    //os << "vert_size = " << vert_size;
    return os;
}

// Cpp MeshData for lua to manipulate
static std::vector<MeshData> gMESH_DATA;
int main() {
    // Initialize Lua state
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    const std::string lua_path=LUA_DIR"/simple.lua";
    // setup package search paths
    std::string lua_src =
        "package.path=package.path..';" + getDirectory(lua_path)+"/?.lua'\n" +
        "package.path=package.path..';" + getDirectory(lua_path)+"/?/init.lua'\n"
    ;
    luaL_dostring(L, lua_src.c_str());
    readfile(lua_path, lua_src);// overwrites lua_src
    std::cout << "\n```" << BLUE(lua_path) << "\n" << lua_src << "\n```" << std::endl;

    // Create new MeshData for Cpp
    // args: layout, mesh, [indices]
    lua_register(L, "createMesh", [](lua_State* L) -> int {
        // construct new MeshData at end of vector
        gMESH_DATA.emplace_back(/*
            vertexData,
            indices,
            attributes
        */);
        // return: mesh_id
        int& mesh_id = *(int*)(lua_newuserdata(L, sizeof(int)));
        mesh_id = gMESH_DATA.size();
        return 1;// mesh_id
    });

    // Update existing MeshData in Cpp
    // args: mesh_id, layout, mesh, [indices]
    lua_register(L, "updateMesh", [](lua_State* L) -> int {
        assert(lua_isuserdata(L, 1));// arg: mesh_id
        //int& mesh_id=*(int*)(lua_touserdata(L, 1));
        //assert(lua_istable(L, 2));// arg: layout
        //assert(lua_istable(L, 3));// arg: mesh
        // Optionally handle indices
        if (lua_gettop(L) == 4) {
            assert(lua_istable(L, 4));// arg: indices
        }
        return 0;
    });

    // Print MeshData
    // args: userdata_t mesh_id
    lua_register(L, "drawMesh", [](lua_State* L) -> int {
        assert(lua_isuserdata(L, 1));
        std::cout << gMESH_DATA[*(int*)(lua_touserdata(L, 1)) - 1] << std::endl;
        return 0;
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
