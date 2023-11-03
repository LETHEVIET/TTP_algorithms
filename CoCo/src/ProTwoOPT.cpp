/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   ProTwoOPT.cpp
 * Author: Majid Namazi
 * 
 * Created on 27 December 2019
 */

#include "ProTwoOPT.h"


void ProTwoOPT::updateMinPRatioes(int PosNumA, int PosNumB) {
    int          numCities    = ttp.getNumCities();
    const Item  *items        = ttp.getItems();
    double       maxRatio     = ttp.getMaxRatio();
    int         *tour         = sol.getTour();
    SolPosition *solPositions = sol.getPositions();
    SolCity     *solCities    = sol.getCities();
    
    if (0<PosNumA && PosNumA<PosNumB) {
        double  minPRatio     = solPositions[PosNumA-1].minPRatio; 
        double  prevMinPRatio = solPositions[PosNumB].minPRatio;
        for (int p = PosNumA; p <= PosNumB; p++) {
            double  lowPickedRatio;
            int     cityId        = tour[p];
            int     lowPicked     = solCities[cityId].lowPicked;
            int     numAvailItems = ttp.getNumAvailItems(cityId);
            const int *availItems = ttp.getAvailItems(cityId);
            if (lowPicked < numAvailItems) {
                int   lowPickedItemId = availItems[lowPicked];
                lowPickedRatio = items[lowPickedItemId].ratio;
            }
            else
                lowPickedRatio = maxRatio + 1;
            if (lowPickedRatio < minPRatio)
                minPRatio = lowPickedRatio;
            solPositions[p].minPRatio = minPRatio;
        }
        if (minPRatio == prevMinPRatio)
            return;
        for (int p = PosNumB+1; p < numCities; p++) {
            double  lowPickedRatio;
            int     cityId        = tour[p];
            int     lowPicked     = solCities[cityId].lowPicked;
            int     numAvailItems = ttp.getNumAvailItems(cityId);
            const int *availItems = ttp.getAvailItems(cityId);
            if (lowPicked < numAvailItems) {
                int   lowPickedItemId = availItems[lowPicked];
                lowPickedRatio = items[lowPickedItemId].ratio;
            }
            else
                lowPickedRatio = maxRatio + 1;
            if (lowPickedRatio < minPRatio)
            {
                minPRatio = lowPickedRatio;
                if (minPRatio < prevMinPRatio)
                    break;
            }
            solPositions[p].minPRatio = minPRatio;
        }
    }
    else {
        double  minPRatio = maxRatio+1;
        solPositions[0].minPRatio = minPRatio;
        for (int p = 1; p < numCities; p++) {
            double  lowPickedRatio;
            int     cityId        = tour[p];
            int     lowPicked     = solCities[cityId].lowPicked;
            int     numAvailItems = ttp.getNumAvailItems(cityId);
            const int *availItems = ttp.getAvailItems(cityId);
            if (lowPicked < numAvailItems) {
                int   lowPickedItemId = availItems[lowPicked];
                lowPickedRatio = items[lowPickedItemId].ratio;
            }
            else
                lowPickedRatio = maxRatio + 1;
            if (lowPickedRatio < minPRatio)
                minPRatio = lowPickedRatio;
            solPositions[p].minPRatio = minPRatio;
        }
    }    
}

void ProTwoOPT::updateMaxURatioes(int PosNumA, int PosNumB) {
    int          numCities    = ttp.getNumCities();
    const Item  *items        = ttp.getItems();
    int         *tour         = sol.getTour();
    SolPosition *solPositions = sol.getPositions();
    SolCity     *solCities    = sol.getCities();
    
    if (0<PosNumA && PosNumA<PosNumB) {
        double  maxURatio     = PosNumB==numCities-1 ? 0 : solPositions[PosNumB+1].maxURatio;
        double  prevMaxURatio = solPositions[PosNumA].maxURatio;
        for (int p = PosNumB; p >= PosNumA; p--) {
            double   highUnPickedRatio;
            int      cityId       = tour[p];
            int      highUnPicked = solCities[cityId].highUnPicked;
            const int *availItems = ttp.getAvailItems(cityId);
            if (highUnPicked >= 0) {
                int   highUnPickedItemId = availItems[highUnPicked];
                highUnPickedRatio = items[highUnPickedItemId].ratio;
            }
            else
                highUnPickedRatio = 0;
            if (highUnPickedRatio > maxURatio)
                maxURatio = highUnPickedRatio;
            solPositions[p].maxURatio = maxURatio;
        }
        if (maxURatio == prevMaxURatio)
            return;
        for (int p = PosNumA-1; p > 0; p--) {
            double   highUnPickedRatio;
            int      cityId       = tour[p];
            int      highUnPicked = solCities[cityId].highUnPicked;
            const int *availItems = ttp.getAvailItems(cityId);
            if (highUnPicked >= 0) {
                int   highUnPickedItemId = availItems[highUnPicked];
                highUnPickedRatio = items[highUnPickedItemId].ratio;
            }
            else
                highUnPickedRatio = 0;
            if (highUnPickedRatio > maxURatio)
            {
                maxURatio = highUnPickedRatio;
                if (maxURatio > prevMaxURatio)
                    break;
            }    
            solPositions[p].maxURatio = maxURatio;
        }
    }
    else {
        double  maxURatio = 0;
        for (int p = numCities-1; p > 0; p--) {
            double   highUnPickedRatio;
            int      cityId       = tour[p];
            int      highUnPicked = solCities[cityId].highUnPicked;
            const int *availItems = ttp.getAvailItems(cityId);
            if (highUnPicked >= 0) {
                int   highUnPickedItemId = availItems[highUnPicked];
                highUnPickedRatio = items[highUnPickedItemId].ratio;
            }
            else
                highUnPickedRatio = 0;
            if (highUnPickedRatio > maxURatio)
                maxURatio = highUnPickedRatio;
            solPositions[p].maxURatio = maxURatio;
        }
    }    
}
