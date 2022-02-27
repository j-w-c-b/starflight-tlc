#include "Officer.h"
#include "Game.h"
#include "Util.h"

using namespace std;

ALLEGRO_DEBUG_CHANNEL("Officer")

const string
to_string(Skill s) {
    static map<Skill, const string> skill_names = {
        {SKILL_SCIENCE, "science"},
        {SKILL_NAVIGATION, "navigation"},
        {SKILL_ENGINEERING, "engineering"},
        {SKILL_COMMUNICATION, "communication"},
        {SKILL_MEDICAL, "medical"},
        {SKILL_TACTICAL, "tactical"},
        {SKILL_LEARN_RATE, "learning"},
        {SKILL_DURABILITY, "durability"}};

    return skill_names[s];
}

const string
to_string(OfficerType officer_type) {
    static map<OfficerType, const string> officer_type_names = {
        {OFFICER_NONE, "Unassigned"},
        {OFFICER_CAPTAIN, "Captain"},
        {OFFICER_SCIENCE, "Science"},
        {OFFICER_NAVIGATION, "Navigation"},
        {OFFICER_ENGINEER, "Engineering"},
        {OFFICER_COMMUNICATION, "Communication"},
        {OFFICER_MEDICAL, "Medical"},
        {OFFICER_TACTICAL, "Tactical"}};

    return officer_type_names[officer_type];
}

// Constructor for random officers
Officer::Officer() {
    auto [first_name, surname] = g_game->dataMgr->GetRandMixedName();
    m_first_name = first_name;
    m_surname = surname;

    for (auto skill : ProfessionSkillIterator) {
        m_attributes[skill] = Util::Random(5, 50);
    }
    m_attributes[SKILL_DURABILITY] = 5;
    m_attributes[SKILL_LEARN_RATE] = 5;

    m_vitality = 100;
    for (auto i : ProfessionSkillIterator) {
        m_experience[i] = 0;
    }
    isHealing = false;

    // specialization in a random skill
    m_attributes[Util::random_choice(ProfessionSkillIterator)] =
        Util::Random(50, 75);
    m_officer_type = OFFICER_NONE;
}

// Constructor for captains
Officer::Officer(const string &name, const map<Skill, int> &attributes) {
    string::size_type loc = name.find(" ", 0);
    if (loc != string::npos) {
        m_first_name = name.substr(0, loc);
        m_surname = name.substr(loc + 1);
    } else {
        m_first_name = "";
        m_surname = name;
    }

    m_officer_type = OFFICER_CAPTAIN;

    m_attributes = attributes;

    m_vitality = 100;
    for (auto i : ProfessionSkillIterator) {
        m_experience[i] = 0;
    }
    isHealing = false;
}

std::string
Officer::get_abbreviated_title() const {
    static const map<OfficerType, string> abbreviated_title = {
        {OFFICER_NONE, ""},
        {OFFICER_CAPTAIN, "Cap. "},
        {OFFICER_SCIENCE, "Sci. Off. "},
        {OFFICER_NAVIGATION, "Nav. Off. "},
        {OFFICER_ENGINEER, "Eng. Off. "},
        {OFFICER_COMMUNICATION, "Com. Off. "},
        {OFFICER_MEDICAL, "Med. Off. "},
        {OFFICER_TACTICAL, "Tac. Off. "}

    };
    return abbreviated_title.at(m_officer_type) + m_surname;
}

Officer::Officer(InputArchive &ar) { ar >> *this; }

/**
    This function increases a crew member's skill in a specific area as a result
of performing a task.
**/
bool
Officer::SkillUp(Skill skill, int amount) {
    if (m_officer_type == OFFICER_CAPTAIN)
        return false;

    if (m_attributes[skill] >= 255) {
        return false;
    }
    if ((m_attributes[skill] + amount) >= 255) {
        m_attributes[skill] = 255;
    } else {
        m_attributes[skill] += amount;
    }

    return true;
}

OfficerType
Officer::get_officer_type() const {
    return m_officer_type;
}

