Terrain Shading

-- vertex ---------------------------------------

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;

in vec3 position;

void main(void)
{
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}

-- fragment ---------------------------------------

uniform vec3 terrain_color;
out vec4 color;

void main(void)
{
    float far = 90.0f;
    float near = 40.0f;
    float fog = 1;
    float distance = gl_FragCoord.z/gl_FragCoord.w;
    vec3 target_color = vec3(1.0f,1.0f,1.0f);

    if (distance > near && distance < far){
        // TODO: make this a different function to look better
        fog = (far - distance)/(far - near);
    }
    if (distance > far){
        fog = 0;
    }

    color = vec4(terrain_color*fog + target_color*(1 - fog), 1.0f);
}
