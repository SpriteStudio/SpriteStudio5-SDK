"\n\
#version 120															\n\
uniform  float	rates[5];												\n\
attribute vec2	vertexID;												\n\
																		\n\
void main(void)															\n\
{																		\n\
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;			\n\
	gl_Position = ftransform();											\n\
																		\n\
	gl_FrontColor = gl_Color;											\n\
	gl_FrontSecondaryColor = gl_Color;									\n\
																		\n\
	int id = int(vertexID.x);											\n\
																		\n\
	gl_FrontSecondaryColor.a = rates[id];								\n\
}\n\
";
