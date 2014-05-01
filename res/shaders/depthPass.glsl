Depth buffer pass

-- vertex.depth ---------------------------------------

in vec3 position;
in float size;
uniform mat4 mv_matrix;

out V_OUT
{
    float size;
    vec4 csPos;
} v_out;

void main(void)
{
    v_out.size = size;
    v_out.csPos = mv_matrix * vec4(position, 1.0);
    gl_Position = vec4(position, 1.0);
}

-- geometry.depth ---------------------------------------

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 p_matrix;
uniform mat4 v_matrix;

in V_OUT
{
    float size;
    vec4 csPos;
} v_in[];

out G_OUT
{
    vec4 csPos;
    vec2 texcoord;
} g_out;

out vec4 cs_position;

void main(void)
{
    g_out.csPos = v_in[0].csPos;

    vec4 pos = v_matrix * gl_in[0].gl_Position;

    float fullSize = v_in[0].size;
    float halfSize = v_in[0].size / 2.0;

    // bottom left
    pos.x -= halfSize;
    pos.y += halfSize;
    cs_position = pos;
    gl_Position = p_matrix * pos;
    g_out.texcoord = vec2(0.0, 1.0);
    EmitVertex();

    // top left
    pos.y -= fullSize;
    cs_position = pos;
    gl_Position = p_matrix * pos;
    g_out.texcoord = vec2(0.0, 0.0);
    EmitVertex();

    // bottom right
    pos.y += fullSize;
    pos.x += fullSize;
    cs_position = pos;
    gl_Position = p_matrix * pos;
    g_out.texcoord = vec2(1.0, 1.0);
    EmitVertex();

    // top right
    pos.y -= fullSize;
    cs_position = pos;
    gl_Position = p_matrix * pos;
    g_out.texcoord = vec2(1.0, 0.0);
    EmitVertex();

    EndPrimitive();
}


-- fragment.depth ---------------------------------------

uniform sampler2D tex_depth;


uniform mat4 p_matrix;
uniform mat4 v_matrix;

uniform float zNear = 0.1;
uniform float zFar = 100.0;

in G_OUT
{
    vec4 csPos;
    vec2 texcoord;
} g_in;

float lum(vec3 v)
{
    return 0.2126*v.x + 0.7152*v.y + 0.0722*v.z;
}

void main(void)
{
    vec4 depth = texture(tex_depth, g_in.texcoord);

    // Modify the depth according the texture values
    float d = lum(depth.xyz);

    vec4 cameraCoords = g_in.csPos;
    cameraCoords.z -= 2*(1.0-d);
    vec4 clipCoords = p_matrix * cameraCoords;

    float ndc_depth = clipCoords.z / clipCoords.w;
    float Far = gl_DepthRange.far;
    float Near = gl_DepthRange.near;
    float newdepth = (((Far - Near) * ndc_depth) + Near + Far) * 0.5;
    gl_FragDepth = newdepth;
}
