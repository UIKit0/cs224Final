Gooch Shaders and Contour Detection

-- vertex.debug ------------------------------------------------

void main(void)
{
    const vec4 vertices[3] = vec4[3](vec4( 0.25,  0.25, 0.5, 1.0),
                                     vec4(-0.25, -0.25, 0.5, 1.0),
                                     vec4( 0.25, -0.25, 0.5, 1.0));
    gl_Position = vertices[gl_VertexID];
}

-- vertex.mvp -------------------------------------------------

uniform mat4 mvp_matrix;

in vec3 position;

void main(void)
{
    gl_Position = mvp_matrix * vec4(position, 1.0);
}

-- vertex.line -------------------------------------------------

in vec3 position;

void main(void)
{
    gl_Position = vec4(position, 1.0);
}

-- fragment.black ------------------------------------------------

out vec4 color;

void main(void)
{
    color = vec4(0.0, 0.0, 0.0, 1.0);
}

-- vertex ----------------------------------------------------------

uniform vec3 lightPos;

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;

in vec3 position;
in vec3 normal;
in vec2 texcoord;

out V_OUT
{
    vec3 normal;
    vec2 texcoord;

    float lightDotN;
    vec3 reflectVec;
    vec3 viewVec;
} v_out;

void main(void)
{
    vec3 ecPos = vec3(mv_matrix * vec4(position, 1.0));
    // TODO: properly compute normal passing inversetranspose
    vec3 tnorm = normalize(mat3(mv_matrix) * normal);
    vec3 lightVec = normalize(lightPos - ecPos);

    v_out.normal = normal;
    v_out.texcoord = texcoord;

    v_out.lightDotN = (dot (lightVec, tnorm) + 1.0) * 0.5;
    v_out.reflectVec = normalize(reflect(-lightVec, tnorm));
    v_out.viewVec = normalize(-ecPos);

    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}

-- geometry ------------------------------------------------------

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 12) out;
// 3 edges x 4 quads = 12 vertices

uniform float HalfWidth = 0.005;
uniform float OverhangLength = 0.15;

bool IsFront(vec3 A, vec3 B, vec3 C)
{
    float area = (A.x * B.y - B.x * A.y) +
                 (B.x * C.y - C.x * B.y) +
                 (C.x * A.y - A.x * C.y);
    return area > 0;
}

void EmitEdge(vec3 P0, vec3 P1)
{
    vec3  E = OverhangLength * vec3(P1.xy - P0.xy, 0);
    vec2  V = normalize(E.xy);
    vec3  N = vec3(-V.y, V.x, 0) * HalfWidth;
    vec3  S = -N;
    float D = HalfWidth;

    gl_Position = vec4(P0 + N - E, 1); EmitVertex();
    gl_Position = vec4(P0 + S - E, 1); EmitVertex();

    gl_Position = vec4(P1 + N + E, 1); EmitVertex();
    gl_Position = vec4(P1 + S + E, 1); EmitVertex();
    EndPrimitive();
}

void main()
{
    // ps (perspective division)
    vec3 v0 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
    vec3 v1 = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
    vec3 v2 = gl_in[2].gl_Position.xyz / gl_in[2].gl_Position.w;
    vec3 v3 = gl_in[3].gl_Position.xyz / gl_in[3].gl_Position.w;
    vec3 v4 = gl_in[4].gl_Position.xyz / gl_in[4].gl_Position.w;
    vec3 v5 = gl_in[5].gl_Position.xyz / gl_in[5].gl_Position.w;

    if (IsFront(v0, v2, v4)) {
        if (!IsFront(v0, v1, v2)) EmitEdge(v0, v2);
        if (!IsFront(v2, v3, v4)) EmitEdge(v2, v4);
        if (!IsFront(v0, v4, v5)) EmitEdge(v4, v0);
    }
}

-- geometry2 ---------------------------------------------------------

layout(triangles_adjacency) in;
layout(points, max_vertices = 12) out;
// 3 edges x 4 quads = 12 vertices

uniform mat4 mvp_matrix;
uniform mat4 m_matrix;
uniform mat4 v_matrix;
uniform vec3 viewVec;

// clockwise order
void EdgeContour(vec4 v1, vec4 v2, vec4 v3, vec3 edge, vec3 nface1)
{
    vec3 tv1 = vec3(m_matrix * v1);
    vec3 tv2 = vec3(m_matrix * v2);
    vec3 tv3 = vec3(m_matrix * v3);

    vec3 nface2 = normalize(cross(v2 - v1, v3 - v1));

    if (dot(nface2, edge - viewVec) * dot(nface1, edge - viewVec) > 0) {
        gl_Position = mvp_matrix * vec4(v1, 1); EmitVertex();
        gl_Position = mvp_matrix * vec4(v2, 1); EmitVertex();
        gl_Position = mvp_matrix * vec4(v3, 1); EmitVertex();
        EndPrimitive();
    }
}

void main(void)
{
    // get face normal (0,2,4) [ triangle ]
    // adjecent indices (5,1,3)

    vec3 v0 = vec3(m_matrix * gl_in[0].gl_Position);
    vec3 v2 = vec3(m_matrix * gl_in[2].gl_Position);
    vec3 v4 = vec3(m_matrix * gl_in[4].gl_Position);

    vec3 nface = normalize(cross(v2 - v0, v4 - v0));

    float viewDotN = dot(nface, viewVec);

    // check if triangle is front-facing
    if (viewDotN > 0) {
        EdgeContour(gl_in[0].gl_Position, gl_in[0].gl_Position, gl_in[0].gl_Position)
//        gl_Position = mvp_matrix * gl_in[0].gl_Position; EmitVertex();
//        gl_Position = mvp_matrix * gl_in[2].gl_Position; EmitVertex();
//        gl_Position = mvp_matrix * gl_in[4].gl_Position; EmitVertex();
//        EndPrimitive();
    }
}

-- fragment ------------------------------------------------------

uniform sampler2D tex_color;

uniform vec3 surfaceColor;
uniform vec3 warmColor;
uniform vec3 coolColor;
uniform float diffuseWarm;
uniform float diffuseCool;

in V_OUT
{
    vec2 texcoord;
    float lightDotN;
    vec3 reflectVec;
    vec3 viewVec;
} v_in;

out vec4 color;

void main(void)
{
    vec3 kcool = min(coolColor + diffuseCool * surfaceColor, 1.0);
    vec3 kwarm = min(warmColor + diffuseWarm * surfaceColor, 1.0);
    vec3 kfinal = mix(kcool, kwarm, v_in.lightDotN);

    vec3 nreflect = normalize(v_in.reflectVec);
    vec3 nview = normalize(v_in.viewVec);

    float spec = max(dot (nreflect, nview), 0.0);
    spec = pow(spec, 32.0);

    color = vec4(min (kfinal + spec, 1.0), 1.0);
}

-- directives.gles ------------------------------------------------

// TODO: directives should be in a separate file

"#ifdef GL_ES\n"
"precision highp float;\n"
"precision highp int;\n"
"precision lowp sampler2D;\n"
"#endif\n"
