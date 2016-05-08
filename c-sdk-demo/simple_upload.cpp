#include "qiniu/io.h"
#include "qiniu/http.h"
#include "qiniu/rs.h"
#include "qiniu/base.h"
#include "simple_upload.h"
#include <stdio.h>
/**
七牛c语言的sdk分为服务器场景和客户端场景。
对于客户端的场景来说，需要做的是从业务服务器获取上传凭证，然后上传文件。
上传的方式有两种，一种是表单上传，另一种是分片上传。需要注意，客户端
不能生成上传的凭证，因为生成上传凭证需要AccessKey和SecretKey，而这对
密钥是不可以存储在客户端的。
本例子使用表单上传的方式来上传文件
*/

//简单文件上传，上传凭证从业务服务器获取
void simpleUploadWithoutKey(Qiniu_Mac *mac,const char *bucket, const char *localFile)
{
	//实际情况下，从业务服务器获取，通过http请求
	Qiniu_RS_PutPolicy putPolicy = Qiniu_RS_PutPolicy();
	putPolicy.scope = bucket;
	char *upToken = Qiniu_RS_PutPolicy_Token(&putPolicy, mac);
	Qiniu_Error error;
	Qiniu_Io_PutRet putRet;

	//初始化
	Qiniu_Client client;
	Qiniu_Global_Init(-1);
	Qiniu_Client_InitNoAuth(&client, 1024);
	Qiniu_Zero(putRet);
	Qiniu_Zero(error);
	//上传
	error = Qiniu_Io_PutFile(&client, &putRet, upToken, NULL, localFile, NULL);
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

//指定上传文件保存在七牛空间中的名字
void simpleUploadWithKey(Qiniu_Mac *mac, const char *bucket,const char *key, const char *localFile)
{
	//实际情况下，从业务服务器获取，通过http请求
	Qiniu_RS_PutPolicy putPolicy = Qiniu_RS_PutPolicy();
	putPolicy.scope = bucket;
	char *upToken = Qiniu_RS_PutPolicy_Token(&putPolicy, mac);
	Qiniu_Error error;
	Qiniu_Io_PutRet putRet;

	//初始化
	Qiniu_Client client;
	Qiniu_Global_Init(-1);
	Qiniu_Client_InitNoAuth(&client, 1024);
	Qiniu_Zero(putRet);
	Qiniu_Zero(error);
	//上传
	error = Qiniu_Io_PutFile(&client, &putRet, upToken, key, localFile, NULL);
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


//解析扩展参数的方法
Qiniu_Error simpleUploadWithPutExtraParser(void* callbackRet, Qiniu_Json* root)
{
	Qiniu_Error error;
	Qiniu_Zero(error);

	Qiniu_Io_PutRet_WithExtra *putRet = (Qiniu_Io_PutRet_WithExtra *)callbackRet;
	putRet->key = Qiniu_Json_GetString(root, "key", NULL);
	putRet->hash = Qiniu_Json_GetString(root, "hash", NULL);
	putRet->param1 = Qiniu_Json_GetString(root, "x:hello", NULL);
	putRet->param2 = Qiniu_Json_GetString(root, "x:qiniu", NULL);

	error.code = 200;
	return error;
}

//带扩展参数上传，所谓扩展参数就是除了七牛规定的参数之外的请求参数， 
//必须符合一定的命名规则，而且值不能为空。

//指定mimeType上传，在客户端指定上传文件的类型

//带crc32校验上传，启用文件上传的内容crc32校验
void simpleUploadWithPutExtra(Qiniu_Mac *mac, const char *bucket, const char *key, const char *localFile)
{
	//实际情况下，从业务服务器获取，通过http请求
	Qiniu_RS_PutPolicy putPolicy = Qiniu_RS_PutPolicy();
	putPolicy.scope = bucket;
	char *upToken = Qiniu_RS_PutPolicy_Token(&putPolicy, mac);
	Qiniu_Error error;
	//这里的putRet其实没有用，解析回复都在putRetExtra中
	//但是因为sdk的逻辑问题，所以不能去掉
	Qiniu_Io_PutRet putRet;
	Qiniu_Io_PutRet_WithExtra putRetExtra;

	Qiniu_Io_PutExtra putExtra;
	Qiniu_Zero(putRet);
	Qiniu_Zero(putExtra);
	Qiniu_Zero(putRetExtra);
	Qiniu_Zero(error);

	//扩展参数
	Qiniu_Io_PutExtraParam param1;
	param1.key = "x:hello";
	param1.value = "hello world";
	Qiniu_Io_PutExtraParam param2;
	param2.key = "x:qiniu";
	param2.value = "qiniu storage";
	//将扩展参数连接起来
	param1.next = &param2;
	param2.next = NULL;
	//mimeType
	char *mimeType = "image/jpg";

	putExtra.params = &param1;
	putExtra.mimeType = mimeType;
	putExtra.callbackRet = &putRetExtra;
	putExtra.callbackRetParser = simpleUploadWithPutExtraParser;

	//初始化
	Qiniu_Client client;
	Qiniu_Global_Init(-1);
	Qiniu_Client_InitNoAuth(&client, 1024);
	//上传
	error = Qiniu_Io_PutFile(&client, &putRet, upToken, key, localFile, &putExtra);
	if (error.code != 200)
	{
		printf("%d\n", error.code);
		printf("%s\n", error.message);
	}
	else
	{
		printf("Key: %s\n", putRetExtra.key);
		printf("Hash: %s\n", putRetExtra.hash);
		printf("Extra Param1: %s\n", putRetExtra.param1);
		printf("Extra Param2: %s\n",putRetExtra.param2);
	}

	Qiniu_Client_Cleanup(&client);
	Qiniu_Global_Cleanup();
}

