/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   main.cpp
 * Author: Majid Namazi
 *
 * Created on 27 December 2019
 */

#include <cstdlib>
#include <ctime>

#include "TTPSolver.h"

using namespace std;

int main(int argc, char** argv) {
    
    double startWallTime = get_wall_time();
    srand(time(0));

    if (argc<2) {
        cout<<"Usage: CoCo instance_name [timeout(in seconds) [algorithm_number(1,2,3 or 4)]]"<<endl<<endl;
        cout<<"Usage example: "<<endl;
        cout<<"CoCo a280_n2790_uncorr_10 600 4"<<endl;
        cout<<"Supposing that the current directory includes /data/a280-ttp/a280_n2790_uncorr_10.ttp";
        exit(1);
    }

    TTPInstance ttp(argv[1]);
    
    int     timeLimit;
    int     algorithm;
    if (argc>=3)   
        timeLimit = atoi(argv[2]);
    else
        timeLimit = 600;  
    if (argc==4)
        algorithm = atoi(argv[3]);
    else
        algorithm = 4;    
    
    TTPSolver  solver(ttp,startWallTime,timeLimit);
    TTPSolution sol = solver.solve(algorithm);
    
    cout<<sol<<endl;
    
}

