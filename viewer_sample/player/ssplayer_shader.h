#ifndef __SSOPENGLSHADER__
#define __SSOPENGLSHADER__

#include "../../loader/ssloader.h"
#include <string>


#define PUT_UNIFORM_WARNIG	(1)


class	SSOpenGLShader {
public:
	GLhandleARB	h;

protected:
	SsString	myname;
	std::string	source;

public:
	SSOpenGLShader( const SsString& str, const GLenum shader_type );
	SSOpenGLShader( const SsString& name, const SsString& str, const GLenum shader_type );
	virtual	~SSOpenGLShader();
	virtual	int Compile( void );
	inline GLhandleARB GetHandle( void )	const { return h; }
};


class	SSOpenGLVertexShader : public SSOpenGLShader
{
public:
	SSOpenGLVertexShader( const SsString& str );
	SSOpenGLVertexShader( const SsString& name , const SsString& str );
	virtual	~SSOpenGLVertexShader(){}

};


class	SSOpenGLFragmentShader : public SSOpenGLShader
{
public:
	SSOpenGLFragmentShader( const SsString& filename );
	SSOpenGLFragmentShader( const SsString& name , const SsString& str );
	virtual	~SSOpenGLFragmentShader(){}

};


class	SSOpenGLProgramObject {
public:
	GLhandleARB	h;

protected:

public:
	SSOpenGLProgramObject(  );
	virtual	~SSOpenGLProgramObject(  );

	GLhandleARB GetID(){ return h; }

	virtual	void	Attach( const SSOpenGLShader *s );
	virtual	int		Link( void );
	virtual void	Use( void );

	inline	void 	Enable( void ) { Use(); }
	inline	void 	Disable( void ) { glUseProgramObjectARB( 0 ); }

	inline	GLint GetUniformLocation( const char *name )
	{
		GLint ul = glGetUniformLocationARB( h, name );
		if ( ul == -1 ) {
		}
		return ul;
	}

	inline	GLint GetAttribLocation( const char *name )
	{
		GLint al = glGetAttribLocationARB( h, name );
		if ( al == -1 ) {
		}
		return al;
	}
};

extern 	SSOpenGLProgramObject*			glpgObject;    //カレントシェーダーオブジェクチE

class   SSOpenGLShaderMan
{
private:
    std::vector<SSOpenGLProgramObject*> m_shader_list;
    static	SSOpenGLShaderMan*			m_Myinst;
public:
	SSOpenGLShaderMan(){}
	virtual ~SSOpenGLShaderMan(){}

	static SSOpenGLProgramObject*	SetCurrent(int index)
	{
		glpgObject = m_Myinst->m_shader_list[index];
		return glpgObject;
	}

	static  void	Create()
	{
		if ( m_Myinst == 0)
			m_Myinst = new  SSOpenGLShaderMan();
	}

	static	void	Destory()
	{
		m_Myinst->_Destroy();
	}

	void		_Destroy()
	{
		for ( std::vector<SSOpenGLProgramObject*>::iterator itr = m_shader_list.begin();
				itr != m_shader_list.end() ; itr++ )
		{
			delete (*itr);
		}
       	m_Myinst->m_shader_list.clear();

		if ( m_Myinst != 0)
        {
			delete m_Myinst;
			m_Myinst = 0;
		}
	}

	static void	PushPgObject(SSOpenGLProgramObject *obj)
	{
    	m_Myinst->m_shader_list.push_back(obj);
	}

};



int SsGL_CheckShaderReady( void );
extern	SSOpenGLShaderMan*              glshaderMan;


#endif

