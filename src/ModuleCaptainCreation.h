#ifndef MODULECAPTAINCREATION_H
#define MODULECAPTAINCREATION_H
#pragma once

#include "AudioSystem.h"
#include "GameState.h"
#include "Module.h"
#include "ResourceManager.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

class Button;
class Label;

class ModuleCaptainCreation : public Module {
  public:
    ModuleCaptainCreation(void);
    virtual ~ModuleCaptainCreation(void);
    virtual bool Init() override;
    virtual void Update() override;
    virtual void Draw() override;
    virtual void OnKeyPress(int keyCode) override;
    virtual void OnKeyPressed(int keyCode) override;
    virtual void OnKeyReleased(int keyCode) override;
    virtual void OnMouseMove(int x, int y) override;
    virtual void OnMouseClick(int button, int x, int y) override;
    virtual void OnMousePressed(int button, int x, int y) override;
    virtual void OnMouseReleased(int button, int x, int y) override;
    virtual void OnMouseWheelUp(int x, int y) override;
    virtual void OnMouseWheelDown(int x, int y) override;
    virtual void OnEvent(Event *event) override;
    virtual void Close() override;

  private:
    void chooseFreelance();
    void chooseMilitary();
    void chooseScience();

    typedef enum
    {
        WP_NONE = 0,
        WP_PROFESSION_CHOICE = 1,
        WP_DETAILS = 2
    } WizPage;

    WizPage m_wizPage;

    ALLEGRO_BITMAP *m_professionChoiceBackground;

    ALLEGRO_BITMAP *m_scientificBtn;
    ALLEGRO_BITMAP *m_scientificBtnMouseOver;
    ALLEGRO_BITMAP *m_freelanceBtn;
    ALLEGRO_BITMAP *m_freelanceBtnMouseOver;
    ALLEGRO_BITMAP *m_militaryBtn;
    ALLEGRO_BITMAP *m_militaryBtnMouseOver;

    Label *m_profInfoScientific;
    Label *m_profInfoFreelance;
    Label *m_profInfoMilitary;

    Label *m_profInfoBox;

    ALLEGRO_BITMAP *m_detailsBackground;

    ALLEGRO_BITMAP *m_plusBtn;
    ALLEGRO_BITMAP *m_plusBtnMouseOver;

    ALLEGRO_BITMAP *m_resetBtn;
    ALLEGRO_BITMAP *m_resetBtnMouseOver;

    Button *m_finishBtn;

    ALLEGRO_BITMAP *m_cursor[2];
    int m_cursorIdx;
    int m_cursorIdxDelay;

    ALLEGRO_BITMAP *m_backBtn;
    ALLEGRO_BITMAP *m_backBtnMouseOver;

    ALLEGRO_BITMAP *m_mouseOverImg;
    int m_mouseOverImgX;
    int m_mouseOverImgY;

    std::shared_ptr<Sample> m_sndBtnClick;
    std::shared_ptr<Sample> m_sndClick;
    std::shared_ptr<Sample> m_sndErr;

    // in progress captain vars; once finished with creation, these
    // get stored to the game state
    ProfessionType m_profession;
    std::string m_name;
    Attributes m_attributes;

    // intermediate captain vars used while creating the captain
    Attributes m_attributesMax;
    Attributes m_attributesInitial;
    int m_availPts;
    int m_availProfPts;

    Button *m_minusBtns[8];
    ResourceManager<ALLEGRO_BITMAP> m_resources;
};

#endif
