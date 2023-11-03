/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   CLKSolver.h
 * Author: majid
 *
 * Created on 27 December 2019
 */

// Imported from Concorde TSP Solver at 
// http://www.math.uwaterloo.ca/tsp/concorde.html

#ifndef CLKSOLVER_H
#define CLKSOLVER_H

#define BIGDOUBLE (1e30)

#define LK_RANDOM   (0)
#define LK_NEIGHBOR (1)
#define LK_GREEDY   (2)
#define LK_BORUVKA  (3)
#define LK_QBORUVKA (4)

#define MATRIX_LOWER_DIAG_ROW  0      
#define MATRIX_UPPER_ROW       1
#define MATRIX_UPPER_DIAG_ROW  2
#define MATRIX_FULL_MATRIX     3

class CLKSolver {

    private:
        int norm;
        int seed = 0;
        int binary_in = 0;
        int binary_edges = 0;
        int tsplib_in = 1;
        int nnodes_want = 0;
        int gridsize = 0;
        int nearnum = 0;
        int quadtry = 2;
        int run_silently = 1;
        int kick_type;
        int tour_type = LK_QBORUVKA;

        int in_repeater = -1;
        int number_runs = 0;
        double time_bound = -1.0;
        double length_bound = -1.0;
        
    public:    
        int *tour(int Ncount, double *X, double *Y, int Norm, double Time_bound, int In_repeater);
        CLKSolver();
        ~CLKSolver() {};
};

#endif /* CLKSOLVER_H */

