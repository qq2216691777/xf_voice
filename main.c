#include <stdio.h>
#include <pthread.h>
#include <signal.h>

#include "main.h"
#include "xf_voice_pthread.h"
#include "play_voice.h"

int main( int argc, char **argv)
{
    int ret;
    pthread_t pthread_xf_voice_id;
    pthread_t pthread_audio_device_id;

    ret = pthread_create( &pthread_audio_device_id, NULL, &Device_audio_Pthread, NULL );
    if( ret )
    {
        perror("pthread_create audio_device");
        return -1;
    }
    DEBUG_PRINT("pthread Device audio Init successed\n");

    ret = pthread_create( &pthread_xf_voice_id, NULL, &xf_Voice_Pthread, NULL );
    if( ret )
    {
        perror("pthread_create xv_voice");
        return -1;
    }
    DEBUG_PRINT("pthread XF Voice Init successed\n");


    while(1)
    {
        if( 0!= pthread_kill(pthread_audio_device_id,0))
        {
            perror("audio_device stop");
            return -1;
        }

        if( 0!= pthread_kill(pthread_xf_voice_id,0))
        {
            perror("xv_voice stop");
            return -1;
        }

        sleep(2);


    }

    return 0;
}
