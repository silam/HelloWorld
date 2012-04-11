#version 150

in vec4 vPosition;
in vec4 vAmbientDiffuseColor;
in vec3 vNormal; //must be in position 3 for teapot
in vec4 vSpecularColor;
in float vSpecularExponent;
uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 ambient_light;
uniform vec4 light_position;
uniform vec4 light_color;
out vec4 color;

void
main()
{
	vec4 veyepos = model_view*vPosition;
	vec4 normal = vec4(vNormal, 0);
	vec3 L = normalize( light_position.xyz - veyepos.xyz);
	vec3 N = normalize((model_view * normal).xyz);
	vec3 V = normalize(-veyepos.xyz);
	vec3 H = normalize(L+V);
	
    gl_Position = projection*veyepos;
	vec4 ambient = ambient_light*vAmbientDiffuseColor;
	
	vec4 diffuse = light_color * vAmbientDiffuseColor * max(0.0,dot(L, N));
	
	vec4 specular = light_color * vSpecularColor * pow(max(0.0, dot(N, H)), vSpecularExponent);

	if(dot( L, N) < 0){
		specular = vec4(0, 0, 0, 1);
	}

	color = ambient + diffuse + specular;
}
