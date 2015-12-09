//
//  HLSDownloadUrl.cpp
//  VideoPlayer
//
//  Created by wangyu on 15/11/16.
//  Copyright © 2015年 huangyi. All rights reserved.
//

#include "HLSDownloadUrl.h"

int hls_readline(http_payload_t *payload, char* buf)
{
    unsigned int s = 0;
    if (payload->size <= 0) {
        return 0;
    }
    while (s < payload->size) {
        buf[s] = payload->memory[0];
        payload->memory++;
        s++;
        if (buf[s-1] == '\r' || buf[s-1] == '\n')
            break;
    }
    payload->size -= s;
    if (payload->memory[0] == '\r' || payload->memory[0] == '\n') {
        payload->size--;
        payload->memory++;
    }
    buf[s-1] = '\0';
    return s;
}

void hls_string_split(char* s, int slen, char c, char *f, int *flen, char* b, int *blen)
{
    int i = 0,j;
    *flen = 0;
    *blen = 0;
    while (i < slen) {
        f[i] = s[i];
        if (s[i] == c) {
            break;
        }
        (*flen)++;
        i++;
    }
    f[i] = '\0';
    j = ++i;
    while (i < slen) {
        if (s[i] == ' ') {
            i++;
            continue;
        }
        b[i-j] = s[i];
        i++;
        (*blen)++;
    }
    b[i-j] = '\0';
}

void hls_get_base_url(char* ori_url, int ori_len,char* url)
{
    int i = ori_len;
    while ( i >= 0) {
        if (ori_url[i] == '/') {
            break;
        }
        i--;
    }
    memcpy(url, ori_url, sizeof(char)*i);
}

void HLSDownloadUrl::hls_newHLSContext(HLSContext* c)
{
    c->cur_variants = 0;
    c->n_variants = 0;
    c->variants = new struct variant* [MAX_VARIANT_NUM];
    for (int i = 0; i < MAX_VARIANT_NUM; i++) {
        c->variants[i] = new struct variant();
    }
    
    memory_pool_initialize( &pool, 4096, 4 );
    hash_set_initialize(&urlset, sizeof(char)*20, &pool);
}

static void hls_new_variant(HLSContext* c)
{
    c->variants[c->n_variants]->md = 0;
    c->cur_variants = c->n_variants;
    c->n_variants++;
}

int HLSDownloadUrl::hls_add_segment(char* url)
{
    char tmp[20];
    memset(tmp, 0, 20);
    memcpy(tmp, url, 20);
    if (hash_set_exist(&urlset, tmp))
    {
        return 1;
    }
    if (hash_set_insert(&urlset, tmp)) {
        return 0;
    } else {
        return 1;
    }
}

enum HLS_EVENT{
    hls_stream_inf,
    hls_inf,
};

int HLSDownloadUrl::parse_playlist(HLSContext *c, http_payload_t *payload)
{
    char linebuf[MAX_URL_SIZE];
    char fs[MAX_URL_SIZE];
    char bs[MAX_URL_SIZE];
    int linelen;
    int flen,blen;
    int hlstype = -1;
    float maxDuration = 0;
    
    linelen = hls_readline(payload,linebuf);
    if (strncmp(linebuf, "#EXTM3U",linelen-1)) {
//        hls_err("not m3u8 file");
        return -1;
    }
    while ((linelen = hls_readline(payload, linebuf))) {
        hls_string_split(linebuf, linelen, ':', fs, &flen, bs, &blen);
        if (strncmp(fs, "#EXT-X-VERSION",flen) == 0) {
            c->version = atoi(bs);
//            hls_dbg("hls version:%d\n",c->version);
        } else if (strncmp(fs, "#EXT-X-ALLOW-CACHE",flen) == 0) {
            if (strncmp("YES", bs, 3) == 0) {
                c->allcache = 1;
            } else if (strncmp("NO", bs, 2) == 0) {
                c->allcache = 0;
            }
        } else if (strncmp(fs, "#EXT-X-STREAM-INF",flen) == 0) {
            hls_new_variant(c);
            hlstype = hls_stream_inf;
        } else if (strncmp(fs, "#EXT-X-TARGETDURATION",flen) == 0) {
            c->variants[c->cur_variants]->md = atof(bs);
        } else if (strncmp(fs, "#EXT-X-MEDIA-SEQUENCE",flen) == 0) {
            int cur_sequence = atoi(bs);
            if (c->variants[c->cur_variants]->lastSequeue > cur_sequence)
            {
//                hls_err("new sequence %d is less than cur %d\n",cur_sequence,c->variants[c->cur_variants]->lastSequeue);
            }
        } else if (strncmp(fs, "#EXTINF",flen) == 0) {
            hls_string_split(bs, blen, ',', fs, &flen, bs, &blen);
            maxDuration = atof(fs);
            hlstype = hls_inf;
        } else {
            if (hlstype == hls_stream_inf) {
                memcpy(c->variants[c->cur_variants]->request_url, linebuf, sizeof(char)*linelen);
                hls_get_base_url(linebuf,linelen,c->variants[c->cur_variants]->base_url);
            } else if (hlstype == hls_inf) {
                if (!hls_add_segment(linebuf))
                {
                    hlsurldata *urldata = (hlsurldata*)GetOutputBuffer( HLS_URL_PACKAGE, sizeof(hlsurldata) );
                    if ( NULL != urldata) {
                        urldata->duration = maxDuration;
                        memcpy(fs, c->variants[c->cur_variants]->base_url, sizeof(char)*MAX_URL_SIZE);
                        memcpy(urldata->url, strcat(strcat(fs, "/"), linebuf), sizeof(char)*MAX_URL_SIZE);
//                        hls_dbg("add segment %s\n", urldata->url);
                        CommitOutputBuffer(HLS_URL_PACKAGE);
                    }
                }
                maxDuration = 0;
            }
        }
    }
    return 0;
}

HLSDownloadUrl::HLSDownloadUrl( PlayerContext* context ) : PlayerDataSource( context )
{

}

HLSDownloadUrl::~HLSDownloadUrl()
{
    
}

bool HLSDownloadUrl::Initialize()
{
    if ( !PlayerProcedure::Initialize() )
    {
        return false;
    }
    return true;
}

void HLSDownloadUrl::Destroy()
{
    PlayerProcedure::Destroy();
    for (int i = 0; i < MAX_VARIANT_NUM; i++) {
        delete mainContext.variants[i];
    }
    delete mainContext.variants;
    hash_set_destroy(&urlset);
    memory_pool_destroy(&pool);
}

bool HLSDownloadUrl::SetURL( const char* url )
{
//    const char* testurl = "http://radiolive.8686c.com/tjradio/XIANGSHENG/playlist.m3u8?wsSecret=44287bc3ece8c559a96ed588e7dc89f2&wsTime=564a9482";
    hls_newHLSContext(&mainContext);
    http_payload_t *payload = new http_payload_t();
    if (curl_http_download(url, payload) == 0)
    {
        parse_playlist(&mainContext,payload);
        return true;
    }
    delete payload;
    return false;
}
bool HLSDownloadUrl::Seek( int64 position )
{
    return true;
}

void HLSDownloadUrl::OnStart()
{

}
void HLSDownloadUrl::OnStop()
{
    
}
bool HLSDownloadUrl::GetData()
{
    http_payload_t *payload = new http_payload_t();
    if (mainContext.variants) {
        if (curl_http_download(mainContext.variants[mainContext.cur_variants]->request_url, payload) == 0)
        {
            parse_playlist(&mainContext,payload);
        }
    }
    delete payload;
    return true;
}
