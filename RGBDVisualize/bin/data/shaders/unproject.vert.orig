<<<<<<< HEAD


uniform vec2 dim; //texture dimensions, width, height
uniform vec2 fudge; //xmult, ymult, texture offsets supplied by user
=======
#version 110

uniform vec2 dim;
uniform vec2 fudge;
>>>>>>> upstream/master

//uniform sampler2DRect depthTexture;
uniform vec2 principalPoint;
uniform vec2 imageSize;
uniform vec2 fov;
uniform float farClip;

varying float VZPositionValid0;
void main(void)
{

<<<<<<< HEAD
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
=======
//    float depthValue = texture2DRect(tex0, gl_TexCoord[0].st);
	VZPositionValid0 = (gl_Vertex.z < farClip && gl_Vertex.z > 200.) ? 1.0 : 0.0;

    vec4 pos = vec4( ((gl_Vertex.x - principalPoint.x) / imageSize.x) * gl_Vertex.z * fov.x,
                     ((gl_Vertex.y - principalPoint.y) / imageSize.y) * gl_Vertex.z * fov.y,
                      gl_Vertex.z, 1.0);
    
    //projective texture on the 
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
    gl_FrontColor = gl_Color;
    
    mat4 tTex = gl_TextureMatrix[0];
    vec4 texCd = tTex * pos;
    texCd.xyz /= texCd.w;
    
    texCd.xy *= -1.;
    texCd.xy += 1.;
    texCd.xy /= 2.;
    texCd.xy += fudge;
>>>>>>> upstream/master
    
    texCd.xy *= dim;
    gl_TexCoord[0] = texCd;
}
