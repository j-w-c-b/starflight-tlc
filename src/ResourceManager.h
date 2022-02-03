#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include <allegro5/allegro.h>

struct ResourceName {
    std::string name;
    std::string path;
};

template <class T> struct Resource : public ResourceName {
    std::unique_ptr<T, void (*)(T *)> data;
    static void deleter(T *);

    Resource() : data(nullptr, deleter){};
    Resource(const ResourceName &r) : ResourceName(r), data(nullptr, deleter){};
};

template <class T> class ResourceManager {
  public:
    explicit ResourceManager(const ResourceName resource_names[]);
    virtual ~ResourceManager();

    bool load();
    void unload();
    void add(const ResourceName &name);
    T *get(const std::string &name);
    T *operator[](const std::string &name);

  private:
    std::map<std::string, Resource<T>> resources;
    T *load(Resource<T> &data);
};
