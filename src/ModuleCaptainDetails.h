#pragma once

#include <string>

#include <allegro5/allegro.h>

#include "Button.h"
#include "Event.h"
#include "Label.h"
#include "Module.h"
#include "ResourceManager.h"

class ModuleCaptainAttribute : public Module {
  public:
    ModuleCaptainAttribute(
        const std::string &name,
        int value,
        int max,
        int x,
        int y,
        int width,
        int height,
        EventType plus_event,
        EventType minus_event,
        ResourceManager<ALLEGRO_BITMAP> &resources);

    int get_value() const { return m_value; }

    bool plus() {
        if (m_value < m_max) {
            ++m_value;
            m_value_label->set_text(std::to_string(m_value));
            return true;
        } else {
            return false;
        }
    }
    bool minus() {
        if (m_value > m_min) {
            --m_value;
            m_value_label->set_text(std::to_string(m_value));
            return true;
        } else {
            return false;
        }
    }
    void reset() {
        if (m_value != m_min) {
            m_value = m_min;
            m_value_label->set_text(std::to_string(m_value));
        }
    }

  private:
    Label *m_name_label;
    Label *m_value_label;
    NewButton *m_plus_button;
    NewButton *m_minus_button;
    int m_value;
    int m_min;
    int m_max;
};

class ModuleCaptainAttributeGroup : public Module {
  public:
    ModuleCaptainAttributeGroup(
        int x,
        int y,
        int width,
        int height,
        int max,
        ResourceManager<ALLEGRO_BITMAP> &resources);

    void add_attribute(
        const std::string &name,
        int value,
        int max,
        EventType plus_event,
        EventType minus_event);

    int get_value(const std::string &name) {
        auto i = m_attributes.find(name);

        return i->second->get_value();
    }

    void reset() {
        for (auto i : m_attributes) {
            i.second->reset();
        }
        m_sum = 0;
    }

    int get_available_points() const { return m_max - m_sum; }

    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    int m_sum;
    int m_max;
    std::map<const std::string, ModuleCaptainAttribute *> m_attributes;
    std::map<const EventType, ModuleCaptainAttribute *> m_attributes_by_plus;
    std::map<const EventType, ModuleCaptainAttribute *> m_attributes_by_minus;
    ResourceManager<ALLEGRO_BITMAP> &m_resources;
    Label *m_available_label;
};

class ModuleCaptainDetails : public Module {
  public:
    explicit ModuleCaptainDetails(ResourceManager<ALLEGRO_BITMAP> &resources);
    virtual ~ModuleCaptainDetails();

  private:
    ModuleCaptainAttributeGroup *m_physical_attributes;
    ModuleCaptainAttributeGroup *m_professional_attributes;
    ;
    Button *m_finishBtn;
};
