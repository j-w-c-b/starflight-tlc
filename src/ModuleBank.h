/*
	STARFLIGHT - THE LOST COLONY
	ModuleBank.h - The Bank module.
	Author: Keith "Daikaze" Patch
	Date: ?-?-2007
*/
#pragma once

#include <list>

#include <allegro5/allegro.h>
#include "Module.h"
#include "Stardate.h"
#include "Button.h"
#include "Events.h"
#include "ModeMgr.h"
#include "GameState.h"
#include "Game.h"
#include "ScrollBox.h"
#include "ResourceManager.h"


#define NUM_CALC_BUTTONS 11

//bank needs punishment
//bank needs credit scoring and interest rates influenced by credit scores
//bank needs time delay before taking new loan

//score will be based on player purchases with the trade depot, but majorly influenced by previous loans.


class ModuleBank : public Module{
public:
	ModuleBank(void);
	bool Init() override;
	void Update() override;
	void Draw() override;
	void OnKeyPress(int keyCode) override;
	void OnKeyPressed(int keyCode) override;
	void OnKeyReleased(int keyCode) override;
	void OnMouseMove(int x, int y) override;
	void OnMouseClick(int button, int x, int y) override;
	void OnMousePressed(int button, int x, int y) override;
	void OnMouseReleased(int button, int x, int y) override;
	void OnMouseWheelUp(int x, int y) override;
	void OnMouseWheelDown(int x, int y) override;
	void OnEvent(Event *event) override;
	void Close() override;

	void take_loan(); 
	void pay_loan();
	bool init_images();
	bool init_buttons();
	void render_images();
	void render_text();
	void push_digit(int value);
	bool is_overdue();

	bool PerformCreditCheck();

	private:
	~ModuleBank(void);
        ResourceManager<ALLEGRO_BITMAP>         resources;
	int					i_original_loan,		//value of the loan before interest
						i_max_loan,				//what is the value of the selected loan
						i_amount_owed,			//value of loan with interest
						i_time_lapsed,			//amount of time passed between interest updates
						i_last_time,			//the last time the player visited the bank
						i_minimum_payment;		//minimum amount to be payed
	bool				b_has_loan,				//does the player have a loan?
						b_considering_pay,		//does the player wish to pay the loan?
						b_considering_take;		//does the player wish to pay the loan;
	ALLEGRO_BITMAP				*bmp_bank_background,	//background image
						*bmp_bank_banner;
	Stardate			date_taken,				//date that the loan was taken
						m_due_date;				//when is the next payment due
	float				f_interest_rate;		//interest rate of loan
	Button				*exit_button,			//static button
						*calc_buttons[NUM_CALC_BUTTONS],
						*pay_button,
						*take_button,
						*confirm_button,
						*help_button;
	std::list<int>		digit_list;

	ScrollBox::ScrollBox    *m_help_window;
	ALLEGRO_BITMAP					*bmp_help_window;
	bool					b_help_visible;
	bool m_bWarned;
};
