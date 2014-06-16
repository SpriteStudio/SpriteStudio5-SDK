#ifndef __RS_TASK__
#define __RS_TASK__

#include <vector>
#include <list>

class	treeitem_uid
{
private:
	static unsigned long	m_tree_item_uid;
public:
	treeitem_uid(){}
	~treeitem_uid(){}
	static unsigned long	getuid(){ return ++m_tree_item_uid; }
};


template<class __myclass>
class treeitem :  public __myclass
{
private:
	char		m_ident[32+1];
	treeitem*	sibling_prev;	
	treeitem*	sibling_next;
	treeitem*	child;
	treeitem*	parent;

	bool	_child_destroy;
	bool    _my_destroy;
	
	unsigned long m_uid;
	
	int	childnum;
public:
	
	treeitem()
	: sibling_next(0) , sibling_prev(0) , child(0) , childnum(0) , parent(0) ,_child_destroy(true),_my_destroy(true)
	{
		m_ident[0] = 0;
		m_uid = treeitem_uid::getuid();
	}
	
	
	virtual ~treeitem()
	{
		destroy();
	}
	
	const char*	getIdentify()
	{
		return m_ident;
	}


	void	setIdentify( const char* _ident )
	{

		size_t l = strlen(_ident);

		if ( l > 32 ) l = 32;
		for ( size_t i = 0 ; i < l ; i++ ) m_ident[i] = _ident[i];
		m_ident[l+1] = 0;

	}

	//子タスクの自動削除フラグの設定 (true/自動削除　false/手動削除)
	void	set_child_auto_destroy( bool flag )
	{
		_child_destroy = flag;
	}

	void	set_my_auto_destroy( bool flag )
	{
		_my_destroy = flag;
	}

	treeitem* get_parent(){ return parent;}
	treeitem* get_child(){ return child;}
	treeitem* get_sibling() { return sibling_next;}
	int	get_childnum(){ return childnum; }

	void	appendchild( treeitem *item)
	{
		if ( child == 0 )
		{
			child = item;
		}else{
			child->appendsibling(item);
		}
		item->parent = this;
		this->childnum++;
	}
	
	void	appendsibling( treeitem* item)
	{
		if ( sibling_next == 0)
		{
			sibling_next = item;
			item->sibling_prev = this;
		}else{
			sibling_next->appendsibling(item);
		}
	}

	void destroysub( treeitem* item )
	{
		if ( item->_my_destroy == false ) return ; //削除しないGC対策

		if ( item->get_child() )
		{
			destroysub( item->child );
			item->child = 0;
		}
		if ( item->get_sibling() )
		{
			destroysub( item->sibling_next );
			item->sibling_next = 0;
		}
		
		delete item;
	}

	void destroy()
	{
		if ( this->get_parent() )
		{
			get_parent()->prependtask(this);
			this->parent = 0;
		}

		if ( _child_destroy )
		{
			if (child)
			{
				destroysub( child );
				child = 0;
			}
		}
			
		//繋ぎなおし
		if ( sibling_next )
		{
			sibling_next->sibling_prev = sibling_prev;
		}

		if ( sibling_prev )
		{
			sibling_prev->sibling_next = sibling_next;
		}
	}
	
	void	prependsibling_sub( treeitem* root , treeitem* sub )
	{
		if ( root )
		{
			if ( root->sibling_next == sub )
			{
				root->sibling_next = sub->sibling_next;
				if ( sub->sibling_next )
				{
					sub->sibling_next->sibling_prev = root;
				}
			}

			if ( root->child == sub )
			{
				treeitem* temp = root->child;
				if ( temp )
				{
					if ( temp->sibling_next )
					{
						temp->sibling_next->sibling_prev = 0;
						temp->sibling_next->parent = root;
					}
				}
				root->child = root->child->sibling_next;

			}

			if ( root->sibling_next )
			{
				prependsibling_sub( root->sibling_next , sub );
			}

			if ( root->child )
			{
				prependsibling_sub( root->child , sub );
			}

		}

	}

	//ぶら下がっている中から指定の要素を外す
	void	prependtask( treeitem* item)
	{
		prependsibling_sub( this , item );
		
	}


	
};




//コントロール管理
class task_imp{
	bool	m_pause;
	bool	m_drawpause;
public:	
	task_imp():
		m_pause(false),m_drawpause(false)
	{}		
	~task_imp(){}
	
	void	setpause( bool sw ){ m_pause = sw; }
	bool	ispause(){return m_pause;}
	void	setdrawpause( bool sw ){ m_drawpause = sw;}
	bool	isdrawpause(){ return m_drawpause;}


	virtual void	draw(){};
//	virtual void	update(){};
	virtual void	update(double delta){};
	virtual void	init(){};

};


typedef treeitem<task_imp> task_base;


//タスクマネージャ

class task_manager{
private:
	
	int			m_priority_max;
	task_base*  m_root;

	void exec_resist_tasks_sub(task_base* task ,double delta_time);	
	void draw_resist_tasks_sub(task_base* task );
	
public:
	task_manager();
	~task_manager();
	

	void resist_task( task_base *parent , task_base* reg_task);
	void unresist_task( task_base *parent , task_base* unreg_task);

	void exec_resist_tasks(double delta_time);
	void draw_resist_tasks();
	task_base* get_root(){ return m_root;} 	
	void destroy_tasks();


};

class task_manager_singleton : public task_manager{
private:
	static task_manager_singleton* g_myinst;
public:
	task_manager_singleton()
	{
		g_myinst = this;
	}
	~task_manager_singleton()
	{
		g_myinst = 0;
	}
	
	static task_manager_singleton* getinst(){ return g_myinst;}
};

inline task_manager_singleton* get_rstaskmanager()
{
	return task_manager_singleton::getinst();
}


inline void task_unresist_task( task_base *parent , task_base* reg_task)
{
	get_rstaskmanager()->unresist_task( parent ,  reg_task);
}
inline task_base* task_getroot()
{
	return get_rstaskmanager()->get_root();
}

inline void task_resist_task( task_base *parent , task_base* reg_task)
{
	if ( parent ){
		get_rstaskmanager()->resist_task( parent ,  reg_task);
	}else{
		get_rstaskmanager()->resist_task( task_getroot() ,  reg_task);
	}
}

inline void task_manager_exec(double delta_time)
{
	get_rstaskmanager()->exec_resist_tasks(delta_time);	
}

inline void task_manager_draw()
{
	get_rstaskmanager()->draw_resist_tasks();
}

inline	void task_manager_destroy()
{
	if ( task_manager_singleton::getinst() )
	{
		delete task_manager_singleton::getinst();
	}
}


#endif
