#ifndef _RADIO_H_
#define _RADIO_H_

// Sounds files
#define HIHAT_FILE "beatbox-wave-files/100053__menegass__gui-drum-cc.wav"
#define BASE_FILE "beatbox-wave-files/100051__menegass__gui-drum-bd-hard.wav"
#define SNARE_FILE "beatbox-wave-files/100059__menegass__gui-drum-snare-soft.wav"
#define SPLASH_FILE "beatbox-wave-files/100060__menegass__gui-drum-splash-hard.wav"
#define TOM_FILE "beatbox-wave-files/100063__menegass__gui-drum-tom-hi-soft.wav"
#define CYN_FILE "beatbox-wave-files/100057__menegass__gui-drum-cyn-soft.wav"

#define DEFAULT_BPM 200

// Initializes audio related stuff
void Radio_init();

// Starts to play sounds
void Radio_startPlayingSounds(void);

// Clean up code for Audio Mixer called here
void Radio_cleanup();

#endif