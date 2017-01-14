#include "xf_voice_pthread.h"
#include "xf_iar.h"
#include "main.h"
#include "cJSON.h"

XFvoice_t g_xf_result;
static int xf_Voice_Init( void )
{
    int ret = MSP_SUCCESS;

    /* login params, please do keep the appid correct */
	const char* login_params = "appid = 574142e4, work_dir = .";

    /* Login first. the 1st arg is username, the 2nd arg is password */
	ret = MSPLogin(NULL, NULL, login_params);
	if (MSP_SUCCESS != ret)
    {
		DEBUG_PRINT("MSPLogin failed , Error code %d.\n",ret);
		MSPLogout();
		return -1;
	}



    return 0;
}

static int xf_Voice_exit(void)
{
    return MSPLogout(); // Logout...
}

extern int play_over;

void *xf_Voice_Pthread( void *arg )
{
    int ret;
    const char*  rec_text        =   NULL;
    cJSON *root;
    cJSON *format;


    ret = xf_Voice_Init();
    if(ret)
    {
        printf("XF Voice Init failed\n");
        xf_Voice_exit();
        pthread_exit(NULL);
    }
    while(1)
    {
        while(play_over)
            usleep(1000);
        Listen_and_analyse();
        printf("lasr=%d\n",g_xf_result.iar_last);
        printf("size=%d\n",g_xf_result.iar_size);
        if((g_xf_result.iar_last)&&(g_xf_result.iar_size==3))
        {

         /*  将结果进行语义分析 */
            rec_text = MSPSearch("nlp_version=2.0",g_xf_result.iar_reslut,(unsigned int*)&g_xf_result.iar_size,&ret);
            if(MSP_SUCCESS !=ret)
            {
                printf("MSPSearch failed ,error code is:%d\n",ret);

            }

            printf("rect_text:%s\n", rec_text);
            /* 将语义分析后的结果通过cJSON库识别出来 */
            root = cJSON_Parse(rec_text);

            switch( cJSON_GetObjectItem(root,"rc")->valueint)
            {
            case 0:
                switch( cJSON_GetObjectItem(root,"service")->valuestring[0] )
                {
                    default:
                    case 'c':
                    case 'f':
                    case 'b':
                    case 'o':
                        format = cJSON_GetObjectItem(root,"answer");
                        printf("%s\n",cJSON_GetObjectItem(format,"text")->valuestring);
                        /* 将提取到的结果进行语音合成 */
                        text_to_speech( cJSON_GetObjectItem(format,"text")->valuestring );
                        break;
                    case 't':
                        text_to_speech( "这个功能还没有完善！" );
                        break;

                    case 'm':
                        text_to_speech( "这个功能还没有完善！" );
                        break;
                }

                break;
            case 1:      //无效请求,通常在用户提交的请求参数有误时返回
            case 2:
            case 3:

                printf("%s\n",cJSON_GetObjectItem(root,"error")->valuestring);
                /* 将提取到的结果进行语音合成 */
                text_to_speech( "错误：" );
                text_to_speech( cJSON_GetObjectItem(root,"error")->valuestring );
                break;
            case 4:
                //text_to_speech( "上帝啊！这个问题太难了！" );
                break;
            }



        }


        sleep(2);
    }


    xf_Voice_exit();
    pthread_exit(NULL);
}


