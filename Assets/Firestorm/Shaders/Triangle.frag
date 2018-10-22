#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Old Code 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// GLSL shader version 1.30 (for OpenGL 3.1)
//#version 130

// Fragment input from the vertex shader
//in vec4 vertexColor;

// Fragment output color
//out vec4 fragColor;

// Fragment shader main function
//void main()
//{
	//fragColor = vertexColor;
//}