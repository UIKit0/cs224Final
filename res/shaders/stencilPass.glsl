Render shadow volumes into the stencil buffer

-- vertex.stencil ---------------------------------------

in vec3 position;

out vec3 WorldPos;                                                                 

uniform mat4 mvp_matrix;
uniform mat4 m_matrix;
                                                                                    
void main()                                                                         
{                                                                                   
    vec4 PosL   = vec4(position, 1.0);
    gl_Position = mvp_matrix * PosL;
    WorldPos    = (m_matrix * PosL).xyz;
}

-- geometry.stencil ---------------------------------------

layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 18) out;

in vec3 WorldPos[];

uniform vec3 LightPosition;
uniform mat4 vp_matrix;

float EPSILON = 0.01;

void EmitQuad(int StartIndex, vec3 StartVertex, int EndIndex, vec3 EndVertex)
{
    vec3 LightDir = normalize(StartVertex - LightPosition);
    vec3 l = LightDir * EPSILON;

    gl_Position = vp_matrix * vec4((StartVertex + l), 1.0);
    EmitVertex();
    
    gl_Position = vp_matrix * vec4(LightDir, 0.0);
    EmitVertex();

    LightDir = normalize(EndVertex - LightPosition);
    l = LightDir * EPSILON;
    gl_Position = vp_matrix * vec4((EndVertex + l), 1.0);
    EmitVertex();
    
    gl_Position = vp_matrix * vec4(LightDir, 0.0);
    EmitVertex();

    EndPrimitive();            
}


void main()
{
    vec3 e1 = WorldPos[2] - WorldPos[0];
    vec3 e2 = WorldPos[4] - WorldPos[0];
    vec3 e3 = WorldPos[1] - WorldPos[0];
    vec3 e4 = WorldPos[3] - WorldPos[2];
    vec3 e5 = WorldPos[4] - WorldPos[2];
    vec3 e6 = WorldPos[5] - WorldPos[0];

    vec3 Normal = cross(e1,e2);
    vec3 LightDir = LightPosition - WorldPos[0];


    // Calculate front top vertices
    //  Extend bottom two vertices far away
    //  Emit a quad

    // Emit the degenerate endpoints

    // Calculate back top vertices
    //  Extend bottom two vertices far away
    //  Emit a quad

    if (dot(Normal, LightDir) > 0.000001) {

        Normal = cross(e3,e1);

        if (dot(Normal, LightDir) <= 0) {
            vec3 StartVertex = WorldPos[0];
            vec3 EndVertex = WorldPos[2];
            EmitQuad(0, StartVertex, 2, EndVertex);
        }

        Normal = cross(e4,e5);
        LightDir = LightPosition - WorldPos[2];

        if (dot(Normal, LightDir) <= 0) {
            vec3 StartVertex = WorldPos[2];
            vec3 EndVertex = WorldPos[4];
            EmitQuad(2, StartVertex, 4, EndVertex);
        }

        Normal = cross(e2,e6);
        LightDir = LightPosition - WorldPos[4];

        if (dot(Normal, LightDir) <= 0) {
            vec3 StartVertex = WorldPos[4];
            vec3 EndVertex = WorldPos[0];
            EmitQuad(4, StartVertex, 0, EndVertex);
        }

        vec3 LightDir = (normalize(WorldPos[0] - LightPosition)) * EPSILON;
        gl_Position = vp_matrix * vec4((WorldPos[0] + LightDir), 1.0);
        EmitVertex();

        LightDir = (normalize(WorldPos[2] - LightPosition)) * EPSILON;
        gl_Position = vp_matrix * vec4((WorldPos[2] + LightDir), 1.0);
        EmitVertex();

        LightDir = (normalize(WorldPos[4] - LightPosition)) * EPSILON;
        gl_Position = vp_matrix * vec4((WorldPos[4] + LightDir), 1.0);
        EmitVertex();
        EndPrimitive();
    }
}

-- fragment.stencil ---------------------------------------

void main()
{
	// Stencil buffer automatically updated
}


-- geometry.billboard -----------------------------------------

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

void main(void)
{
    g_out.csPos = v_in[0].csPos;

    vec4 pos = v_matrix * gl_in[0].gl_Position;

    float fullSize = v_in[0].size;
    float halfSize = v_in[0].size / 2.0;

    // TODO: Outline is in Shadow Volumes how to

    // bottom left
    pos.x -= halfSize;
    pos.y += halfSize;
    gl_Position = p_matrix * pos;
    g_out.texcoord = vec2(0.0, 1.0);
    EmitVertex();

    // bottom right
    pos.y += fullSize;
    pos.x += fullSize;
    gl_Position = p_matrix * pos;
    g_out.texcoord = vec2(1.0, 1.0);
    EmitVertex();

    // top left
    pos.y -= fullSize;
    gl_Position = p_matrix * pos;
    g_out.texcoord = vec2(0.0, 0.0);
    EmitVertex();

    // top right
    pos.y -= fullSize;
    gl_Position = p_matrix * pos;
    g_out.texcoord = vec2(1.0, 0.0);
    EmitVertex();

    EndPrimitive();
}
