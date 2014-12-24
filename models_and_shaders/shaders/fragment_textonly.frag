#version 130

in vec3 outNormal;
in vec3 outTangent;
in vec3 outColor;
in vec3 outLightPos;
in vec2 outUV;
in vec3 position;

out vec4 fragColor;

uniform sampler2D texture0;
uniform vec3 cameraPosition;
uniform vec3 lightPos;

const vec3 ambientColor=vec3(0.05,0.05,0.05);
const vec3 diffuseColor=vec3(0.7,0.7,0.7);
const vec3 specularColor=vec3(1.0,1.0,1.0);

void main() {
		fragColor=texture2D( texture0, outUV );
}