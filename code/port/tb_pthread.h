#ifndef TB_PTHREAD_H
#define TB_PTHREAD_H

#ifdef _MSC_VER
#include <windows.h>
//#include <stdafx.h>
#define MUTEX_T            HANDLE
#define MUTEX_INIT( a )    a = CreateMutex( NULL, FALSE, NULL )
#define MUTEX_DESTROY( a ) CloseHandle( a )
#define MUTEX_LOCK( a )    WaitForSingleObject( a, INFINITE )
#define MUTEX_UNLOCK( a )  ReleaseMutex( a )
#else
#include <pthread.h>
#define MUTEX_T            pthread_mutex_t
#define MUTEX_INIT( a )    pthread_mutex_init( &a, NULL ) 
#define MUTEX_DESTROY( a ) pthread_mutex_destroy( &a )
#define MUTEX_LOCK( a )    pthread_mutex_lock( &a )
#define MUTEX_UNLOCK( a )  pthread_mutex_unlock( &a )
#endif // _MSC_VER

#endif 
