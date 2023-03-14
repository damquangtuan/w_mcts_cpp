#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <random>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "coord.h"
#include "memorypool.h"
#include <algorithm>
#include "boost/random.hpp"
#include "boost/random/normal_distribution.hpp"
#define LargeInteger 1000000
#define Infinity 1e+10
#define Tiny 1e-10

#ifdef DEBUG
#define safe_cast dynamic_cast
#else
#define safe_cast static_cast
#endif

namespace UTILS
{
    struct data {
        double p;
        double std;
        double C;
        bool useWstein;
        std::string visits_file;
        bool useOS;
        data() {

        }
        data(double p_, double std_, double C_) {
            p = p_;
            std = std_;
            C = C_;
        }
        void setP(double p_) {
            p = p_;
        }
        void setC(double C_) {
            C = C_;
        }
        void setStd(double std_) {
            std = std_;
        }
        void setUseWstein(bool b) {
            useWstein = b;
        }
        void setVisitString(std::string s) {
            visits_file = s;
        }
        void setUseOS(bool b) {
            useOS = b;
        }
    };
    extern data dat;
    inline static bool getUSeWstein() {
        return dat.useWstein;
    }
    inline static double getC() {
        return dat.C;
    }

    inline static double getInitStd() {
        return dat.std;
    }
    inline static double getP() {
        return dat.p;
    }
    inline static std::string getVisitsString() {
        return dat.visits_file;
    }
    inline static bool getUseOS() {
        return dat.useOS;
    }

    static  boost::mt19937 rng(time(0));
    inline double Gaussian(double mean, double std) {
        boost::normal_distribution<> nd(mean, std);
        boost::variate_generator<boost::mt19937&,
            boost::normal_distribution<> > var_nor(rng, nd);
        double q = var_nor();
        return q;
    }
    inline double UniformDist(double min, double max) {
        boost::uniform_real<> ud(min, max);
        boost::variate_generator<boost::mt19937&,
            boost::uniform_real<> > var_nor(rng, ud);
        double q = var_nor();
        return q;
    }

    inline int Sign(int x)
    {
        return (x > 0) - (x < 0);
    }

    inline int sample_discrete(std::vector<double> &probabilities) {
        std::mt19937 gen;
        std::uniform_real_distribution<double> dis;
        double v = dis(gen);
        int count = 0;
        double acc = probabilities[0];
        while (v > acc) {
            count += 1;
            acc += probabilities[count];
        }

        return count;
    }

    inline int Random(int max)
    {
        return rand() % max;
    }

    inline int Random(int min, int max)
    {
        return rand() % (max - min) + min;
    }

    inline double RandomDouble(double min, double max)
    {
        return (double)rand() / RAND_MAX * (max - min) + min;
    }

    inline void RandomSeed(int seed)
    {
        srand(seed);
    }

    inline bool Bernoulli(double p)
    {
        return rand() < p * RAND_MAX;
    }

    inline bool Near(double x, double y, double tol)
    {
        return fabs(x - y) <= tol;
    }

    inline bool CheckFlag(int flags, int bit) { return (flags & (1 << bit)) != 0; }

    inline void SetFlag(int& flags, int bit) { flags = (flags | (1 << bit)); }

    template<class T>
    inline bool Contains(std::vector<T>& vec, const T& item)
    {
        return std::find(vec.begin(), vec.end(), item) != vec.end();
    }

    void UnitTest();

}

#endif // UTILS_H
