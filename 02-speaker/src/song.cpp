/*
  Arduino Mario Bros Tunes
  ========================
  Played with piezo buzzer using Pulse Width Modulation (PWM)
  and an LED to blink with the songs.

  Setup
  -----
  Connect the positive side of the Buzzer to the "buz" pin.
  Connect the negative side of the Buzzer to a 1k Ohm resistor.
  Connect the other side of the 1k Ohm resistor to the ground (GND).
  Also setup an "led" that is turned on when a note is sung.

  Credits
  -------
  Original Author: Dipto Pratyaksa (inital Arduino sketch)
  Editors:         Uwe Jugel, @ubunatic (modularized for PlatformIO)

*/

#include "pitches.h"
#include "Arduino.h"
#include "buzz.h"
#include "song.h"

// Mario main theme melody
int mario_melody[] = {
    NOTE_E7, NOTE_E7, 0, NOTE_E7,
    0, NOTE_C7, NOTE_E7, 0,
    NOTE_G7, 0, 0,  0,
    NOTE_G6, 0, 0, 0,

    NOTE_C7, 0, 0, NOTE_G6,
    0, 0, NOTE_E6, 0,
    0, NOTE_A6, 0, NOTE_B6,
    0, NOTE_AS6, NOTE_A6, 0,

    NOTE_G6, NOTE_E7, NOTE_G7,
    NOTE_A7, 0, NOTE_F7, NOTE_G7,
    0, NOTE_E7, 0, NOTE_C7,
    NOTE_D7, NOTE_B6, 0, 0,

    NOTE_C7, 0, 0, NOTE_G6,
    0, 0, NOTE_E6, 0,
    0, NOTE_A6, 0, NOTE_B6,
    0, NOTE_AS6, NOTE_A6, 0,

    NOTE_G6, NOTE_E7, NOTE_G7,
    NOTE_A7, 0, NOTE_F7, NOTE_G7,
    0, NOTE_E7, 0, NOTE_C7,
    NOTE_D7, NOTE_B6, 0, 0
};

//Mario main them tempo
int mario_tempo[] = {
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    9, 9, 9,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    9, 9, 9,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
};

// Mario underworld melody
int underworld_melody[] = {
    NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
    NOTE_AS3, NOTE_AS4, 0,
    0,
    NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
    NOTE_AS3, NOTE_AS4, 0,
    0,
    NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
    NOTE_DS3, NOTE_DS4, 0,
    0,
    NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
    NOTE_DS3, NOTE_DS4, 0,
    0, NOTE_DS4, NOTE_CS4, NOTE_D4,
    NOTE_CS4, NOTE_DS4,
    NOTE_DS4, NOTE_GS3,
    NOTE_G3, NOTE_CS4,
    NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
    NOTE_GS4, NOTE_DS4, NOTE_B3,
    NOTE_AS3, NOTE_A3, NOTE_GS3,
    0, 0, 0
};

// Mario underworld tempo
int underworld_tempo[] = {
    12, 12, 12, 12,
    12, 12, 6,
    3,
    12, 12, 12, 12,
    12, 12, 6,
    3,
    12, 12, 12, 12,
    12, 12, 6,
    3,
    12, 12, 12, 12,
    12, 12, 6,
    6, 18, 18, 18,
    6, 6,
    6, 6,
    6, 6,
    18, 18, 18, 18, 18, 18,
    10, 10, 10,
    10, 10, 10,
    3, 3, 3
};

#define SONG_NONE 0
#define SONG_MARIO 1
#define SONG_UNDERWORLD 2

int playlist[] = {SONG_MARIO, SONG_MARIO, SONG_UNDERWORLD};
int playlist_length = sizeof(playlist) / sizeof(int);

int getFreq(int song, int pos) {
    switch (song) {
    case SONG_NONE:       return -1;
    case SONG_UNDERWORLD: return underworld_melody[pos];
    case SONG_MARIO:      return mario_melody[pos];
    default: return -1;
    }
}

int getTempo(int song, int pos) {
    switch (song) {
    case SONG_NONE:       return -1;
    case SONG_UNDERWORLD: return underworld_tempo[pos];
    case SONG_MARIO:      return mario_tempo[pos];
    default: return -1;
    }
}

/* Implement private SongControl methods */

void SongControl::unloadSong() {
    current_song = SONG_NONE;
    current_song_index = -1;
    current_note = 0;
    song_length = 0;
}

void SongControl::loadSong(int index) {
    // playlist finished, no more notes to play.
    if (index >= playlist_length) {
        Serial.println("playlist finished");
        unloadSong();
        return;
    }

    if (index < 0) {
        Serial.println("ERROR: song index out of range, unloading current song");
        unloadSong();
        return;
    }

    // load next song and schedule the one after.
    current_song = playlist[index];
    current_song_index = index;
    current_note = 0;

    // load song paramaters.
    switch (current_song) {
    case SONG_NONE: return;
    case SONG_MARIO:
        song_length = sizeof(mario_melody) / sizeof(int);
        Serial.println(" Playing 'Mario Theme'");
        break;
    case SONG_UNDERWORLD:
        song_length = sizeof(underworld_melody) / sizeof(int);
        Serial.println(" Playing 'Underworld Theme'");
        break;
    default:
        break;
    }
}

bool SongControl::playNextNote() {
    if (current_note == -1) return false;
    if (current_song == SONG_NONE) return false;

    // a song was or is now loaded and we can play the next note.
    int f = getFreq(current_song, current_note);
    int t = getTempo(current_song, current_note);
    playNote(buz, led, f, t);
    current_note++;

    // check if song finished and play next song.
    if (current_note >= song_length) loadSong(current_song_index + 1);

    return true;
}

/* Implement public SongControl methods */

// begin sets the buzzer and led pins to be used for the song.
void SongControl::begin(int buz, int led) {
    this->buz = buz;
    this->led = led;
    unloadSong();
};

// load loads a song by its song index (starting from 0).
void SongControl::load(int song_index) { loadSong(song_index); }

// stop unloads the current song and thus stops playback.
void SongControl::stop()  { unloadSong(); }

// playNextNote plays the next note from the current song.
void SongControl::next() {
    playNextNote();
}

// Song is the conroller for playing Mario songs.
SongControl Song;
