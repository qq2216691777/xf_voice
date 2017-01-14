#include "play_voice.h"
#include "main.h"


char *buffer;
static snd_pcm_uframes_t frames=32;
static snd_pcm_t *playback_handle=NULL;//PCM设备句柄pcm.h

v_queue *g_voice_data;

static int device_voide_init( void )
{
	int ret;
	unsigned int val;
    int dir=0;

    snd_pcm_uframes_t periodsize;
	snd_pcm_hw_params_t *hw_params;//硬件信息和PCM流配置


	//1. 打开PCM，最后一个参数为0意味着标准配置
	ret = snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (ret < 0) {
		perror("snd_pcm_open");
		return -1;
	}

	//2. 分配snd_pcm_hw_params_t结构体
	ret = snd_pcm_hw_params_malloc(&hw_params);
	if (ret < 0) {
		perror("snd_pcm_hw_params_malloc");
		return -1;
	}
	//3. 初始化hw_params
	ret = snd_pcm_hw_params_any(playback_handle, hw_params);
	if (ret < 0) {
		perror("snd_pcm_hw_params_any");
		return -1;
	}
	//4. 初始化访问权限
	ret = snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (ret < 0) {
		perror("snd_pcm_hw_params_set_access");
		return -1;
	}
	//5. 初始化采样格式SND_PCM_FORMAT_U8,8位
	ret = snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S16_LE);
	if (ret < 0) {
		perror("snd_pcm_hw_params_set_format");
		return -1;
	}
	//6. 设置采样率，如果硬件不支持我们设置的采样率，将使用最接近的
	//val = 44100,有些录音采样频率固定为8KHz


	val = 16000;
	ret = snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, &val, &dir);
	if (ret < 0) {
		perror("snd_pcm_hw_params_set_rate_near");
		return -1;
	}
	//7. 设置通道数量  1
	ret = snd_pcm_hw_params_set_channels(playback_handle, hw_params, 1);
	if (ret < 0) {
		perror("snd_pcm_hw_params_set_channels");
		return -1;
	}

	/* Set period size to 32 frames. */
    periodsize = frames ;
    ret = snd_pcm_hw_params_set_buffer_size_near(playback_handle, hw_params, &periodsize);
    if (ret < 0)
    {
         printf("Unable to set buffer size %li : %s\n", frames, snd_strerror(ret));
         return -1;

    }
    periodsize /= 2;

    ret = snd_pcm_hw_params_set_period_size_near(playback_handle, hw_params, &periodsize, 0);
    if (ret < 0)
    {
        printf("Unable to set period size %li : %s\n", periodsize,  snd_strerror(ret));
        return -1;
    }

	//8. 设置hw_params
	ret = snd_pcm_hw_params(playback_handle, hw_params);
	if (ret < 0) {
		perror("snd_pcm_hw_params");
		return -1;
	}

	 /* Use a buffer large enough to hold one period */
    snd_pcm_hw_params_get_period_size(hw_params, &frames, &dir);

   // size = frames * 2; /* 2 bytes/sample, 2 channels */

    snd_pcm_hw_params_free (hw_params);

    return 0;
}

int play_over = 0;
static int play_voice( char *databuffer , int Dsize)
{
    int ret;
    int size = frames * 2;
    int use_size=0;

    if( NULL == playback_handle )
    {
        printf("please init the audio device\n");
        return -1;
    }

    while (1)
    {
		//9. 写音频数据到PCM设备
        while((ret = snd_pcm_writei(playback_handle, databuffer, frames))<0)
        {

            usleep(200);
            if (ret == -EPIPE)
            {
                  /* EPIPE means underrun */
                  fprintf(stderr, "underrun occurred\n");
				  //完成硬件参数设置，使设备准备好
                  snd_pcm_prepare(playback_handle);
            }
            else if (ret < 0)
            {
                  fprintf(stderr,
                      "error from writei: %s\n",
                      snd_strerror(ret));
            }
        }

        databuffer += size;
        use_size += size;
        if( use_size>Dsize)
            break;
    }
	return 0;
}

static int device_close( void )
{
    //10. 关闭PCM设备句柄
    return snd_pcm_close(playback_handle);
}


void *Device_audio_Pthread( void *arg )
{
    int ret;
    v_queue *tp;
    v_queue *l_tp;
    ret = device_voide_init();
    if(ret)
    {
        device_close();
        pthread_exit(NULL);
    }
    g_voice_data=calloc( sizeof(v_queue), 1 );
    tp = g_voice_data;
    tp->Vsize = 0;


    while(1)
    {
        if( tp->Vsize )
        {
            play_over = 1;
            play_voice( tp->Vdata, tp->Vsize );

            l_tp = tp->next;
            free( tp->Vdata );
            free( tp );
            tp = l_tp;
        }
        else
        {
            play_over = 0;
            usleep(1000);
        }

        //play_voice();
    }

    device_close();
    pthread_exit(NULL);
}


