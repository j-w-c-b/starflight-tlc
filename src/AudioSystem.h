#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <allegro5/allegro_audio.h>

#include <map>
#include <memory>
#include <string>

class Sample {
  private:
    bool loop;
    ALLEGRO_SAMPLE *sample;

  public:
    ALLEGRO_SAMPLE_INSTANCE *sample_instance;

    explicit Sample(const std::string &filename, float volume = 1.0);
    ~Sample();

    bool IsInitialized() const;
    bool SetVolume(float volume);
    bool SetLoop(bool doLoop);
    bool SetPaused(bool doPause);
    bool IsPlaying() const;
};

class AudioSystem {
  public:
    AudioSystem();
    ~AudioSystem();

    bool Init();

    std::shared_ptr<Sample>
    Load(const std::string &filename, float volume = 1.0f);
    std::shared_ptr<Sample>
    LoadMusic(const std::string &filename, float volume = 0.4f);
    bool Play(std::shared_ptr<Sample> sample, bool doLoop = false);
    void Stop(std::shared_ptr<Sample> sample);

    bool PlayMusic(std::shared_ptr<Sample> sample, bool doLoop = true);
    void StopMusic();
    void StartMusic();

    bool IsPlaying(std::shared_ptr<Sample> sample);

  private:
    ALLEGRO_VOICE *voice;
    ALLEGRO_MIXER *mixer;

    bool bPlay;
    std::shared_ptr<Sample> m_music;
};
#endif // AUDIOSYSTEM_H
// vi: ft=cpp
