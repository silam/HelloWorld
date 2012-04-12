#version 150
//You need the texture coordinates to be passed in from the vertex shader (interpolated)

//You also need your uniform sampler so you can use those coordinates to look up a texel
uniform sampler2D texture;

in vec2 vtexCoord;

out vec4  fColor;

void main()
{
	

	//you need to look up the appropriate color for this fragment based on the texture map and the texture cordinates
	fColor = texture2D(texture, vtexCoord);
	
	

}