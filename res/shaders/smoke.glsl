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

-- fragment.debug ---------------------------------------

uniform sampler2D tex_color;

in V_OUT
{
    vec2 texcoord;
} f_in;

out vec4 color;

void main(void)
{
//    color = texture(tex_color, f_in.texcoord);
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
//    vec4 cameraCoords = cs_position;
//    cameraCoords.z -= 2*(1.0-d);
//    vec4 clipcoords = proj_matrix * cameraCoords;
//    vec4 ndCoords = vec4((clipcoords.xyz)/clipcoords.w,0);
//    gl_FragDepth = ndCoords.z;

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
