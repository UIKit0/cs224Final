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

float SCALE = 100.0;
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

uniform sampler2D tex_color;
uniform sampler2D tex_depth;
uniform sampler2D tex_norm;

vec4 lightPos;

//in V_OUT
//{

//} f_in;

out vec4 color;

void main(void)
{

    color = texture(tex_color, gl_PointCoord);
//    color = vec4(1.0,0,0,1.0);
}


-- fragment.depth ---------------------------------------

// Depth altering smoke shader
// Fragment

in vec2 TexCoord;
uniform mat4 V_Matrix;

uniform sampler2D AlphaMap;
uniform sampler2D DepthMap;
uniform sampler2D NormalMap;
uniform sampler2D TexMap;

uniform vec3 Ld = vec3(1.0,1.0,1.0);
uniform vec3 LightPosition = vec3(0.0f, 0.0f, 2.0f);

vec3 ambLight;
vec3 difLight;
vec3 specLight;

float granularity = 3.0;
float invGranularity = 1.0/granularity;
float depth;

vec4 lightPos;

out vec4 FragColor;

void main()
{
    // Calculate the light position
    lightPos = V_Matrix*vec4(-LightPosition.xyz,0);

    // Retrieve and unpack the normal vector from the billboard
    vec4 Normal = texture(tex_norm, gl_PointCoord);
    Normal.x = (Normal.x * 2.0) - 1.0;
    Normal.y = (Normal.y * 2.0) - 1.0;
    Normal.z = (Normal.z * 2.0) - 1.0;

    // Retrieve depth info
    depth = texture(tex_depth, gl_PointCoord).x - 0.5;

    // Calculate lighting
    vec3 n = normalize(Normal.xyz);
    vec3 hitToLight = normalize(vec3(lightPos));
    float difDot = max(dot(hitToLight,n),0.0);
    float toonDif = floor(difDot * granularity) * invGranularity;

    // Color the pixel
    vec4 color = texture(tex_color, gl_PointCoord);
    float alpha = color.w;
    vec3 Kd = color.xyz;
    ambLight = Ld*Kd;
    difLight = Ld*(Kd*toonDif);
    specLight = vec3(0);

    FragColor = vec4(ambLight + difLight + specLight, alpha);

    // Modify the depth according the texture values
    gl_FragDepth = gl_FragCoord.z + depth;
}
