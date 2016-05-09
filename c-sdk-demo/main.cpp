#include "simple_upload.h"
#include "resumable_upload.h"
#include "qiniu/rs.h"
#include "qiniu/http.h"

int main(int argc, char*argv[])
{
	char *bucket = "if-pbl";
	char *imageKey = "2016/05/08/test.jpg";
	char *videoKey = "2016/05/08/test.wmv22";

	//从 https://portal.qiniu.com/user/key 这里获取
	char *accessKey = "<AccessKey>";
	char *secretKey = "<SecretKey>";
	Qiniu_Mac mac;
	mac.accessKey = accessKey;
	mac.secretKey = secretKey;

	char *localImageFile = "C:\\Users\\Public\\Pictures\\Sample Pictures\\Koala.jpg";
	char *localVideoFile = "C:\\Users\\Public\\Videos\\Sample Videos\\Wildlife.wmv";
	 
	//simpleUploadWithoutKey(&mac, bucket, localImageFile);
	//simpleUploadWithKey(&mac,bucket,imageKey,localImageFile);
	//simpleUploadWithPutExtra(&mac,bucket,imageKey,localImageFile);
	//simpleUploadWithPfop(&mac,bucket,imageKey,localImageFile);
	//simpleUploadWithCallback(&mac, bucket, imageKey, localImageFile);
	resumableUploadWithKey(&mac,bucket,videoKey,localVideoFile);
	//wait to exit
	getchar();
	return 0;
}