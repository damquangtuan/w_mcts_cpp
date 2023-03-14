#include "node.h"
#include "history.h"
#include "utils.h"

using namespace std;

//-----------------------------------------------------------------------------

int QNODE::NumChildren = 0;

void QNODE::Initialise()
{
    assert(NumChildren);
    Children.resize(NumChildren);
    for (int observation = 0; observation < QNODE::NumChildren; observation++)
        Children[observation] = 0;
    AlphaData.AlphaSum.clear();
}

double QNODE::getNextStateValueMean(int observation, double combVisits)
{
    double mean = 0.0;

    for (int observation = 0; observation < NumChildren; observation++)
    {
        if (Children[observation])
        {
            mean += Children[observation]->Value.GetCount() * Children[observation]->Value.GetMean();
          
        }
    }
   
    if (combVisits > 0.0) return mean / combVisits;
    else return mean;
}

double QNODE::getNextStateValueStd(int observation, double combVisits)
{
    double mean = 0.0;
   
    for (int observation = 0; observation < NumChildren; observation++)
    {
        if (Children[observation])
        {
            mean += Children[observation]->Value.GetCount() * Children[observation]->Value.GetStd();
          
        }
    }

    if (combVisits > 0.0 && mean > 0.0) {
        return mean / combVisits;
    }
    else return UTILS::getInitStd() ;
}

double QNODE::getNextStateValues()
{
    
    double mean = 0.0;
    for (int observation = 0; observation < NumChildren; observation++)
    {
        if (Children[observation])
        {
            mean += Children[observation]->Value.GetCount() * Children[observation]->Value.GetMean();
           
        }
    }

  
    return mean;
}

void QNODE::DisplayValue(HISTORY& history, int maxDepth, ostream& ostr) const
{
    history.Display(ostr);
    ostr << ": " << Value.GetMean() << " (" << Value.GetCount() << ")\n";
    if (history.Size() >= maxDepth)
        return;

    for (int observation = 0; observation < NumChildren; observation++)
    {
        if (Children[observation])
        {
            history.Back().Observation = observation;
            Children[observation]->DisplayValue(history, maxDepth, ostr);
        }
    }
}

void QNODE::DisplayPolicy(HISTORY& history, int maxDepth, ostream& ostr) const
{
    history.Display(ostr);
    ostr << ": " << Value.GetMean() << " (" << Value.GetCount() << ")\n";
    if (history.Size() >= maxDepth)
        return;

    for (int observation = 0; observation < NumChildren; observation++)
    {
        if (Children[observation])
        {
            history.Back().Observation = observation;
            Children[observation]->DisplayPolicy(history, maxDepth, ostr);
        }
    }
}

//-----------------------------------------------------------------------------

MEMORY_POOL<VNODE> VNODE::VNodePool;

int VNODE::NumChildren = 0;

void VNODE::Initialise()
{
    assert(NumChildren);
    Children.resize(VNODE::NumChildren);
    for (int action = 0; action < VNODE::NumChildren; action++)
        Children[action].Initialise();
}

VNODE* VNODE::Create()
{
    VNODE* vnode = VNodePool.Allocate();
    vnode->Initialise();
    return vnode;
}

void VNODE::Free(VNODE* vnode, const SIMULATOR& simulator)
{
   
        vnode->BeliefState.Free(simulator);
        VNodePool.Free(vnode);
        for (int action = 0; action < VNODE::NumChildren; action++)
            for (int observation = 0; observation < QNODE::NumChildren; observation++)
                if (vnode->Child(action).Child(observation))
                    Free(vnode->Child(action).Child(observation), simulator);
    
}

void VNODE::Free2(VNODE* vnode, const SIMULATOR& simulator, int action_, int obs_)
{
    vnode->BeliefState.Free(simulator);
    VNodePool.Free(vnode);
    for (int action = 0; action < VNODE::NumChildren; action++)
            for (int observation = 0; observation < QNODE::NumChildren; observation++)
                if (action_ != action && observation != obs_) {
                    if (vnode->Child(action).Child(observation))
                        Free(vnode->Child(action).Child(observation), simulator);
                }
}

void VNODE::FreeAll()
{
    VNodePool.DeleteAll();
}

double VNODE::getQNodeMean()
{
    double mean = 0.0;
    for (int node = 0; node < VNODE::NumChildren; node++) {
        QNODE current = Children[node];
        if (current.Value.GetMean() <= -Infinity) continue;
        mean += current.Value.GetCount() * std::pow(current.Value.GetMean(), UTILS::getP());
    }
    return mean / double(Value.GetCount());
}

double VNODE::getQNodeMax()
{
    double mean = -Infinity;
    for (int node = 0; node < VNODE::NumChildren; node++) {
        QNODE current = Children[node];
        if (current.Value.GetMean() <= -Infinity) continue;
        if (current.Value.GetMean() > mean) mean = current.Value.GetMean();
    }
    if (mean == -Infinity) return 0.0;
    return mean;
}

double VNODE::getQNodeStdMean()
{
    double mean = 0.0;
    for (int node = 0; node < VNODE::NumChildren; node++) {
        QNODE current = Children[node];
        if (current.Value.GetMean() <= -Infinity) continue;
        mean += current.Value.GetCount() * std::pow(current.Value.GetStd(), UTILS::getP());
    }
    return mean / double(Value.GetCount());
}

void VNODE::SetChildren(int count, double value)
{
    for (int action = 0; action < NumChildren; action++)
    {
        QNODE& qnode = Children[action];
        qnode.Value.Set(count, value, UTILS::getInitStd());

    }
}

void VNODE::DisplayValue(HISTORY& history, int maxDepth, ostream& ostr) const
{
    if (history.Size() >= maxDepth)
        return;

    for (int action = 0; action < NumChildren; action++)
    {
        history.Add(action);
        Children[action].DisplayValue(history, maxDepth, ostr);
        history.Pop();
    }
}

void VNODE::DisplayPolicy(HISTORY& history, int maxDepth, ostream& ostr) const
{
    if (history.Size() >= maxDepth)
        return;

    double bestq = -Infinity;
    int besta = -1;
    for (int action = 0; action < NumChildren; action++)
    {
        if (Children[action].Value.GetMean() > bestq)
        {
            besta = action;
            bestq = Children[action].Value.GetMean();
        }
    }

    if (besta != -1)
    {
        history.Add(besta);
        Children[besta].DisplayPolicy(history, maxDepth, ostr);
        history.Pop();
    }
}

//-----------------------------------------------------------------------------
