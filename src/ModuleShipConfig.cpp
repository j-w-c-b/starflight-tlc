/*
	STARFLIGHT - THE LOST COLONY
	ModuleStarport.cpp - Handles Starport activity
	Author: Scott Idler
	Date: June 29, 2007

	This module is the common starport area, where the user gains access to the
	areas where he can create his character, customize his ship, hire a crew,
	go to the bank, trade goods and receive orders.
*/

#include "ModuleShipConfig.h"
#include "AudioSystem.h"
#include "Game.h"
#include "Events.h"
#include "ModeMgr.h"
#include "DataMgr.h"
#include "QuestMgr.h"
#include "Util.h"
#include "shipconfig_resources.h"

using namespace std;

ALLEGRO_DEBUG_CHANNEL("ModuleShipConfig")

#define SHIPNAME_MAXLEN 20

ModuleShipConfig::ModuleShipConfig(void) : m_resources(SHIPCONFIG_IMAGES)
{
}

//Init is a good place to load resources
bool ModuleShipConfig::Init()
{
	ALLEGRO_DEBUG("  ShipConfig Initialize\n");
	
	//load the datafile
	if (!m_resources.load()) {
		g_game->message("ShipConfig: Error loading resources");
		return false;
	}

	inputName = false;

	//create button images
	ALLEGRO_BITMAP *btnNorm, *btnOver, *btnDeact;
	btnNorm = m_resources[SHIPCONFIG_BTN_NORM];
	btnOver = m_resources[SHIPCONFIG_BTN_OVER];
	btnDeact = m_resources[SHIPCONFIG_BTN_DEACTIVE];

	//initialize array of button ptrs
	for(int i=0; i<NUMBER_OF_BUTTONS; ++i)
	{
		if(i < NUMBER_OF_BUTTONS)
		{
			buttons[i] = new Button(btnNorm, btnOver, btnDeact, 
				BUTTON_X_START, BUTTON_Y_START+i*(BUTTON_HEIGHT+PIXEL_BUFFER), 0, 0, g_game->font22, "def", al_map_rgb(0,255,0));
		}
		if(buttons[i])
		{
			if( !buttons[i]->IsInitialized() ) 
				return false;
		}
		else
			return false;
	}

	//setup up the pathing
	menuPath.clear();
	Event e(ModuleEntry);
	g_game->modeMgr->BroadcastEvent(&e);

	//load background image
	shipConfig = m_resources[SHIPCONFIG];

	//load ship image
	switch(g_game->gameState->getProfession()) {
		case PROFESSION_FREELANCE:	shipImage = m_resources[FREELANCE];
		case PROFESSION_MILITARY:	shipImage = m_resources[MILITARY];
		case PROFESSION_SCIENTIFIC:	shipImage = m_resources[SCIENCE];
		default:
			ALLEGRO_ERROR("***ERROR: ShipConfig: Player's profession is invalid.");
	}


	//load audio files
	m_sndClick = g_game->audioSystem->Load("data/shipconfig/click.ogg");
	if (!m_sndClick) {
		g_game->message("ShipConfig: Error loading click.ogg");
		return false;
	}
	m_sndErr = g_game->audioSystem->Load("data/shipconfig/error.ogg");
	if (!m_sndErr) {
		g_game->message("ShipConfig: Error loading error.ogg");
		return false;
	}
	m_cursor = m_resources[SHIPCONFIG_CURSOR0];

	//tell questmgr that this module has been entered
	g_game->questMgr->raiseEvent(22);

	return true;
}

void ModuleShipConfig::OnKeyPressed(int keyCode)
{
	ALLEGRO_KEYBOARD_STATE keyboard_state;

	if (inputName)
	{
		bool playKeySnd = false;
		bool playErrSnd = false;

		al_get_keyboard_state(&keyboard_state);
		bool shifted = al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT)
			|| al_key_down(&keyboard_state, ALLEGRO_KEY_RSHIFT);

		if (((keyCode >= ALLEGRO_KEY_A) && (keyCode <= ALLEGRO_KEY_PAD_9)) || (keyCode == ALLEGRO_KEY_SPACE))
		{
		if (shipName.size() < SHIPNAME_MAXLEN)
		{
			char c;
			if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z)
			{
				c = (keyCode - ALLEGRO_KEY_A) + (shifted ?'A': 'a');
			}
			else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9)
			{
				c = (keyCode - ALLEGRO_KEY_0) + '0';
			}
			else if (keyCode >= ALLEGRO_KEY_PAD_0 && keyCode <= ALLEGRO_KEY_PAD_9)
			{
				c = (keyCode - ALLEGRO_KEY_PAD_0) + '0';
			}
			else
			{
				c = ' ';
			}

			shipName.push_back(c);

			playKeySnd = true;
		}
		else
			playErrSnd = true;
		}
		else if (keyCode == ALLEGRO_KEY_BACKSPACE)
		{
		if (shipName.size() > 0)
		{
			shipName.erase(--(shipName.end()));

			playKeySnd = true;
		}
		else
			playErrSnd = true;
		}

		if (playKeySnd)
		{		
			g_game->audioSystem->Play(m_sndClick);
		}

		if (playErrSnd)
		{		
			g_game->audioSystem->Play(m_sndErr); 
		}

	}
}

