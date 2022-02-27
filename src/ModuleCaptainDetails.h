#ifndef MODULE_CAPTAIN_DETAILS_H
#define MODULE_CAPTAIN_DETAILS_H

#include <memory>
#include <string>

#include <allegro5/allegro.h>

#include "Button.h"
#include "Events.h"
#include "GameState.h"
#include "Label.h"
#include "Module.h"
#include "TextEntry.h"

using namespace std;

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
        EventType minus_event);

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

  protected:
    bool on_draw(ALLEGRO_BITMAP *) override { return true; }

  private:
    std::shared_ptr<Label> m_name_label;
    std::shared_ptr<Label> m_value_label;
    std::shared_ptr<Button> m_plus_button;
    std::shared_ptr<Button> m_minus_button;

    int m_value;
    int m_min;
    int m_max;
};

class ModuleCaptainAttributeGroup : public Module {
  public:
    ModuleCaptainAttributeGroup(int x, int y, int width, int max);

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

    void clear();
    void reset();
    void do_resize();

    int get_available_points() const { return m_max - m_sum; }

    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    int m_sum;
    int m_max;
    std::map<const std::string, std::shared_ptr<ModuleCaptainAttribute>>
        m_attributes;
    std::map<const EventType, std::shared_ptr<ModuleCaptainAttribute>>
        m_attributes_by_plus;
    std::map<const EventType, std::shared_ptr<ModuleCaptainAttribute>>
        m_attributes_by_minus;
    std::shared_ptr<Label> m_available_label;
};

class ModuleCaptainDetails : public Module {
  public:
    using Attributes = std::map<Skill, int>;
    ModuleCaptainDetails();
    virtual ~ModuleCaptainDetails() {}
    void setup_attributes(
        const Attributes &initial_attributes,
        const Attributes &max_attributes);

    bool on_event(ALLEGRO_EVENT *event) override;

    std::string get_name() const;
    Attributes get_attributes() const;

    void reset();

  private:
    std::shared_ptr<Bitmap> m_background;
    std::shared_ptr<Label> m_title_label;
    std::shared_ptr<TextEntry> m_name_entry;
    std::shared_ptr<class ModuleCaptainAttributeGroup> m_physical_attributes;
    std::shared_ptr<class ModuleCaptainAttributeGroup>
        m_professional_attributes;
    std::shared_ptr<Button> m_reset_button;
    std::shared_ptr<Button> m_accept_button;
};
#endif // MODULE_CAPTAIN_DETAILS_H
// vi: ft=cpp
