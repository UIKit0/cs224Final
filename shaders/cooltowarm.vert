#ifdef GL_ES
precision highp float;
precision highp int;
precision lowp sampler2D;
#endif

attribute highp vec3 a_position;

attribute lowp vec2 a_texcoord;
varying lowp vec2 v_texcoord;

uniform highp mat4 matrix;

uniform vec3 LightPosition;
varying float NdotL;
varying vec3  ReflectVec;
varying vec3  ViewVec;

void main()
{        
        vec3 ecPos    = vec3 (gl_ModelViewMatrix * gl_Vertex);
        vec3 tnorm    = normalize(gl_NormalMatrix * gl_Normal);
        vec3 lightVec = normalize(LightPosition - ecPos);
        ReflectVec    = normalize(reflect(-lightVec, tnorm));
        ViewVec       = normalize(-ecPos);
        NdotL         = (dot (lightVec, tnorm) + 1.0) * 0.5;
        gl_Position   = ftransform();
}
