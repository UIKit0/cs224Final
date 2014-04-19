#ifdef GL_ES
precision highp float;
precision highp int;
precision lowp sampler2D;
#endif

uniform lowp sampler2D texture;
varying lowp vec2 v_texcoord;

uniform highp vec3  SurfaceColor;
uniform highp vec3  WarmColor;
uniform highp vec3  CoolColor;
uniform highp float DiffuseWarm;
uniform highp float DiffuseCool;

varying highp float NdotL;
varying highp vec3  ReflectVec;
varying highp vec3  ViewVec;

void main()
{

	vec3 kcool    = min (CoolColor + DiffuseCool * SurfaceColor, 1.0);
	vec3 kwarm    = min (WarmColor + DiffuseWarm * SurfaceColor, 1.0);
	vec3 kfinal   = mix (kcool, kwarm, NdotL);

	vec3 nreflect = normalize (ReflectVec);
	vec3 nview    = normalize (ViewVec);

	float spec    = max (dot (nreflect, nview), 0.0);
	spec          = pow (spec, 32.0);
	
	gl_FragColor  = vec4 (min (kfinal + spec, 1.0), 1.0);
}
