#version 130

in vec3 vertex;
in vec3 normal;
in vec3 tangent;
in vec3 color;
in vec2 UV;

out vec3 outNormal;
out vec3 outTangent;
out vec3 outColor;
out vec2 outUV;
out vec3 position;

uniform mat4 uniMVP;

void main()
{
	gl_Position=uniMVP*vec4(vertex,1.0);
	position=vec3(uniMVP*vec4(vertex,1.0));
	outNormal=gl_NormalMatrix*normal;
	outTangent=gl_NormalMatrix*outTangent;
	outColor=color;
	outUV=UV;
}