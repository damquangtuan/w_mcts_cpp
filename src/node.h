#ifndef NODE_H
#define NODE_H

#include "beliefstate.h"
#include "utils.h"
#include <iostream>

class HISTORY;
class SIMULATOR;
class QNODE;
class VNODE;

//-----------------------------------------------------------------------------
// Efficient computation of value from alpha vectors
// Only used for explicit POMDPs
struct ALPHA
{
    std::vector<double> AlphaSum;
    double MaxValue;
};

//-----------------------------------------------------------------------------

template<class COUNT>
class VALUE
{
public:

    void Set(double count, double mean_, double std_)
    {
        Count = count;
        mean = mean_;
        std = std_;
    }
    void Add(double totalReward)
    {
        Count += 1.0;
        Total += totalReward;
    }

    void Add(double totalReward, COUNT weight)
    {
        Count += weight;
        Total += totalReward * weight;
    }
    void IncrementCount() {
        Count += 1.0;
    }
    void Add_VValue_Wstein(double qmean, double qstd_mean)
    {
        mean = std::pow(qmean, 1.0 /UTILS::getP());
        std = std::pow(qstd_mean, 1.0 / UTILS::getP());
        
        //Total += totalReward;
        //W Update
    }

    void Add_QValue_Wstein(double totalReward, double nextStateMean, double nextStateStd)
    {

        mean = (mean * Count + totalReward + nextStateMean) / (Count + 1.0);
        
        std = (Count * nextStateStd) / (Count + 1.0);
    }

    void Add_QValue_WsteinRollout(double totalReward)
    {

        mean = totalReward;

           
    }

    void Add_VValue_PowerUCT(double qmean)
    {
        mean = std::pow(qmean, 1.0 / UTILS::getP());
     
        //Total += totalReward;
        //W Update
    }

    void Add_QValue_PowerUCT(double totalReward, double nextStateMean)
    {

        mean = (totalReward + nextStateMean) / (Count);
       
    }

    void Add_QValue_PowerUCTRollout(double totalReward)
    {

        mean = totalReward;

    }
    double GetMean() const
    {
        return mean;
    }

    double GetStd() const {
        return std;
    }

    double GetValue() const
    {
        return Count == 0 ? Total : Total / Count;
    }

    double GetTotalValue() const
    {
        return Total;
    }

    COUNT GetCount() const
    {
        return Count;
    }

private:

    COUNT Count;
    double Total;
    double mean, std;
};

//-----------------------------------------------------------------------------

class QNODE
{
public:

    VALUE<int> Value;


    void Initialise();

    VNODE*& Child(int c) { return Children[c]; }
    VNODE* Child(int c) const { return Children[c]; }
    VNODE*& Father() { return Parent; }
    VNODE* Father() const { return Parent; }

    void SetFather(VNODE* father);
    ALPHA& Alpha() { return AlphaData; }
    const ALPHA& Alpha() const { return AlphaData; }

    double getNextStateValueMean(int observation, double combVisits);
    double getNextStateValueStd(int observation, double combVisits);
    double getNextStateValues();

    void DisplayValue(HISTORY& history, int maxDepth, std::ostream& ostr) const;
    void DisplayPolicy(HISTORY& history, int maxDepth, std::ostream& ostr) const;

    static int NumChildren;

private:

    std::vector<VNODE*> Children;
    VNODE* Parent;
    ALPHA AlphaData;

    friend class VNODE;
};

//-----------------------------------------------------------------------------

class VNODE : public MEMORY_OBJECT
{
public:

    VALUE<int> Value;

    double CurExp;
    double PrevExp;

    void Initialise();
    static VNODE* Create();
    static void Free(VNODE* vnode, const SIMULATOR& simulator);
    static void Free2(VNODE* vnode, const SIMULATOR& simulator, int action, int obs);
    static void FreeAll();

    QNODE& Child(int c) { return Children[c]; }
    const QNODE& Child(int c) const { return Children[c]; }
    QNODE* Father() { return Parent; }
    QNODE* Father() const { return Parent; }
    void SetFather(QNODE* father);
    BELIEF_STATE& Beliefs() { return BeliefState; }
    const BELIEF_STATE& Beliefs() const { return BeliefState; }

    double getQNodeMean();
    double getQNodeMax();
    double getQNodeStdMean();

    void SetChildren(int count, double value);

    void DisplayValue(HISTORY& history, int maxDepth, std::ostream& ostr) const;
    void DisplayPolicy(HISTORY& history, int maxDepth, std::ostream& ostr) const;

    static int NumChildren;

private:

    std::vector<QNODE> Children;
    QNODE* Parent;
    BELIEF_STATE BeliefState;
    static MEMORY_POOL<VNODE> VNodePool;
};

#endif // NODE_H
