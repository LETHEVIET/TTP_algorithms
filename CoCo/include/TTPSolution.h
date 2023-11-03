/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   TTPSolution.h
 * Author: Majid Namazi
 *
 * Created on 27 December 2019
 */

#ifndef TTPSOLUTION_H
#define TTPSOLUTION_H

#include <iostream>

using namespace std;

class TwoOPT;
class PGCH;
class BitFlip;
class TTPSolver;

struct SolPosition
{
    double  accTime;            /// Accumulated time 
    double  addTime;            /// Added time in a position 
    long    accWeight;          /// Accumulated weight
    long    addWeight;          /// Added weight in a position
    double  minPRatio;          /// Minimum picked ratio so far, looking forwards ---> 
    double  maxURatio;          /// Maximum un-picked ratio hereafter, looking backwards <---
};

struct SolCity
{
    int     posNum;             /// Position number in the tour
    int     lowPicked;          /// Location of the lowest-ratio picked item in the availItems list
    int     highUnPicked;       /// Location of the highest-ratio un-picked item in the availItems list
};

class TTPSolution {
        int          numCities;          /// Number of cities
        int          numItems;           /// Number of items
        long         totalProfit;        /// Total profit value
        double       totalTime;          /// Total traveling time 
        double       totalGain;          /// Total gain value
        long         remSpace;           /// Remained space in the knapsack      
        int         *tour;               /// Position based, cyclic tour
        int         *plan;               /// Item based, collection plan
        SolPosition *positions;          /// Keeps position related data
        SolCity     *cities;             /// Keeps city related data
        void         init();
        void         destroy();
        void         copy(const TTPSolution &Sol);       
        
    public:
        
        long         getTotalProfit() { return totalProfit;};
        
        double       getTotalTime()   {return totalTime;};
        
        double       getTotalGain()   {return totalGain;};         
        
        long         getRemSpace()    {return remSpace;};      
        
        int         *getTour(){ return tour;};
        
        void         setTour(int *Tour);
        
        int         *getPlan() { return plan;};
        
        void         setPlan(int *Plan);
        
        SolPosition *getPositions() {return positions;};
        
        SolCity     *getCities() {return cities;};
        
        TTPSolution();
        
        TTPSolution(int NumCities, int NumItems, int *Tour, int *Plan);
        
        TTPSolution(const TTPSolution &Sol);
        
        TTPSolution &operator=(TTPSolution &Sol);
        
        ~TTPSolution();
        
        friend ostream &operator<<(ostream &Out, TTPSolution &Sol);
        
        friend class TwoOPT; 
        friend class PGCH;
        friend class BitFlip;
        friend class TTPSolver;
};

#endif /* TTPSOLUTION_H */

