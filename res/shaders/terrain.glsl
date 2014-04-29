Terrain Shading

-- vertex ---------------------------------------

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;

in vec3 position;

void main(void)
{
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}

-- fragment ---------------------------------------

uniform vec3 terrain_color;
out vec4 color;

void main(void)
{
    color = vec4(terrain_color, 1.0);
}
