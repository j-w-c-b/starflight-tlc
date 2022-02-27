/*
        STARFLIGHT - THE LOST COLONY
        ModuleStarmap.h - The QuestLog Module
        Author: Justin Sargent
        Date: Nov-24-2007
*/

#ifndef MODULEQUESTLOG_H
#define MODULEQUESTLOG_H

#include <memory>

#include <allegro5/allegro.h>

#include "Bitmap.h"
#include "Label.h"
#include "Module.h"
#include "SlidingModule.h"

class ModuleQuestLog : public Module {
  public:
    ModuleQuestLog();
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_update() override;

  private:
    std::shared_ptr<SlidingModule<Bitmap>> m_viewer;

    std::shared_ptr<Label> m_quest_name;
    std::shared_ptr<Label> m_quest_desc;
    std::shared_ptr<Label> m_quest_status;
    bool m_is_active;
};

#endif
