#include "RiverSwim.h"
#include "utils.h"

using namespace std;
using namespace UTILS;
/*
RIVERSWIM::RIVERSWIM(int size)
    : Grid(size, 1),
    Size(size)
   
   
{
    NumActions = 2;
    NumObservations = 5;
    RewardRange = 20;
    Discount = 0.95;

    
    InitGeneral();
}

void RIVERSWIM::InitGeneral()
{
   
    StartPos = COORD(0, 0);
    RandomSeed(0);
    Grid.SetAllValues(-1);
    
}

COORD RIVERSWIM::get_agent_pos(STATE& state) const
{
    const RIVER_STATE& rockstate = safe_cast<const RIVER_STATE&>(state);
    const COORD pos = rockstate.AgentPos;
    return pos;
}






STATE* RIVERSWIM::Copy(const STATE& state) const
{
    const RIVER_STATE& rockstate = safe_cast<const RIVER_STATE&>(state);
    RIVER_STATE* newstate = MemoryPool.Allocate();
    *newstate = rockstate;
    return newstate;
}

void RIVERSWIM::Validate(const STATE& state) const
{
    const RIVER_STATE& rockstate = safe_cast<const RIVER_STATE&>(state);
    assert(Grid.Inside(rockstate.AgentPos));
}

STATE* RIVERSWIM::CreateStartState() const
{
    RIVER_STATE* rockstate = MemoryPool.Allocate();
    rockstate->AgentPos = StartPos;
    return rockstate;
}

void RIVERSWIM::FreeState(STATE* state) const
{
    RIVER_STATE* rockstate = safe_cast<RIVER_STATE*>(state);
    MemoryPool.Free(rockstate);
}

bool RIVERSWIM::Step(STATE& state, int action,
    int& observation, double& reward) const
{
    RIVER_STATE& rockstate = safe_cast<RIVER_STATE&>(state);
    reward = 0;
  

    double min_rew = 0.0005;
   
    double high_rew = 1.0;

    if (action == 0) {
        if (rockstate.AgentPos.X - 1 >= 0)
            rockstate.AgentPos.X--;
        else {
            reward = min_rew;
        }
    }
    else if (action == 1) {
        double rnd = UTILS::UniformDist(0.0, 1.0);
        int currentState = rockstate.AgentPos.X;
        if (currentState == 0) {
            if (rnd < 0.7) {
                reward = min_rew;
            }
            else {
                rockstate.AgentPos.X++;
            }
        }
        else if (currentState == Size - 1) {
            if (rnd < 0.7) {
                rockstate.AgentPos.X--;
            }
            else {
                reward = high_rew;
            }
        }
        else {
            if (rnd < 0.1) {
                rockstate.AgentPos.X--;
            }
            else if (rnd < 0.4) {
                rockstate.AgentPos.X++;
            }
            else {

            }
        }
       
       
    }
    else {
        cout << "Invalid";
    }
    observation = rockstate.AgentPos.X;

    //assert(reward != min_rew);
    return false;
}


void RIVERSWIM::GenerateLegal(const STATE& state, const HISTORY& history,
    vector<int>& legal, const STATUS& status) const
{

    const RIVER_STATE& rockstate =
        safe_cast<const RIVER_STATE&>(state);

    legal.push_back(0);
    legal.push_back(1);

    
}

void RIVERSWIM::GeneratePreferred(const STATE& state, const HISTORY& history,
    vector<int>& actions, const STATUS& status) const
{

    actions.push_back(0);
    actions.push_back(1);
}





void RIVERSWIM ::DisplayBeliefs(const BELIEF_STATE& beliefState,
    std::ostream& ostr) const
{
}

void RIVERSWIM::DisplayState(const STATE& state, std::ostream& ostr) const
{
    const RIVER_STATE& rockstate = safe_cast<const RIVER_STATE&>(state);
    ostr << endl;
    ostr << rockstate.AgentPos.X;
}

void RIVERSWIM::DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const
{
    const RIVER_STATE& rockstate = safe_cast<const RIVER_STATE&>(state);
    ostr << endl;
    ostr << rockstate.AgentPos.X;
}

void RIVERSWIM::DisplayAction(int action, std::ostream& ostr) const
{
    ostr << action;
}
*/