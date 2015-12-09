//
//  HLScURL.cpp
//  VideoPlayer
//
//  Created by wangyu on 15/11/16.
//  Copyright © 2015年 huangyi. All rights reserved.
//

#include "HLScURL.h"

#include "curl.h"

/*
 * libcurl callback func for http payload data parsing
 */
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    http_payload_t *mem = (http_payload_t *)userp;
    
    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
//        hls_err("not enough memory (realloc returned NULL)\n");
    }
    
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}

/*
 * func for http downloading by libcurl
 */
int curl_http_download(const char *url_live, http_payload_t *http_payload)
{
    CURL *curl_handle;
    http_payload_t *chunk = http_payload;
    
    chunk->memory = (char *)malloc(1);  	/* will be grown as needed by the realloc above */
    chunk->size = 0;    			/* no data at this point */
    
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url_live);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl_handle, CURLOPT_FAILONERROR, 1);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)chunk);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);
    
    CURLcode res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK){
//        hls_err("CURL failed with error code %d", res);
        return res;
    }
    
    curl_easy_cleanup(curl_handle);
    return CURLE_OK;
}