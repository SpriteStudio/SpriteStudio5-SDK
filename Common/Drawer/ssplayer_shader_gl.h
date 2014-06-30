#ifndef __SSOPENGLSHADER__
#define __SSOPENGLSHADER__

#include <string>
#include <vector>

#define PUT_UNIFORM_WARNIG	(1)


class	SSOpenGLShader {
public:
	GLhandleARB	h;

protected:
	std::string	myname;
	std::string	source;

public:
	SSOpenGLShader( const std::string& str, const GLenum shader_type );
	SSOpenGLShader( const std::string& name, const std::string& str, const GLenum shader_type );
	virtual	~SSOpenGLShader();
	virtual	int Compile( void );
	inline GLhandleARB GetHandle( void )	const { return h; }
};


class	SSOpenGLVertexShader : public SSOpenGLShader
{
public:
	SSOpenGLVertexShader( const std::string& str );
	SSOpenGLVertexShader( const std::string& name , const std::string& str );
	virtual	~SSOpenGLVertexShader(){}

};


class	SSOpenGLFragmentShader : public SSOpenGLShader
{
public:
	SSOpenGLFragmentShader( const std::string& filename );
	SSOpenGLFragmentShader( const std::string& name , const std::string& str );
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
    void 	Disable( void );
	GLint GetUniformLocation( const char *name );

	GLint GetAttribLocation( const char *name );
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

