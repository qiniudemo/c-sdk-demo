#include "res_manage.h"
#include "qiniu/base.h"
#include "qiniu/rs.h"
#include "qiniu/http.h"

void rsStat(Qiniu_Client* client, const char* bucket, const char* key)
{
	Qiniu_RS_StatRet ret;
	Qiniu_Error err = Qiniu_RS_Stat(client, &ret, bucket, key);
	if (err.code != 200) {
		printf("%d %s\n", err.code, err.message);
		return;
	}
	printf("hash: %s, fsize: %lld, mimeType: %s\n", ret.hash, ret.fsize, ret.mimeType);
}