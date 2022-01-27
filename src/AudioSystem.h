#pragma once

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
  private:
    ALLEGRO_VOICE *voice;
    ALLEGRO_MIXER *mixer;

    std::map<std::string, std::shared_ptr<Sample>> samples;
    bool bPlay;

  public:
    AudioSystem();
    ~AudioSystem();

    bool Init();

    bool Load(
        const std::string &filename,
        const std::string &name,
        float volume = 1.0f);
    std::shared_ptr<Sample>
    Load(const std::string &filename, float volume = 1.0f);
    bool LoadMusic(
        const std::string &filename,
        const std::string &name,
        float volume = 0.4f);
    std::shared_ptr<Sample>
    LoadMusic(const std::string &filename, float volume = 0.4f);
    bool Play(const std::string &name, bool doLoop = false);
    bool Play(std::shared_ptr<Sample> sample, bool doLoop = false);
    bool PlayMusic(const std::string &name, bool doLoop = true);
    bool PlayMusic(std::shared_ptr<Sample> sample, bool doLoop = true);

    void Stop(const std::string &name);
    void Stop(std::shared_ptr<Sample> sample);
    bool IsPlaying(const std::string &name);
    bool IsPlaying(std::shared_ptr<Sample> sample);
    bool SampleExists(const std::string &name);
};
