#include "ResourceManager.h"

ALLEGRO_DEBUG_CHANNEL("ResourceManager")

template <class T>
ResourceManager<T>::ResourceManager(const ResourceName resource_names[])
    : resources() {
    for (size_t i = 0; resource_names[i].name != ""; i++) {
        add(resource_names[i]);
    }
}

template <class T> ResourceManager<T>::~ResourceManager() { resources.clear(); }

template <class T>
void
ResourceManager<T>::unload() {
    for (auto &resource : resources) {
        resource.second.data = nullptr;
    }
}

template <class T>
bool
ResourceManager<T>::load() {
    bool ok = true;
    for (auto &resource : resources) {
        if (resource.second.data == nullptr) {
            if (!load(resource.second)) {
                ok = false;
            }
        }
    }
    return ok;
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
T *
ResourceManager<T>::get(const std::string &name) {
    if (resources.find(name) == resources.end()) {
        return nullptr;
    }

    auto &data = resources[name];
    if (data.data == nullptr) {
        return load(data);
    } else {
        return data.data.get();
    }
}

template <class T>
T *
ResourceManager<T>::operator[](const std::string &name) {
    return get(name);
}

template <>
void
Resource<ALLEGRO_BITMAP>::deleter(ALLEGRO_BITMAP *b) {
    al_destroy_bitmap(b);
}

template <>
ALLEGRO_BITMAP *
ResourceManager<ALLEGRO_BITMAP>::load(Resource<ALLEGRO_BITMAP> &data) {
    ALLEGRO_BITMAP *b = al_load_bitmap(data.path.c_str());
    if (b == nullptr) {
        ALLEGRO_ERROR("Unable to load resource %s\n", data.path.c_str());
        return nullptr;
    }
    size_t idx = data.path.find(".bmp");
    if (idx != std::string::npos) {
        al_convert_mask_to_alpha(b, al_map_rgb(255, 0, 255));
    }
    resources[data.name].data =
        std::unique_ptr<ALLEGRO_BITMAP, void (*)(ALLEGRO_BITMAP *)>(
            b, Resource<ALLEGRO_BITMAP>::deleter);
    return b;
}

template class ResourceManager<ALLEGRO_BITMAP>;
