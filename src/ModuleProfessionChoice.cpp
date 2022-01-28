#include "ModuleProfessionChoice.h"

using namespace std;

const string ModuleProfessionChoice::c_prof_info_scientific_text =
    "Even though the universe regresses towards smaller and smaller "
    "components, it is still plenty large to hide a few mysteries. The "
    "Scientific Officer represents the pinnacle of Myrrdanian brainpower. "
    "Armed with wit, cunning, intelligence... and a stun gun these brave "
    "souls explore the edges of the galaxy documenting planets and "
    "capturing life forms for study. Not to mention, the ability to "
    "recommend a planet for colonization comes with monetary and "
    "retirement perks. Mostly monetary seeing as distant planet "
    "construction usually takes some time to kick start.";

const string ModuleProfessionChoice::c_prof_info_freelance_text =
    "There is a lot of money to be made in the Gamma Sector and the "
    "Freelancer's job is to get his hands on some. This jack of all trades "
    "profession is easily the most versatile Captain type in the galaxy. "
    "Capable of interstellar combat and properly equipped with modern "
    "scanning and exploring technology there is ample opportunity for the "
    "Freelancer to respond to most situations. One distinguishing feature "
    "is the greatly expanded cargo room which, of course, makes all those "
    "lowly Glush Cola shipments that much more profitable.";
const string ModuleProfessionChoice::c_prof_info_military_text =
    "The galaxy is a rough and tumble place where there is hardly ever a "
    "shortage of conflict. The Military Officer is the spear point of "
    "Myrrdan's influence and is often called upon to serve 'for the "
    "greater good.' Trained in tactical combat and given access to some of "
    "the highest class weaponry in the sector, it is never a bad time to "
    "be at the helm of a Wraith class warship. Being in front of it, "
    "however, is another scenario entirely.";

ModuleProfessionChoice::ModuleProfessionChoice(
    ResourceManager<ALLEGRO_BITMAP> &resources)
    : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
      m_background(
          new Bitmap(resources[I_CAPTAINCREATION_PROFESSIONBACKGROUND])),
      m_scientific_button(new NewButton(
          PROFBTN_SCIENTIFIC_X,
          PROFBTN_SCIENTIFIC_Y,
          -1,
          -1,
          EVENT_PROFESSION_SCIENTIFIC_MOUSEOVER,
          EVENT_PROFESSION_SCIENTIFIC,
          resources[I_CAPTAINCREATION_SCIENTIFIC],
          resources[I_CAPTAINCREATION_SCIENTIFIC_MOUSEOVER],
          nullptr,
          S_BUTTONCLICK)),
      m_freelance_button(new NewButton(
          PROFBTN_FREELANCE_X,
          PROFBTN_FREELANCE_Y,
          -1,
          -1,
          EVENT_PROFESSION_FREELANCE_MOUSEOVER,
          EVENT_PROFESSION_FREELANCE,
          resources[I_CAPTAINCREATION_FREELANCE],
          resources[I_CAPTAINCREATION_FREELANCE_MOUSEOVER],
          nullptr,
          S_BUTTONCLICK)),
      m_military_button(new NewButton(
          PROFBTN_MILITARY_X,
          PROFBTN_MILITARY_Y,
          -1,
          -1,
          EVENT_PROFESSION_MILITARY_MOUSEOVER,
          EVENT_PROFESSION_MILITARY,
          resources[I_CAPTAINCREATION_MILITARY],
          resources[I_CAPTAINCREATION_MILITARY_MOUSEOVER],
          nullptr,
          S_BUTTONCLICK)),
      m_prof_info_label(new Label(
          "",
          PROFESSION_BOX_X + 22,
          PROFESSION_BOX_Y + 22,
          PROFESSION_BOX_WIDTH - 44,
          PROFESSION_BOX_HEIGHT - 44,
          true,
          0,
          g_game->font22,
          WHITE)) {
    add_child_module(m_background);
    add_child_module(m_scientific_button);
    add_child_module(m_freelance_button);
    add_child_module(m_military_button);
    add_child_module(m_prof_info_label);
}

bool
ModuleProfessionChoice::on_mouse_move(ALLEGRO_MOUSE_EVENT * /*event*/) {
    m_prof_info_label->set_text("");

    return true;
}

bool
ModuleProfessionChoice::on_event(ALLEGRO_EVENT *event) {
    switch (event->type) {
    case EVENT_PROFESSION_SCIENTIFIC_MOUSEOVER:
        m_prof_info_label->set_text(c_prof_info_scientific_text);
        return false;
    case EVENT_PROFESSION_FREELANCE_MOUSEOVER:
        m_prof_info_label->set_text(c_prof_info_freelance_text);
        return false;
    case EVENT_PROFESSION_MILITARY_MOUSEOVER:
        m_prof_info_label->set_text(c_prof_info_military_text);
        return false;
    }
    return true;
}
