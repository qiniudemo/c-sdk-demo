#include <stdio.h>
#include "qiniu/http.h"

#ifndef _DEBUG_LOG_H
#define _DEBUG_LOG_H

#ifdef __cplusplus
extern "C"
{
#endif

void debug_log(Qiniu_Client *client, Qiniu_Error err);
int str_empty(const char* str);
//cdn examples
void cdn_create_timestamp_url();
void cdn_get_bandwidth_data();
void cdn_get_flux_data();
void cdn_get_log_list();
void cdn_prefetch_urls();
void cdn_refresh_dirs();
void cdn_refresh_urls();
//fop
void fop_video_avthumb();
//batch
void rs_batch_change_mime();
void rs_batch_change_type();
void rs_batch_copy();
void rs_batch_delete();
void rs_batch_delete_after_days();
void rs_batch_move();
void rs_batch_stat();
void rs_change_mime();
void rs_change_type();
//resource manage
void rs_stat();
void rs_copy();
void rs_delete();
void rs_delete_after_days();
void rs_fetch();
void rs_move();
void rs_prefetch();
//list files
void rsf_list_bucket();
void rsf_list_files();
//upload files
void up_chunk_upload();
void up_form_upload();
void up_create_uptoken();


#endif
#ifdef __cplusplus
}
#endif