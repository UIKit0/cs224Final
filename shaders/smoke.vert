#version 410

// Pass through shader for smoke particles
// Vertex
//


//
// Inputs
//

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TexCoord;


uniform mat4 M_Matrix;	// Model
uniform mat4 V_Matrix;	// View (camera)
uniform mat4 P_Matrix;	// Projection

//
// Outputs
//

out vec2 TexCoord;


void main()
{
	//Compute MVP_Matrix;
	mat4 MVP_Matrix = P_Matrix * V_Matrix * M_Matrix;
	
	mat4 mvmat = V_Matrix*M_Matrix;

	// View Space
	TexCoord = in_TexCoord;

	gl_Position = P_Matrix * (V_Matrix * M_Matrix * vec4(0.0, 0.0, 0.0, 1.0) + vec4(in_Position.x + 0.5, in_Position.y + 0.5, 0.0, 0.0));
}
