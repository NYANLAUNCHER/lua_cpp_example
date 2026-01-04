print("hello from lua!")
local AttribType = {
    GENERIC=0,
    POSITION=1,
    NORMAL=2,
    TEXTURE_COORD=3,
    COLOR=4,
};
local layout = {-- valid types: position, normal, texture_coord, color
  {-- attribute 1
    type=AttribType.POSITION,
    -- Valid Range: 1..4
    size=2 -- X, Y
  },
  {-- attribute 2
    type=AttribType.COLOR,
    -- Valid Range: 1..4
    size=3 -- R, G, B
  },
}
-- Square Mesh
local mesh = {
-- position    color
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
-- Instatiate the corresponding VAO, VBO, EBO in cpp
-- mesh_id = 1 since createMesh() just returns the mesh creation order
local mesh_id = createMesh(layout, mesh)

-- for now, just prints mesh to console (from cpp)
drawMesh(mesh_id)

--[[
mesh = {
--  position       color
  {-0.5, 0.5,  1.0, 0.0, 0.0}, -- Top left (1)
  { 0.5, 0.5,  0.0, 1.0, 0.0}, -- Top right (2)
  { 0.5,-0.5,  0.0, 0.0, 1.0}, -- Bottom Right (3)
  {-0.5,-0.5,  0.0, 0.0, 0.0}, -- Bottom Left (4)
  -- create a tringle on top of the box
  { 0.5, 1.0,  0.5, 0.5, 0.5}, -- Top Center (5)
  -- EBO Data:
  -- Note index starts with 1
  indices={-- using CW winding order
    1, 2, 3, -- upper-right triangle
    1, 3, 4, -- lower-left triangle
    1, 5, 2  -- top triangle
  }
}
-- Notice, you can only update verts and not vertex layout
updateMesh(mesh_id, mesh)
-- Re-draw mesh
drawMesh(mesh_id)
--]]
