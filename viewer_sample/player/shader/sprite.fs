"																			\n\
uniform sampler2DRect textureRect;											\n\
uniform	int		selector;													\n\
																			\n\
void main (void)															\n\
{\n\
	vec4 Pixel;\n\
	Pixel = texture2DRect(textureRect, gl_TexCoord[0].st);					\n\
	float rate = gl_SecondaryColor.a;										\n\
	Pixel.r = Pixel.r / Pixel.a;											\n\
	Pixel.g = Pixel.g / Pixel.a;											\n\
	Pixel.b = Pixel.b / Pixel.a;											\n\
	vec4 PixelBlend = gl_Color;												\n\
	vec4 color[4];															\n\
	color[0] = ( Pixel * (1.0 - rate ) ) + ( PixelBlend * rate);			\n\
	color[1] = ( Pixel * (1.0 - rate )) + ( Pixel * PixelBlend * rate );	\n\
	color[2] = Pixel + PixelBlend * rate;									\n\
	color[3] = Pixel - PixelBlend * rate;									\n\
	int sel = selector;														\n\
	vec4 out_color = color[sel];											\n\
	out_color.w = Pixel.w * gl_Color.a;										\n\
	gl_FragColor = out_color;												\n\
}\n\
";