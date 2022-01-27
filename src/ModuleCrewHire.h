/*
        STARFLIGHT - THE LOST COLONY
        ModuleCrewHire.h - ?
        Author: ?
        Date: 9/21/07
*/

#ifndef MODULECREWHIRE_H
#define MODULECREWHIRE_H

#include "Button.h"
#include "GameState.h"
#include "Label.h"
#include "Module.h"
#include "ResourceManager.h"
#include "ScrollBox.h"
#include "tinyxml/tinyxml.h"
#include <allegro5/allegro.h>

class ModuleCrewHire : public Module {
  public:
    ModuleCrewHire();
    virtual ~ModuleCrewHire();
    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;

    Officer *FindOfficerType(OfficerType type);

  private:
    void RefreshUnassignedCrewBox();
    void RefreshUnemployedCrewBox();
    void DrawOfficerInfo(Officer *officer);

    static std::string c_directions;
    static std::string c_hire_more_directions;
    static std::string c_title;
    static std::string c_statistics_title;

    int currentScreen;
    int selectedPosition;
    int selectedPositionLastRun;
    int selectedEntryLastRun;
    int FALSEHover;
    int lastEmployeeSpawn;
    int currentVisit;

    Label *m_title;
    Label *m_slogan;
    Label *m_directions;

    Button *m_exitBtn;
    Button *m_hireBtn;
    Button *m_hiremoreBtn;
    Button *m_fireBtn;
    Button *m_unassignBtn;
    Button *m_backBtn;

    Button *m_PositionBtns[8];
    ALLEGRO_BITMAP *posNormImages[8];
    ALLEGRO_BITMAP *posOverImages[8];
    ALLEGRO_BITMAP *posDisImages[8];

    std::vector<Officer *> tOfficers;

    Officer *selectedOfficer;

    ScrollBox::ScrollBox *unassignedCrew;
    ScrollBox::ScrollBox *unemployed;
    ScrollBox::ScrollBox *unemployedType;

    ScrollBox::ColoredString coloredString;
    ResourceManager<ALLEGRO_BITMAP> resources;
};

#endif