void ModuleShipConfig::OnMouseMove(int x, int y)
{ 
	for(int i=0; i<buttonsActive; ++i)
		buttons[i]->OnMouseMove(x, y);
}
void ModuleShipConfig::OnMouseReleased(int button, int x, int y)	
{ 
	for(int i=0; i<buttonsActive; ++i)
		if(buttons[i]->OnMouseReleased(button, x, y) )
			return;
}

void ModuleShipConfig::OnEvent(Event *event)
{
	int evnum, maxclass = -1;
	Event e;

	//check for general events
	switch(event->getEventType() ) 
	{
		case UndefButtonType:
			break;		
		case ModuleEntry:
			menuPath.push_back(ModuleEntry);
			buttonsActive = 4;
			configureButton(0, ShipConfig);
			configureButton(1, TVConfig);
			configureButton(2, Launch);
			configureButton(3, Exit);
			break;

		case ShipConfig:
			menuPath.push_back(ShipConfig);
			buttonsActive = 5;
			configureButton(0, Buy);
			configureButton(1, Sell);
			configureButton(2, Repair);
			configureButton(3, Name);
			configureButton(4, Back);
			break;

		case Launch:
			if(g_game->gameState->PreparedToLaunch() )
            {
				ID starid = 2;
				g_game->gameState->player->currentStar = starid;
                                int start_pos_x = g_game->getGlobalNumber("PLAYER_HYPERSPACE_START_X");
                                int start_pos_y = g_game->getGlobalNumber("PLAYER_HYPERSPACE_START_Y");
				g_game->gameState->player->set_galactic_pos(start_pos_x, start_pos_y);

				//compute myrrdan position
				int orbitalpos = 3;
				int tilesacross=100, tilesdown=100, tileswidth=256, tilesheight=256;
				int starX=tilesacross/2, starY=tilesdown/2;

				srand(starid);
				for (int i = 0; i < orbitalpos; i++) rand();
				float radius = (2 + orbitalpos) * 4;
				float angle = rand() % 360;
				int rx = (int)( cos(angle) * radius );
				int ry = (int)( sin(angle) * radius );

				//set player position to start near myrrdan
				g_game->gameState->player->posSystem.SetPosition( (starX+rx)*tileswidth -256, (starY+ry)*tilesheight -135);

				g_game->LoadModule(MODULE_INTERPLANETARY);
			}else{
				g_game->ShowMessageBoxWindow("Not prepared to launch! Make sure you have an engine and a crew.");
			}
			return;
			break;

		case Buy:
			menuPath.push_back(Buy);
			buttonsActive = 7;
			configureButton(0, CargoPods);
			configureButton(1, Engines);
			configureButton(2, Shields);
			configureButton(3, Armor);
			configureButton(4, Missiles);
			configureButton(5, Lasers);
			configureButton(6, Back);
			break;

		case Sell:
			menuPath.push_back(Sell);
			buttonsActive = 7;
			configureButton(0, CargoPods);
			configureButton(1, Engines);
			configureButton(2, Shields);
			configureButton(3, Armor);
			configureButton(4, Missiles);
			configureButton(5, Lasers);
			configureButton(6, Back);
			break;

		case Repair:
        {
            //calculate repair cost
            repairCost = 0;
            Ship ship = g_game->gameState->m_ship;
            if (ship.getHullIntegrity() < 100)
                repairCost += getHullRepair();
            if (ship.getArmorIntegrity() < 100)
                repairCost += getArmorRepair();
            if (ship.getEngineIntegrity() < 100)
                repairCost += getEngineRepair();
            if (ship.getShieldIntegrity() < 100)
                repairCost += getShieldRepair();
            if (ship.getLaserIntegrity() < 100)
                repairCost += getLaserRepair();
            if (ship.getMissileLauncherIntegrity() < 100)
                repairCost += getMissileRepair();

			menuPath.push_back(Repair);
			buttonsActive = 7;
			configureButton(0, Engines);
			configureButton(1, Shields);
			configureButton(2, Armor);
			configureButton(3, Missiles);
			configureButton(4, Lasers);
            configureButton(5, Hull);
			configureButton(6, Back);
        }
			break;

		case Name:
			menuPath.push_back(Name);
			buttonsActive = 2;
			configureButton(0, SaveName);
			configureButton(1, Nevermind);
			inputName = true;
			shipName = g_game->gameState->m_ship.getName();	
			break;

		case Exit:
			g_game->LoadModule(MODULE_STARPORT);
			return;
			break;

		case CargoPods:
			if (menuPath[2] == Buy 
                && g_game->gameState->m_ship.getCargoPodCount() < MAX_CARGOPODS 
                && g_game->gameState->m_credits >= CARGOPODS )
			{
				g_game->gameState->m_ship.cargoPodPlusPlus();
				g_game->gameState->m_credits -= CARGOPODS;
			}
			else if (menuPath[2] == Sell 
                && g_game->gameState->m_ship.getCargoPodCount() > 0)
			{
				if (g_game->gameState->m_ship.getAvailableSpace() < POD_CAPACITY)
					g_game->ShowMessageBoxWindow("", "You can't sell any of your cargo pods--you have too many items in the hold.");
				else {
					g_game->gameState->m_ship.cargoPodMinusMinus();
					g_game->gameState->m_credits += CARGOPODS;
				}
			}
			break;

		//buy/sell ship components
		case Engines:
		case Shields:
		case Armor:
		case Missiles:
		case Lasers:
        case Hull:
			menuPath.push_back( (ButtonType)event->getEventType() );
			if(menuPath[2] == Buy) 
            {
				buttonsActive = 7;
				configureButton(0, Class1);
				configureButton(1, Class2);
				configureButton(2, Class3);
				configureButton(3, Class4);
				configureButton(4, Class5);
				configureButton(5, Class6);
				configureButton(6, Back);

				//limit purchase to maximum class by profession
				evnum = event->getEventType();
				switch(evnum) {
					case Engines:
						maxclass = g_game->gameState->m_ship.getMaxEngineClass();
						break;
					case Shields:
						maxclass = g_game->gameState->m_ship.getMaxShieldClass();
						break;
					case Armor:
						maxclass = g_game->gameState->m_ship.getMaxArmorClass();
						break;
					case Missiles:
						maxclass = g_game->gameState->m_ship.getMaxMissileLauncherClass();
						break;
					case Lasers:
						maxclass = g_game->gameState->m_ship.getMaxLaserClass();
						break;
					default:
						ALLEGRO_ASSERT(0);
				}

				for (int n=6; n>maxclass; n--) {
					configureButton(n-1, UndefButtonType);
				}

			}
			else if(menuPath[2] == Sell) 
            {
				if (checkComponent()) 
                {
					sellComponent();
					menuPath.pop_back();
				}
				else g_game->ShowMessageBoxWindow("", "You don't have one to sell!", 400, 200);

				menuPath.pop_back();
				e = menuPath.back();
				menuPath.pop_back();
				g_game->modeMgr->BroadcastEvent(&e);
			}
            else if (menuPath[2] == Repair)
            {
                if (checkComponent())
                {
                    repairComponent();
                    menuPath.pop_back();
                }
                else g_game->ShowMessageBoxWindow("", "You don't have one to repair!", 400, 200);

                menuPath.pop_back();
                e = menuPath.back();
                menuPath.pop_back();
                g_game->modeMgr->BroadcastEvent(&e);
            }
			break;

		case Back:
			inputName = false;
			menuPath.pop_back();
			e = menuPath.back();
			menuPath.pop_back();
			g_game->modeMgr->BroadcastEvent(&e);
			break;

		//buy new class of component
		case Class1:
		case Class2:
		case Class3:
		case Class4:
		case Class5:
		case Class6:
			menuPath.push_back( (ButtonType)event->getEventType() );
			if(menuPath[2] == Buy)
			{
				if(!checkComponent() )
				{
					buyComponent();
					menuPath.pop_back();
					menuPath.pop_back();
				}
			}
			menuPath.pop_back();
			e = menuPath.back();
			menuPath.pop_back();
			g_game->modeMgr->BroadcastEvent(&e);
			break;

		case Nevermind:
			menuPath.pop_back();
			e = menuPath.back();
			menuPath.pop_back();
			g_game->modeMgr->BroadcastEvent(&e);
			break;

		case SaveName:
			if(menuPath[2] == Name)
			{
				if (shipName != "")
				{
					g_game->gameState->m_ship.setName(shipName);

					menuPath.pop_back();
					e = menuPath.back();
					menuPath.pop_back();
					g_game->modeMgr->BroadcastEvent(&e);
				}
				else
				{
					g_game->ShowMessageBoxWindow("", "You must first christen your ship!", 400, 200);
				}
			}
			break;

		case TVConfig:
			menuPath.push_back(TVConfig);
			buttonsActive = 2;
			configureButton(0, BuyTV);
			configureButton(1, Back);
			break;

		case BuyTV:
			if (g_game->gameState->m_ship.getHasTV())
			{
				g_game->ShowMessageBoxWindow("", "You already own a Terrain Vehicle!", 400, 200);
			}
			else
			{
				if (g_game->gameState->getCredits() >= 2000)
				{
					g_game->gameState->m_ship.setHasTV(true);
					g_game->gameState->augCredits(-2000);
					
					menuPath.pop_back();
					e = menuPath.back();
					menuPath.pop_back();
					g_game->modeMgr->BroadcastEvent(&e);
				}
				else
				{
					g_game->ShowMessageBoxWindow("", "A new Terrain Vehicle costs 2000 credits.", 400, 200);
				}
			}
			break;

		default:
			break;
	}
}


