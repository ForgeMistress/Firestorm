#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) out vec3 fragColor;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Old Code 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GLSL shader version 1.30 (for OpenGL 3.1)
//#version 130

// Vertex attributes (these names must match our vertex format attributes)
//in vec2 position;
//in vec4 color;

// Vertex output to the fragment shader
//out vec4 vertexColor;

// Vertex shader main function
//void main()
//{
	//gl_Position = vec4(position, 0, 1);
	//vertexColor = color;
//}