Smoke Shading

-- vertex.debug ---------------------------------------

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;

in vec3 position;
in vec3 normal;
in vec2 texcoord;

out V_OUT
{
    vec2 texcoord;
} v_out;

void main(void)
{
    v_out.texcoord = texcoord;
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}

-- vertex.billboard -------------------------------------------

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

-- fragment.billboard ---------------------------------------

uniform sampler2D tex_alpha;
uniform sampler2D tex_color;
uniform sampler2D tex_depth;
uniform sampler2D tex_norm;

uniform vec3 Ld = vec3(1.0,1.0,1.0);
uniform vec3 LightPosition = vec3(-1.0, -1.0, -1.0);

float granularity = 2.0;
float invGranularity = 1.0/granularity;

uniform mat4 p_matrix;
uniform mat4 v_matrix;

uniform float zNear = 0.1;
uniform float zFar = 100.0;

in G_OUT
{
    vec4 csPos;
    vec2 texcoord;
} g_in;

out vec4 color;

float lum(vec3 v)
{
    return 0.2126*v.x + 0.7152*v.y + 0.0722*v.z;
}

void main(void)
{
    vec4 alpha = texture(tex_alpha, g_in.texcoord);
    vec4 shade = texture(tex_color, g_in.texcoord);
    vec4 depth = texture(tex_depth, g_in.texcoord);
    vec4 normal = texture(tex_norm, g_in.texcoord);

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

    // Calculate the light position
    vec4 lightPos = v_matrix * vec4(LightPosition.xyz,0);

    // Calculate lighting

        // A. Rotate normals into their perp plane from image plane
        //      Does proper lighting

        // Get axis of rotation
        vec3 pos = g_in.csPos.xyz + d*normalize(g_in.csPos.xyz);
        vec3 look = vec3(0,0,-1);
        vec3 axis = cross(pos, look);
        axis = normalize(axis);
        float theta = acos(dot(look,pos)/(length(look)*length(pos)));

        // Create A
        mat3 A;
        A[0] = vec3(      0,-axis.z, axis.y);
        A[1] = vec3( axis.z,      0,-axis.x);
        A[2] = vec3(-axis.y, axis.x,      0);

        // Find rotation matrix
        mat3 R = mat3(1.0) + sin(theta)*A + (1.0-cos(theta))*A*A;

        // B. Calculate cel-shading pixel lighting
        //
        vec3 n = normalize(R*normal.xyz);
        vec3 hitToLight = normalize(vec3(lightPos));
        float difDot = max(dot(hitToLight,n),0.0);
        float toonDif = floor(difDot * granularity) * invGranularity;

    // Color the pixel
    vec3 Kd = shade.xyz;
    vec3 ambLight = Ld*Kd*0.1;
    vec3 difLight = Ld*(Kd*toonDif);

    float a = lum(alpha.xyz);
    color = vec4(ambLight + difLight, a);
}

-- fragment.debug ---------------------------------------

uniform sampler2D tex_color;

in G_OUT
{
    vec2 texcoord;
} g_in;

out vec4 color;

void main(void)
{
//    color = texture(tex_color, g_in.texcoord);
    color = vec4(1.0, 0, 0, 1.0);
}

-- vertex.point ---------------------------------------

float SCALE = 200.0;

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;

in vec3 position;
in float size;

//out V_OUT
//{

//} v_out;

out vec4 cs_position;

void main(void)
{
    float distance = 1.0;
    vec4 pos = proj_matrix * mv_matrix * vec4(position, 1.0);

    if (pos.w == 0.0) {
        distance = 0.00001;
    } else {
        distance = pos.w;
    }

    cs_position = mv_matrix * vec4(position, 1.0);
    gl_Position = pos;
    gl_PointSize = (size * SCALE) / distance;
}




-- fragment.point ---------------------------------------

uniform sampler2D tex_alpha;
uniform sampler2D tex_color;
uniform sampler2D tex_depth;
uniform sampler2D tex_norm;

uniform vec3 Ld = vec3(1.0,1.0,1.0);
uniform vec3 LightPosition = vec3(-1.0, -1.0, -1.0);

float granularity = 2.0;
float invGranularity = 1.0/granularity;

uniform mat4 proj_matrix;
uniform mat4 V_Matrix;

uniform float zNear = 0.1;
uniform float zFar = 100.0;

//in V_OUT
//{

//} f_in;

in vec4 cs_position;

out vec4 color;

float lum(vec3 v)
{
    return 0.2126*v.x + 0.7152*v.y + 0.0722*v.z;
}

void main(void)
{
    vec4 alpha = texture(tex_alpha, gl_PointCoord);
    vec4 shade = texture(tex_color, gl_PointCoord);
    vec4 depth = texture(tex_depth, gl_PointCoord);
    vec4 normal = texture(tex_norm, gl_PointCoord);

    // Modify the depth according the texture values
    float d = lum(depth.xyz);
    vec4 cameraCoords = cs_position;
    cameraCoords.z -= 2.0*(1.0-d);
    vec4 clipCoords = proj_matrix * cameraCoords;
    float ndc_depth = clipCoords.z / clipCoords.w;
    float Far = gl_DepthRange.far;
    float Near = gl_DepthRange.near;
    float newdepth = (((Far - Near) * ndc_depth) + Near + Far) * 0.5;
    gl_FragDepth = newdepth;

    // Calculate the light position
    vec4 lightPos = V_Matrix * vec4(LightPosition.xyz,0);

    // Calculate lighting

        // A. Rotate normals into their perp plane from image plane
        //      Does proper lighting

        // Get axis of rotation
        vec3 pos = cs_position.xyz + d*normalize(cs_position.xyz);
        vec3 look = vec3(0,0,-1);
        vec3 axis = cross(pos, look);
        axis = normalize(axis);
        float theta = acos(dot(look,pos)/(length(look)*length(pos)));

        // Create A
        mat3 A;
        A[0] = vec3(      0,-axis.z, axis.y);
        A[1] = vec3( axis.z,      0,-axis.x);
        A[2] = vec3(-axis.y, axis.x,      0);

        // Find rotation matrix
        mat3 R = mat3(1.0) + sin(theta)*A + (1.0-cos(theta))*A*A;

        // B. Calculate cel-shading pixel lighting
        //
        vec3 n = normalize(R*normal.xyz);
        vec3 hitToLight = normalize(vec3(lightPos));
        float difDot = max(dot(hitToLight,n),0.0);
        float toonDif = floor(difDot * granularity) * invGranularity;

    // Color the pixel
    vec3 Kd = shade.xyz;
    vec3 ambLight = Ld*Kd*0.1;
    vec3 difLight = Ld*(Kd*toonDif);

    float a = lum(alpha.xyz);
    color = vec4(ambLight + difLight, a);
}
