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
#include "GameState.h"
#include "Label.h"
#include "Module.h"
#include "ResourceManager.h"

class ModuleQuestLog : public Module {
  public:
    ModuleQuestLog();
    virtual ~ModuleQuestLog();
    virtual bool on_update() override;

  private:
    ResourceManager<ALLEGRO_BITMAP> m_resources;
    std::shared_ptr<Bitmap> m_background;

    std::shared_ptr<Label> m_quest_name;
    std::shared_ptr<Label> m_quest_desc;
    std::shared_ptr<Label> m_quest_status;
};

#endif