void ModuleShipConfig::Close()
{
	ALLEGRO_DEBUG("ShipConfig Destroy\n");

	try {
		menuPath.clear();
		for(int a = 0; a < NUMBER_OF_BUTTONS; ++a)
		{
			buttons[a]->Destroy();
			buttons[a] = NULL;
		}

		m_sndClick.reset();
		m_sndErr.reset();
		
		//unload the data file
		m_resources.unload();
	}
	catch(std::exception e) {
		ALLEGRO_DEBUG("%s\n", e.what());
	}
	catch(...) {
		ALLEGRO_DEBUG("Unhandled exception in ShipConfig::Close\n");
	}	
	
}
std::string ModuleShipConfig::convertMenuPathToString() const
{
	if(menuPath.size() < 2)
		return "Error menuPath.size() < 1";
	std::string result = convertButtonTypeToString(menuPath[1]);
	for(int i=2; i<(int)menuPath.size(); ++i)
		result = result + "->" + convertButtonTypeToString(menuPath[i]);
	return result;
}
std::string ModuleShipConfig::convertButtonTypeToString(ButtonType btnType) const
{
	switch(btnType)
	{
	case UndefButtonType: case ModuleEntry:  return "";
	case ShipConfig:	return "Ship Systems";	break;
	case Launch:		return "Launch";		break;
	case Buy:		    return "Buy";		    break;
	case Sell:		    return "Sell";		    break;
	case Repair:		return "Repair";		break;
	case Name:		    return "Name";		    break;
	case Exit:		    return "Exit";		    break;
	case CargoPods:		return "Cargo Pods";	break;
	case Engines:		return "Engines";		break;
	case Shields:		return "Shields";		break;
	case Armor:		    return "Armor";		    break;
	case Missiles:		return "Missiles";		break;
	case Lasers:		return "Lasers";		break;
    case Hull:          return "Hull";          break;
	case Back:		    return "Back";		    break;
	case Class1:		return "Class 1";		break;
	case Class2:		return "Class 2";		break;
	case Class3:		return "Class 3";		break;
	case Class4:		return "Class 4";		break;
	case Class5:		return "Class 5";		break;
	case Class6:		return "Class 6";       break;
	case Pay:   		return "Pay";		    break;
	case Nevermind:		return "Cancel";		break;
	case SaveName:		return "Save Name";		break;
	case TVConfig:		return "Terrain Vehicle";		break;
	case BuyTV:	    	return "Buy T.V. (2000)";		break;
	}
	return "";
}

