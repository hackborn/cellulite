#version 150

uniform sampler2D uTex0;

in vec4	Color;
in vec3	Normal;
in vec2	TexCoord;

out vec4 			oColor;

void main( void )
{
//	vec3 normal = normalize( -Normal );
//	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0 );
//	oColor = texture( uTex0, TexCoord.st ) * Color * diffuse;
//	oColor = Color;

//	oColor = texture( uTex0, TexCoord.st ) * Color;
	oColor = texture( uTex0, TexCoord.st );
oColor[0] = 1;
oColor[1] = 1;
oColor[2] = 1;

//	oColor = Color;
}