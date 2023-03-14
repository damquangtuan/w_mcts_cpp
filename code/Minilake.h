#pragma once
#include "simulator.h"
#include "coord.h"
#include "grid.h"

class MINILAKE_STATE : public STATE
{
public:

    COORD AgentPos;

};

class MINILAKE : public SIMULATOR
{
public:

    MINILAKE(int size);
    ~MINILAKE();
    virtual STATE* Copy(const STATE& state) const;
    virtual void Validate(const STATE& state) const;
    virtual STATE* CreateStartState() const;
    virtual void FreeState(STATE* state) const;
    virtual bool Step(STATE& state, int action,
        int& observation, double& reward, bool save) const;

    void GenerateLegal(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;
    void GeneratePreferred(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;


    virtual void DisplayBeliefs(const BELIEF_STATE& beliefState,
        std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    virtual void DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;


    float data[12545];
    float data2[12544 * 6 + 1];

    COORD get_agent_pos(STATE& state) const;
    virtual void saveVisits() const;

protected:

    enum
    {
        E_NONE,
        E_GOOD,
        E_BAD
    };

    enum
    {
        E_SAMPLE = 4
    };

    void InitGeneral();



    GRID<int> Grid;
    int Size, uselessNum;
    COORD StartPos;


private:

    mutable MEMORY_POOL<MINILAKE_STATE> MemoryPool;
    int up;
    int down;
    int left;
    int right;
    std::vector<double>* visits;
};