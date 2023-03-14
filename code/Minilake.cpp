#include "Minilake.h"
using namespace std;
using namespace UTILS;
#include <fstream>
MINILAKE::MINILAKE(int size)
    : Grid(2, 2),
    Size(2)


{
    NumActions = 4;
    NumObservations = Size * Size;
    RewardRange = 20;
    Discount = 0.95;

    up = 3;
    down = 1;
    left = 0;
    right = 2;


    visits = new std::vector<double>();
    for (int i = 0; i < NumObservations; i++) visits->emplace_back(0.0f);
    InitGeneral();
}

MINILAKE::~MINILAKE()
{
    delete visits;
    visits = nullptr;
}

void MINILAKE::InitGeneral()
{

    StartPos = COORD(0, 0);
    RandomSeed(0);
    Grid.SetAllValues(-1);

}

COORD MINILAKE::get_agent_pos(STATE& state) const
{
    const MINILAKE_STATE& rockstate = safe_cast<const MINILAKE_STATE&>(state);
    const COORD pos = rockstate.AgentPos;
    return pos;
}






STATE* MINILAKE::Copy(const STATE& state) const
{
    const MINILAKE_STATE& rockstate = safe_cast<const MINILAKE_STATE&>(state);
    MINILAKE_STATE* newstate = MemoryPool.Allocate();
    *newstate = rockstate;
    return newstate;
}

void MINILAKE::Validate(const STATE& state) const
{
    const MINILAKE_STATE& rockstate = safe_cast<const MINILAKE_STATE&>(state);
    assert(Grid.Inside(rockstate.AgentPos));
}

STATE* MINILAKE::CreateStartState() const
{
    MINILAKE_STATE* rockstate = MemoryPool.Allocate();
    rockstate->AgentPos = StartPos;
    return rockstate;
}

void MINILAKE::FreeState(STATE* state) const
{
    MINILAKE_STATE* rockstate = safe_cast<MINILAKE_STATE*>(state);
    MemoryPool.Free(rockstate);
}

bool MINILAKE::Step(STATE& state, int action,
    int& observation, double& reward, bool save) const
{
    MINILAKE_STATE& rockstate = safe_cast<MINILAKE_STATE&>(state);
    reward = 0;

    double high_rew = 1.0;
    double rnd = UTILS::UniformDist(0.0, 1.0);
    double flip = UTILS::UniformDist(0.0, 1.0);
    if (action == left) {
        if (rnd > 0.666666) {
            action = up;
        }
        else if (rnd > 0.33333) {
            action = down;
        }

    }
    else if (action == down) {
        if (rnd > 0.666666) {
            action = left;
        }
        else if (rnd > 0.33333) {
            action = right;
        }
    }
    else if (action == right) {
        if (rnd > 0.666666) {
            action = up;
        }
        else if (rnd > 0.33333) {
            action = down;
        }
    }
    else if (action == up) {
        if (rnd > 0.666666) {
            action = left;
        }
        else if (rnd > 0.33333) {
            action = right;
        }
    }
    else {
        cout << "Invalid";
    }


    if (action == left) {
        if (rockstate.AgentPos.X > 0) {
            rockstate.AgentPos.X--;
        }
    }
    else if (action == down) {
        if (rockstate.AgentPos.Y < Size - 1) {
            rockstate.AgentPos.Y++;
        }
    }
    else if (action == right) {
        if (rockstate.AgentPos.X < Size - 1) {
            rockstate.AgentPos.X++;
        }
    }
    else if (action == up) {
        if (rockstate.AgentPos.Y > 0) {
            rockstate.AgentPos.Y--;
        }
    }
    else {
        cout << "Invalid";
    }


    int pos = rockstate.AgentPos.X + rockstate.AgentPos.Y * Size;
    observation = pos;
    (*visits)[observation] += 1.0;
    if (pos ==1) return true;
    if (pos == 3) {
        reward = high_rew;
        return true;
    }




    //assert(reward != min_rew);
    return false;
}


void MINILAKE::GenerateLegal(const STATE& state, const HISTORY& history,
    vector<int>& actions, const STATUS& status) const
{

    const MINILAKE_STATE& rockstate =
        safe_cast<const MINILAKE_STATE&>(state);

    int pos = rockstate.AgentPos.X + rockstate.AgentPos.Y * Size;


    if (pos == 0) {

        actions.push_back(left);
        


    }
    else if (pos == 2) {
        actions.push_back(down);
    }
    



}

void MINILAKE::GeneratePreferred(const STATE& state, const HISTORY& history,
    vector<int>& actions, const STATUS& status) const
{

    const MINILAKE_STATE& rockstate =
        safe_cast<const MINILAKE_STATE&>(state);

    int pos = rockstate.AgentPos.X + rockstate.AgentPos.Y * 3;


    if (pos == 0) {

        actions.push_back(down);
        actions.push_back(right);


    }
    else if (pos == 1) {
        actions.push_back(up);
    }
    else if (pos == 2) {

        actions.push_back(down);

    }
    else if (pos == 3) {
        actions.push_back(up);
    }
    else if (pos == 4) {
        actions.push_back(left);

    }
    else if (pos == 6) {
        actions.push_back(left);
        actions.push_back(right);
    }
    else if (pos == 8) {

        actions.push_back(up);
    }
    else if (pos == 9) {
        actions.push_back(down);

    }
    else if (pos == 10) {
        actions.push_back(left);

    }
    else if (pos == 13) {
        actions.push_back(right);
    }
    else if (pos == 14) {

        actions.push_back(down);
        actions.push_back(right);
        actions.push_back(up);

    }
    else {
        actions.push_back(left);
        actions.push_back(down);
        actions.push_back(right);
        actions.push_back(up);
    }
}





void MINILAKE::DisplayBeliefs(const BELIEF_STATE& beliefState,
    std::ostream& ostr) const
{
}

void MINILAKE::DisplayState(const STATE& state, std::ostream& ostr) const
{
    const MINILAKE_STATE& rockstate = safe_cast<const MINILAKE_STATE&>(state);
    ostr << std::endl;
    ostr << rockstate.AgentPos.X;
}

void MINILAKE::DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const
{
    const MINILAKE_STATE& rockstate = safe_cast<const MINILAKE_STATE&>(state);
    ostr << std::endl;
    ostr << rockstate.AgentPos.X;
}

void MINILAKE::DisplayAction(int action, std::ostream& ostr) const
{
    ostr << action;
}
void MINILAKE::saveVisits() const
{
    std::ofstream output;
    output.open(UTILS::getVisitsString(), std::ios_base::app);
    for (int i = 0; i < NumObservations; i++) {
        output << visits->at(i) << " ";
    }
    output << "\n";
    output.close();
}