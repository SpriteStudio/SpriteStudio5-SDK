//#include "stdafx.h"
#include "sshTask.h"
//#include "Debug.h"

task_manager_singleton* task_manager_singleton::g_myinst = 0;
unsigned long	treeitem_uid::m_tree_item_uid	 = 0;


task_manager::task_manager()
	:m_root(0) , m_priority_max(0)
{
	m_root = new task_base();
	m_root->setIdentify( "task_root" );
}

task_manager::~task_manager()
{
	destroy_tasks();
}

void task_manager::destroy_tasks()
{
//	get_root()->destroy();
	delete(m_root);
	m_root = 0;
}


void task_manager::unresist_task( task_base *parent , task_base* unreg_task)
{
	parent->prependtask( unreg_task );
}


void task_manager::resist_task( task_base *parent , task_base* reg_task)
{
	task_base* _parent = parent;
	if ( _parent == 0 )
	{
		_parent = get_root();
	}
	
	if ( _parent )
	{
		reg_task->init();
		_parent->appendchild(reg_task);
	}	
}


void task_manager::draw_resist_tasks_sub(task_base* task)
{

	if ( !task->isdrawpause() )
	{
//		task->exec_drawfunc( priority );
		task->draw();

		if ( task->get_child() )
		{
			draw_resist_tasks_sub( task->get_child()  );
		}
	}
	if ( task->get_sibling() )
	{
		draw_resist_tasks_sub( task->get_sibling()  );
	}
	
}


void task_manager::draw_resist_tasks()
{
	task_base* ptr = get_root();
	
	draw_resist_tasks_sub(ptr);		
}

void task_manager::exec_resist_tasks_sub(task_base* task,double delta_time)
{

	if ( !task->ispause() )
	{
		task->update(delta_time);
		
		if ( task->get_child() )
		{
			exec_resist_tasks_sub( task->get_child(),delta_time );
		}
	}
	if ( task->get_sibling() )
	{
		exec_resist_tasks_sub( task->get_sibling(),delta_time );
	}
}


void task_manager::exec_resist_tasks(double delta_time)
{
	
	task_base* ptr = get_root();
	exec_resist_tasks_sub(ptr,delta_time);
	
}

/*
void	task_manager::debug_print_tasks_sub(task_base* task , int t )
{

	for ( int i = 0 ; i < t ; i++ ) CommonLib::DEBUG_PRINTF( "\t" );
	CommonLib::DEBUG_PRINTF( "%s\n" , task->getIdentify() );

	if ( task->get_child() )
	{
		debug_print_tasks_sub( task->get_child() , t+1);
	}
	if ( task->get_sibling() )
	{
		debug_print_tasks_sub( task->get_sibling() , t );
	}	
}

void	task_manager::debug_print_tasks()
{
	debug_print_tasks_sub( get_root() , 0 );
}

*/


