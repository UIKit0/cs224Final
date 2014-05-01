Depth buffer pass

-- vertex.depth ---------------------------------------

in vec3 position; 

uniform mat4 MVP_Matrix;

void main()
{          
    gl_Position = MVP_Matrix * vec4(position, 1.0);
}

-- fragment.depth ---------------------------------------

void main()
{
	// Do nothing, depth buffer handled
}
