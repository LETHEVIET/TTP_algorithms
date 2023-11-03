/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   CLKSolver.cpp
 * Author: Majid Namazi
 * 
 * Created on 27 December 2019
 */

// Imported from Concorde TSP Solver at 
// http://www.math.uwaterloo.ca/tsp/concorde.html


#include  "CLKSolver.h"
#include  <iostream>

using namespace std;

extern "C" {
#include "concorde.h"
}

void randcycle (int ncount, int *cyc, CCrandstate *rstate)
{
    int i, k, temp;

    for (i = 0; i < ncount; i++)
        cyc[i] = i;

    for (i = ncount; i > 1; i--) {
        k = CCutil_lprand (rstate) % i;
        CC_SWAP (cyc[i - 1], cyc[k], temp);
    }
}
CLKSolver::CLKSolver() 
{
    norm = CC_EUCLIDEAN;
    kick_type = CC_LK_WALK_KICK;
}

int *CLKSolver::tour(int Ncount, double *X, double *Y, int Norm, double Time_bound, int In_repeater){
    int k, ncount;
    double val, best;
    double startzeit;
    int tempcount, *templist;
    int *incycle = (int *) NULL, *outcycle = (int *) NULL;
    CCdatagroup dat;
    int rval = 0;
    CCrandstate rstate;
    int allow_dups;
    int use_gridsize;

    //CCutil_printlabel ();
    CCutil_init_datagroup (&dat);

    //rval = print_command (ac, av);
    //CCcheck_rval (rval, "print_command failed");

    seed = (int) CCutil_real_zeit ();

    CCutil_sprand (seed, &rstate);

    startzeit = CCutil_zeit ();
    CCutil_init_datagroup (&dat);
    norm  = Norm;
    ncount = Ncount;
    time_bound = Time_bound;
    dat.x = X;
    dat.y = Y;
    if (CCutil_dat_setnorm (&dat, norm)) {
        cout<< "ERROR: Couldn't set norm "<<norm<<endl;
        return (int *) NULL;
    }
    in_repeater = In_repeater;

    incycle = new int [ncount];//CC_SAFE_MALLOC (ncount, int);
    if (!incycle) {
        rval = 1;
        goto CLEANUP;
    }
    
    if ((norm & CC_NORM_BITS) == CC_KD_NORM_TYPE) {
        CCkdtree localkt;
        double kzeit = CCutil_zeit ();
        {
            if (CCkdtree_build (&localkt, ncount, &dat, (double *) NULL,
                                &rstate)) {
                fprintf (stderr, "CCkdtree_build failed\n");
                rval = 1;
                goto CLEANUP;
            }
            //printf ("Time to build kdtree: %.2f\n", CCutil_zeit () - kzeit);
            //fflush (stdout);
            {
                kzeit = CCutil_zeit ();
                if (nearnum) {
                    if (CCkdtree_k_nearest (&localkt, ncount, nearnum, &dat,
                         (double *) NULL, 1, &tempcount, &templist,
                         run_silently, &rstate)) {
                        fprintf (stderr, "CCkdtree_k_nearest failed\n");
                        rval = 1;
                        goto CLEANUP;
                    }
                    if (!run_silently) {
                        printf ("Time to find %d-nearest: %.2f\n", nearnum,
                                                     CCutil_zeit () - kzeit);
                        fflush (stdout);
                    }
                } else {
                    if (CCkdtree_quadrant_k_nearest (&localkt, ncount, quadtry,
                           &dat, (double *) NULL, 1, &tempcount, &templist,
                           run_silently, &rstate)) {
                        fprintf (stderr, "CCkdtree-quad nearest code failed\n");
                        rval = 1;
                        goto CLEANUP;
                    }
                    if (!run_silently) {
                        printf ("Time to find quad %d-nearest: %.2f\n",
                                quadtry, CCutil_zeit () - kzeit);
                        fflush (stdout);
                    }
                }
            }
            {
                kzeit = CCutil_zeit ();
                if (tour_type == LK_GREEDY) {
                    if (CCkdtree_greedy_tour (&localkt, ncount,
                              &dat, incycle, &val, run_silently, &rstate)) {
                        fprintf (stderr, "CCkdtree greedy-tour failed\n");
                        rval = 1;
                        goto CLEANUP;
                    }
                } else if (tour_type == LK_QBORUVKA) {
                    if (CCkdtree_qboruvka_tour (&localkt, ncount,
                              &dat, incycle, &val, &rstate)) {
                        fprintf (stderr, "CCkdtree qboruvka-tour failed\n");
                        rval = 1;
                        goto CLEANUP;
                    }
                } else if (tour_type == LK_BORUVKA) {
                    if (CCkdtree_boruvka_tour (&localkt, ncount,
                              &dat, incycle, &val, &rstate)) {
                        fprintf (stderr, "CCkdtree boruvka-tour failed\n");
                        rval = 1;
                        goto CLEANUP;
                    }
                } else if (tour_type == LK_RANDOM) {
                    randcycle (ncount, incycle, &rstate);
                } else {
                    if (CCkdtree_nearest_neighbor_tour (&localkt, ncount,
                               CCutil_lprand (&rstate) % ncount, &dat,
                               incycle, &val, &rstate)) {
                        fprintf (stderr, "CCkdtree NN-tour failed\n");
                        rval = 1;
                        goto CLEANUP;
                    }
                }
                if (!run_silently) {
                    printf ("Time to grow tour: %.2f\n",
                            CCutil_zeit () - kzeit);
                    fflush (stdout);
                }
            }
            CCkdtree_free (&localkt);
        }
    } else if ((norm & CC_NORM_BITS) == CC_X_NORM_TYPE) {
        double xzeit = CCutil_zeit ();
        {
            if (nearnum) {
                if (CCedgegen_x_k_nearest (ncount, nearnum, &dat,
                        (double *) NULL, 1, &tempcount, &templist,
                        run_silently)) {
                    fprintf (stderr, "CCedgegen_x_k_nearest failed\n");
                    rval = 1;
                    goto CLEANUP;
                }
                if (!run_silently) {
                    printf ("Time to find %d-nearest: %.2f\n", nearnum,
                                                 CCutil_zeit () - xzeit);
                    fflush (stdout);
                }
            } else {
                if (CCedgegen_x_quadrant_k_nearest (ncount, quadtry, &dat,
                                 (double *) NULL, 1, &tempcount, &templist,
                                 run_silently)) {
                    fprintf (stderr, "x-quad nearest code failed\n");
                    rval = 1;
                    goto CLEANUP;
                }
                if (!run_silently) {
                    printf ("Time to find quad %d-nearest: %.2f\n", quadtry,
                                                 CCutil_zeit () - xzeit);
                    fflush (stdout);
                }
            }
        }
        {
            xzeit = CCutil_zeit ();
            if (tour_type == LK_GREEDY) {
                if (CCedgegen_x_greedy_tour (ncount, &dat, incycle, &val,
                        tempcount, templist, run_silently)) {
                    fprintf (stderr, "CCedgegen_x_greedy_tour failed\n");
                    rval = 1; goto CLEANUP;
                }
            } else if (tour_type == LK_QBORUVKA) {
                if (CCedgegen_x_qboruvka_tour (ncount, &dat, incycle, &val,
                        tempcount, templist, run_silently)) {
                    fprintf (stderr, "CCedgegen_x_qboruvka_tour failed\n");
                    rval = 1; goto CLEANUP;
                }
            } else if (tour_type == LK_RANDOM) {
                randcycle (ncount, incycle, &rstate);
            } else {
                if (CCedgegen_x_nearest_neighbor_tour (ncount,
                      CCutil_lprand (&rstate) % ncount, &dat, incycle, &val)) {
                    fprintf (stderr, "CCedgegen_x_nearest_neighbor_tour failed\n");
                    rval = 1;
                    goto CLEANUP;
                }
            }
            if (!run_silently) {
                printf ("Time to grow tour: %.2f\n", CCutil_zeit () - xzeit);
                fflush (stdout);
            }
        }
    } else {
        double jzeit = CCutil_zeit ();
        {
            if (!nearnum)
                nearnum = 4 * quadtry;
            if (CCedgegen_junk_k_nearest (ncount, nearnum, &dat,
                    (double *) NULL, 1, &tempcount, &templist, run_silently)) {
                fprintf (stderr, "CCedgegen_junk_k_nearest failed\n");
                rval = 1;
                goto CLEANUP;
            }
            if (!run_silently) {
                printf ("Time to find %d nearest: %.2f\n",
                         nearnum, CCutil_zeit () - jzeit);
                fflush (stdout);
            }
        }
        {
            jzeit = CCutil_zeit();
            if (tour_type == LK_GREEDY) {
                if (CCedgegen_junk_greedy_tour (ncount, &dat, incycle, &val,
                        tempcount, templist, run_silently)) {
                    fprintf (stderr, "CCedgegen_junk_greedy_tour failed\n");
                    rval = 1; goto CLEANUP;
                }
            } else if (tour_type == LK_QBORUVKA) {
                if (CCedgegen_junk_qboruvka_tour (ncount, &dat, incycle, &val,
                        tempcount, templist, run_silently)) {
                    fprintf (stderr, "CCedgegen_junk_qboruvka_tour failed\n");
                    rval = 1; goto CLEANUP;
                }
            } else if (tour_type == LK_RANDOM) {
                randcycle (ncount, incycle, &rstate);
            } else {
                if (CCedgegen_junk_nearest_neighbor_tour (ncount,
                       CCutil_lprand (&rstate) % ncount, &dat, incycle,
                       &val, run_silently)) {
                    fprintf (stderr, "CCedgegen_junk_nearest_neighbor_tour failed\n");
                    rval = 1;
                    goto CLEANUP;
                }
            }
            if (!run_silently) {
                printf ("Time to grow tour: %.2f\n", CCutil_zeit () - jzeit);
                fflush (stdout);
            }
        }
    }

    outcycle = new int [ncount];//CC_SAFE_MALLOC (ncount, int);
    if (!outcycle) {
        rval = 1;
        goto CLEANUP;
    }

    if (number_runs) {
        k = 0;
        best = BIGDOUBLE;
        do {
            printf ("\nStarting Run %d\n", k);
            if (CClinkern_tour (ncount, &dat, tempcount, templist, 100000000,
                   in_repeater, incycle, outcycle, &val, run_silently,
                   time_bound, length_bound, (char *) NULL, kick_type,
                   &rstate)) {
                fprintf (stderr, "CClinkern_tour failed\n");
                rval = 1;
                goto CLEANUP;
            }
            if (val < best) {
                best = val;
            }
        } while (++k < number_runs);
        printf ("Overall Best Cycle: %.0f\n", val);
        fflush (stdout);
    } else {
        //cout<<"Linkern started."<<endl;
        double lkzeit = CCutil_zeit ();
        int attempt = 1;
        do {
            if (CClinkern_tour (ncount, &dat, tempcount, templist, 10000000,
                   in_repeater, incycle, outcycle, &val, run_silently,
                   time_bound, length_bound, (char *) NULL, kick_type,
                   &rstate)) {
                fprintf (stderr, "CClinkern_tour failed\n");
                rval = 1;
                goto CLEANUP;
            }
            if (length_bound != -1 && val > length_bound) {
                printf ("Cycle of value %.0f  -  did not reach %.0f\n",
                    val, length_bound);
                printf ("Try again. Number of attempts: %d\n", ++attempt);
            }
        } while (length_bound != -1 && val > length_bound);
        
        //if (run_silently)
            //printf ("Lin-Kernighan Running Time: %.2f\n",
            //        CCutil_zeit () - lkzeit);
        //printf ("Final Cycle: %.0f\n", val);
        //fflush (stdout);
    }
    //printf ("Total Running Time: %.2f\n", CCutil_zeit () - startzeit);
    //fflush (stdout);

CLEANUP:

#ifndef BIG_PROBLEM
    CC_IFFREE (templist, int);
#endif
    delete [] incycle;
    dat.x = (double *)nullptr;
    dat.y = (double *)nullptr;
    CCutil_freedatagroup (&dat);

    return outcycle;
}

