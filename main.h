#ifndef _MAIN_H__
#define _MAIN_H__

#define DEBUG_PRINT printf

typedef struct Voice_queue{
    long Vsize;
    char *Vdata;
    struct Voice_queue *next;
}v_queue;


#endif // _MAIN_H__
