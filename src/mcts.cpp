#include "mcts.h"
#include "testsimulator.h"
#include <math.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include "boost/random.hpp"
#include "boost/random/normal_distribution.hpp"
#include "Minilake.h"
#include <algorithm>

using namespace std;
using namespace UTILS;

//-----------------------------------------------------------------------------

MCTS::PARAMS::PARAMS()
    : Verbose(0),
    MaxDepth(100),
    NumSimulations(1000),
    NumStartStates(1000),
    UseTransforms(false),
    NumTransforms(1000),
    MaxAttempts(1000),
    ExpandCount(1),
    ExplorationConstant(1),
    UseRave(false),
    RaveDiscount(1.0),
    RaveConstant(0.01),
    DisableTree(false),
    heur(0),
    s_time_elap(0.0),
    mytime(100),
    Rollouts(1)
{
}

MCTS::MCTS(const SIMULATOR& simulator, const PARAMS& params)
    : Simulator(simulator),
    Params(params),
    TreeDepth(0)
{
    VNODE::NumChildren = Simulator.GetNumActions();
    QNODE::NumChildren = Simulator.GetNumObservations();

    Root = ExpandNode(Simulator.CreateStartState());

    for (int i = 0; i < Params.NumStartStates; i++)
        Root->Beliefs().AddSample(Simulator.CreateStartState());

    //cout << "We are here" << endl;

/*
    std::ifstream file("val.csv");
    int row = 0;
    while (!file.eof())
    {
        file >> data[row];
        //cout << Simulator.data[row] << endl;
        row++;
    }
    //Simulator.get_data(data);
    std::ifstream file2("q_val.csv");
    int row2 = 0;
    while (!file2.eof())
    {
        file2 >> data2[row2];
        row2++;
    }
    */
    for (int i = 0; i < 10; i++)
    {
        num_so_far[i] = 0;
        ave_rol[i] = 0;
    }
    
    //cout << "Not my fault here" << endl;

}

MCTS::~MCTS()
{
    VNODE::Free(Root, Simulator);
    VNODE::FreeAll();
}

bool MCTS::Update(int action, int observation, double reward)
{
   
     History.Add(action, observation);
    BELIEF_STATE beliefs;

    // Find matching vnode from the rest of the tree
    QNODE& qnode = Root->Child(action);
    VNODE* vnode = qnode.Child(observation);
    if (vnode)
    {
        if (Params.Verbose >= 1)
            cout << "Matched " << vnode->Beliefs().GetNumSamples() << " states" << endl;
        beliefs.Copy(vnode->Beliefs(), Simulator);
    }
    else
    {
        if (Params.Verbose >= 1)
            cout << "No matching node found" << endl;
    }

    // Generate transformed states to avoid particle deprivation
    if (Params.UseTransforms)
        AddTransforms(Root, beliefs);

    // If we still have no particles, fail
    if (beliefs.Empty() && (!vnode || vnode->Beliefs().Empty()))
        return false;

    if (Params.Verbose >= 1)
        Simulator.DisplayBeliefs(beliefs, cout);

    // Find a state to initialise prior (only requires fully observed state)
    const STATE* state = 0;
    if (vnode && !vnode->Beliefs().Empty())
        state = vnode->Beliefs().GetSample(0);
    else
        state = beliefs.GetSample(0);

  
    // Delete old tree and create new root
    VNODE::Free(Root, Simulator);
    VNODE* newRoot = ExpandNode(state);
    newRoot->Beliefs() = beliefs;
    Root = newRoot;
    
    
    return true;
}

int MCTS::SelectAction()
{
    if (Params.DisableTree)
        RolloutSearch();
    else
        UCTSearch();
    if (UTILS::getUSeWstein()) {
        if (UTILS::getUseOS()) return OptimisticSampling(Root, true);
        else return ThompsonSampling(Root, true);
    }
    else return UCT(Root, true);
}

