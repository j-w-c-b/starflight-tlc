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
#include "Module.h"
#include "ResourceManager.h"
#include "ScrollBox.h"
#include "tinyxml/tinyxml.h"
#include <allegro5/allegro.h>

class Label;

class ModuleCrewHire : public Module {
  public:
    ModuleCrewHire();
    virtual ~ModuleCrewHire();
    virtual bool Init() override;
    virtual void Update() override;
    virtual void Draw() override;
    virtual void OnMouseMove(int x, int y) override;
    virtual void OnMouseClick(int button, int x, int y) override;
    virtual void OnMousePressed(int button, int x, int y) override;
    virtual void OnMouseReleased(int button, int x, int y) override;
    virtual void OnMouseWheelUp(int x, int y) override;
    virtual void OnMouseWheelDown(int x, int y) override;
    virtual void OnEvent(Event *event) override;
    virtual void Close() override;

    Officer *FindOfficerType(OfficerType type);

  private:
    void RefreshUnassignedCrewBox();
    void RefreshUnemployeedCrewBox();
    void DrawOfficerInfo(Officer *officer);

    int currentScreen;
    int selectedPosition;
    int selectedPositionLastRun;
    int selectedEntryLastRun;
    int FALSEHover;
    int lastEmployeeSpawn;
    int currentVisit;

    Label *title;
    Label *slogan;
    Label *directions;
    Label *hiremoreDirections;
    Label *stats;

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
    ScrollBox::ScrollBox *unemployeed;
    ScrollBox::ScrollBox *unemployeedType;

    ScrollBox::ColoredString coloredString;
    ResourceManager<ALLEGRO_BITMAP> resources;
};

#endif
