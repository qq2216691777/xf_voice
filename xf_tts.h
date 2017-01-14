#ifndef _XF_TTS_H__
#define _XF_TTS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"

int text_to_speech(const char* src_text );

#endif // _XF_TTS_H__
