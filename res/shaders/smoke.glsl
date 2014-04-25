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
} f_out;

void main(void)
{
    f_out.texcoord = texcoord;
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}

-- fragment.debug ---------------------------------------

uniform sampler2D tex_color;

in V_OUT
{
    vec2 texcoord;
} v_in;

out vec4 color;

void main(void)
{
//    color = texture(tex_color, v_in.texcoord);
    color = vec4(1.0, 0, 0, 1.0);
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
    vec4 Normal = texture(NormalMap, TexCoord);
    Normal.x = (Normal.x * 2.0) - 1.0;
    Normal.y = (Normal.y * 2.0) - 1.0;
    Normal.z = (Normal.z * 2.0) - 1.0;

    // Retrieve depth info
    depth = texture(DepthMap, TexCoord).x - 0.5;

    // Calculate lighting
    vec3 n = normalize(Normal.xyz);
    vec3 hitToLight = normalize(vec3(lightPos));
    float difDot = max(dot(hitToLight,n),0.0);
    float toonDif = floor(difDot * granularity) * invGranularity;

    // Color the pixel
    vec4 color = texture(TexMap, TexCoord);
    float alpha = texture(AlphaMap, TexCoord).x;
    vec3 Kd = color.xyz;
    ambLight = Ld*Kd;
    difLight = Ld*(Kd*toonDif);
    specLight = vec3(0);

    FragColor = vec4(ambLight + difLight + specLight, alpha);
    FragColor.b = 10000;

    // Modify the depth according the texture values
    gl_FragDepth = 0;//gl_FragCoord.z + depth;
}