void MCTS::RolloutSearch()
{
    std::vector<double> totals(Simulator.GetNumActions(), 0.0);
    int historyDepth = History.Size();
    std::vector<int> legal;
    assert(BeliefState().GetNumSamples() > 0);
    Simulator.GenerateLegal(*BeliefState().GetSample(0), GetHistory(), legal, GetStatus());
    random_shuffle(legal.begin(), legal.end());

    for (int i = 0; i < Params.NumSimulations; i++)
    {
        int action = legal[i % legal.size()];
        STATE* state = Root->Beliefs().CreateSample(Simulator);
        Simulator.Validate(*state);

        int observation;
        double immediateReward, delayedReward, totalReward;
        bool terminal = Simulator.Step(*state, action, observation, immediateReward, false);

        VNODE*& vnode = Root->Child(action).Child(observation);
        if (!vnode && !terminal)
        {
            vnode = ExpandNode(state);
            AddSample(vnode, *state);
        }
        History.Add(action, observation);

        delayedReward = Rollout(*state);
        totalReward = immediateReward + Simulator.GetDiscount() * delayedReward;
        Root->Child(action).Value.Set(0.0, totalReward, getInitStd());
        Simulator.FreeState(state);
        History.Truncate(historyDepth);
    }
}

void MCTS::UCTSearch()
{
    ClearStatistics();
    int historyDepth = History.Size();

    
    auto start = chrono::high_resolution_clock::now();
    double elapsed = 0.0;
    int n = 0;
    //for (int n = 0; n < Params.NumSimulations; n++)
    while (n < Params.NumSimulations)
    {
        auto end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        Params.s_time_elap = elapsed;
        STATE* state = Root->Beliefs().CreateSample(Simulator);
        Simulator.Validate(*state);
        Status.Phase = SIMULATOR::STATUS::TREE;
        if (Params.Verbose >= 2)
        {
            cout << "Starting simulation" << endl;
            Simulator.DisplayState(*state, cout);
        }

        TreeDepth = 0;
        PeakTreeDepth = 0;
        double totalReward = SimulateV(*state, Root);
        StatTotalReward.Add(totalReward);
        StatTreeDepth.Add(PeakTreeDepth);

        if (Params.Verbose >= 2)
            cout << "Total reward = " << totalReward << endl;
        if (Params.Verbose >= 3)
            DisplayValue(4, cout);

        Simulator.FreeState(state);
        History.Truncate(historyDepth);

        auto end_ = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_ - start).count();
        //if (elapsed > Params.mytime)
        //{
          //  break;
        //}
        n++;

    }
    //if (n >= Params.NumSimulations)
   //     cout << "Simulations OVERRRRRR *************** \n";

    DisplayStatistics(cout);
}

double MCTS::SimulateV(STATE& state, VNODE* vnode)
{
    const MINILAKE_STATE& rockstate =
        safe_cast<const MINILAKE_STATE&>(state);
    int action;
    if (UTILS::getUSeWstein()) {
        if (UTILS::getUseOS()) action = OptimisticSampling(vnode, false);
        else action = ThompsonSampling(vnode, false);
    }
    else action = UCT(vnode, false);


    PeakTreeDepth = TreeDepth;
    if (TreeDepth >= Params.MaxDepth) // search horizon reached
        return 0;

    if (TreeDepth == 1)
        AddSample(vnode, state);

    QNODE& qnode = vnode->Child(action);
    double combVisits = vnode->Value.GetCount();
    double totalReward = SimulateQ(state, qnode, action, combVisits);



    vnode->Value.IncrementCount();
    if (UTILS::getUSeWstein()) {
        double q_node_mean = vnode->getQNodeMean();
        double q_node_std_mean = vnode->getQNodeStdMean();
        vnode->Value.Add_VValue_Wstein(q_node_mean, q_node_std_mean);
    }
    else {
        double q_node_mean = vnode->getQNodeMax();
        vnode->Value.Add_VValue_PowerUCT(q_node_mean);
    }
    //AddRave(vnode, totalReward);
    return totalReward;
}

