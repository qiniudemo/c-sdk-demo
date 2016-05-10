#pragma once
#include "qiniu/http.h"
#include "qiniu/resumable_io.h"

typedef struct _Qiniu_Rio_PutProgress_Recvr {
	const char *progressFilePath;
	Qiniu_Int64 fsize;
	int blkCnt;
	Qiniu_Rio_BlkputRet *blkputRets;
} Qiniu_Rio_PutProgress_Recvr;

void resumableUploadWithKey(Qiniu_Mac *mac, const char *bucket, const char *key, const char *localFile);