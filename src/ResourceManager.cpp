#include <allegro5/allegro.h>
#include <allegro5/allegro_physfs.h>
#include <physfs.h>

#include "Game.h"
#include "ResourceManager.h"
#include "Util.h"

using namespace std;

ALLEGRO_DEBUG_CHANNEL("ResourceManager")

template <class T> ResourceManager<T>::ResourceManager() : resources() {}

template <class T>
ResourceManager<T>::ResourceManager(const ResourceName resource_names[])
    : resources() {
    add(resource_names);
}

template <class T> ResourceManager<T>::~ResourceManager() { resources.clear(); }

template <class T>
void
ResourceManager<T>::add(const ResourceName names[]) {
    for (size_t i = 0; names[i].name != ""; i++) {
        add(names[i]);
    }
}

template <class T>
void
ResourceManager<T>::add(const ResourceName &name) {
    auto &data = resources[name.name];
    if (data.name == "") {
        data.name = name.name;
        data.path = name.path;
    }
}

template <class T>
void
ResourceManager<T>::clear() {
    resources.clear();
}

template <class T>
shared_ptr<T>
ResourceManager<T>::get(const std::string &name) {
    if (resources.find(name) == resources.end()) {
        return nullptr;
    }

    auto &data = resources[name];
    if (data.data.expired()) {
        return load(data);
    } else {
        return data.data.lock();
    }
}

template <class T>
shared_ptr<T>
ResourceManager<T>::operator[](const std::string &name) {
    return get(name);
}

template <>
void
Resource<ALLEGRO_BITMAP>::deleter(ALLEGRO_BITMAP *b) {
    al_destroy_bitmap(b);
}

template <>
shared_ptr<ALLEGRO_BITMAP>
ResourceManager<ALLEGRO_BITMAP>::load(Resource<ALLEGRO_BITMAP> &data) {
    ALLEGRO_BITMAP *b;
    b = al_load_bitmap(data.path.c_str());
    if (b == nullptr) {
        ALLEGRO_ERROR("Unable to load resource %s\n", data.path.c_str());
        return nullptr;
    }
    al_convert_mask_to_alpha(b, al_map_rgb(255, 0, 255));
    auto sp =
        std::shared_ptr<ALLEGRO_BITMAP>(b, Resource<ALLEGRO_BITMAP>::deleter);
    resources[data.name].data = sp;
    return sp;
}

template class ResourceManager<ALLEGRO_BITMAP>;

template <>
void
Resource<Sample>::deleter(Sample *s) {
    delete s;
}

template <>
shared_ptr<Sample>
ResourceManager<Sample>::load(Resource<Sample> &data) {
    shared_ptr<Sample> s;

    s = g_game->audioSystem->Load(data.path.c_str());
    if (s == nullptr) {
        ALLEGRO_ERROR("Unable to load resource %s\n", data.path.c_str());
        return nullptr;
    }
    resources[data.name].data = s;
    return s;
}

template class ResourceManager<Sample>;
// vi: ft=cpp
