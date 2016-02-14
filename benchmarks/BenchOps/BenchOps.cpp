#include <Core/Core.h>

using namespace Upp;

Atomic val;

struct Data {
	int64 val[4];
	
	void Zero() { val[0] = val[1] = val[2] = val[3] = 0; }
};

Data src[16], dst[16];

#define N 10000000

CONSOLE_APP_MAIN
{
	{
		RTIMING("Atomic");
		for(int i = 0; i < N; i++) {
			AtomicInc(val);
			AtomicDec(val);
		}
	}
	{
		static StaticMutex mtx;
		RTIMING("StaticMutex");
		for(int i = 0; i < N; i++) {
			Mutex::Lock __(mtx);
		}
	}
	{
		static Mutex mtx;
		RTIMING("Mutex");
		for(int i = 0; i < N; i++) {
			Mutex::Lock __(mtx);
		}
	}
	{
		static SpinLock lock;
		RTIMING("SpinLock");
		for(int i = 0; i < N; i++) {
			SpinLock::Lock __(lock);
		}
	}
	{
		RTIMING("Alloc/Free");
		for(int i = 0; i < N; i++) {
			delete[] new byte[32];
		}
	}
	{
		RTIMING("Alloc/Free large");
		for(int i = 0; i < N; i++) {
			delete[] new byte[3000];
		}
	}
	{
		RTIMING("malloc/free 32");
		for(int i = 0; i < N; i++) {
			free(malloc(32));
		}
	}
	{
		RTIMING("malloc/free 3000");
		for(int i = 0; i < N; i++) {
			free(malloc(3000));
		}
	}
	{
		RTIMING("Zero32");
		for(int i = 0; i < N; i++) {
			dst[i & 15].Zero();
		}
	}
	{
		RTIMING("Copy32");
		for(int i = 0; i < N; i++) {
			dst[i & 15] = src[i & 15];
		}
	}
	int cnt = max((int)Random(10), 32);
	{
		RTIMING("memcpy variable 32");
		for(int i = 0; i < N; i++) {
			memcpy(&dst[i & 15], &src[i & 15], cnt);
		}
	}
	{
		RTIMING("memcpy fixed 32");
		for(int i = 0; i < N; i++) {
			memcpy(&dst[i & 15], &src[i & 15], 32);
		}
	}
	{
		RTIMING("Copy64");
		for(int i = 0; i < N; i++) {
			dst[i & 15] = src[i & 15];
			dst[i & 7] = src[i & 7];
		}
	}
	{
		RTIMING("Copy96");
		for(int i = 0; i < N; i++) {
			dst[i & 15] = src[i & 15];
			dst[i & 7] = src[i & 7];
			dst[i & 3] = src[i & 3];
		}
	}
	{
		RTIMING("Copy128");
		for(int i = 0; i < N; i++) {
			dst[i & 1] = src[i & 1];
			dst[i & 15] = src[i & 15];
			dst[i & 7] = src[i & 7];
			dst[i & 3] = src[i & 3];
		}
	}
}