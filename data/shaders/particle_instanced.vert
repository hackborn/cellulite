#version 150

uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

in vec4		ciPosition;
in vec2		ciTexCoord0;
in vec3		ciNormal;
in vec4		ciColor;
//in vec3		vInstancePosition; // per-instance position variable
in vec4		vInstancePosition; // per-instance position variable
out highp vec2	TexCoord;
out lowp vec4	Color;
out highp vec3	Normal;

void main( void )
{
	// The per-instance data consists of x,y,z in the first 3 floats
	// and RGB (8 bits each) in the 4th float. I did it this way partially because
	// of performance -- I have no idea if the time added to decode is worth
	// the memory saved transporting the full 32 bit color components, but I'd
	// guess so -- but mostly because I don't know how to get custom per-instance
	// data of anything larger than a vec4.
	vec3			inst_pos = vec3(vInstancePosition[0], vInstancePosition[1], vInstancePosition[2]);
//	int				e_clr = int(vInstancePosition[3]);
//	vec3			inst_clr = vec3(float((e_clr>>16)&0xff) / 255.0,
//									float((e_clr>>8)&0xff) / 255.0,
//									float(e_clr&0xff) / 255.0);

	gl_Position	= ciModelViewProjection * ( ciPosition + vec4( inst_pos, 0 ) );
//	Color 		= ciColor * vec4(inst_clr, 1);
//	Color 		= vec4(1, 1, 1, 1);
//	Color 		= ciColor;
	Color 		= ciColor * vec4(1, 1, 1, vInstancePosition[3]);
	TexCoord	= ciTexCoord0;
	Normal		= ciNormalMatrix * ciNormal;
}