void ModuleShipConfig::configureButton(int btn, ButtonType btnType)
{
	if(0 <= btn && btn < NUMBER_OF_BUTTONS)
	{
		buttons[btn]->SetClickEvent(btnType);
		if(Class1 <= btnType && btnType <= Class6)
		{
			buttons[btn]->SetButtonText(convertButtonTypeToString(btnType) + 
				"  " + Util::ToString(ITEM_PRICES[menuPath[3] - ITEM_ENUM_DIF][btn]) );

		}	
		else if(btnType == CargoPods)
			buttons[btn]->SetButtonText(convertButtonTypeToString(btnType) + 
				"  " + Util::ToString(CARGOPODS) );
		else
			buttons[btn]->SetButtonText(convertButtonTypeToString(btnType) );
		
		if(btnType == Exit || btnType == Back)
		{
			buttons[btn]->SetX(BOTTOM_CORNER_X);
			buttons[btn]->SetY(BOTTOM_CORNER_Y);
		}
		else
		{
			buttons[btn]->SetX(BUTTON_X_START);
			buttons[btn]->SetY(BUTTON_Y_START+btn*(BUTTON_HEIGHT+PIXEL_BUFFER) );
		}
	}
}
bool ModuleShipConfig::checkComponent() const
{
	if( (int)menuPath.size() < 4) return false;

	switch(menuPath[2])
	{
	case Buy:
		{
			int itemIndex = menuPath[3] - ITEM_ENUM_DIF;
			int classIndex = menuPath[4] - CLASS_ENUM_DIF - 1;
			int cost = ITEM_PRICES[itemIndex][classIndex];

            //can player afford it?
			int cash = g_game->gameState->m_credits;
			if(cash - cost < 0)	return true;

			switch(menuPath[3])
			{
			case Engines: 
                return g_game->gameState->m_ship.getEngineClass() == menuPath[4] - CLASS_ENUM_DIF;
			case Shields: 
                return g_game->gameState->m_ship.getShieldClass() == menuPath[4] - CLASS_ENUM_DIF;
			case Armor: 
                return g_game->gameState->m_ship.getArmorClass() == menuPath[4] - CLASS_ENUM_DIF;
			case Missiles: 
                return g_game->gameState->m_ship.getMissileLauncherClass() == menuPath[3] - CLASS_ENUM_DIF;
			case Lasers: 
                return g_game->gameState->m_ship.getLaserClass() == menuPath[4] - CLASS_ENUM_DIF;
                        default: break;
			}
		}
		break;
	case Sell:
		switch(menuPath[3])
		{
		case Engines:  return g_game->gameState->m_ship.getEngineClass() != NotInstalledType;
		case Shields:  return g_game->gameState->m_ship.getShieldClass() != NotInstalledType;
		case Armor:    return g_game->gameState->m_ship.getArmorClass() != NotInstalledType;
		case Missiles: return g_game->gameState->m_ship.getMissileLauncherClass() != NotInstalledType;
		case Lasers:   return g_game->gameState->m_ship.getLaserClass() != NotInstalledType;
                default: break;
		}
		break;

    case Repair:
        switch(menuPath[3])
        {
		case Engines:  return g_game->gameState->m_ship.getEngineClass() != NotInstalledType;
		case Shields:  return g_game->gameState->m_ship.getShieldClass() != NotInstalledType;
		case Armor:    return g_game->gameState->m_ship.getArmorClass() != NotInstalledType;
		case Missiles: return g_game->gameState->m_ship.getMissileLauncherClass() != NotInstalledType;
		case Lasers:   return g_game->gameState->m_ship.getLaserClass() != NotInstalledType;
        case Hull:     return true; 
        default: break;
        }

    default:
        break;
	}
	return false;
}


