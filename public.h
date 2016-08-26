// public.h ʵ��һЩ�����ķ���
#ifndef PUBLIC_H_H
#define PUBLIC_H_H

// Windowsƽ̨
#ifdef __Windows
#include <process.h>
#include <windows.h>
// Unixƽ̨
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#endif

#include <string>
using namespace std;

/* ���ݲ�ͬƽ̨��ȡ������͸��ݴ����뷵�ض�Ӧƽ̨ϵͳ������Ϣ�ĺ��� */
int get_lastError();
string get_sysErrorMessage(int nErrorCode);

#endif