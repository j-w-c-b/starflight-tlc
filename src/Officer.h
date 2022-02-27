#ifndef OFFICER_H
#define OFFICER_H

#include <map>
#include <string>

#include "Archive.h"
#include "Stardate.h"

enum Skill
{
    SKILL_SCIENCE = 0,
    SKILL_NAVIGATION,
    SKILL_TACTICAL,
    SKILL_ENGINEERING,
    SKILL_COMMUNICATION,
    SKILL_MEDICAL,
    SKILL_LEARN_RATE,
    SKILL_DURABILITY,
};

constexpr std::initializer_list<Skill> SkillIterator = {
    SKILL_SCIENCE,
    SKILL_NAVIGATION,
    SKILL_TACTICAL,
    SKILL_ENGINEERING,
    SKILL_COMMUNICATION,
    SKILL_MEDICAL,
    SKILL_LEARN_RATE,
    SKILL_DURABILITY,
};

constexpr std::initializer_list<Skill> ProfessionSkillIterator = {
    SKILL_SCIENCE,
    SKILL_NAVIGATION,
    SKILL_TACTICAL,
    SKILL_ENGINEERING,
    SKILL_COMMUNICATION,
    SKILL_MEDICAL,
};

const std::string
to_string(Skill s);

enum OfficerType
{
    OFFICER_NONE,
    OFFICER_CAPTAIN,
    OFFICER_SCIENCE,
    OFFICER_NAVIGATION,
    OFFICER_ENGINEER,
    OFFICER_COMMUNICATION,
    OFFICER_MEDICAL,
    OFFICER_TACTICAL
};

constexpr std::initializer_list<OfficerType> OfficerIterator = {
    OFFICER_CAPTAIN,
    OFFICER_SCIENCE,
    OFFICER_NAVIGATION,
    OFFICER_ENGINEER,
    OFFICER_COMMUNICATION,
    OFFICER_MEDICAL,
    OFFICER_TACTICAL};

const std::string
to_string(OfficerType officer_type);

class Officer {
  public:
    Officer();
    explicit Officer(InputArchive &ar);
    explicit Officer(
        const std::string &name,
        const std::map<Skill, int> &attributes);

    std::string get_abbreviated_title() const;
    OfficerType get_officer_type() const;
    void set_officer_type(OfficerType type);
    std::string get_title() const;
    OfficerType get_preferred_profession() const;

    static constexpr std::string_view class_name = "Officer";

    friend InputArchive &operator>>(InputArchive &ar, Officer &officer);
    friend OutputArchive &operator<<(OutputArchive &ar, const Officer &officer);

    bool SkillUp(Skill skill, int amount = 1);
    bool SkillCheck();
    bool add_experience(Skill skill, int amount = 1);
    int get_experience(Skill skill) const { return m_experience.at(skill); }
    bool CanSkillCheck() const;

    bool isBeingHealed() const { return isHealing; }
    void Recovering(bool recovering) { isHealing = recovering; }

    void heal(float value);

    std::string get_name() const;
    std::string get_first_name() const;
    std::string get_last_name() const;

    float get_vitality() const { return m_vitality; }
    int get_skill(Skill s) const { return m_attributes.at(s); }

  private:
    std::string m_first_name;
    std::string m_surname;

    Stardate m_last_skill_check;
    bool isHealing;

    OfficerType m_officer_type;
    std::map<Skill, int> m_attributes;
    float m_vitality;
    std::map<Skill, int> m_experience;
};
#endif // OFFICER_H
// vi: ft=cpp