string
Officer::get_title() const {
    static map<OfficerType, string> titles = {
        {OFFICER_CAPTAIN, "Captain"},
        {OFFICER_SCIENCE, "Sciences"},
        {OFFICER_NAVIGATION, "Navigation"},
        {OFFICER_ENGINEER, "Engineering"},
        {OFFICER_COMMUNICATION, "Communications"},
        {OFFICER_MEDICAL, "Medical"},
        {OFFICER_TACTICAL, "Tactical"},
        {OFFICER_NONE, "None"}};

    return titles[m_officer_type];
}

OfficerType
Officer::get_preferred_profession() const {
    Skill best_skill = SKILL_SCIENCE;
    int highestValue = -1;

    for (auto i : ProfessionSkillIterator) {
        if (m_attributes.at(i) > highestValue) {
            highestValue = m_attributes.at(i);
            best_skill = i;
        }
    }
    return GameState::skill_map.at(best_skill);
}

void
Officer::set_officer_type(OfficerType role) {
    m_officer_type = role;
}

InputArchive &
operator>>(InputArchive &ar, Officer &officer) {
    string class_name = string(Officer::class_name);
    // Schema 0: name as single string, Attributes class
    // Schema 1: separate first_name and surname, attributes are iterated,
    //           m_last_skill_check, isHealing,
    //           replace m_extra_variable with map of attribute -> int
    int schema = 1;

    string load_class_name;
    ar >> load_class_name;
    if (load_class_name != class_name) {
        std::error_code ec(al_get_errno(), std::system_category());
        throw std::system_error(
            ec,
            "Invalid save file: expected " + class_name + ", got "
                + load_class_name);
    }

    int load_schema;
    ar >> load_schema;
    if (load_schema > schema) {
        std::error_code ec(al_get_errno(), std::system_category());
        throw std::system_error(
            ec,
            "Invalid save file: unknown schema: " + class_name + " version "
                + std::to_string(load_schema));
    }

    if (load_schema == 0) {
        string name;
        ar >> name;

        string::size_type loc = name.find(" ", 0);
        if (loc != string::npos) {
            officer.m_first_name = name.substr(0, loc);
            officer.m_surname = name.substr(loc + 1);
        } else {
            officer.m_first_name = "";
            officer.m_surname = name;
        }
        officer.m_last_skill_check = g_game->gameState->stardate;
        officer.isHealing = false;
        string load_attributes_name;
        ar >> load_attributes_name;
        if (load_attributes_name != "Attributes") {
            std::error_code ec(al_get_errno(), std::system_category());
            throw std::system_error(
                ec,
                "Invalid save file: expected Attributes , got "
                    + load_attributes_name);
        }
        int load_attribute_schema;
        ar >> load_attribute_schema;

        if (load_attribute_schema != 0) {
            std::error_code ec(al_get_errno(), std::system_category());
            throw std::system_error(
                ec, "Invalid save file: unknown schema: Attributes version 0");
        }

        ar >> officer.m_attributes[SKILL_DURABILITY];
        ar >> officer.m_attributes[SKILL_LEARN_RATE];
        ar >> officer.m_attributes[SKILL_SCIENCE];
        ar >> officer.m_attributes[SKILL_NAVIGATION];
        ar >> officer.m_attributes[SKILL_TACTICAL];
        ar >> officer.m_attributes[SKILL_ENGINEERING];
        ar >> officer.m_attributes[SKILL_MEDICAL];
        ar >> officer.m_attributes[SKILL_COMMUNICATION];
        ar >> officer.m_vitality;
        float extra_variable;
        ar >> extra_variable;
        auto preferred = officer.get_preferred_profession();
        for (auto i : ProfessionSkillIterator) {
            if (GameState::skill_map.at(i) == preferred) {
                officer.m_experience[i] = static_cast<int>(extra_variable);
            } else {
                officer.m_experience[i] = 0;
            }
        }
    } else {
        ar >> officer.m_first_name;
        ar >> officer.m_surname;
        ar >> officer.m_last_skill_check;
        ar >> officer.isHealing;
        for (auto i : SkillIterator) {
            ar >> officer.m_attributes[i];
        }
        ar >> officer.m_vitality;
        for (auto i : ProfessionSkillIterator) {
            ar >> officer.m_experience[i];
        }
    }

    int tmpi;
    ar >> tmpi;
    officer.m_officer_type = static_cast<OfficerType>(tmpi);

    return ar;
}

