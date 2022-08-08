/***************************************************************************
 * 
 * 
 **************************************************************************/
 
 
 
/**
 * @file category.h
 * @brief 
 *  
 **/


#ifndef  __CATEGORY_H_
#define  __CATEGORY_H_

#include <stdio.h>
#include <stdarg.h>

#include "comlog.h"
#include "logstat.h"
#include "appender/appender.h"
#include "event.h"

namespace comspace
{

class Category
{
	class Push
	{
		Category * _cate;
	public:
		Push(Category *);
		Push & operator () (const char *key, const char *fmt, ...);
		int operator () (int level, const char *fmt, ...);
	};

	public:
		Event *_event;

	protected:
		Category *_parent;	//���׾��
		LogStat *_logstat;	//��־״̬���ƾ��

		Appender *_appenders[COM_MAXDEVICENUM];
		int _appnum;

	protected:
		Category(Category *parent = NULL);
	public:
		virtual ~Category();

		int setLogStat(LogStat *logstat = NULL);	//�������ÿ��ƾ��
		int print(u_int level, const char *fmt, ...);	//������־�ȼ���ӡ��־

		//���,ɾ�������appender
		int addAppender(Appender *);
		int removeAppender(Appender *);
		int clearAppender();

		int print(u_int level, const char *fmt, va_list args);
		inline LogStat * logstat() {
			return _logstat;
		}
		inline int appender_num() {
			return _appnum;
		}
	public:
		Push push;

	public:
		//static pthread_mutex_t glock;
		static Category * getInstance(const char *name);
		//�Զ�����־�ȼ�
		//������־�ȼ�����־���Ķ�Ӧ��ϵ
		//�Զ�����־id��USERLOGLEVEL(32)��
		//���ݵȼ����õ��ȼ�id������ȼ��������ڣ���ô���Զ�����һ��id����಻�������Զ�����id��
		//static unsigned int getLogID(const char *name);
};
};

#endif  //__CATEGORY_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
