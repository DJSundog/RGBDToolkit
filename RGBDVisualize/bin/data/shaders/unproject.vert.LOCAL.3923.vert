

uniform vec2 dim; //texture dimensions, width, height
uniform vec2 fudge; //xmult, ymult, texture offsets supplied by user

void main(void)
{

	gl_Position = ftransform();
    gl_FrontColor = gl_Color;
    
	mat4 tTex = gl_TextureMatrix[0]; //this is the identity matrix by default
                                     //do you change this anywhere?
	vec4 texCd = tTex * gl_Vertex;
	texCd.xyz /= texCd.w; //the vector should be normalized here, 
                          //but where did you get the valid .w
                          //coordinate from?
    
    //this is self explanitory math we get and can reproduce
	texCd.xy += 1.;
	texCd.xy /= 2.;
	texCd.y = 1. - texCd.y;
    texCd.x = 1. - texCd.x;
	texCd.xy += fudge;
    
	texCd.xy *= dim;
	gl_TexCoord[0] = texCd;
}
