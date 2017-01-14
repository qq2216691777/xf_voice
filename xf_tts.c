#include "xf_tts.h"
#include "play_voice.h"
#include "main.h"

v_queue *g_voice_data;
/* 文本合成 */
int text_to_speech(const char* src_text )
{
	int          ret          = -1;
	const char*  sessionID    = NULL;
	unsigned int audio_len    = 0;
	int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;
	const char* params = "voice_name = xiaoyan, text_encoding = utf8, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";

    long Wave_size = 0;
    static v_queue *ptts_voice = NULL ;
    if( NULL == ptts_voice)
        ptts_voice = g_voice_data;

	if (NULL == src_text )
	{
		printf("params is error!\n");
		return ret;
	}

	/* 开始合成 */
	sessionID = QTTSSessionBegin(params, &ret);
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSSessionBegin failed, error code: %d.\n", ret);
		return ret;
	}
	ret = QTTSTextPut(sessionID, src_text, (unsigned int)strlen(src_text), NULL);
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSTextPut failed, error code: %d.\n",ret);
		QTTSSessionEnd(sessionID, "TextPutError");

		return ret;
	}
	printf("正在合成 ...\n");
	while (1)
	{
		/* 获取合成音频 */
		const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
		if (MSP_SUCCESS != ret)
			break;
		if (NULL != data)
		{
		    ptts_voice->next = calloc( sizeof(v_queue), 1 );
            ptts_voice->next->Vsize = 0;
		    ptts_voice->Vdata = calloc( audio_len, 1 );
		    memcpy( ptts_voice->Vdata, data, audio_len );
		    ptts_voice->Vsize = audio_len;
		    ptts_voice = ptts_voice->next;

		    Wave_size += audio_len; //计算data_size大小
		}
		if (MSP_TTS_FLAG_DATA_END == synth_status)
			break;
		printf(">");
		usleep(150*1000); //防止频繁占用CPU
	}                           //合成状态synth_status取值请参阅《讯飞语音云API文档》

	if (MSP_SUCCESS != ret)
	{
		printf("QTTSAudioGet failed, error code: %d.\n",ret);
		QTTSSessionEnd(sessionID, "AudioGetError");

		return ret;
	}

	/* 合成完毕 */
	ret = QTTSSessionEnd(sessionID, "Normal");
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSSessionEnd failed, error code: %d.\n",ret);
	}
    printf("total size:%ld\r\n",Wave_size );
    //play_voice(  Wave_Buffer ,Wave_size);

	return ret;
}

