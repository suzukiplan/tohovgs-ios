//
//  vge_i.c
//  SHOT04
//
//  Created by Yoji Suzuki on 2013/03/03.
//  Copyright (c) 2013, SUZUKI PLAN. All rights reserved.
//

#include <pthread.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <CoreAudio/CoreAudioTypes.h>
#include <AudioUnit/AudioUnit.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "vge.h"
#include "vgeint.h"
#include "vgsdec.h"

#define kOutputBus 0
#define kInputBus 1
#define BUFNUM 2

#define SAMPLE_RATE 22050            /* sampling rate */
#define SAMPLE_BITS AL_FORMAT_MONO16 /* sampling bits */
#define SAMPLE_BUFS 4410             /* buffer size of foreground (50ms) */
#define SAMPLE_BUFS2 17640           /* buffer size of background (200ms) */

int _flingX;
int _flingY;
int _forcePause;
void* _psg;

/*
 *-----------------------------------------------------------------------------
 * Tables
 *-----------------------------------------------------------------------------
 */
struct binrec {
    char name[16];
    int size;
    char* data;
};

/*
 *-----------------------------------------------------------------------------
 * Variables
 *-----------------------------------------------------------------------------
 */
static int BN;
static struct binrec* BR = NULL;
static pthread_mutex_t sndLock;
static pthread_t sndTid;
static ALCdevice* sndDev;
static ALCcontext* sndCtx;
static ALuint sndABuf;
static ALuint sndASrc;
static char sndbuf[SAMPLE_BUFS2];
unsigned short ADPAL[256];
static char pathbuf[4096];
int isIphone5;
volatile int isBackground = 0;
volatile int isBGActive = 0;

/*
 *-----------------------------------------------------------------------------
 * Functions
 *-----------------------------------------------------------------------------
 */
