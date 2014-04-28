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
float distance;

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;

in vec3 position;
in float size;

//out V_OUT
//{

//} v_out;
void main(void)
{
    vec4 pos = proj_matrix * mv_matrix * vec4(position, 1.0);

    if (pos.w == 0.0) {
        distance = 0.00001;
    } else {
        distance = pos.w;
    }

    gl_Position = pos;
    gl_PointSize = (size * SCALE) / distance;
}

-- fragment.point ---------------------------------------

uniform sampler2D tex_alpha;
uniform sampler2D tex_color;
uniform sampler2D tex_depth;
uniform sampler2D tex_norm;


uniform vec3 Ld = vec3(1.0,1.0,1.0);
uniform vec3 lightPos;

float granularity = 3.0;
float invGranularity = 1.0/granularity;


uniform mat4 view_matrix;

//in V_OUT
//{

//} f_in;

out vec4 color;

void main(void)
{
    vec4 alpha = texture(tex_alpha, gl_PointCoord);
    vec4 shade = texture(tex_color, gl_PointCoord);
    vec4 depth = texture(tex_depth, gl_PointCoord);
    vec4 normal = texture(tex_norm, gl_PointCoord);

    // Calculate the light position
    vec4 tlightPos = view_matrix*vec4(lightPos.xyz,1);

    // Retrieve and unpack the normal vector from the billboard
    vec4 Normal = normal;
    Normal.x = (Normal.x * 2.0) - 1.0;
    Normal.y = (Normal.y * 2.0) - 1.0;
    Normal.z = (Normal.z * 2.0) - 1.0;

    // Calculate lighting
    vec3 n = normalize(Normal.xyz);
    vec3 hitToLight = normalize(vec3(tlightPos));
    float difDot = max(dot(hitToLight,n),0.0);
    float toonDif = floor(difDot * granularity) * invGranularity;

    // Color the pixel
    vec3 Kd = shade.xyz * vec3(0.9,0.9,0);
    vec3 ambLight = Ld*Kd;
    vec3 difLight = Ld*(Kd*toonDif);

    color = vec4(ambLight + difLight, alpha.x);

    // Modify the depth according the texture values
    float d = depth.x - 0.5;
    gl_FragDepth = gl_FragCoord.z + d;
}
