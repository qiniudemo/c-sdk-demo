#include "simple_upload.h"
#include "qiniu/rs.h"
#include "qiniu/http.h"

int main(int argc, char*argv[])
{
	int exit;

	char *bucket = "if-pbl";
	char *imgKey = "2016/05/08/test.jpg";
	//从 https://portal.qiniu.com/user/key 这里获取
	char *accessKey = "<Access Key>";
	char *secretKey = "<Secret Key>";
	Qiniu_Mac mac;
	mac.accessKey = accessKey;
	mac.secretKey = secretKey;

	char *localFile = "C:\\Users\\xin\\Pictures\\doit.jpg";
	 
	//simpleUploadWithoutKey(&mac, bucket, localFile);
	//simpleUploadWithKey(&mac,bucket,imgKey,localFile);
	simpleUploadWithPutExtra(&mac,bucket,imgKey,localFile);
	//wait to exit
	scanf("%c",&exit);
	return 0;
}