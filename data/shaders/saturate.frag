#version 150

out vec4 oColor;
uniform sampler2D uTex0;
in vec2	TexCoord;

void main( void )
{
	oColor = vec4( 1 ) * texture( uTex0, TexCoord.st );
	oColor[0] = 0;
	oColor[1] = 0;
	oColor[2] = 0;

	// Saturate the alpha
	float max = 0.33;
	float c = oColor[3] * 4.0;
	if (c > max) {
		c = max;
	}
	oColor[3] = c;
}