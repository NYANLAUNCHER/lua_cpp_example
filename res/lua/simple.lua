local attrib=require('CPP').AttribType
local mesh_data = {
  layout = {
    {
      type=attrib.POSITION,
      size=2
    },
    {
      type=attrib.COLOR,
      size=3
    },
  },
  --position    color
  {-0.5, 0.5,  1.0, 0.0, 0.0}, -- Top left (1)
  { 0.5, 0.5,  0.0, 1.0, 0.0}, -- Top right (2)
  { 0.5,-0.5,  0.0, 0.0, 1.0}, -- Bottom Right (3)
  {-0.5,-0.5,  0.0, 0.0, 0.0}, -- Bottom Left (4)
  -- EBO Data:
  -- Notes:
  -- - must contain only valid keys in `mesh`
  -- - vertices can have any key as long as they are indexed in `indices`
  indices={-- using CW winding order
    1, 2, 3, -- upper-right triangle
    1, 3, 4  -- lower-left triangle
  }
}
-- args: mesh_data
local mesh = createMesh(mesh_data)
mesh:draw()

-- args: [indices]
mesh:draw({-- only draw a sub-set of indices
  1, 2, 3, -- upper-right triangle
})
