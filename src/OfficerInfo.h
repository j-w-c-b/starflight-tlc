#ifndef OFFICER_INFO_H
#define OFFICER_INFO_H

#include <map>
#include <memory>

#include "GameState.h"
#include "Label.h"
#include "Module.h"

class OfficerInfo : public Module {
  public:
    OfficerInfo(int x, int y);
    virtual ~OfficerInfo() {}
    void set_officer(const Officer *officer);

  private:
    std::shared_ptr<Label> m_name_label;
    std::shared_ptr<Label> m_preferred_job_label;
    std::map<Skill, std::shared_ptr<class OfficerInfoGauge>> m_gauges;
};

#endif // OFFICER_INFO_H
// vi: ft=cpp
