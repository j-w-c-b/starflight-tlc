/*
        STARFLIGHT - THE LOST COLONY
        ModuleStarmap.h - The QuestLog Module
        Author: Justin Sargent
        Date: Nov-24-2007
*/

#ifndef MODULEQUESTLOG_H
#define MODULEQUESTLOG_H

#include "GameState.h"
#include "Label.h"
#include "Module.h"
#include "ResourceManager.h"
#include <allegro5/allegro.h>

class ModuleQuestLog : public Module {
  public:
    ModuleQuestLog();
    virtual ~ModuleQuestLog();
    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    bool log_active;
    int viewer_offset_x, viewer_offset_y;

    ALLEGRO_BITMAP *window;

    Label *m_quest_name;
    Label *m_quest_desc;
    Label *m_quest_status;

    ResourceManager<ALLEGRO_BITMAP> resources;
};

#endif
