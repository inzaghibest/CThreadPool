// public.h 实现一些公共的方法
#ifndef PUBLIC_H_H
#define PUBLIC_H_H

// Windows平台
#ifdef __Windows
#include <process.h>
#include <windows.h>
// Unix平台
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

/* 根据不同平台获取错误码和根据错误码返回对应平台系统错误信息的函数 */
int get_lastError();
string get_sysErrorMessage(int nErrorCode);

#endif