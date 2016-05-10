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



//带扩展参数上传，所谓扩展参数就是除了七牛规定的参数之外的请求参数， 
//必须符合一定的命名规则，而且值不能为空。
//指定mimeType上传，在客户端指定上传文件的类型

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
	Qiniu_Free(upToken);
}


///带持久化数据处理上传，这种上传方式在文件到达七牛存储之后可以立即进行数据处理
///提交数据处理的请求是异步的，你可以解析出服务端返回的persistentId来进行进度查询

//解析持久化参数的方法
Qiniu_Error simpleUploadWithPfopParser(void* callbackRet, Qiniu_Json* root)
{
	Qiniu_Error error;
	Qiniu_Zero(error);

	Qiniu_Io_PutRet_WithPfop *putRet = (Qiniu_Io_PutRet_WithPfop *)callbackRet;
	putRet->key = Qiniu_Json_GetString(root, "key", NULL);
	putRet->hash = Qiniu_Json_GetString(root, "hash", NULL);
	putRet->persistentId = Qiniu_Json_GetString(root, "persistentId", NULL);

	error.code = 200;
	return error;
}

void simpleUploadWithPfop(Qiniu_Mac *mac, const char *bucket, const char *key, const char *localFile)
{
	//实际情况下，从业务服务器获取，通过http请求
	Qiniu_RS_PutPolicy putPolicy = Qiniu_RS_PutPolicy();
	putPolicy.scope = bucket;

	//设置图片处理指令
	char imageViewSaveas[100];
	char imageViewFopWebp[200];
	char imageViewFopPng[200];
	
	sprintf_s(imageViewSaveas,"%s:%s%s",bucket,key,"_80x80.webp");
	char *imageViewSaveasEncoded = Qiniu_String_Encode(imageViewSaveas);
	sprintf_s(imageViewFopWebp, "imageView2/0/w/80/h/80/format/webp|saveas/%s", imageViewSaveasEncoded);
	Qiniu_Free(imageViewSaveasEncoded);

	sprintf_s(imageViewSaveas, "%s:%s%s", bucket, key, "_80x80.png");
	imageViewSaveasEncoded = Qiniu_String_Encode(imageViewSaveas);
	sprintf_s(imageViewFopPng, "imageView2/0/w/80/h/80/format/png|saveas/%s", imageViewSaveasEncoded);
	Qiniu_Free(imageViewSaveasEncoded);

	char persistentFops[400];
	sprintf_s(persistentFops, "%s;%s",imageViewFopWebp,imageViewFopPng);
	
	//set putPolicy
	putPolicy.persistentOps = persistentFops;
	//私有队列参数 pipeline 必须指定以加快处理速度
	//私有队列在这里创建 https://portal.qiniu.com/create/mps
	putPolicy.persistentPipeline = "jemy"; 
	//处理结果主动通知地址
	//通知的POST请求体格式固定的，可以参考 http://developer.qiniu.com/code/v6/api/dora-api/pfop/prefop.html
	putPolicy.persistentNotifyUrl = "http://api.example.com/qiniu/pfop/notify";

	char *upToken = Qiniu_RS_PutPolicy_Token(&putPolicy, mac);
	Qiniu_Error error;
	//这里的putRet其实没有用，解析回复都在putRetExtra中
	//但是因为sdk的逻辑问题，所以不能去掉
	Qiniu_Io_PutRet putRet;
	Qiniu_Io_PutRet_WithPfop putRetExtra;

	Qiniu_Io_PutExtra putExtra;
	Qiniu_Zero(putRet);
	Qiniu_Zero(putExtra);
	Qiniu_Zero(putRetExtra);
	Qiniu_Zero(error);

	putExtra.callbackRet = &putRetExtra;
	putExtra.callbackRetParser = simpleUploadWithPfopParser;

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
		printf("PersistentId: %s\n", putRetExtra.persistentId);
	}

	Qiniu_Client_Cleanup(&client);
	Qiniu_Global_Cleanup();
	Qiniu_Free(upToken);
}

//带回调业务服务器上传，在文件上传到七牛之后，七牛会根据指定的CallbackUrl
//将CallbackBody的内容回调到业务服务器，其中CallbackBody的内容可以包含关于
//该文件的内置变量信息（比如文件保存在空间的名字和hash值），也可以包含客户端
//指定的扩展参数信息，这些信息只要在CallbackBody中按照占位符的方式组织即可，
//七牛会自动将这些信息填充完毕并将结果以POST BODY的方式发送到业务服务器，业务
//服务器在收到回调之后，必须回复一个JSON格式的内容给到七牛，然后这个JSON格式的
//内容会被七牛转发给客户端，从而实现了业务服务器和客户端的信息交互。

//解析回调参数的方法
Qiniu_Error simpleUploadWithCallbackParser(void* callbackRet, Qiniu_Json* root)
{
	Qiniu_Error error;
	Qiniu_Zero(error);

	Qiniu_Io_PutRet_WithCallback *putRet = (Qiniu_Io_PutRet_WithCallback *)callbackRet;
	putRet->code =(int)Qiniu_Json_GetInt64(root,"code",NULL);
	putRet->message = Qiniu_Json_GetString(root, "message", NULL);

	error.code = 200;
	return error;
}

void simpleUploadWithCallback(Qiniu_Mac *mac, const char *bucket, const char *key, const char *localFile)
{
	//实际情况下，从业务服务器获取，通过http请求
	Qiniu_RS_PutPolicy putPolicy = Qiniu_RS_PutPolicy();
	putPolicy.scope = bucket;

	//设置回调地址和回调Body格式
	putPolicy.callbackUrl = "http://api.example.com/qiniu/callback";
	putPolicy.callbackBody = "key=$(key)&hash=$(etag)&param1=$(x:hello)&param2=$(x:qiniu)";

	//生成上传凭证
	char *upToken = Qiniu_RS_PutPolicy_Token(&putPolicy, mac);
	Qiniu_Error error;
	Qiniu_Io_PutRet putRet;
	Qiniu_Io_PutRet_WithCallback putRetCallback;

	Qiniu_Io_PutExtra putExtra;
	Qiniu_Zero(putRet);
	Qiniu_Zero(putExtra);
	Qiniu_Zero(putRetCallback);
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

	putExtra.params = &param1;
	putExtra.callbackRet = &putRetCallback;
	putExtra.callbackRetParser = simpleUploadWithCallbackParser;

	//初始化
	Qiniu_Client client;
	Qiniu_Global_Init(-1);
	Qiniu_Client_InitNoAuth(&client, 1024);
	Qiniu_Zero(putRet);
	Qiniu_Zero(error);
	//上传
	error = Qiniu_Io_PutFile(&client, &putRet, upToken, key, localFile, &putExtra);
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