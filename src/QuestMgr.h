// don't create new instances of this class, use the instance provided in the
// Game class

#pragma once

#include "QuestMgr.h"
#include "Script.h"
#include <string>

#define QUEST_EVENT_ORBIT 100
#define QUEST_EVENT_PLANETSCAN 101

class QuestMgr {
  public:
    QuestMgr();
    virtual ~QuestMgr();

    bool Initialize();
    bool getNextQuest();
    bool getQuestByID(int id);
    bool getActiveQuest();

    void raiseEvent(int eventid, int param1 = -1, int param2 = -1);

    std::string getName() { return questName; }
    std::string getShort() { return questShort; }
    std::string getLong() { return questLong; }
    std::string getDebrief() { return questDebrief; }

    void getScriptGlobals();
    void setStoredValue(int value);
    void
    VerifyRequirements(int reqCode = -1, int reqType = -1, int reqAmt = -1);
    void giveReward();

    int questReqCode, questReqType;
    float questReqAmt;
    int questRewCode, questRewType;
    float questRewAmt;

  private:
    Script *script;

    std::string questName;
    std::string questShort;
    std::string questLong;
    std::string questDebrief;
    int debriefStatus;
};
