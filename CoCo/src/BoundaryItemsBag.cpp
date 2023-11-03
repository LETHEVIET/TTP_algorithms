/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   BoundaryItemsBag.cpp
 * Author: Majid Namazi
 * 
 * Created on 27 December 2019
 */

#include "BoundaryItemsBag.h"

void BoundaryItemsBag::fillBagBasedOnMinPRatioes(void) {
    
    int          numCities    = ttp.getNumCities();
    int         *tour         = sol.getTour();
    SolCity     *solCities    = sol.getCities();
    SolPosition *solPositions = sol.getPositions();
    
    double minPRatio = ttp.getMaxRatio() + 1;
    for (int  posNum = 1; posNum < numCities; posNum++) {
        int cityId = tour[posNum];
        if (solPositions[posNum].minPRatio < minPRatio) {
            minPRatio = solPositions[posNum].minPRatio;
            lowPicked = solCities[cityId].lowPicked;
            int     numAvailItems = ttp.getNumAvailItems(cityId);
            const int *availItems = ttp.getAvailItems(cityId);
            if (lowPicked < numAvailItems) {
                int    lowPickedItem = availItems[lowPicked] ;
                insert(lowPickedItem);
            }    
        }
    }
}

void BoundaryItemsBag::fillBagBasedOnMaxURatioes(void) {
    
    int          numCities    = ttp.getNumCities();
    int         *tour         = sol.getTour();
    SolCity     *solCities    = sol.getCities();
    SolPosition *solPositions = sol.getPositions();
    
    double maxURatio = 0;
    for (int  posNum = numCities-1; posNum > 0; posNum--) {
        int cityId = tour[posNum];
        if (solPositions[posNum].maxURatio > maxURatio) {
            maxURatio    = solPositions[posNum].maxURatio;
            highUnPicked = solCities[cityId].highUnPicked;
            const int *availItems = ttp.getAvailItems(cityId);
            if (highUnPicked >= 0) {
                int    highUnPickedItem = availItems[highUnPicked];
                insert(highUnPickedItem);
            }
        }
    }
}

void BoundaryItemsBag::initBag(void) {
    
    curNumItems = 0;
    for (int index  = 0, itemId = 0; index < maxNumItems; index++, itemId++) {
        bag[index]  = -1;
        loc[itemId] = -1;
    }
    fillBagBasedOnMinPRatioes();
    fillBagBasedOnMaxURatioes();
}

void BoundaryItemsBag::insert(int ItemId) {    
    
    if (loc[ItemId]>=0 && loc[ItemId]<curNumItems)
        return;
    bag[curNumItems] = ItemId;
    loc[ItemId] = curNumItems++;
}


void BoundaryItemsBag::remove(int ItemId) {    
    
    if (loc[ItemId]<0 || loc[ItemId]>=curNumItems)
        return;
    if (loc[ItemId] < numChecked) {
        numChecked--;
        bag[loc[ItemId]] = bag[numChecked];
        loc[bag[numChecked]] = loc[ItemId];
        curNumItems--;
        if (curNumItems > numChecked) {
            bag[numChecked] = bag[curNumItems];
            loc[bag[curNumItems]] = numChecked;
        }    
    } 
    else {
        curNumItems--;
        bag[loc[ItemId]] = bag[curNumItems];
        loc[bag[curNumItems]] = loc[ItemId];
    }
    bag[curNumItems] = -1;
    loc[ItemId] = -1;    
}

void    BoundaryItemsBag::updateLocalLowPickedHighUnPicked(int ItemId) {
    const Item  *items         = ttp.getItems();
    int         *plan          = sol.getPlan();
    SolCity     *solCities     = sol.getCities();
    int          cityId        = items[ItemId].cityId;
    int          location      = items[ItemId].location;
    int          numAvailItems = ttp.getNumAvailItems(cityId);
    const int   *availItems    = ttp.getAvailItems(cityId);
    
    remove(ItemId);
    lowPicked    = solCities[cityId].lowPicked;
    highUnPicked = solCities[cityId].highUnPicked;
    if (location == lowPicked) {   // flipping from 1 to 0
        if (highUnPicked < lowPicked) {
            if (highUnPicked >= 0) {
                int highUnPickedItem = availItems[highUnPicked];
                remove(highUnPickedItem);
            }
            highUnPicked = lowPicked;
        }    
        for (location++; location < numAvailItems; location++) {
            int itemId = availItems[location];
            if (plan[itemId])
                break;
        }    
        lowPicked = location;
    }
    else {//(location == highUnPicked) flipping from 0 to 1
        if (highUnPicked < lowPicked) {
            if (lowPicked < numAvailItems) {
                int lowPickedItem = availItems[lowPicked];
                remove(lowPickedItem);
            }
            lowPicked = highUnPicked;
        }    
        for (location--; location >= 0; location--) {
            int itemId = availItems[location];
            if (!plan[itemId])
                break;
        }    
        highUnPicked = location;
    }
}

