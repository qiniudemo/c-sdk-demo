#pragma once
#include "qiniu/http.h"

typedef struct _Qiniu_Io_PutRet_WithExtra {
	const char* key;
	const char* hash;
 	const char* param1;
  	const char* param2;
} Qiniu_Io_PutRet_WithExtra;

typedef struct _Qiniu_Io_PutRet_WithPfop {
	const char* key;
	const char* hash;
	const char *persistentId;
}Qiniu_Io_PutRet_WithPfop;

typedef struct _Qiniu_Io_PutRet_WithCallback {
	int code;
	const char* message;
}Qiniu_Io_PutRet_WithCallback;

void simpleUploadWithoutKey(Qiniu_Mac *mac, const char *bucket, const char *localFile);
void simpleUploadWithKey(Qiniu_Mac *mac, const char *bucket, const char *key, const char *localFile);
void simpleUploadWithPutExtra(Qiniu_Mac *mac, const char *bucket, const char *key, const char *localFile);
void simpleUploadWithPfop(Qiniu_Mac *mac, const char *bucket, const char *key, const char *localFile);
void simpleUploadWithCallback(Qiniu_Mac *mac, const char *bucket, const char *key, const char *localFile);