OutputArchive &
operator<<(OutputArchive &ar, const Officer &officer) {
    string class_name = string(Officer::class_name);
    // Schema 0: name as single string, Attributes class
    // Schema 1: separate first_name and surname, attributes are iterated,
    //           m_last_skill_check, isHealing,
    //           replace m_extra_variable with map of attribute -> int
    int schema = 1;

    ar << class_name;
    ar << schema;

    ar << officer.m_first_name;
    ar << officer.m_surname;

    ar << officer.m_last_skill_check;
    ar << officer.isHealing;

    for (auto i : SkillIterator) {
        ar << officer.m_attributes.at(i);
    }
    ar << officer.m_vitality;
    for (auto i : ProfessionSkillIterator) {
        ar << officer.m_experience.at(i);
    }
    ar << static_cast<int>(officer.m_officer_type);

    return ar;
}

bool
Officer::SkillCheck() {
    int skill_value = 0;
    int chance = 0;

    m_last_skill_check = g_game->gameState->stardate;

    switch (m_officer_type) {
    case OFFICER_SCIENCE:
        skill_value = m_attributes[SKILL_SCIENCE];
        break;
    case OFFICER_ENGINEER:
        skill_value = m_attributes[SKILL_ENGINEERING];
        break;
    case OFFICER_MEDICAL:
        skill_value = m_attributes[SKILL_MEDICAL];
        break;
    case OFFICER_NAVIGATION:
        skill_value = m_attributes[SKILL_NAVIGATION];
        break;
    case OFFICER_TACTICAL:
        skill_value = m_attributes[SKILL_TACTICAL];
        break;
    case OFFICER_COMMUNICATION:
        skill_value = m_attributes[SKILL_COMMUNICATION];
        break;
    default:
        return false;
    }

    // 250+ is guaranteed pass
    if (skill_value > 250) {
        return true;
    }
    // any below 200 is % chance to pass skill check
    else if (skill_value > 200) {
        chance = 90;
    } else if (skill_value > 150) {
        chance = 80;
    } else if (skill_value > 100) {
        chance = 70;
    } else if (skill_value > 75) {
        chance = 60;
    } else if (skill_value > 50) {
        chance = 50;
    } else
        chance = 25;

    int roll = sfrand() % 100;
    return (roll < chance);
}

bool
Officer::add_experience(Skill skill, int amount) {
    ALLEGRO_ASSERT(skill != SKILL_DURABILITY);
    ALLEGRO_ASSERT(skill != SKILL_LEARN_RATE);

    m_experience[skill] += amount;
    m_last_skill_check = g_game->gameState->stardate;

    switch (skill) {
    case SKILL_SCIENCE:
        break;
    case SKILL_NAVIGATION:
        if (m_experience[skill] >= 168) {
            m_experience[skill] -= 168;
            return SkillUp(skill);
        }
        break;
    case SKILL_ENGINEERING:
        if (m_experience[skill] >= 4) {
            m_experience[skill] -= 4;
            return SkillUp(skill);
        }
        break;
    case SKILL_COMMUNICATION:
    case SKILL_MEDICAL:
        if (m_experience[skill] >= 12) {
            m_experience[skill] -= 12;
            return SkillUp(skill);
        }
        break;
    case SKILL_TACTICAL:
    default:
        break;
    }
    return false;
}

bool
Officer::CanSkillCheck() const {
    return (m_last_skill_check < g_game->gameState->stardate);
}

std::string
Officer::get_name() const {
    if (m_first_name.length() != 0) {
        return m_first_name + " " + m_surname;
    } else {
        return m_surname;
    }
}

std::string
Officer::get_first_name() const {
    return m_first_name;
}

std::string
Officer::get_last_name() const {
    return m_surname;
}

void
Officer::heal(float value) {
    m_vitality += value;
    if (m_vitality > 100) {
        m_vitality = 100;
    } else if (m_vitality < 0) {
        m_vitality = 0;
    }
}

// vi: ft=cpp