void    BoundaryItemsBag::updateBagBasedOnNewMinPRatioes(int ItemId) {
    
    int          numCities     = ttp.getNumCities();
    const Item  *items         = ttp.getItems();
    int         *tour          = sol.getTour();
    SolCity     *solCities     = sol.getCities();
    SolPosition *solPositions  = sol.getPositions();
    int          cityId        = items[ItemId].cityId;
    int          posNum        = solCities[cityId].posNum;
    int          numAvailItems = ttp.getNumAvailItems(cityId);
    const int   *availItems    = ttp.getAvailItems(cityId);
    
    double       minPRatio     = solPositions[posNum-1].minPRatio;
    double       prevMinPRatio = solPositions[posNum].minPRatio;
    int          lowPickedItem;
    double       lowPickedRatio;
    
    if (lowPicked < numAvailItems) {     
        lowPickedItem  = availItems[lowPicked];
        lowPickedRatio = items[lowPickedItem].ratio;
        if (lowPickedRatio < minPRatio) {
            insert(lowPickedItem);
            minPRatio = lowPickedRatio;
        }    
    }
    
    if (minPRatio != prevMinPRatio) {
        for (int p = posNum+1; p < numCities; p++) {
            int cityId    = tour[p];
            lowPicked     = solCities[cityId].lowPicked;
            numAvailItems = ttp.getNumAvailItems(cityId);
            availItems    = ttp.getAvailItems(cityId);
            if (lowPicked < numAvailItems) {
                lowPickedItem  = availItems[lowPicked];
                lowPickedRatio = items[lowPickedItem].ratio;
                if (lowPickedRatio < minPRatio) {    
                    minPRatio  = lowPickedRatio;
                    if (minPRatio < prevMinPRatio)
                        break;
                    insert(lowPickedItem);
                }
                else
                    remove(lowPickedItem);
            }
        }
    }        
}

void    BoundaryItemsBag::updateBagBasedOnNewMaxURatioes(int ItemId) {
    
    int          numCities     = ttp.getNumCities();
    const Item  *items         = ttp.getItems();
    int         *tour          = sol.getTour();
    SolCity     *solCities     = sol.getCities();
    SolPosition *solPositions  = sol.getPositions();
    int          cityId        = items[ItemId].cityId;
    int          posNum        = solCities[cityId].posNum;
    const int   *availItems    = ttp.getAvailItems(cityId);
    
    double       maxURatio     = posNum==numCities-1 ? 0 : solPositions[posNum+1].maxURatio;
    double       prevMaxURatio = solPositions[posNum].maxURatio;
    int          highUnPickedItem; 
    double       highUnPickedRatio;
    
    if (highUnPicked >= 0) {
        highUnPickedItem  = availItems[highUnPicked];
        highUnPickedRatio = items[highUnPickedItem].ratio;
        if (highUnPickedRatio > maxURatio) {
            insert(highUnPickedItem);
            maxURatio = highUnPickedRatio;
        } 
    } 
        
    if (maxURatio != prevMaxURatio) {
        for (int p = posNum-1; p > 0; p--) {
            int cityId   = tour[p];
            highUnPicked = solCities[cityId].highUnPicked;
            const int *availItems = ttp.getAvailItems(cityId);
            if (highUnPicked >= 0) {
                highUnPickedItem  = availItems[highUnPicked];
                highUnPickedRatio = items[highUnPickedItem].ratio;
                if (highUnPickedRatio > maxURatio) {    
                    maxURatio = highUnPickedRatio;
                    if (maxURatio > prevMaxURatio)
                        break;
                    insert(highUnPickedItem);
                }
                else
                    remove(highUnPickedItem);
            }  
        }
    }
}

