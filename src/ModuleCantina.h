/*
	STARFLIGHT - THE LOST COLONY
	ModuleCrewHire.h - ?
	Author: ?
	Date: 9/21/07
*/

#ifndef MODULECANTINA_H
#define MODULECANTINA_H

#include <allegro5/allegro.h>
#include "Module.h"
#include "GameState.h"
#include "Button.h"
#include "ScrollBox.h"
#include "Label.h"
#include "ResourceManager.h"

class ModuleCantina : public Module
{
public:
	ModuleCantina();
	virtual ~ModuleCantina();
	virtual bool Init() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void OnKeyReleased(int keyCode) override;
	virtual void OnMouseMove(int x, int y) override;
	virtual void OnMouseReleased(int button, int x, int y) override;
	virtual void OnEvent(Event *event) override;
	virtual void Close() override;

private:
	ALLEGRO_BITMAP 					*m_background;
	Button 					*m_exitBtn;
	Button 					*m_turninBtn;

	Label *questTitle;
	Label *questLong;
	Label *questReward;

	std::string label1, label2, label3, label4;
	ALLEGRO_COLOR labelcolor, textcolor;
	bool selectedQuestCompleted;
	std::string requirementLabel;
	ALLEGRO_COLOR requirementColor;
	ResourceManager<ALLEGRO_BITMAP> resources;
};

#endif