double MCTS::SimulateQ(STATE& state, QNODE& qnode, int action, double combVisits)
{
    int observation;
    double immediateReward, delayedReward = 0;
    float value2 = Simulator.value2(state, action);

    if (Simulator.HasAlpha())
        Simulator.UpdateAlpha(qnode, state);
    STATE* prev_state = Simulator.Copy(state);
    bool terminal = Simulator.Step(state, action, observation, immediateReward, true);
  
    assert(observation >= 0 && observation < Simulator.GetNumObservations());
    History.Add(action, observation);

    if (Params.Verbose >= 3)
    {
        cout << "This is in simulateQ  " << endl;
        Simulator.DisplayAction(action, cout);
        Simulator.DisplayObservation(state, observation, cout);
        Simulator.DisplayReward(immediateReward, cout);
        Simulator.DisplayState(state, cout);
    }

    VNODE*& vnode = qnode.Child(observation);
    if (!vnode && !terminal && qnode.Value.GetCount() >= Params.ExpandCount)
        vnode = ExpandNode(&state);

    if (!terminal)
    {
        TreeDepth++;
        if (vnode)
            delayedReward = SimulateV(state, vnode);
        else
            if (Params.heur == 0)
            {
                //if (Params.s_time_elap < Params.mytime / 2)
                   // delayedReward = 0;
                if (Params.Rollouts == 1) {
                    delayedReward = Rollout(state);
                }
                else {
                    for (int i = 0; i < Params.Rollouts; ++i) {
                        STATE* copy = Simulator.Copy(state);
                        delayedReward += Rollout(*copy);
                        Simulator.FreeState(copy);
                    }
                    delayedReward = delayedReward / double(Params.Rollouts);
                }
             
            }
            else if (Params.heur == 1)
            {
                if (Params.s_time_elap < Params.mytime * 3 / 4)
                    delayedReward = 0;
                else
                    delayedReward = Rollout(state);
            }
            else if (Params.heur == 2)
                // MDP
            {
                if (Params.s_time_elap < Params.mytime / 2)
                    delayedReward = 0;
                else
                    delayedReward = Simulator.value(state, data) / 5.0;
            }
        //delayedReward = 60;
            else if (Params.heur == 3)
                // MDP
            {
                if (Params.s_time_elap < Params.mytime * 3 / 4)
                    delayedReward = 0;
                else
                    delayedReward = Simulator.value(state, data) / 5.0;
            }
            else if (Params.heur == 4)
            {
                if (Params.s_time_elap < Params.mytime / 2)
                    delayedReward = 0;
                else
                    delayedReward = Simulator.value(state, data);
            }
            else if (Params.heur == 5)
            {
                if (Params.s_time_elap < Params.mytime * 3 / 4)
                    delayedReward = 0;
                else
                    delayedReward = Simulator.value(state, data);
            }
        TreeDepth--;
    }

    double totalReward = immediateReward + Simulator.GetDiscount() * delayedReward;
    
    if (qnode.Value.GetCount() == 0) {
        if (UTILS::getUSeWstein()) {
           //totalReward = Simulator.getNNPrediction(prev_state, action);
            qnode.Value.Add_QValue_WsteinRollout(totalReward);
        }
        else {
           
            qnode.Value.Add_QValue_PowerUCTRollout(totalReward);
        }
        qnode.Value.IncrementCount();
    }
    else {
        
        if (UTILS::getUSeWstein()) {           
            double nextValueMean = Simulator.GetDiscount() * qnode.getNextStateValueMean(observation, qnode.Value.GetCount());
            double nextValueStd = Simulator.GetDiscount() * qnode.getNextStateValueStd(observation, qnode.Value.GetCount());
            qnode.Value.IncrementCount();
            qnode.Value.Add_QValue_Wstein(immediateReward, nextValueMean, nextValueStd);
        }
        else {
            qnode.Value.IncrementCount();
            double nextStateValues = Simulator.GetDiscount() * qnode.getNextStateValues();
            qnode.Value.Add_QValue_PowerUCT(totalReward, nextStateValues);
        }
    }
    Simulator.FreeState(prev_state);
    return totalReward;
}



VNODE* MCTS::ExpandNode(const STATE* state)
{
    VNODE* vnode = VNODE::Create();
    vnode->Value.Set(0.0, 0.0, getInitStd());
    Simulator.Prior(state, History, vnode, Status);

    if (Params.Verbose >= 2)
    {
        cout << "Expanding node: ";
        History.Display(cout);
        cout << endl;
    }

    return vnode;
}

