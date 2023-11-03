/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   ProBitFlip.cpp
 * Author: Majid Namazi
 * 
 * Created on 27 December 2019
 */

#include "ProBitFlip.h"

void    ProBitFlip::updateLowPickedHighUnPicked(int ItemId)
{
    const Item  *items         = ttp.getItems();
    int         *plan          = sol.getPlan();
    SolCity     *solCities     = sol.getCities();
    int          cityId        = items[ItemId].cityId;   // City in which item is located
    int          location      = items[ItemId].location; // Location in the availItems list of the city
    int          numAvailItems = ttp.getNumAvailItems(cityId);
    const int   *availItems    = ttp.getAvailItems(cityId);
    
    if (plan[ItemId] == 0) {    // 1 has been flipped to 0   
        if (location > solCities[cityId].highUnPicked)
            solCities[cityId].highUnPicked = location;
        if (location == solCities[cityId].lowPicked) {
            int  loc;
            for (loc = location+1; loc < numAvailItems; loc++) {
                int ItemId = availItems[loc];
                if (plan[ItemId])
                    break;
            }
            solCities[cityId].lowPicked = loc;   
        }    
    }
    else {    // 0 has been flipped to 1
        if (location < solCities[cityId].lowPicked)
            solCities[cityId].lowPicked = location;
        if (location == solCities[cityId].highUnPicked) {
            int  loc;
            for (loc = location-1; loc >= 0; loc--) {
                int ItemId = availItems[loc];
                if (!plan[ItemId])
                    break;
            }
            solCities[cityId].highUnPicked = loc;
        }                
    }
}

void ProBitFlip::updateMinPRatioes(int ItemId) {
    int          numCities     = ttp.getNumCities();
    double       maxRatio      = ttp.getMaxRatio();
    const Item  *items         = ttp.getItems();
    int         *tour          = sol.getTour();
    SolPosition *solPositions  = sol.getPositions();
    SolCity     *solCities     = sol.getCities();
    int          cityId        = items[ItemId].cityId;
    int          lowPicked     = solCities[cityId].lowPicked;
    int          posNum        = solCities[cityId].posNum;     // Position number in the tour
    double       minPRatio     = solPositions[posNum-1].minPRatio;
    double       prevMinPRatio = solPositions[posNum].minPRatio;
    int          numAvailItems = ttp.getNumAvailItems(cityId);
    const int   *availItems    = ttp.getAvailItems(cityId);
    double       lowPickedRatio;
    
    if (lowPicked < numAvailItems)
    {
        int lowPickedItemId = availItems[lowPicked];
        lowPickedRatio = items[lowPickedItemId].ratio;
    }
    else
        lowPickedRatio = maxRatio + 1;
    if (lowPickedRatio < minPRatio)
        minPRatio = lowPickedRatio;
    solPositions[posNum].minPRatio = minPRatio;
    if (minPRatio == prevMinPRatio)
        return;
    for (int p = posNum+1; p < numCities; p++) {
        cityId        = tour[p];
        lowPicked     = solCities[cityId].lowPicked;
        numAvailItems = ttp.getNumAvailItems(cityId);
        availItems    = ttp.getAvailItems(cityId);
        if (lowPicked < numAvailItems)
        {
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

void ProBitFlip::updateMaxURatioes(int ItemId)
{
    int          numCities     = ttp.getNumCities();
    const Item  *items         = ttp.getItems();
    int         *tour          = sol.getTour();
    SolPosition *solPositions  = sol.getPositions();
    SolCity     *solCities     = sol.getCities();
    int          cityId        = items[ItemId].cityId;
    int          highUnPicked  = solCities[cityId].highUnPicked;
    int          posNum        = solCities[cityId].posNum;         // Position number in the tour
    double       maxURatio     = posNum == numCities-1 ? 0: solPositions[posNum+1].maxURatio; 
    double       prevMaxURatio = solPositions[posNum].maxURatio;
    const int   *availItems    = ttp.getAvailItems(cityId);
    double       highUnPickedRatio;
    
    if (highUnPicked >= 0) {
        int   highUnPickedItemId = availItems[highUnPicked];
        highUnPickedRatio = items[highUnPickedItemId].ratio;
    }
    else
        highUnPickedRatio = 0;
    if (highUnPickedRatio > maxURatio)
        maxURatio = highUnPickedRatio;
    solPositions[posNum].maxURatio = maxURatio;
    if (maxURatio == prevMaxURatio)
        return;
    for (int p = posNum-1; p > 0; p--) {
        cityId       = tour[p];
        highUnPicked = solCities[cityId].highUnPicked;
        availItems   = ttp.getAvailItems(cityId);
        if (highUnPicked >= 0) {
            int   highUnPickedItemId = availItems[highUnPicked];
            highUnPickedRatio = items[highUnPickedItemId].ratio;
        } else    
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
