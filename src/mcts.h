#ifndef MCTS_H
#define MCTS_H

#include "simulator.h"
#include "node.h"
#include "statistic.h"

class MCTS
{
public:

    struct PARAMS
    {
        PARAMS();

        int Verbose;
        int MaxDepth;
        int NumSimulations;
        int NumStartStates;
        bool UseTransforms;
        int NumTransforms;
        int MaxAttempts;
        int ExpandCount;
        double ExplorationConstant;
        bool UseRave;
        double RaveDiscount;
        double RaveConstant;
        bool DisableTree;
        int heur;
        float s_time_elap;
        float mytime;
        double Temperature;
        double Epsilon;
        bool useMents;
        int Rollouts;
    };

    MCTS(const SIMULATOR& simulator, const PARAMS& params);
    ~MCTS();

    int SelectAction();
    bool Update(int action, int observation, double reward);

    void UCTSearch();
    void RolloutSearch();

    int EXT3(VNODE* vnode, bool greedy) const;

    double Rollout(STATE& state);

    const BELIEF_STATE& BeliefState() const { return Root->Beliefs(); }
    const HISTORY& GetHistory() const { return History; }
    const SIMULATOR::STATUS& GetStatus() const { return Status; }
    void ClearStatistics();
    void DisplayStatistics(std::ostream& ostr) const;
    void DisplayValue(int depth, std::ostream& ostr) const;
    void DisplayPolicy(int depth, std::ostream& ostr) const;

    static void UnitTest();
    static void InitFastUCB(double exploration);
    float data[12545];
    float data2[12544 * 6 + 1];

    //my extra
    double ave_rol[10];
    int num_so_far[10];
    double av_move;
    int num_roll;


private:

    const SIMULATOR& Simulator;
    int TreeDepth, PeakTreeDepth;
    PARAMS Params;
    VNODE* Root;
    HISTORY History;
    SIMULATOR::STATUS Status;

    STATISTIC StatTreeDepth;
    STATISTIC StatRolloutDepth;
    STATISTIC StatTotalReward;


    int ThompsonSampling(VNODE* vnode, bool greedy) const;
    int OptimisticSampling(VNODE* vnode, bool greedy) const;
    int UCT(VNODE* vnode, bool greedy) const;
    int SelectRandom() const;
    double SimulateV(STATE& state, VNODE* vnode);
    double SimulateQ(STATE& state, QNODE& qnode, int action, double combVisits);
    double SimulateVMents(STATE& state, VNODE* vnode);
    double SimulateQMents(STATE& state, QNODE& qnode, int action);
    void AddRave(VNODE* vnode, double totalReward);
    VNODE* ExpandNode(const STATE* state);
    void AddSample(VNODE* node, const STATE& state);
    void AddTransforms(VNODE* root, BELIEF_STATE& beliefs);
    STATE* CreateTransform() const;
    void Resample(BELIEF_STATE& beliefs);
    void saveVisitValue(VNODE* r, int level, std::ofstream& output, int n, double max);

    // Fast lookup table for UCB
    static const int UCB_N = 10000, UCB_n = 100;
    static double UCB[UCB_N][UCB_n];
    static bool InitialisedFastUCB;

    double FastUCB(int N, int n, double logN) const;


    static void UnitTestGreedy();
    static void UnitTestUCB();
    static void UnitTestRollout();
    static void UnitTestSearch(int depth);

};

#endif // MCTS_H
