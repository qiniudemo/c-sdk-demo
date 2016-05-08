#include "qiniu/resumable_io.h"
#include "qiniu/http.h"
#include "qiniu/rs.h"
#include "qiniu/base.h"
#include "resumable_upload.h"
#include "cJSON/cJSON.h"
#include <stdio.h>

//七牛设计了自己的分片上传的机制并集成到sdk中，分片上传是实现断点续传的基础。
//分片上传的基本原理是把一个大文件按照4MB一个块进行切割，然后再把每个块再切割
//成较小的片，比如256KB，512KB，1MB，2MB，4MB。上传的基本单位就是片，当一个
//块里面的片上传完成之后，可以记录下这个块的相关信息，然后当所有块上传完成之后，
//再把所有块的信息上传，合并为一个文件。在这种机制下，我们可以把块的上传进度都
//写入本地临时文件中，如果中途发生文件上传中断，下次可以再从这个中断的地方继续上传。

int getFileSzie(const char *localFile)
{
	Qiniu_FileInfo fi;
	Qiniu_Int64 fsize;
	Qiniu_Error error;
	Qiniu_File *f;

	error = Qiniu_File_Open(&f, localFile);
	if (error.code != 200)
	{
		fsize = 0;
	}
	else
	{
		error = Qiniu_File_Stat(f, &fi);
		if (error.code == 200) {
			fsize = Qiniu_FileInfo_Fsize(fi);
		}
	}

	return fsize;
}

static int BLOCK_SIZE = 4 * 1024 * 1024;
static const int BLOCK_CONTEXT_LENGTH = 196;

char *resumableProgressMarshal(Qiniu_Rio_BlkputRet *putRets, int blockCnt)
{
	char *progress ;
	cJSON *root = cJSON_CreateArray();
	for (int i = 0; i < blockCnt; i++)
	{
		cJSON *item = cJSON_CreateObject();
		Qiniu_Rio_BlkputRet *blk = putRets+i;
		if (blk && blk->ctx)
		{
			cJSON_AddStringToObject(item, "ctx", blk->ctx);
		}
		else
		{
			cJSON_AddStringToObject(item, "ctx", "");
		}

		cJSON_AddItemToArray(root, item);
	}

	progress = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);
	return progress;
}


int resumableUploadNotify(void* recvr, int blkIdx, int blkSize, Qiniu_Rio_BlkputRet* ret)
{
	Qiniu_Rio_PutProgress_Recvr *pRecvr = (Qiniu_Rio_PutProgress_Recvr*)recvr;
	printf("Fsize: %d, BlkIndex: %d, Offset: %d\n",pRecvr->fsize,blkIdx,ret->offset);
	
	if (pRecvr->progressFilePath)
	{
		if (ret->offset % BLOCK_SIZE == 0 || ret->offset == pRecvr->fsize)
		{
			printf("Write block %d progress\n", blkIdx);
			Qiniu_Rio_BlkputRet *blk = pRecvr->blkputRets+blkIdx;
			blk->checksum = ret->checksum;
			blk->crc32 = ret->crc32;
			blk->ctx = strdup(ret->ctx);
			blk->host = strdup(ret->host);
			blk->offset = ret->offset;

			FILE *progressRecordHandle = fopen(pRecvr->progressFilePath, "wb+");
			if (progressRecordHandle)
			{
				char *progress = resumableProgressMarshal(pRecvr->blkputRets, pRecvr->blkCnt);
				fwrite(progress, strlen(progress) + 1, 1, progressRecordHandle);
				fflush(progressRecordHandle);
				fclose(progressRecordHandle);
			}
		}
	}

	return QINIU_RIO_NOTIFY_OK;
}


int resumableUploadNotifyErr(void* recvr, int blkIdx, int blkSize, Qiniu_Error err)
{
	return QINIU_RIO_NOTIFY_OK;
}

void resumableUploadWithKey(Qiniu_Mac *mac, const char *bucket, const char *key, const char *localFile)
{
	//设置分片上传的参数
	Qiniu_Rio_Settings rioSettings;
	Qiniu_Zero(rioSettings);
	//可以设置为 256KB, 512KB, 1MB, 2MB, 4MB
	rioSettings.workers = 1;
	rioSettings.chunkSize = 256 * 1024;
	Qiniu_Rio_SetSettings(&rioSettings);

	//实际情况下，从业务服务器获取，通过http请求
	Qiniu_RS_PutPolicy putPolicy = Qiniu_RS_PutPolicy();
	putPolicy.scope = bucket;
	char *upToken =Qiniu_RS_PutPolicy_Token(&putPolicy, mac);

	Qiniu_Rio_PutProgress_Recvr putProgressRecvr;
	Qiniu_Error error;
	Qiniu_Rio_PutRet putRet;
	Qiniu_Rio_PutExtra putExtra;
	int blockCnt = 0;
	int blockIndex = 0;

	//初始化
	Qiniu_Client client;
	Qiniu_Global_Init(-1);
	Qiniu_Client_InitNoAuth(&client, 1024);
	Qiniu_Zero(putRet);
	Qiniu_Zero(putExtra);
	Qiniu_Zero(putProgressRecvr);
	Qiniu_Zero(error);

	//初始化进度状态
	putProgressRecvr.fsize = getFileSzie(localFile);
	blockCnt = Qiniu_Rio_BlockCount(putProgressRecvr.fsize);
	putProgressRecvr.blkCnt = blockCnt;
	putProgressRecvr.blkputRets = (Qiniu_Rio_BlkputRet*)malloc(sizeof(Qiniu_Rio_BlkputRet)*blockCnt);
	for (int i = 0; i < blockCnt; i++)
	{
		Qiniu_Zero(*(putProgressRecvr.blkputRets+i));
	}

	char progressFileKey[200];
	sprintf(progressFileKey, "%s:%s", bucket, key);
	char *progressFilePath = strdup(Qiniu_String_Encode(progressFileKey));
	putProgressRecvr.progressFilePath = progressFilePath;
	printf("Local progress file is %s\n",putProgressRecvr.progressFilePath);
	//尝试读取本地进度


	//设置上传进度记录
	putExtra.notifyRecvr = &putProgressRecvr;
	putExtra.notify = resumableUploadNotify;
	putExtra.notifyErr = resumableUploadNotifyErr;

	//上传
	error = Qiniu_Rio_PutFile(&client, &putRet, upToken, key, localFile, &putExtra);
	if (error.code != 200)
	{
		printf("%d\n", error.code);
		printf("%s\n", error.message);
	}
	else
	{
		printf("Key: %s\n", putRet.key);
		printf("Hash: %s\n", putRet.hash);
	}

	Qiniu_Client_Cleanup(&client);
	Qiniu_Global_Cleanup();
	Qiniu_Free(upToken);
}