static int init_audio();
static int init_openAL();
// static void term_openAL();
static void* sound_thread(void* args);
static void* bg_thread(void* args);
typedef ALvoid AL_APIENTRY (*alBufferDataStaticProcPtr)(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
static alBufferDataStaticProcPtr alBufferDataStaticProc;

/*
 *-----------------------------------------------------------------------------
 * Initialize procedure of VGS
 *-----------------------------------------------------------------------------
 */
int vgsint_init(const char* rompath)
{
    char log[1024];
    static char* rom = NULL;
    char* bin;
    FILE* fp;
    int fsize;
    int i, j, size;
    int cn, pn, bn;
    unsigned char s[4];
    char path[80];
    struct stat stbuf;
    unsigned char r, g, b;
    struct sched_param param;

    // initialize mutex
    pthread_mutex_init(&sndLock, NULL);

    // get file size
    stat(rompath, &stbuf);
    fsize = (int)stbuf.st_size;

    // open
    fp = fopen(rompath, "rb");
    if (NULL == fp) {
        return -1;
    }

    // allocate buffer
    rom = (char*)malloc(fsize);
    if (NULL == rom) {
        fclose(fp);
        return -1;
    }
    // read
    fread(rom, fsize, 1, fp);
    fclose(fp);
    bin = rom;

    /* get the number of records */
    memcpy(&BN, bin, 4);
    bin += 4;

    /* get records buffer */
    BR = (struct binrec*)malloc(BN * sizeof(struct binrec));
    if (NULL == BR) {
        return -1;
    }
    memset(BR, 0, BN * sizeof(struct binrec));

    /* set record name */
    for (i = 0; i < BN; i++) {
        memset(BR[i].name, 0, 16);
        memcpy(BR[i].name, bin, 16);
        bin += 16;
        for (j = 0; '\0' != BR[i].name[j]; j++) {
            if (15 == j) {
                return -1;
            }
            BR[i].name[j] ^= 0xAA;
        }
    }

    /* set record buffer */
    for (i = 0; i < BN; i++) {
        memcpy(s, bin, 4);
        bin += 4;
        size = s[0];
        size <<= 8;
        size |= s[1];
        size <<= 8;
        size |= s[2];
        size <<= 8;
        size |= s[3];
        BR[i].size = size;
        BR[i].data = (char*)malloc(BR[i].size);
        if (NULL == BR[i].data) {
            return -1;
        }
        memcpy(BR[i].data, bin, BR[i].size);
        bin += BR[i].size;
    }

    cn = 0;
    pn = 0;
    bn = 0;
    for (i = 0; i < 256; i++) {
        sprintf(path, "GSLOT%03d.CHR", i);
        if (0 == vge_gload(i, path)) cn++;
        sprintf(path, "ESLOT%03d.PCM", i);
        if (0 == vge_eload(i, path)) pn++;
        sprintf(path, "BSLOT%03d.BGM", i);
        if (0 == vge_bload(i, path)) bn++;
    }
    sprintf(log, "Data has extracted. (CHR=%d, PCM=%d, BGM=%d)", cn, pn, bn);
    puts(log);

    /* initialize UP */
    if (0 != vge_init()) {
        return -1;
    }

    /* create 16bit color palette */
    for (i = 0; i < 256; i++) {
        r = (_vram.pal[i] & 0x00FF0000) >> 16;
        g = (_vram.pal[i] & 0x0000FF00) >> 8;
        b = (_vram.pal[i] & 0x000000FF);
        r &= 0xF8;
        r >>= 3;
        g &= 0xF8;
        g >>= 3;
        b &= 0xF8;
        b >>= 3;
        ADPAL[i] = r;
        ADPAL[i] <<= 5;
        ADPAL[i] |= g;
        ADPAL[i] <<= 5;
        ADPAL[i] |= b;
    }

    /* initialize vgs-bgm-decoder */
    _psg = vgsdec_create_context();
    if (NULL == _psg) {
        return -1;
    }

    /* initialize OpenAL */
    if (0 != init_openAL()) {
        return -1;
    }

    /* start sound thread */
    pthread_create(&sndTid, NULL, sound_thread, NULL);
    memset(&param, 0, sizeof(param));
    param.sched_priority = 46;
    pthread_setschedparam(sndTid, SCHED_OTHER, &param);

    /* start BG thread */
    pthread_create(&sndTid, NULL, bg_thread, NULL);
    return 0;
}

/*
 *-----------------------------------------------------------------------------
 * Initialize audio unit
 *-----------------------------------------------------------------------------
 */
static int init_audio()
{
    AudioComponentDescription desc;
    AudioComponent comp;
    AudioComponentInstance cinst;
    UInt32 enableIO = 1;
    OSStatus osst;
    AudioStreamBasicDescription fmt;

    memset(&desc, 0, sizeof(desc));
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_RemoteIO;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;

    memset(&fmt, 0, sizeof(fmt));
    fmt.mSampleRate = 22050.0;
    fmt.mFormatID = kAudioFormatLinearPCM;
    fmt.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    fmt.mBitsPerChannel = 16;
    fmt.mChannelsPerFrame = 1;
    fmt.mFramesPerPacket = 1;
    fmt.mBytesPerFrame = 2;
    fmt.mBytesPerPacket = 2;
    fmt.mReserved = 0;

    // Set audio format
    comp = AudioComponentFindNext(NULL, &desc);
    if (NULL == comp) {
        return -1;
    }
    osst = AudioComponentInstanceNew(comp, &cinst);
    if (osst != noErr) {
        return -1;
    }
    osst = AudioUnitSetProperty(cinst, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, kOutputBus, &enableIO, sizeof(enableIO));
    if (osst != noErr) {
        return -1;
    }
    osst = AudioUnitSetProperty(cinst, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, kOutputBus, &fmt, sizeof(fmt));
    if (osst != noErr) {
        return -1;
    }
    osst = AudioOutputUnitStart(cinst);
    if (osst != noErr) {
        return -1;
    }
    return 0;
}

/*
 *-----------------------------------------------------------------------------
 * Initialize OpenAL
 *-----------------------------------------------------------------------------
 */
static int init_openAL()
{
    //  init_audio();

    // initialize OpenAL
    sndDev = alcOpenDevice(NULL);
    if (NULL == sndDev) {
        return -1;
    }
    sndCtx = alcCreateContext(sndDev, NULL);
    if (NULL == sndCtx) {
        return -1;
    }
    if (!alcMakeContextCurrent(sndCtx)) {
        return -1;
    }
    alBufferDataStaticProc = (alBufferDataStaticProcPtr)alcGetProcAddress(nil, (const ALCchar*)"alBufferDataStatic");
    //  alSourcei( sndASrc, AL_LOOPING, AL_FALSE );
    return 0;
}

/*
 *-----------------------------------------------------------------------------
 * Terminate OpenAL
 *-----------------------------------------------------------------------------
 */
/*
static void term_openAL()
{
    if(sndASrc) {
        alDeleteSources(1,&sndASrc);
        alDeleteBuffers(BUFNUM,sndABuf);
    }
    alcMakeContextCurrent(NULL);
    if(sndCtx) alcDestroyContext(sndCtx);
    sndCtx=NULL;
    if(sndDev) alcCloseDevice(sndDev);
    sndDev=NULL;
}
*/

/*
 *-----------------------------------------------------------------------------
 * sound thread procedure
 *-----------------------------------------------------------------------------
 */
static void* sound_thread(void* args)
{
    ALint st;

    alGenSources(1, &sndASrc);
    memset(sndbuf, 0, SAMPLE_BUFS2);

    while (1) {
        alGetSourcei(sndASrc, AL_BUFFERS_QUEUED, &st);
        if (st < BUFNUM) {
            alGenBuffers(1, &sndABuf);
        } else {
            alGetSourcei(sndASrc, AL_SOURCE_STATE, &st);
            if (st != AL_PLAYING) {
                alSourcePlay(sndASrc);
            }
            while (alGetSourcei(sndASrc, AL_BUFFERS_PROCESSED, &st), st == 0) {
                usleep(1000);
            }
            alSourceUnqueueBuffers(sndASrc, 1, &sndABuf);
            alDeleteBuffers(1, &sndABuf);
            alGenBuffers(1, &sndABuf);
        }
        if (isBackground) {
            vgsbuf(sndbuf, SAMPLE_BUFS2);
            alBufferData(sndABuf, SAMPLE_BITS, sndbuf, SAMPLE_BUFS2, SAMPLE_RATE);
        } else {
            vgsbuf(sndbuf, SAMPLE_BUFS);
            alBufferData(sndABuf, SAMPLE_BITS, sndbuf, SAMPLE_BUFS, SAMPLE_RATE);
        }
        alSourceQueueBuffers(sndASrc, 1, &sndABuf);
    }

    /*        do {
                alGetSourcei(sndASrc, AL_SOURCE_STATE, &st);
                if (st != AL_PLAYING) {
                    printf("replay ");
                    alSourcePlay(sndASrc);
                }
                alGetSourcei(sndASrc, AL_BUFFERS_PROCESSED, &st);
                printf("prc=%d ",st);
                if(0==st) {
                    usleep(100);
                }
            } while(0==st);
            ptr++;
            if(BUFNUM<=ptr) ptr=0;
            printf("rep=q#%d ",ptr);
            alSourceUnqueueBuffers(sndASrc, 1, &sndABuf[ptr]);
            alDeleteBuffers(1,&sndABuf[ptr]);
            vgsbuf(sndbuf,SAMPLE_BUFS);
            alBufferData(sndABuf[ptr],SAMPLE_BITS,sndbuf,SAMPLE_BUFS,SAMPLE_RATE);
            alSourceQueueBuffers(sndASrc, 1, &sndABuf[ptr]);
            while(1) {
                alGetSourcei(sndASrc, AL_SOURCE_STATE, &st);
                if(st!=AL_PLAYING) {
                    break;
                }
                usleep(100);
            }
        }*/
    return NULL;
}

/*
 *-----------------------------------------------------------------------------
 * background thread procedure
 *-----------------------------------------------------------------------------
 */
static void* bg_thread(void* args)
{
    while (1) {
        isBGActive = 0;
        while (0 == isBackground) {
            usleep(1000);
        }
        isBGActive = 1;
        while (0 != isBackground) {
            vge_loop();
            usleep(20000);
        }
    }
    return NULL;
}

/*
s *-----------------------------------------------------------------------------
 * Destroy procedure of VGS
 * #Not implemented now
 *-----------------------------------------------------------------------------
 */
/*int vgsint_term()
{
    pthread_mutex_destroy(&sndLock);
}*/

void lock()
{
    pthread_mutex_lock(&sndLock);
}

void unlock()
{
    pthread_mutex_unlock(&sndLock);
}

/*
 *-----------------------------------------------------------------------------
 * set sound effect flag
 *-----------------------------------------------------------------------------
 */
void eff_flag(struct _EFF* e, unsigned int f)
{
    pthread_mutex_lock(&sndLock);
    e->flag = f;
    pthread_mutex_unlock(&sndLock);
}

/*
 *-----------------------------------------------------------------------------
 * set sound effect position
 *-----------------------------------------------------------------------------
 */
void eff_pos(struct _EFF* e, unsigned int f)
{
    pthread_mutex_lock(&sndLock);
    e->pos = f;
    pthread_mutex_unlock(&sndLock);
}

/*
 *-----------------------------------------------------------------------------
 * get binary data
 *-----------------------------------------------------------------------------
 */
char* getbin(const char* name, int* size)
{
    int i;
    for (i = 0; i < BN; i++) {
        if (0 == strcmp(BR[i].name, name)) {
            *size = BR[i].size;
            return BR[i].data;
        }
    }
    return NULL;
}

/*
 *-----------------------------------------------------------------------------
 * get file mode (always appdata on iOS)
 *-----------------------------------------------------------------------------
 */
int vge_filemode()
{
    return 1;
}

/*
 *-----------------------------------------------------------------------------
 * set data directory
 *-----------------------------------------------------------------------------
 */
void vgsint_setdir(const char* dir)
{
    strcpy(pathbuf, dir);
    strcat(pathbuf, "/");
}

/*
 *-----------------------------------------------------------------------------
 * fopen
 *-----------------------------------------------------------------------------
 */
FILE* vge_fopen(const char* fname, const char* mode)
{
    char path[4096];
    strcpy(path, pathbuf);
    strcat(path, fname);
    return fopen(path, mode);
}
