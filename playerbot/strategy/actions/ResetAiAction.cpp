
#include "playerbot/playerbot.h"
#include "ResetAiAction.h"
#include "playerbot/PlayerbotDbStore.h"

using namespace ai;

bool ResetAiAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    ResetValues();
    ResetStrategies();

    if (fullReset)
    {
        ai->TellError(requester, "AI was reset to defaults");
    }

    return true;
}

void ResetAiAction::ResetStrategies()
{
    bool loadStrats = !fullReset;

    ai->ResetStrategies(loadStrats);

    if (fullReset)
        sPlayerbotDbStore.Save(ai);
}

void ResetAiAction::ResetValues()
{
    uint64 guid = ai->GetBot()->GetObjectGuid().GetRawValue();

    auto results = CharacterDatabase.PQuery("SELECT `value` FROM `ai_playerbot_db_store` WHERE `guid` = '%lu' and `key` = 'value'", guid);
    if (results)
    {
        std::list<std::string> values;
        do
        {
            Field* fields = results->Fetch();
            std::string val = fields[0].GetString();

            std::vector<std::string> parts = split(val, '>');
            if (parts.size() != 2) continue;

            std::string name = parts[0];
            std::string text = parts[1];

            UntypedValue* value = context->GetUntypedValue(name);
            if (!value) continue;

            if (fullReset)
                value->Reset();
            else
                value->Load(text);
            
        } while (results->NextRow());

        if(fullReset)
            CharacterDatabase.PExecute("DELETE FROM `ai_playerbot_db_store` WHERE `guid` = '%lu' and `key` = 'value'", guid);
    }
}

bool ResetStratsAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    ResetStrategies();
   
    if (fullReset)
    {
        ai->TellError(requester, "Strategies reset to defaults");
    }

    return true;
}

bool ResetValuesAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    ResetValues();

    if (fullReset)
    {
        ai->TellError(requester, "Settings reset to defaults");
    }

    return true;
}