/*****************************************************************************
** $Source: /cygdrive/d/Private/_SVNROOT/bluemsx/blueMSX/Src/Win32/Win32VideoIn.c,v $
**
** $Revision: 1.6 $
**
** $Date: 2008-03-30 18:38:48 $
**
** More info: http://www.bluemsx.com
**
** Copyright (C) 2003-2004 Daniel Vik
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
******************************************************************************
*/
#include "Win32VideoIn.h"
#include "ArchVideoIn.h"
#include "Language.h"

typedef struct {
    UInt16* buffer;
    int     width;
    int     height;

    int inputIndex;
    int inputCount;
} VideoIn;

static VideoIn videoIn;


static int updateVideoInBuffer(int width, int height)
{
    // If the size is different, free the old buffer
    if (videoIn.width != width || videoIn.height != height) {
        if (videoIn.buffer != NULL) {
            free(videoIn.buffer);
        }
        videoIn.buffer = NULL;
    }

    // If we have a buffer, reuse it
    if (videoIn.buffer != NULL) {
        return 0;
    }

    videoIn.buffer = (UInt16*)calloc(sizeof(UInt16), width* height);
    videoIn.width  = width;
    videoIn.height = height;
    return 1;
}

int archVideoInIsVideoConnected()
{
    return videoIn.inputIndex > 0;
}

UInt16* archVideoInBufferGet(int width, int height)
{
    static UInt32 motion = 0;
    int x, y;

    if (videoIn.inputIndex == 0) {
        return NULL;
    }

    // For now, just create a nice autogenerated image
    updateVideoInBuffer(width, height);
    for (x = 0; x < width; x++) {
        for (y = 0; y < height; y++) {
            videoIn.buffer[x + y * width] = (UInt16)(4*x + 4*y + motion) & 0x7fff; 
        }
    }
    motion += 4;

    return videoIn.buffer;
}

void videoInInitialize(Properties* properties)
{
    // Read Properties
    // Should read videoIn.inputName and match with existing
    // input devices. If no match is found, set inputIndex=0
    videoIn.inputCount = 2;
    
    videoIn.inputIndex = properties->videoIn.inputIndex;

    if (videoIn.inputIndex >= videoIn.inputCount) {
        videoIn.inputIndex = 0;
    }
}

void videoInCleanup(Properties* properties)
{
    properties->videoIn.inputIndex = videoIn.inputIndex;

    // Should also save name of current input device
}

int  videoInGetCount()
{
    return videoIn.inputCount;
}

int videoInIsActive(int index)
{
    return index == videoIn.inputIndex;
}

int videoInGetActive()
{
    return videoIn.inputIndex;
}

const char* videoInGetName(int index)
{
    // Should return a list of input devices
    // First should be 'None', second, 'Use Bitmap File'
    // Others should be Real input devices
    // The list probably needs updating everytime
    // videoInGetInputCount is called (which is
    // from the menu
    switch (index) {
        case 0: return langTextNone();
        case 1: return langMenuVideoInBitmap();
    }

    return langTextUnknown();
}

void videoInSetActive(int index)
{
    if (index >= videoIn.inputCount) {
        return;
    }
    videoIn.inputIndex = index;
}