void MCTS::AddSample(VNODE* node, const STATE& state)
{
    STATE* sample = Simulator.Copy(state);
    node->Beliefs().AddSample(sample);
    if (Params.Verbose >= 2)
    {
        cout << "Adding sample:" << endl;
        Simulator.DisplayState(*sample, cout);
    }
}



int MCTS::ThompsonSampling(VNODE* vnode, bool greedy) const
{
    int besta = 0;
    double bestq = -Infinity;

   // boost::mt19937 rng(time(0));
    for (int action = 0; action < Simulator.GetNumActions(); action++)
    {
        QNODE& qnode = vnode->Child(action);
        double mean = qnode.Value.GetMean();
        double std = qnode.Value.GetStd();
        double q;
        if (!greedy) {
            //boost::normal_distribution<> nd(mean, std);
            //boost::variate_generator<boost::mt19937&,
             //   boost::normal_distribution<> > var_nor(rng, nd);
            q = Gaussian(mean, std);
        }
        else {
            q = mean;
        }
        if (q > bestq) {
            bestq = q;
            besta = action;
        }
    }
    return besta;

}

int MCTS::OptimisticSampling(VNODE* vnode, bool greedy) const
{
    int besta = 0;
    double bestq = -Infinity;


    double big_N_log = std::log(double(vnode->Value.GetCount() + 1.0));
    for (int action = 0; action < Simulator.GetNumActions(); action++)
    {
        QNODE& qnode = vnode->Child(action);
        double mean = qnode.Value.GetMean();
        double std = qnode.Value.GetStd();
        double q;
        if (!greedy) {
            if (qnode.Value.GetCount() == 0) q = Infinity;
            else q = mean + UTILS::getC() * std *  std::sqrt(big_N_log);
            
        }
        else {
            q = mean;
        }
        if (q > bestq) {
            bestq = q;
            besta = action;
        }
    }
  
    return besta;
}

int MCTS::UCT(VNODE* vnode, bool greedy) const
{
    int besta = 0;
    double bestq = -Infinity;
    

    double big_N_log = std::log(double(vnode->Value.GetCount()  + 1.0));
    for (int action = 0; action < Simulator.GetNumActions(); action++)
    {
        QNODE& qnode = vnode->Child(action);
        double mean = qnode.Value.GetMean();
        double small_n = double(qnode.Value.GetCount());
        double q;
        if (!greedy) {
            if (small_n == 0.0) q = Infinity;
            else {
                q = mean + UTILS::getC() * std::sqrt(big_N_log / small_n);
            }
        }
        else {
            q = mean;
        }
        if (q > bestq) {
            bestq = q;
            besta = action;
        }
    }
    return besta;
}

double MCTS::Rollout(STATE& state)
{
    Status.Phase = SIMULATOR::STATUS::ROLLOUT;
    if (Params.Verbose >= 3)
        cout << "Starting rollout" << endl;

    double totalReward = 0.0;
    double discount = 1.0;
    bool terminal = false;
    int numSteps;
    for (numSteps = 0; numSteps + TreeDepth < Params.MaxDepth && !terminal; ++numSteps)
    {
        int observation;
        double reward;

        int action = Simulator.SelectRandom(state, History, Status);
        terminal = Simulator.Step(state, action, observation, reward, false);
        History.Add(action, observation);

        if (Params.Verbose >= 4)
        {
            Simulator.DisplayAction(action, cout);
            Simulator.DisplayObservation(state, observation, cout);
            Simulator.DisplayReward(reward, cout);
            Simulator.DisplayState(state, cout);
        }

        totalReward += reward * discount;
        discount *= Simulator.GetDiscount();
    }

    StatRolloutDepth.Add(numSteps);
    if (Params.Verbose >= 3)
        cout << "Ending rollout after " << numSteps
        << " steps, with total reward " << totalReward << endl;
    return totalReward;
}

