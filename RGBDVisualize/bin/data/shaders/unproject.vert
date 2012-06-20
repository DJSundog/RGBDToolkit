

uniform vec2 dim; //texture dimensions, width, height
uniform vec2 fudge; //xmult, ymult, texture offsets supplied by user

void main(void)
{

	gl_Position = ftransform();
    gl_FrontColor = gl_Color;
    
	mat4 tTex = gl_TextureMatrix[0];
	vec4 texCd = tTex * gl_Vertex;
	texCd.xyz /= texCd.w; 
    
 
	texCd.xy += 1.;
	texCd.xy /= 2.;
	texCd.y = 1. - texCd.y;
    texCd.x = 1. - texCd.x;
	texCd.xy += fudge;
    
	texCd.xy *= dim;
	gl_TexCoord[0] = texCd;
}
