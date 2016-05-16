#pragma once

#ifndef QINIU_RES_MANAGE
#define QINIU_RES_MANAGE
#include "qiniu/rs.h"

void rsStat(Qiniu_Client* client, const char* bucket, const char* key);
#endif