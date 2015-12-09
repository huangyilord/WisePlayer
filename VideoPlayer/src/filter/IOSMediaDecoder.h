//
//  IOSMediaDecoder.h
//  VideoPlayer
//
//  Created by DongRanRan on 15/11/16.
//  Copyright © 2015年 huangyi. All rights reserved.
//

/**
  decoder type
 */
typedef enum MediaType {
    MediaType_Video,
    MediaType_Audio
} MediaType;

class IOSMediaDecoder
{
public:
    void* mediaDecoder(char *url, MediaType mediaType);
};