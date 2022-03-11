#include <allegro5/allegro_acodec.h>

#include "AudioSystem.h"
#include "Game.h"
#include "Util.h"

using std::make_shared;
using std::shared_ptr;
using std::string;

ALLEGRO_DEBUG_CHANNEL("AudioSystem")

Sample::Sample(const string &filename, float volume)
    : loop(false), sample(al_load_sample(filename.c_str())),
      sample_instance(al_create_sample_instance(sample)) {
    if (IsInitialized()) {
        SetVolume(volume);
    }
}

Sample::~Sample() {
    if (sample_instance != NULL) {
        al_destroy_sample_instance(sample_instance);
        sample_instance = NULL;
    }
    if (sample != NULL) {
        al_destroy_sample(sample);
        sample = NULL;
    }
}

bool
Sample::IsInitialized() const {
    return sample && sample_instance;
}

/**
    volume is based on 1.0 = 100%
**/
bool
Sample::SetVolume(float volume) {
    ALLEGRO_ASSERT(sample_instance);

    // perform a reasonable bounds check
    if (volume < 0.0f || volume > 10.0f)
        return false;

    return al_set_sample_instance_gain(sample_instance, volume);
}

/*
 * put a channel into looping/non-looping state
 *
 * doLoop == false --> oneshot playback
 * doLoop == true  --> playback will loop forever
 */
bool
Sample::SetLoop(bool doLoop) {
    loop = doLoop;

    ALLEGRO_ASSERT(sample_instance != NULL);

    if (loop) {
        return al_set_sample_instance_playmode(
            sample_instance, ALLEGRO_PLAYMODE_LOOP);
    } else {
        return al_set_sample_instance_playmode(
            sample_instance, ALLEGRO_PLAYMODE_ONCE);
    }
}

/*
 * pause/unpause a channel
 */
bool
Sample::SetPaused(bool doPause) {
    ALLEGRO_ASSERT(sample_instance != NULL);
    return al_set_sample_instance_playing(sample_instance, !doPause);
}

bool
Sample::IsPlaying() const {
    return IsInitialized() && al_get_sample_instance_playing(sample_instance);
}

AudioSystem::AudioSystem() : voice(nullptr), mixer(nullptr), bPlay(true) {}

AudioSystem::~AudioSystem() {
    if (mixer) {
        al_destroy_mixer(mixer);
    }
    if (voice) {
        al_destroy_voice(voice);
    }
}

bool
AudioSystem::Init() {
    // retrieve global music playback setting
    bPlay = g_game->getGlobalBoolean("AUDIO_GLOBAL");

    if (!al_install_audio()) {
        ALLEGRO_ERROR("al_init_acodec_addon failed\n");
        return false;
    }
    if (!al_init_acodec_addon()) {
        ALLEGRO_ERROR("al_init_acodec_addon failed\n");
        return false;
    }
    voice = al_create_voice(
        44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
    if (!voice) {
        ALLEGRO_ERROR("al_create_voice failed\n");
        return false;
    }
    mixer = al_create_mixer(
        44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
    if (!mixer) {
        ALLEGRO_ERROR("al_create_mixer failed\n");
        return false;
    }
    if (!al_attach_mixer_to_voice(mixer, voice)) {
        ALLEGRO_ERROR("al_attach_mixer_to_voice failed");
        return false;
    }

    return true;
}

shared_ptr<Sample>
AudioSystem::Load(const string &filename, float volume) {
    if (filename.length() == 0)
        return NULL;
    auto sample = make_shared<Sample>(filename.c_str(), volume);

    if (!sample->IsInitialized()) {
        return nullptr;
    }
    sample->SetVolume(volume);
    al_attach_sample_instance_to_mixer(sample->sample_instance, mixer);
    return sample;
}

shared_ptr<Sample>
AudioSystem::LoadMusic(const string &filename, float volume) {
    return Load(filename, volume);
}

bool
AudioSystem::IsPlaying(shared_ptr<Sample> sample) {
    return sample->IsPlaying();
}

bool
AudioSystem::Play(shared_ptr<Sample> sample, bool doLoop) {
    // prevent playback based on global setting
    if (!bPlay)
        return true;

    return sample && sample->IsInitialized() && sample->SetLoop(doLoop)
           && sample->SetPaused(false);
}

bool
AudioSystem::PlayMusic(shared_ptr<Sample> sample, bool doLoop) {
    m_music = sample;
    return Play(sample, doLoop);
}

void
AudioSystem::StopMusic() {
    if (m_music) {
        m_music->SetPaused(true);
    }
}

void
AudioSystem::StartMusic() {
    if (m_music) {
        m_music->SetPaused(false);
    }
}

void
AudioSystem::Stop(shared_ptr<Sample> sample) {
    if (sample == nullptr)
        return;
    sample->SetPaused(true);
}
// vi: ft=cpp
