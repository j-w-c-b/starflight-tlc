#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include <allegro5/allegro.h>

#include "AudioSystem.h"

struct ResourceName {
    std::string name;
    std::string path;
};

template <class T> struct Resource : public ResourceName {
    std::shared_ptr<T> data;
    static void deleter(T *);

    Resource() : data(nullptr, deleter) {}
    explicit Resource(const ResourceName &r)
        : ResourceName(r), data(nullptr, deleter) {}
};

template <class T> class ResourceManager {
  public:
    ResourceManager();
    explicit ResourceManager(const ResourceName resource_names[]);
    virtual ~ResourceManager();

    void add(const ResourceName &name);
    void add(const ResourceName resource_names[]);

    void clear();

    std::shared_ptr<T> get(const std::string &name);
    std::shared_ptr<T> operator[](const std::string &name);

  private:
    std::map<std::string, Resource<T>> resources;
    std::shared_ptr<T> load(Resource<T> &data);
};

using ImageManager = ResourceManager<ALLEGRO_BITMAP>;
using SampleManager = ResourceManager<Sample>;

extern ImageManager images;
extern SampleManager samples;

// vi: ft=cpp
