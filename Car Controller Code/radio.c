#include "audioMixer.h"
#include "radio.h"

#include <pthread.h>
#include <time.h>

static int rockBeatSequence[8] = {1, 2, 3, 2, 1, 2, 3, 2};

// Thread stuff
// static pthread_t Radio_tid;
// static pthread_attr_t Radio_attr;
// pthread_mutex_t Radio_mutex = PTHREAD_MUTEX_INITIALIZER;
static void Radio_notesToPlay (int notes);
static void Radio_playNextBeat (int noteToPlay, long long sleepForBPM, int mode);
static void Radio_sleepForMs(long long delayInMs);

// Files containing the sounds
static wavedata_t baseFile;
static wavedata_t cynFile;
static wavedata_t splashFIle;
static wavedata_t tomFIle;
static wavedata_t snareFile;
static wavedata_t hihatFile;

static int numberOfBeatsInRockSequence = 8;
static int BPM = DEFAULT_BPM;
static int beatIndex = 0;

// Initializes audio related stuff
void Radio_init()
{
    AudioMixer_init();

    AudioMixer_readWaveFileIntoMemory(BASE_FILE, &baseFile);
    AudioMixer_readWaveFileIntoMemory(CYN_FILE, &cynFile);
    AudioMixer_readWaveFileIntoMemory(SPLASH_FILE, &splashFIle);
    AudioMixer_readWaveFileIntoMemory(TOM_FILE, &tomFIle);
    AudioMixer_readWaveFileIntoMemory(HIHAT_FILE, &hihatFile);
    AudioMixer_readWaveFileIntoMemory(SNARE_FILE, &snareFile);

    // pthread_attr_init(&Radio_attr);
    // pthread_create(&Radio_tid, &Radio_attr, Radio_startPlayingSounds, NULL);
    // pthread_join(Radio_tid, NULL);
}

// Clean up code for Audio Mixer called here
void Radio_cleanup()
{
    AudioMixer_freeWaveFileData(&baseFile);
    AudioMixer_freeWaveFileData(&tomFIle);
    AudioMixer_freeWaveFileData(&splashFIle);
    AudioMixer_freeWaveFileData(&hihatFile);
    AudioMixer_freeWaveFileData(&cynFile);

    AudioMixer_cleanup();
    // pthread_exit(NULL);
}

// Starts to play sounds
void Radio_startPlayingSounds(void) 
{
    Radio_playNextBeat (beatIndex, BPM, 0);
    Radio_sleepForMs(BPM);

    // If at the end of current beat sequence, start playing from the top
    if (beatIndex < numberOfBeatsInRockSequence - 1) {
        beatIndex++;
    } else {
        beatIndex = 0;
    }
}

// Plays next beat in the sequence
static void Radio_playNextBeat (int noteToPlay, long long sleepForBPM, int mode)
{
    Radio_notesToPlay(rockBeatSequence[noteToPlay]);
}

// Function holding all the possible note combinations that are played in this module
static void Radio_notesToPlay (int notes)
{
    switch (notes)
    {
        case 1:
            AudioMixer_queueSound(&hihatFile);
            AudioMixer_queueSound(&baseFile);
            break;
        case 2:
            AudioMixer_queueSound(&hihatFile);
            break;
        case 3:
            AudioMixer_queueSound(&hihatFile);
            AudioMixer_queueSound(&snareFile);
            break;
        case 4:
            AudioMixer_queueSound(&baseFile);
            AudioMixer_queueSound(&splashFIle);
            break;
        case 5:
            AudioMixer_queueSound(&baseFile);
            AudioMixer_queueSound(&tomFIle);
            break;
        case 6:
            AudioMixer_queueSound(&baseFile);
            break;
        case 7:
            AudioMixer_queueSound(&cynFile);
            break;
        case 8:
            AudioMixer_queueSound(&splashFIle);
            break;
        case 9:
            AudioMixer_queueSound(&tomFIle);
            break;
        default:
            //assert(false);
            break;
    }
}

// Function provided by Dr. Brian
static void Radio_sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}