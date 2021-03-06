#include "simple_upload.h"
#include "up_resume_upload.h"
#include "qiniu/http.h"
#include "qiniu/cdn.h"
#include <tchar.h>
#include <stdlib.h>
#include "debug.h"

#pragma comment(lib,"qiniu.lib")
void upload_files(Qiniu_Mac *mac)
{
	char *bucket = "audio-message";
	char *imageKey = "2017/04/14/test.jpg";

	//注意中文文件名的写法
	//wchar_t *videoKey = L"2016/05/08/测试.wmv";
	//covert to utf8
	//char videoKeyUtf8[256];
	//WideCharToMultiByte(CP_UTF8, 0, videoKey, -1, videoKeyUtf8, 256, NULL, NULL);
	char *localImageFile = "C:\\Users\\Public\\Pictures\\Sample Pictures\\Koala.jpg";
	//char *localImageFile = "e:\\Documents\\Pictures\\shiyitu.png";
	//注意中文路径名的写法
	//char *localVideoFile = (char*)L"C://Users//Public//Videos//Sample Videos//测试.wmv";
	//localVideoFile = (char*)L"E:\\测试.txt";
	//localVideoFile = argv[1];
	//printf("Upload %s\n",localVideoFile);
	//simpleUploadWithoutKey(&mac, bucket, localImageFile);
	simpleUploadWithKey(mac,bucket,imageKey,localImageFile);
	//simpleUploadWithPutExtra(&mac,bucket,imageKey,localImageFile);
	//simpleUploadWithPfop(&mac,bucket,imageKey,localImageFile);
	//simpleUploadWithCallback(&mac, bucket, imageKey, localImageFile);

	//resumableUploadWithKey(mac, bucket, videoKeyUtf8, localVideoFile);
}

void resource_manage(Qiniu_Mac *mac)
{
	char *bucket = "temp";
	wchar_t *key = L"中文名字.jpg";

	// change to utf8
	char keyUtf8[256];
	WideCharToMultiByte(CP_UTF8, 0, key, -1, keyUtf8, 256, NULL, NULL);

	Qiniu_Client client;

	Qiniu_Global_Init(-1);
	Qiniu_Client_InitMacAuth(&client, 1024, mac);

	Qiniu_Client_Cleanup(&client);
	Qiniu_Global_Cleanup();
}

int _tmain(int argc, _TCHAR*argv[])
{

	//从 https://portal.qiniu.com/user/key 这里获取
	//char *accessKey = "<AccessKey>";
	//char *secretKey = "<SecretKey>";

	char *accessKey = "anEC5u_72gw1kZPSy3Dsq1lo_DPXyvuPDaj4ePkN";
	char *secretKey = "Ybuk4unN4XL267Jsr8LRFPiaJzNOGLxqZSc8heL3";
	char *bucket = "csdk";
	_putenv_s("QINIU_ACCESS_KEY", accessKey);
	_putenv_s("QINIU_SECRET_KEY", secretKey);
	_putenv_s("QINIU_TEST_BUCKET", bucket);

	//set zone
	Qiniu_Use_Zone_Huadong(Qiniu_True);

	//examples 
	//rs_stat();
	//rs_copy();
	//rs_delete();
	//rs_move();
	//rs_change_mime();
	//rs_change_type();
	//rs_delete_after_days();
	//rs_prefetch();
	//rs_fetch();

	//rsf_list_files();
	//rsf_list_bucket();

	//batch examples
	//rs_batch_stat();
	//rs_batch_copy();
	//rs_batch_delete();
	//rs_batch_move();
	//rs_batch_change_mime();
	//rs_batch_change_type();
	//rs_batch_delete_after_days();

	//fop example
	//fop_video_avthumb();
	
	//upload examples
	//up_create_uptoken();
	//up_form_upload();
	//up_chunk_upload();
	
	//cdn examples
	cdn_create_timestamp_url();
	cdn_get_bandwidth_data();
	cdn_get_flux_data();
	cdn_get_log_list();
	cdn_prefetch_urls();
	cdn_refresh_dirs();
	cdn_refresh_urls();


	getchar();
	return 0;
}
