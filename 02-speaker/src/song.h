
class SongControl {
  private:
    int buz;
    int led;
    int current_song;
    int current_song_index;
    int current_note;
    int song_length;
    void loadSong(int song_index);
    void unloadSong();
    bool playNextNote();
  public:
    inline SongControl() {};
    void begin(int buz, int led);
    void load(int song_index);
    void stop();
    void next();
};

extern SongControl Song;
