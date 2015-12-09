//
//  HLScURL.hpp
//  VideoPlayer
//
//  Created by wangyu on 15/11/16.
//  Copyright © 2015年 huangyi. All rights reserved.
//

#ifndef HLScURL_hpp
#define HLScURL_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct http_payload {
    char *memory;
    uint32_t size;
} http_payload_t;

int curl_http_download(const char *url_live, http_payload_t *http_payload);

#endif /* HLScURL_hpp */
