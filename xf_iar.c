#include "xf_voice_pthread.h"
#include "xf_iar.h"
#include "main.h"
#include "cJSON.h"
#include "xf_tts.h"

/* Upload User words */
static int upload_userwords( char *filename)
{
	char*			userwords	=	NULL;
	size_t			len			=	0;
	size_t			read_len	=	0;
	FILE*			fp			=	NULL;
	int				ret			=	-1;

	fp = fopen(filename , "rb");
	if (NULL == fp)
	{
		DEBUG_PRINT("\nopen [userwords.txt] failed! \n");
		goto upload_exit;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	userwords = (char*)malloc(len + 1);
	if (NULL == userwords)
	{
		DEBUG_PRINT("\nout of memory! \n");
		goto upload_exit;
	}

	read_len = fread((void*)userwords, 1, len, fp);
	if (read_len != len)
	{
		DEBUG_PRINT("\nread [userwords.txt] failed!\n");
		goto upload_exit;
	}
	userwords[len] = '\0';

	MSPUploadData("userwords", userwords, len, "sub = uup, dtt = userword", &ret); //脡脧麓芦脫脙禄搂麓脢卤铆
	if (MSP_SUCCESS != ret)
	{
		DEBUG_PRINT("\nMSPUploadData failed ! errorCode: %d \n", ret);
		goto upload_exit;
	}

upload_exit:
	if (NULL != fp)
	{
		fclose(fp);
		fp = NULL;
	}
	if (NULL != userwords)
	{
		free(userwords);
		userwords = NULL;
	}

	return ret;
}

static void show_result(char *string, char is_over)
{
    printf("\rResult: [ %s ]", string);
	if(is_over)
		putchar('\n');

}


static char *g_result = NULL;
static unsigned int g_buffersize = BUFFER_SIZE;

void on_result(const char *result, char is_last)
{

	if (result)
    {
		size_t left = g_buffersize - 1 - strlen(g_result);
		size_t size = strlen(result);
		if (left < size) {
			g_result = (char*)realloc(g_result, g_buffersize + BUFFER_SIZE);
			if (g_result)
				g_buffersize += BUFFER_SIZE;
			else {
				printf("mem alloc failed\n");
				return;
			}
		}
		/*  得到语音听写结果 */
		strncat(g_result, result, size);
		show_result(g_result, is_last);


        g_xf_result.iar_reslut = g_result;
        g_xf_result.iar_size = size;
        g_xf_result.iar_last = is_last;


	}
}

static void on_speech_begin()
{
	if (g_result)
	{
		free(g_result);
	}
	g_result = (char*)malloc(BUFFER_SIZE);
	g_buffersize = BUFFER_SIZE;
	memset(g_result, 0, g_buffersize);

	printf("Start Listening...\n");
	g_xf_result.iar_size=0;
	g_xf_result.iar_last = 0;

}
static void on_speech_end(int reason)
{
	if (reason == END_REASON_VAD_DETECT)
    {
        printf("\nSpeaking done \n");
        if( !g_xf_result.iar_last )
            g_xf_result.iar_last = 102;
    }
	else
		printf("\nRecognizer error %d\n", reason);
}

static void Listen_from_mic(const char* session_begin_params)
{

	int errcode;

	struct speech_rec iat;

	struct speech_rec_notifier recnotifier = {
		on_result,
		on_speech_begin,
		on_speech_end
	};

	errcode = sr_init(&iat, session_begin_params, SR_MIC, &recnotifier);
	if (errcode) {
		printf("speech recognizer init failed\n");
		return;
	}
	errcode = sr_start_listening(&iat);
	if (errcode) {
		printf("start listen failed %d\n", errcode);
	}
	/* demo 15 seconds recording */
	while(!g_xf_result.iar_last)
		usleep(100000);
	errcode = sr_stop_listening(&iat);
	if (errcode) {
		printf("stop listening failed %d\n", errcode);
	}

	sr_uninit(&iat);
}

int Listen_and_analyse( void )
{
    int ret;

    /*
	* See "iFlytek MSC Reference Manual"
	*/
	const char* session_begin_params =
		"sub = iat, domain = iat, language = zh_cn, "
		"accent = mandarin, sample_rate = 16000, "
		"result_type = plain, result_encoding = utf8";

//	ret = upload_userwords( USER_WORDS_FILE );
//    if (MSP_SUCCESS != ret)
 //   {
//        DEBUG_PRINT("MSPLogin failed , Error code %d.\n",ret);
//		return -1;
//    }
//    printf("Uploaded successfully\n");


	printf("Speak in 150 seconds\n");
    Listen_from_mic(session_begin_params);
    printf("150 sec passed\n");


    return 0;
}
