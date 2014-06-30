#ifdef _WIN32
#include <GL/glew.h>
#else
//#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#endif
#include "ssplayer_shader_gl.h"

#include	<fstream>
#include	<iostream>
#include	<sstream>



SSOpenGLProgramObject*			glpgObject = 0 ;
SSOpenGLShaderMan*              SSOpenGLShaderMan::m_Myinst = 0;



using namespace std;


int SsGL_CheckShaderReady( void )
{
#ifdef _WIN32
  if ( !GLEW_ARB_texture_rectangle ) return -1;
    if (!( GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader )) return -1;
#endif
	const char* ptr = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
//    char* result;
	if (ptr != NULL)
	{
		if ( atof(ptr) < 1.2 )
		{
//			SsLogInfo( _D( "GLSL ready ... version %s NG." ), ptr );
			return -1;
		}else{
//			SsLogInfo( _D( "GLSL ready ... version %s OK." ), ptr );
		}
	}
	return 0;


  return -1;
}

SSOpenGLShader::SSOpenGLShader( const std::string&  name , const std::string& str, const GLenum shader_type )
	: myname( name ) , source( str )
{
	h = glCreateShaderObjectARB( shader_type );

	const char	*s = str.c_str();
	int l = str.length();

	glShaderSourceARB( h, 1, &s, &l );
	if ( glGetError() != GL_NO_ERROR ) {
//		SsLogInfo( _D("ShaderObject::ShaderObject(): cannot set shader source: " ) , myname );
	}

	// compile

	Compile();


}

SSOpenGLShader::SSOpenGLShader( const std::string& filename, const GLenum shader_type )
	: myname( filename )
{
	// create

	h = glCreateShaderObjectARB( shader_type );
	if ( glGetError() != GL_NO_ERROR ) {
//		SsLogInfo( _D( "SSOpenGLShader : cannot create shader object: ") , myname );
	}

	static std::string fname;

	ifstream	f_in( 
		filename.c_str() , ios::binary );

	if ( f_in.fail()) {
//		SsLogInfo( _D("ShaderObject::ShaderObject(): cannot open file: ") , filename );
	}
	ostringstream	str_out;
	str_out << f_in.rdbuf();
	source = str_out.str();
	f_in.close();

	// set shader source
	const char	*s = source.c_str();
	int l = source.length();

	glShaderSourceARB( h, 1, &s, &l );
	if ( glGetError() != GL_NO_ERROR ) {
//		SsLogInfo( _D("ShaderObject::ShaderObject(): cannot set shader source: " ) , myname );
	}

	// compile

	Compile();
}






SSOpenGLShader::~SSOpenGLShader()
{
	glDeleteObjectARB( h );
}

int SSOpenGLShader::Compile( void )
{
	// check whether error occurred

	if ( glGetError() != GL_NO_ERROR ) {
//		SsLogInfo( _D("GLSL Compile : errors " ) );
		return -1;
	}

	// compile
	glCompileShaderARB( h );

	// get errors

	GLint	result;
	glGetObjectParameterivARB( h, GL_OBJECT_COMPILE_STATUS_ARB, &result );

	if ( glGetError() != GL_NO_ERROR || result == GL_FALSE ) {
//		SsLogInfo( _D("GLSL Compile: cannot compile shader: %s" ), myname );

		int	length;
		glGetObjectParameterivARB( h, GL_OBJECT_INFO_LOG_LENGTH_ARB,
								   &length );
		if ( length > 0 ) {
			int	l;
			GLcharARB *info_log = new GLcharARB[ length ];
			glGetInfoLogARB( h, length, &l, info_log );
//			SsLogDbg( info_log );
			delete [] info_log;
		}
		return -1;
	}

	return 0;
}



//------------------------------------------------------------------
SSOpenGLVertexShader::SSOpenGLVertexShader( const std::string& filename )
	: SSOpenGLShader( filename, GL_VERTEX_SHADER_ARB )
{
}

SSOpenGLVertexShader::SSOpenGLVertexShader( const std::string& name , const std::string& str )
	: SSOpenGLShader( name , str, GL_VERTEX_SHADER_ARB )
{

}



SSOpenGLFragmentShader::SSOpenGLFragmentShader( const std::string& filename )
	: SSOpenGLShader( filename, GL_FRAGMENT_SHADER_ARB )
{
}
SSOpenGLFragmentShader::SSOpenGLFragmentShader( const std::string& name , const std::string& str )
	: SSOpenGLShader( name , str, GL_FRAGMENT_SHADER_ARB )
{

}


//------------------------------------------------------------------

SSOpenGLProgramObject::SSOpenGLProgramObject()
{
	//glpgObject = this;
	h = glCreateProgramObjectARB();
	if ( glGetError() != GL_NO_ERROR ) {
		//SsLogInfo( _D("SSOpenGLProgramObject: cannot create .. glCreateProgramObjectARB ") );
	}
}

SSOpenGLProgramObject::~SSOpenGLProgramObject()
{
	glDeleteObjectARB( h );
}

void SSOpenGLProgramObject::Attach( const SSOpenGLShader *s )
{
	glAttachObjectARB( h, s->GetHandle());
	if ( glGetError() != GL_NO_ERROR ) {
		//SsLogInfo( _D("SSOpenGLProgramObject: Attach error cannnot attach shader") );
	}
}



void 	SSOpenGLProgramObject::Disable( void ) { glUseProgramObjectARB( 0 ); }
GLint   SSOpenGLProgramObject::GetUniformLocation( const char *name )
{
    GLint ul = glGetUniformLocationARB( h, name );
    if ( ul == -1 ) {
    }
    return ul;
}
GLint SSOpenGLProgramObject::GetAttribLocation( const char *name )
{
    GLint al = glGetAttribLocationARB( h, name );
    if ( al == -1 ) {
    }
    return al;
}




int
SSOpenGLProgramObject::Link( void )
{
	// check whether error occurred

	if ( glGetError() != GL_NO_ERROR ) {
		//SsLogInfo( _D("SSOpenGLProgramObject: Link error") );
		return -1;
	}

	// link

	glLinkProgramARB( h );

	// get errors

	GLint	result;
	glGetObjectParameterivARB( h, GL_OBJECT_LINK_STATUS_ARB, &result );

	if ( glGetError() != GL_NO_ERROR || result == GL_FALSE ) {
		//SsLogInfo( _D("SSOpenGLProgramObject: cannot link shader object\n") );

		int	length;
		glGetObjectParameterivARB( h, GL_OBJECT_INFO_LOG_LENGTH_ARB,
								   &length );
		if ( length > 0 ) {
			int	l;
			GLcharARB *info_log = new GLcharARB[ length ];
			glGetInfoLogARB( h, length, &l, info_log );
			//SsLogDbg( info_log );
			delete [] info_log;
		}
		return -1;
	}

	return 0;
}

void
SSOpenGLProgramObject::Use( void )
{
	glUseProgramObjectARB( h );
	if ( glGetError() != GL_NO_ERROR ) {
#if PUT_UNIFORM_WARNIG
//		SsLogInfo( _D("SSOpenGLProgramObject: glUseProgramObjectARB cannot use object\n") );
#endif
	}
}