void MCTS::AddTransforms(VNODE* root, BELIEF_STATE& beliefs)
{
    int attempts = 0, added = 0;

    // Local transformations of state that are consistent with history
    while (added < Params.NumTransforms && attempts < Params.MaxAttempts)
    {
        STATE* transform = CreateTransform();
        if (transform)
        {
            beliefs.AddSample(transform);
            added++;
        }
        attempts++;
    }

    if (Params.Verbose >= 1)
    {
        cout << "Created " << added << " local transformations out of "
            << attempts << " attempts" << endl;
    }
}

STATE* MCTS::CreateTransform() const
{
    int stepObs;
    double stepReward;

    STATE* state = Root->Beliefs().CreateSample(Simulator);
    Simulator.Step(*state, History.Back().Action, stepObs, stepReward, false);
    if (Simulator.LocalMove(*state, History, stepObs, Status))
        return state;
    Simulator.FreeState(state);
    return 0;
}

void MCTS::saveVisitValue(VNODE* r, int level, std::ofstream& output, int parent_n, double max)
{
   
   
    if (level <4) {
        
        for (int node = 0; node < VNODE::NumChildren; node++) {
            QNODE current = r->Child(node);
           double current_max = 0.0;
            for (int observation = 0; observation < QNODE::NumChildren; observation++) {
                VNODE* n = current.Child(observation);
                if (n) {
                    if (current_max > n->Value.GetMean()) current_max = n->Value.GetMean();
                }
            }
            for (int observation = 0; observation < QNODE::NumChildren; observation++) {
                VNODE* n = current.Child(observation);
                    if (n) {
                    saveVisitValue(n, level + 1, output, r->Value.GetCount(), current_max );

                }
            }
          
        }
    }
   
    if (level == 4) {
        output << r->Value.GetMean() << " " << r->Value.GetCount() / float(parent_n);
        output << "\n";
    }
  
}

double MCTS::UCB[UCB_N][UCB_n];
bool MCTS::InitialisedFastUCB = true;

void MCTS::InitFastUCB(double exploration)
{
    cout << "Initialising fast UCB table... ";
    for (int N = 0; N < UCB_N; ++N)
        for (int n = 0; n < UCB_n; ++n)
            if (n == 0)
                UCB[N][n] = Infinity;
            else
                UCB[N][n] = exploration * sqrt(log(N + 1) / n);
    cout << "done" << endl;
    InitialisedFastUCB = true;
}

inline double MCTS::FastUCB(int N, int n, double logN) const
{
    if (InitialisedFastUCB && N < UCB_N && n < UCB_n)
        return UCB[N][n];

    if (n == 0)
        return Infinity;
    else
        return Params.ExplorationConstant * sqrt(logN / n);
}

void MCTS::ClearStatistics()
{
    StatTreeDepth.Clear();
    StatRolloutDepth.Clear();
    StatTotalReward.Clear();
}

void MCTS::DisplayStatistics(ostream& ostr) const
{
    if (Params.Verbose >= 1)
    {
        StatTreeDepth.Print("Tree depth", ostr);
        StatRolloutDepth.Print("Rollout depth", ostr);
        StatTotalReward.Print("Total reward", ostr);
    }

    if (Params.Verbose >= 2)
    {
        ostr << "Policy after " << Params.NumSimulations << " simulations" << endl;
        DisplayPolicy(6, ostr);
        ostr << "Values after " << Params.NumSimulations << " simulations" << endl;
        DisplayValue(6, ostr);
    }
}

void MCTS::DisplayValue(int depth, ostream& ostr) const
{
    HISTORY history;
    ostr << "MCTS Values:" << endl;
    Root->DisplayValue(history, depth, ostr);
}

void MCTS::DisplayPolicy(int depth, ostream& ostr) const
{
    HISTORY history;
    ostr << "MCTS Policy:" << endl;
    Root->DisplayPolicy(history, depth, ostr);
}

//-----------------------------------------------------------------------------

void MCTS::UnitTest()
{

}

void MCTS::UnitTestGreedy()
{

}

void MCTS::UnitTestUCB()
{

}

void MCTS::UnitTestRollout()
{
}

void MCTS::UnitTestSearch(int depth)
{

}

//-----------------------------------------------------------------------------
