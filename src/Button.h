#ifndef BUTTON_H
#define BUTTON_H
#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "AudioSystem.h"
#include "Game.h"
#include <string>

/**
 * re-usable button class
 */
class Button
{
public:

	//ctors
	Button(
		const std::string &initImgFileNormal, 
		const std::string &initImgFileMouseOver, 
		const std::string &initImgFileDisabled, 
		int initX, 
		int initY, 
		int initMouseOverEvent, 
		int initClickEvent, 
		const std::string &initButtonSound = "", 
		bool initEnabled = true, 
		bool initVisible = true);

	Button(
		const std::string &initImgFileNormal, 
		const std::string &initImgFileMouseOver, 
		const std::string &initImgFileDisabled,
		int initX, 
		int initY, 
		int initMouseOverEvent, 
		int initClickEvent, 
		ALLEGRO_FONT *initFontPtr, 
		const std::string &initButtonText, 
		ALLEGRO_COLOR initTextColor, 
		const std::string &initButtonSound = "", 
		bool initEnabled = true, 
		bool initVisible = true);

	Button(
		ALLEGRO_BITMAP *initImgBMPNormal, 
		ALLEGRO_BITMAP *initImgBMPMouseOver, 
		ALLEGRO_BITMAP *initImgBMPDisabled, 
		int initX, 
		int initY, 
		int initMouseOverEvent, 
		int initClickEvent, 
		const std::string &initButtonSound = "", 
		bool initEnabled = true, 
		bool initVisible = true);

	Button(
		ALLEGRO_BITMAP *initImgBMPNormal, 
		ALLEGRO_BITMAP *initImgBMPMouseOver, 
		ALLEGRO_BITMAP *initImgBMPDisabled, 
		int initX, 
		int initY, 
		int initMouseOverEvent, 
		int initClickEvent, 
		ALLEGRO_FONT *initFontPtr, 
		const std::string &initButtonText,
		ALLEGRO_COLOR initTextColor, 
		const std::string &initButtonSound = "", 
		bool initEnabled = true, 
		bool initVisible = true);

	virtual ~Button();

	//accessors
	ALLEGRO_BITMAP * GetImgNormal() const;
	ALLEGRO_BITMAP * GetImgMouseOver() const;
	ALLEGRO_BITMAP * GetImgDisabled() const;
	int	GetX() const;
	int	GetY() const;
	bool IsVisible() const;
	std::string GetButtonText() const;
	bool IsInitialized() const;
	int	GetWidth() const;
	int	GetHeight() const;

	//mutators
	void SetX(int initX);
	void SetY(int initY);
	void SetClickEvent(int initClickEvent);
	void SetEnabled(bool enabled);
	void SetVisible(bool visible);	
	void SetButtonText(const std::string &initButtonText);	
	void SetTextColor(ALLEGRO_COLOR initTextColor);  	
	void SetHighlight(bool initHighlight);	

	//functions
	void Destroy();

	bool Run(ALLEGRO_BITMAP *canvas);
	bool OnMouseMove(int initX, int initY);
	bool OnMouseReleased(int button, int initX, int initY);
	bool PtInBtn(int initX, int initY);

private:

	ALLEGRO_BITMAP			*imgNormal;
	ALLEGRO_BITMAP			*imgMouseOver;
	ALLEGRO_BITMAP			*imgDisabled;
	std::string		buttonSound;
	int				x;
	int				y;
	int				mouseOverEvent;
	int				clickEvent;
	bool			enabled;
	bool			visible;
	std::string		buttonText;
	bool			initialized;
	bool			deleteBitmaps;
	bool			highlight;
	int				lastMouseX;
	int				lastMouseY;
	ALLEGRO_FONT		*fontPtr;
	ALLEGRO_COLOR			textColor;

	//private functions

};

#endif