int ModuleShipConfig::getEngineValue()
{
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int engine = ship.getEngineClass();
    switch(engine)
    {
        case 0: value = 0; break;
        case 1: value = ENGINE_CLASS1; break;
        case 2: value = ENGINE_CLASS2; break;
        case 3: value = ENGINE_CLASS3; break;
        case 4: value = ENGINE_CLASS4; break;
        case 5: value = ENGINE_CLASS5; break;
        case 6: value = ENGINE_CLASS6; break;
    }
    float health = ship.getEngineIntegrity() * 0.01f;
    health *= 0.75f;
    value = (int)((float)value * health);
    return value;
}

int ModuleShipConfig::getLaserValue()
{
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int laser = ship.getLaserClass();
    switch(laser)
    {
        case 0: value = 0; break;
        case 1: value = LASER_CLASS1; break;
        case 2: value = LASER_CLASS2; break; 
        case 3: value = LASER_CLASS3; break;
        case 4: value = LASER_CLASS4; break;
        case 5: value = LASER_CLASS5; break;
        case 6: value = LASER_CLASS6; break;
    }
    float health = ship.getLaserIntegrity() * 0.01f;
    health *= 0.75f;
    value = (int)((float)value * health);
    return value;
}

int ModuleShipConfig::getMissileValue()
{
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int missile = ship.getMissileLauncherClass();
    switch(missile)
    {
        case 0: value = 0; break;
        case 1: value = MISSILELAUNCHER_CLASS1; break;
        case 2: value = MISSILELAUNCHER_CLASS2; break;
        case 3: value = MISSILELAUNCHER_CLASS3; break;
        case 4: value = MISSILELAUNCHER_CLASS4; break;
        case 5: value = MISSILELAUNCHER_CLASS5; break;
        case 6: value = MISSILELAUNCHER_CLASS6; break;
    }
    float health = ship.getMissileLauncherIntegrity() * 0.01f;
    health *= 0.75f;
    value = (int)((float)value * health);
    return value;
}

int ModuleShipConfig::getShieldValue()
{
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int shield = ship.getShieldClass();
    switch(shield)
    {
        case 0: value = 0; break;
        case 1: value = SHIELD_CLASS1; break;
        case 2: value = SHIELD_CLASS2; break;
        case 3: value = SHIELD_CLASS3; break;
        case 4: value = SHIELD_CLASS4; break;
        case 5: value = SHIELD_CLASS5; break;
        case 6: value = SHIELD_CLASS6; break;
    }
    float health = ship.getShieldIntegrity() * 0.01f;	
    health *= 0.75f;
    value = (int)((float)value * health);
    return value;
}

int ModuleShipConfig::getArmorValue()
{
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int armor = ship.getArmorClass();
    switch(armor)
    {
        case 0: value = 0; break;							
        case 1: value = ARMOR_CLASS1; break;
        case 2: value = ARMOR_CLASS2; break;
        case 3: value = ARMOR_CLASS3; break;
        case 4: value = ARMOR_CLASS4; break;
        case 5: value = ARMOR_CLASS5; break;
        case 6: value = ARMOR_CLASS6; break;
    }

	float health = (ship.getArmorIntegrity() / (armor * 120.0f));
	value = (int)((float)value * health * 0.75f );
    return value;
}

int ModuleShipConfig::getEngineRepair()
{
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int engine = ship.getEngineClass();
    switch(engine)
    {
        case 0: value = 0; break;
        case 1: value = ENGINE_CLASS1; break;
        case 2: value = ENGINE_CLASS2; break;
        case 3: value = ENGINE_CLASS3; break;
        case 4: value = ENGINE_CLASS4; break;
        case 5: value = ENGINE_CLASS5; break;
        case 6: value = ENGINE_CLASS6; break;
    }
    float damage = (100 - ship.getEngineIntegrity()) * 0.01f;
    value = (int)((float)value * damage);
    return value;
}

int ModuleShipConfig::getLaserRepair()
{
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int laser = ship.getLaserClass();
    switch(laser)
    {
        case 0: value = 0; break;
        case 1: value = LASER_CLASS1; break;
        case 2: value = LASER_CLASS2; break; 
        case 3: value = LASER_CLASS3; break;
        case 4: value = LASER_CLASS4; break;
        case 5: value = LASER_CLASS5; break;
        case 6: value = LASER_CLASS6; break;
    }
    float damage = (100 - ship.getLaserIntegrity()) * 0.01f;
    value = (int)((float)value * damage);
    return value;
}

int ModuleShipConfig::getMissileRepair()
{
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int missile = ship.getMissileLauncherClass();
    switch(missile)
    {
        case 0: value = 0; break;
        case 1: value = MISSILELAUNCHER_CLASS1; break;
        case 2: value = MISSILELAUNCHER_CLASS2; break;
        case 3: value = MISSILELAUNCHER_CLASS3; break;
        case 4: value = MISSILELAUNCHER_CLASS4; break;
        case 5: value = MISSILELAUNCHER_CLASS5; break;
        case 6: value = MISSILELAUNCHER_CLASS6; break;
    }
    float damage = (100 - ship.getMissileLauncherIntegrity()) * 0.01f;
    value = (int)((float)value * damage);
    return value;
}

