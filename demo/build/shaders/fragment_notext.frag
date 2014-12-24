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

void main()
{
		vec3 normal=normalize(outNormal);
		vec3 surf2light=normalize(lightPos-position);
		vec3 surf2camera=normalize(-position);
		vec3 reflection=-reflect(surf2camera,normal);
		float diffuseContribution=max(0.0,dot(normal,surf2light));
		float specularContribution=pow(max(0.0,dot(reflection,surf2light)),4.0);
		fragColor=vec4(ambientColor*outColor+diffuseContribution *diffuseColor*outColor+ specularContribution*specularColor,1.0);
}