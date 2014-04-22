Gooch Shaders and Contour Detection

-- vertex.debug ------------------------------------------------

in vec4 color;

void main(void)
{
    const vec4 vertices[3] = vec4[3](vec4( 0.25, -0.25, 0.5, 1.0),
                                     vec4(-0.25, -0.25, 0.5, 1.0),
                                     vec4( 0.25,  0.25, 0.5, 1.0));
    gl_Position = vertices[gl_VertexID];
}

-- fragment.debug ------------------------------------------------

out vec4 color;

void main(void)
{
    color = vec4(0.0, 0.8, 1.0, 1.0);
}

-- vertex ----------------------------------------------------------

uniform mat4 mv;
uniform mat4 proj;

uniform vec3 lightPos;

in vec3 position;
in vec3 normal;
in vec2 texcoord;

out V_OUT
{
    vec2 texcoord;
    float lightdotn;
    vec3 reflectVec;
    vec3 viewVec;
} f_out;

void main(void)
{
    vec3 ecPos = vec3(mv * vec4(position, 1.0));
    // TODO: properly normalize the normal
    vec3 tnorm = normalize(mat3(mv) * normal);
    vec3 lightVec = normalize(lightPos - ecPos);
    f_out.reflectVec = normalize(reflect(-lightVec, tnorm));
    f_out.viewVec = normalize(-ecPos);
    f_out.lightdotn = (dot (lightVec, tnorm) + 1.0) * 0.5;
    f_out.texcoord = texcoord;

    gl_Position = proj * mv * vec4(position, 1.0);
}

-- fragment ------------------------------------------------------

uniform sampler2D texture;
uniform vec3 surfaceColor;
uniform vec3 warmColor;
uniform vec3 coolColor;
uniform float diffuseWarm;
uniform float diffuseCool;

in V_OUT
{
    vec2 texcoord;
    float lightdotn;
    vec3 reflectVec;
    vec3 viewVec;
} v_in;

out vec4 color;

void main(void)
{
    vec3 kcool = min(coolColor + diffuseCool * surfaceColor, 1.0);
    vec3 kwarm = min(warmColor + diffuseWarm * surfaceColor, 1.0);
    vec3 kfinal = mix(kcool, kwarm, v_in.lightdotn);

    vec3 nreflect = normalize(v_in.reflectVec);
    vec3 nview = normalize(v_in.viewVec);

    float spec = max(dot (nreflect, nview), 0.0);
    spec = pow(spec, 32.0);

    color = vec4(min (kfinal + spec, 1.0), 1.0);
//    color = vec4(1.0, 0.0, 0.0, 1.0);
}