int ModuleShipConfig::getShieldRepair()
{
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int shield = ship.getShieldClass();
    switch(shield)
    {
        case 0: value = 0; break;
        case 1: value = SHIELD_CLASS1; break;
        case 2: value = SHIELD_CLASS2; break;
        case 3: value = SHIELD_CLASS3; break;
        case 4: value = SHIELD_CLASS4; break;
        case 5: value = SHIELD_CLASS5; break;
        case 6: value = SHIELD_CLASS6; break;
    }
    float damage = (100 - ship.getShieldIntegrity()) * 0.01f;
    value = (int)((float)value * damage);
    return value;
}

int ModuleShipConfig::getArmorRepair()
{
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int armor = ship.getArmorClass();
    switch(armor)
    {
        case 0: value = 0; break;
        case 1: value = ARMOR_CLASS1; break;
        case 2: value = ARMOR_CLASS2; break;
        case 3: value = ARMOR_CLASS3; break;
        case 4: value = ARMOR_CLASS4; break;
        case 5: value = ARMOR_CLASS5; break;
        case 6: value = ARMOR_CLASS6; break;
    }
    float damage = (100 - ship.getArmorIntegrity()) * 0.01f;
    value = (int)((float)value * damage);
    return value;
}

int ModuleShipConfig::getHullRepair()
{
    Ship ship = g_game->gameState->getShip();
    int damage = (100 - ship.getHullIntegrity()) * 100;
    return damage;
}

void ModuleShipConfig::repairComponent()
{
    if( (int)menuPath.size() < 4 ) return;

    string noMsg = "You don't have enough credits to pay for the repairs.";
    int itemCost = 0;
	switch(menuPath[3])
	{
        case Hull:
            itemCost = getHullRepair();
            if (itemCost > 0)
            {
                if (g_game->gameState->m_credits >= itemCost)
                {
        		    g_game->gameState->augCredits(-itemCost);
                    g_game->gameState->m_ship.setHullIntegrity(100);
                    g_game->ShowMessageBoxWindow("", "Hull breaches have been patched up.", 400, 200);
                }
	            else
		            g_game->ShowMessageBoxWindow("", noMsg, 400, 200);
            }
            break;
	    case Engines:    
            itemCost = getEngineRepair();
            if (itemCost > 0)
            {
                if (g_game->gameState->m_credits >= itemCost)
                {
        		    g_game->gameState->augCredits(-itemCost);
                    g_game->gameState->m_ship.setEngineIntegrity(100);
                    g_game->ShowMessageBoxWindow("", "Engines now at peak operating efficiency.", 400, 200);
                }
	            else
		            g_game->ShowMessageBoxWindow("", noMsg, 400, 200);
            }
            break;
	    case Shields:    
            itemCost = getShieldRepair();	
            if (itemCost > 0)
            {
                if (g_game->gameState->m_credits >= itemCost)
                {
        		    g_game->gameState->augCredits(-itemCost);
                    g_game->gameState->m_ship.setShieldIntegrity(100);
                    g_game->ShowMessageBoxWindow("", "Shield capability fully restored.", 400, 200);
                }
	            else
		            g_game->ShowMessageBoxWindow("", noMsg, 400, 200);
            }
            break;
	    case Armor:      
            itemCost = getArmorRepair();	
            if (itemCost > 0)
            {
                if (g_game->gameState->m_credits >= itemCost)
                {
        		    g_game->gameState->augCredits(-itemCost);
                    g_game->gameState->m_ship.setArmorIntegrity(100);
                    g_game->ShowMessageBoxWindow("", "Armor plating has been reinforced.", 400, 200);
                }
	            else
		            g_game->ShowMessageBoxWindow("", noMsg, 400, 200);
            }
            break;
	    case Missiles:   
            itemCost = getMissileRepair();	
            if (itemCost > 0)
            {
                if (g_game->gameState->m_credits >= itemCost)
                {
        		    g_game->gameState->augCredits(-itemCost);
                    g_game->gameState->m_ship.setMissileLauncherIntegrity(100);
                    g_game->ShowMessageBoxWindow("", "Missile launcher fully repaired.", 400, 200);
                }
	            else
		            g_game->ShowMessageBoxWindow("", noMsg, 400, 200);
            }
            break;
	    case Lasers:     
            itemCost = getLaserRepair();	
            if (itemCost > 0)
            {
                if (g_game->gameState->m_credits >= itemCost)
                {
        		    g_game->gameState->augCredits(-itemCost);
                    g_game->gameState->m_ship.setLaserIntegrity(100);
                    g_game->ShowMessageBoxWindow("", "Lasers are ready for action.", 400, 200);
                }
	            else
		            g_game->ShowMessageBoxWindow("", noMsg, 400, 200);
            }
            break;
	    default:    
		    ALLEGRO_ASSERT(0);
	}

	
	g_game->gameState->m_ship.setHullIntegrity(100);
		

}

