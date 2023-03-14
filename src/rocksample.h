#ifndef ROCKSAMPLE_H
#define ROCKSAMPLE_H

#include "simulator.h"
#include "coord.h"
#include "grid.h"

class ROCKSAMPLE_STATE : public STATE
{
public:

    COORD AgentPos;
    struct ENTRY
    {
        bool Valuable;
        bool Collected;
        int Count;    				// Smart knowledge
        int Measured; 				// Smart knowledge
        double LikelihoodValuable;	// Smart knowledge
        double LikelihoodWorthless;	// Smart knowledge
        double ProbValuable;		// Smart knowledge
    };
    std::vector<ENTRY> Rocks;
    int Target; // Smart knowledge
};

class ROCKSAMPLE : public SIMULATOR
{
public:

    ROCKSAMPLE(int size, int rocks);

    virtual STATE* Copy(const STATE& state) const;
    virtual void Validate(const STATE& state) const;
    virtual STATE* CreateStartState() const;
    virtual void FreeState(STATE* state) const;
    virtual bool Step(STATE& state, int action,
        int& observation, double& reward, bool saveVisits) const;

    void GenerateLegal(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;
    void GeneratePreferred(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;
    virtual bool LocalMove(STATE& state, const HISTORY& history,
        int stepObservation, const STATUS& status) const;

    virtual void DisplayBeliefs(const BELIEF_STATE& beliefState,
        std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    virtual void DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;

    virtual float value(STATE& state, float* data) const;
    virtual float value2(STATE& state, int action) const;
    float data[12545];
    float data2[12544 * 6 + 1];
    int state2ind(STATE& state) const;
    int qa2ind(STATE& state, int a) const;
    COORD get_agent_pos(STATE& state) const;
    std::vector<ROCKSAMPLE_STATE::ENTRY> get_rocks(STATE& state) const;

    virtual void saveVisits() const override;

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
    void Init_7_8();
    void Init_11_11();
    int GetObservation(const ROCKSAMPLE_STATE& rockstate, int rock) const;
    int SelectTarget(const ROCKSAMPLE_STATE& rockstate) const;

    GRID<int> Grid;
    std::vector<COORD> RockPos;
    int Size, NumRocks;
    COORD StartPos;
    double HalfEfficiencyDistance;
    double SmartMoveProb;
    int UncertaintyCount;

private:

    mutable MEMORY_POOL<ROCKSAMPLE_STATE> MemoryPool;
    std::vector<double>* visits;
};



class RIVER_STATE : public STATE
{
public:

    COORD AgentPos;

};

class RIVERSWIM : public SIMULATOR
{
public:

    RIVERSWIM(int size);
    ~RIVERSWIM();
    virtual STATE* Copy(const STATE& state) const;
    virtual void Validate(const STATE& state) const;
    virtual STATE* CreateStartState() const;
    virtual void FreeState(STATE* state) const;
    virtual bool Step(STATE& state, int action,
        int& observation, double& reward, bool saveVisits) const;

    void GenerateLegal(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;
    void GeneratePreferred(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;


    virtual void DisplayBeliefs(const BELIEF_STATE& beliefState,
        std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    virtual void DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;

    virtual void saveVisits() const override;

    float data[12545];
    float data2[12544 * 6 + 1];

    COORD get_agent_pos(STATE& state) const;


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

    mutable MEMORY_POOL<RIVER_STATE> MemoryPool;
    std::vector<double>* visits;
};







class NCHAIN_STATE : public STATE
{
public:

    COORD AgentPos;

};

class NCHAIN : public SIMULATOR
{
public:

    NCHAIN(int size);
    ~NCHAIN();
    virtual STATE* Copy(const STATE& state) const;
    virtual void Validate(const STATE& state) const;
    virtual STATE* CreateStartState() const;
    virtual void FreeState(STATE* state) const;
    virtual bool Step(STATE& state, int action,
        int& observation, double& reward, bool saveVisits) const;

    void GenerateLegal(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;
    void GeneratePreferred(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;


    virtual void DisplayBeliefs(const BELIEF_STATE& beliefState,
        std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    virtual void DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;

    virtual void saveVisits() const override;

    float data[12545];
    float data2[12544 * 6 + 1];

    COORD get_agent_pos(STATE& state) const;


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

    mutable MEMORY_POOL<NCHAIN_STATE> MemoryPool;
    std::vector<double>* visits;
};


class ARMS_STATE : public STATE
{
public:

    COORD AgentPos;

};

class ARMS : public SIMULATOR
{
public:

    ARMS(int size);
    ~ARMS();
    virtual STATE* Copy(const STATE& state) const;
    virtual void Validate(const STATE& state) const;
    virtual STATE* CreateStartState() const;
    virtual void FreeState(STATE* state) const;
    virtual bool Step(STATE& state, int action,
        int& observation, double& reward, bool saveVisits) const;

    void GenerateLegal(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;
    void GeneratePreferred(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;


    virtual void DisplayBeliefs(const BELIEF_STATE& beliefState,
        std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    virtual void DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;

    virtual void saveVisits() const override;
    float data[12545];
    float data2[12544 * 6 + 1];

   

    COORD get_agent_pos(STATE& state) const;


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

    mutable MEMORY_POOL<ARMS_STATE> MemoryPool;
    std::vector<double>* visits;
};


class LAKE_STATE : public STATE
{
public:

    COORD AgentPos;

};

class FROZENLAKE : public SIMULATOR
{
public:

    FROZENLAKE(int size);
    ~FROZENLAKE();
    virtual STATE* Copy(const STATE& state) const;
    virtual void Validate(const STATE& state) const;
    virtual STATE* CreateStartState() const;
    virtual void FreeState(STATE* state) const;
    virtual bool Step(STATE& state, int action,
        int& observation, double& reward, bool saveVisits) const;

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
    virtual double getNNPrediction(STATE* state, int action) const override;

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

    mutable MEMORY_POOL<LAKE_STATE> MemoryPool;
    int up;
    int down;
    int left;
    int right;
    std::vector<double>* visits;
};

class TAXI_STATE : public STATE
{
public:

    COORD AgentPos;
    std::vector<bool> passengers;

};

class TAXI : public SIMULATOR
{
public:

    TAXI(int size);
    ~TAXI();
    virtual STATE* Copy(const STATE& state) const;
    virtual void Validate(const STATE& state) const;
    virtual STATE* CreateStartState() const;
    virtual void FreeState(STATE* state) const;
    virtual bool Step(STATE& state, int action,
        int& observation, double& reward, bool saveVisits) const;

    void GenerateLegal(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;
    void GeneratePreferred(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;


    virtual void DisplayBeliefs(const BELIEF_STATE& beliefState,
        std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    virtual void DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;

    virtual void saveVisits() const override;

    float data[12545];
    float data2[12544 * 6 + 1];

    COORD get_agent_pos(STATE& state) const;


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
    int up = 3;
    int down = 1;
    int left = 0;
    int right = 2;
    mutable MEMORY_POOL<TAXI_STATE> MemoryPool;
    std::vector<double>* visits;
};


#endif
