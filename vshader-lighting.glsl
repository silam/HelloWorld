#version 150

in vec4 vPosition; 
in vec2 texCoord;

//you'll need to pass the texture coordinates over to the fragment shader, so you'll need an out

uniform mat4 model_view;
uniform mat4 projection;

out vec2 vtexCoord;


void
main()
{	
	
	
	//don't forget to pass your texture coordinate through!
	vtexCoord = texCoord;

	gl_Position = projection * model_view*vPosition;
	

}