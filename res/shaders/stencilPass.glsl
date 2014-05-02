Render shadow volumes into the stencil buffer

-- vertex.stencil ---------------------------------------

in vec3 position;
in float size;
uniform mat4 mv_matrix;

out V_OUT
{
    float size;
} v_out;

void main(void)
{
    v_out.size = size;
    gl_Position = vec4(position, 1.0);
}

-- geometry.stencil -----------------------------------------

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 p_matrix;
uniform mat4 v_matrix;
uniform mat4 vp_matrix;

uniform vec3 LightPosition;

in V_OUT
{
    float size;
} v_in[];

out G_OUT
{
    vec2 texcoord;
} g_out;

void main(void)
{
    vec4 pos = v_matrix * gl_in[0].gl_Position;
    vec4 opos = pos;

    float fullSize = v_in[0].size;
    float halfSize = v_in[0].size / 2.0;
    halfSize += vp_matrix[0].x*0.0000001;
    halfSize += LightPosition.x*0.0000001;

    vec3 csLPos = -(v_matrix * vec4(LightPosition,0)).xyz;
    vec4 infPos = vec4(csLPos,1);

    // Infinity vertex
    gl_Position = infPos;
    EmitVertex();
    // Front top left
    pos = opos;
    pos.x -= halfSize;
    pos.z += halfSize;
    gl_Position = p_matrix * pos;
    EmitVertex();

    // Infinity vertex
    gl_Position = infPos;
    EmitVertex();
    // Front top right
    pos = opos;
    pos.x += halfSize;
    pos.z += halfSize;
    gl_Position = p_matrix * pos;
    EmitVertex();

    EndPrimitive();

    // back top left
    pos = opos;
    pos.x -= halfSize;
    pos.z -= halfSize;
    gl_Position = p_matrix * pos;
    EmitVertex();
    // Infinity vertex
    gl_Position = infPos;
    EmitVertex();

    // top right
    pos = opos;
    pos.x += halfSize;
    pos.z -= halfSize;
    gl_Position = p_matrix * pos;
    EmitVertex();
    // Infinity vertex
    gl_Position = infPos;
    EmitVertex();

    EndPrimitive();
}

-- fragment.stencil ---------------------------------------

out vec4 color;

void main()
{
	// Stencil buffer automatically updated
//    gl_FragDepth = 0.0;
    color = vec4(0,1,0,1);
}


