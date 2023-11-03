/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   TTPSolver.h
 * Author: Majid Namazi
 *
 * Created on 27 December 2019
 */

#ifndef TTPSOLVER_H
#define TTPSOLVER_H

#include "TTPInstance.h"
#include "TTPSolution.h"
#include  <sys/time.h>

double get_wall_time();

double get_cpu_time();

class TTPSolver {
    
        TTPInstance &ttp = *(TTPInstance *)nullptr;
        int          timeLimit    ; 
        double       startWallTime;
        double       bestOverAllTotalGain; 
        int         *bestOverAllTour;
        int         *bestOverAllPlan;           
        int          acceptedRevCount;
        double       acceptedRevLenAvg;
        
        int         *chainedLinKernTour();
        void         updateBestOverAll(int *Tour, int *Plan, double TotalGain);
        void         quickSort(double *Scores, int *Indices, int Low, int High);
        int         *sortItems(double *Scores);
        void         packItems(int *Tour, double *Len, double Alpha, int *Plan);
        int         *packIterative(int *Tour, double C=5, double D=2.5, int Q=20);
        int         *insertion(int *Tour);
        int         *initPlan(int *Tour);
        double       computeTotalGain(TTPSolution &Sol);
        double       computeTotalGain(int *Tour, int *Plan);
        
        TTPSolution &hillClimbingKPSolverAllItems(TTPSolution &Sol);
        TTPSolution &hillClimbingKPSolverBoundaryItems(TTPSolution &Sol);
        TTPSolution &steepestAscentHillClimbingTspSolver(TTPSolution &Sol);
        TTPSolution &coordSteepestAscentHillClimbingTspSolver(TTPSolution &Sol);
        
        void         initLowPickedHighUnPicked(TTPSolution &Sol);
        void         initMinPRatioes(TTPSolution &Sol);
        
        
        void         initMaxURatioes(TTPSolution &Sol);
        
        void         initMinMaxRatioes(TTPSolution &Sol) {
            initMinPRatioes(Sol);
            initMaxURatioes(Sol);
        };
        void         prepareForCoordMoves(TTPSolution &Sol) {
            initLowPickedHighUnPicked(Sol);
            initMinMaxRatioes(Sol);
        };
        
    public:

        TTPSolution  solve(int Alg);
        
        TTPSolver();

        TTPSolver(TTPInstance &TTP, double StartWallTime, int TimeLimit);

        ~TTPSolver() {};
private:

};

#endif /* TTPSOLVER_H */

