#ifndef _XF_VOICE_PTHREAD_H__
#define _XF_VOICE_PTHREAD_H__
#include <pthread.h>

#define USER_WORDS_FILE     "userwords.txt"

typedef struct xf_voice
{
    char *iar_reslut;
    int  iar_size;
    int  iar_last;
    char *sch_reslut;
    int  sch_size;
}XFvoice_t;

extern XFvoice_t g_xf_result;

void *xf_Voice_Pthread( void *arg );

#endif // _XF_VOICE_PTHREAD_H__