void ModuleShipConfig::sellComponent()
{
	if( (int)menuPath.size() < 4 ) return;

    int salePrice = 0;
    string saleText = "You received ";
    string saleItem = "";

	Ship ship = g_game->gameState->getShip();
	switch(menuPath[3])
	{
	case Engines: 
        salePrice = getEngineValue();
        saleItem = "Class " + Util::ToString(ship.getEngineClass()) + " Engine";
		ship.setEngineClass(NotInstalledType); 
		break;
	case Shields: 
        salePrice = getShieldValue();
        saleItem = "Class " + Util::ToString(ship.getShieldClass()) + " Shield";
		ship.setShieldClass(NotInstalledType); 
		break;
	case Armor: 
        salePrice = getArmorValue();
        saleItem = "Class " + Util::ToString(ship.getArmorClass()) + " Armor";
		ship.setArmorClass(NotInstalledType); 
		break;
	case Missiles: 
        salePrice = getMissileValue();
        saleItem = "Class " + Util::ToString(ship.getMissileLauncherClass()) + " Missile Launcher";
		ship.setMissileLauncherClass(NotInstalledType); 
		break;
	case Lasers: 
        salePrice = getLaserValue();
        saleItem = "Class " + Util::ToString(ship.getLaserClass()) + " Laser";
		ship.setLaserClass(NotInstalledType); 
		break;
	default:
		ALLEGRO_ASSERT(0);
	}

    //save ship changes
	g_game->gameState->setShip(ship);

    //add credits to player's account
    g_game->gameState->augCredits( salePrice );

    saleText = "You received " + Util::ToString(salePrice) + " credits for the " + saleItem + ".";
    g_game->ShowMessageBoxWindow("", saleText, 600, 200);

}

void ModuleShipConfig::buyComponent()
{
	if( (int)menuPath.size() < 5 ) return;

	int itemIndex = menuPath[3] - ITEM_ENUM_DIF;
	int classIndex = menuPath[4] - CLASS_ENUM_DIF - 1;
	int cost = ITEM_PRICES[itemIndex][classIndex];
	Ship ship = g_game->gameState->getShip();

	switch(menuPath[3]) 
    {
		case Engines: 
            if (ship.getEngineClass() == 0)
            {
			    ship.setEngineClass(menuPath[4] - CLASS_ENUM_DIF); 
			    ship.setEngineIntegrity(100.0f);
              	g_game->gameState->augCredits(-cost);
            }
            else
                g_game->ShowMessageBoxWindow("", "Your ship already has an engine!",450,200);
			break;
		case Shields: 
            if (ship.getShieldClass() == 0)
            {
			    ship.setShieldClass(menuPath[4] - CLASS_ENUM_DIF); 
			    ship.setShieldIntegrity(100.0f);
			    ship.setShieldCapacity(ship.getMaxShieldCapacity());
                g_game->gameState->augCredits(-cost);
            }
            else
                g_game->ShowMessageBoxWindow("", "Your ship already has a shield generator!",450,200);
			break;
		case Armor: 
            if (ship.getArmorClass() == 0)
            {
			    ship.setArmorClass(menuPath[4] - CLASS_ENUM_DIF); 
			    ship.setArmorIntegrity(ship.getMaxArmorIntegrity());
                g_game->gameState->augCredits(-cost);
            }
            else
                g_game->ShowMessageBoxWindow("", "Your ship already has armor plating!",450,200);
			break;
		case Missiles: 
            if (ship.getMissileLauncherClass() == 0)
            {
			    ship.setMissileLauncherClass(menuPath[4] - CLASS_ENUM_DIF); 
			    ship.setMissileLauncherIntegrity(100.0f);
                g_game->gameState->augCredits(-cost);
            }
            else 
                g_game->ShowMessageBoxWindow("", "Your ship already has a missile launcher!",450,200);
			break;
		case Lasers: 
            if (ship.getLaserClass() == 0)
            {
			    ship.setLaserClass(menuPath[4] - CLASS_ENUM_DIF);		//jjh
			    ship.setLaserIntegrity(100.0f);
                g_game->gameState->augCredits(-cost);
            }
            else
                g_game->ShowMessageBoxWindow("", "Your ship already has a laser!",450,200);
			break;
		default:
			ALLEGRO_ASSERT(0);
	}
	g_game->gameState->setShip(ship);
}

void ModuleShipConfig::display() const
{
	al_set_target_bitmap(g_game->GetBackBuffer());
	//show menu path
	if (menuPath.back() == Repair)
	{
		std::string temp = "Total Repair Cost: " + Util::ToString(repairCost) + " MU";
		al_draw_text(g_game->font32, WHITE, MENU_PATH_X, MENU_PATH_Y, 0, temp.c_str());
	}
	else if (menuPath.back() == Name)		
	{
        int nlen;
		//print "MSS"
		al_draw_text(g_game->font22, WHITE, MENU_PATH_X, MENU_PATH_Y, 0, "MSS ");
        nlen = al_get_text_width(g_game->font22, "MSS ");

		//print ship name
		al_draw_text(g_game->font22, WHITE, MENU_PATH_X+nlen, MENU_PATH_Y, 0, shipName.c_str());
        nlen += al_get_text_width(g_game->font22, shipName.c_str());

		al_draw_bitmap(m_cursor,MENU_PATH_X+nlen+2,MENU_PATH_Y,0);
		
	}
	else
	{
		if(menuPath.size() > 1)
			al_draw_text(g_game->font32, WHITE, MENU_PATH_X, MENU_PATH_Y, 0, convertMenuPathToString().c_str());
	}

	//draw ship schematic
	al_draw_bitmap(shipImage, 586, 548, 0);

	//static
	int i=0;
	al_draw_textf(g_game->font22, WHITE, STATIC_SHIPNAME_X-10, SHIPNAME_Y, 0, "Ship Name: MSS %s", g_game->gameState->m_ship.getName().c_str());	
	al_draw_text(g_game->font32, WHITE, STATIC_READOUT_X, READOUT_Y+(i++)*READOUT_SPACING, 0, "Cargo Pods");
	al_draw_text(g_game->font32, WHITE, STATIC_READOUT_X, READOUT_Y+(i++)*READOUT_SPACING, 0, "Engine");
	al_draw_text(g_game->font32, WHITE, STATIC_READOUT_X, READOUT_Y+(i++)*READOUT_SPACING, 0, "Shield");
	al_draw_text(g_game->font32, WHITE, STATIC_READOUT_X, READOUT_Y+(i++)*READOUT_SPACING, 0, "Armor");
	al_draw_text(g_game->font32, WHITE, STATIC_READOUT_X, READOUT_Y+(i++)*READOUT_SPACING, 0, "Missile");
	al_draw_text(g_game->font32, WHITE, STATIC_READOUT_X, READOUT_Y+(i++)*READOUT_SPACING, 0, "Laser");
	al_draw_text(g_game->font32, WHITE, STATIC_READOUT_X, READOUT_Y+(i++)*READOUT_SPACING, 0, "Hull Integrity");
	al_draw_text(g_game->font32, WHITE, STATIC_CREDITS_X, CREDITS_Y, 0, "Credits");

	//dynamic
	int j=0;
	al_draw_textf(g_game->font32, WHITE, DYNAMIC_READOUT_X, READOUT_Y+(j++)*READOUT_SPACING, ALLEGRO_ALIGN_RIGHT, "%d", g_game->gameState->m_ship.getCargoPodCount());

    //display class level of engine
    ALLEGRO_COLOR color = WHITE;
    int health = g_game->gameState->m_ship.getEngineIntegrity();
    if (health < 50) color = RED;
    else if (health < 100) color = YELLOW;
    else color = WHITE;
    al_draw_text(g_game->font32, color,
        DYNAMIC_READOUT_X, READOUT_Y+(j++)*READOUT_SPACING, ALLEGRO_ALIGN_RIGHT,
        g_game->gameState->m_ship.getEngineClassString().c_str());

    //display class level of shield
    health = g_game->gameState->m_ship.getShieldIntegrity();
    if (health < 50) color = RED;
    else if (health < 100) color = YELLOW;
    else color = WHITE;
	al_draw_text(g_game->font32, color, 
        DYNAMIC_READOUT_X, READOUT_Y+(j++)*READOUT_SPACING, ALLEGRO_ALIGN_RIGHT,
        g_game->gameState->m_ship.getShieldClassString().c_str());

    //display class level of armor
    health = g_game->gameState->m_ship.getArmorIntegrity();
    if (health < 50) color = RED;
    else if (health < 100) color = YELLOW;
    else color = WHITE;
	al_draw_text(g_game->font32, color, 
        DYNAMIC_READOUT_X, READOUT_Y+(j++)*READOUT_SPACING, ALLEGRO_ALIGN_RIGHT,
        g_game->gameState->m_ship.getArmorClassString().c_str());

    //display class level of missile
    health = g_game->gameState->m_ship.getMissileLauncherIntegrity();
    if (health < 50) color = RED;
    else if (health < 100) color = YELLOW;
    else color = WHITE;
    al_draw_text(g_game->font32, color, 
        DYNAMIC_READOUT_X, READOUT_Y+(j++)*READOUT_SPACING, ALLEGRO_ALIGN_RIGHT,
        g_game->gameState->m_ship.getMissileLauncherClassString().c_str());

    //display class level of laser
    health = g_game->gameState->m_ship.getLaserIntegrity();
    if (health < 50) color = RED;
    else if (health < 100) color = YELLOW;
    else color = WHITE;
	al_draw_text(g_game->font32, color,
        DYNAMIC_READOUT_X, READOUT_Y+(j++)*READOUT_SPACING, ALLEGRO_ALIGN_RIGHT,
        g_game->gameState->m_ship.getLaserClassString().c_str());

	//this should clear up any hull init problem
    health = g_game->gameState->m_ship.getHullIntegrity();
    if (health < 50) color = RED;
    else if (health < 100) color = YELLOW;
    else color = WHITE;
	
    if (g_game->gameState->m_ship.getHullIntegrity() > 100.0f)
		g_game->gameState->m_ship.setHullIntegrity(100.0f);

    //print hull integrity
	al_draw_textf(g_game->font32, WHITE, 
        DYNAMIC_READOUT_X, READOUT_Y+(j++)*READOUT_SPACING, ALLEGRO_ALIGN_RIGHT,
	"%.0f", 
        g_game->gameState->m_ship.getHullIntegrity());

    //print credits
	al_draw_textf(g_game->font32, WHITE, 
        DYNAMIC_CREDITS_X, CREDITS_Y, ALLEGRO_ALIGN_RIGHT, "%d", 
        g_game->gameState->m_credits);

}


void ModuleShipConfig::Update(){}

void ModuleShipConfig::Draw()
{
        al_set_target_bitmap(g_game->GetBackBuffer());
	//blit the background image
	al_draw_bitmap(shipConfig, 0, 0, 0);

	//step through active buttons
	for(int i=0; i<buttonsActive; ++i)
		buttons[i]->Run(g_game->GetBackBuffer());

	display();

}
