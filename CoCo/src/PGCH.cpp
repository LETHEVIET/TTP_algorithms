/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   PGCH.cpp
 * Author: Majid Namazi
 * 
 * Created on 27 December 2019
 */

#include "PGCH.h"

PGCH::PGCH(TTPInstance &TTP,TTPSolution &Sol): ProTwoOPT(TTP,Sol) {
    
    int  numCities = ttp.getNumCities();
    highUnPicked2  = new int  [numCities];      /// City based    
}

void   PGCH::virUnPickLowProfitableItems(void) {
    int          numCities    = ttp.getNumCities();
    const Item  *items        = ttp.getItems();
    int         *plan         = sol.getPlan();
    SolCity     *solCities    = sol.getCities();
    SolPosition *solPositions = sol.getPositions();
    int          startPos;
    int          endPos;
    
    sumUnPickedWeight = 0; 
    sumUnPickedProfit = 0;
    if (0<posNumA && posNumA<posNumB) {
        startPos = posNumA;
        endPos   = posNumB;
    }
    else {
        startPos = 0;
        endPos   = numCities-1;
    }
         
    for (int p = endPos; p >= startPos; p--) {
        int loc;
        int cityId = tour2[p];
        highUnPicked2[cityId] = solCities[cityId].highUnPicked;
        int     numAvailItems = ttp.getNumAvailItems(cityId);
        const int *availItems = ttp.getAvailItems(cityId);
        for (loc = solCities[cityId].lowPicked; loc < numAvailItems; loc++) {
            int itemId = availItems[loc];
            if (items[itemId].ratio < solPositions[p].minPRatio) {
                if (plan[itemId] !=0 ) {
                    sumUnPickedWeight += items[itemId].weight;
                    sumUnPickedProfit += items[itemId].profit;
                    addWeight2[p]     -= items[itemId].weight;
                    flipItems.push_back(itemId);      
                }
            }
            else
                break;
        }
        if (loc-1 > highUnPicked2[cityId])
            highUnPicked2[cityId] = loc-1;
    }
}

void   PGCH::virPickHighProfitableItems(void)
{
    int          numCities    = ttp.getNumCities();
    const Item  *items        = ttp.getItems();
    int         *plan         = sol.getPlan();
    SolPosition *solPositions = sol.getPositions();    
    int          startPos;
    int          endPos;
    
    sumPickedWeight = 0;
    sumPickedProfit = 0;
    if (0<posNumA && posNumA<posNumB) {
        startPos = posNumA;
        endPos   = posNumB;
    }
    else {
        startPos = 0;
        endPos   = numCities-1;
    }
        
    for (int p = endPos; p>=startPos && sumPickedWeight<sumUnPickedWeight; p--) {
        int cityId = tour2[p];
        const int *availItems = ttp.getAvailItems(cityId);
        for (int loc = highUnPicked2[cityId]; loc>=0 && sumPickedWeight<sumUnPickedWeight; loc--) {
            int itemId = availItems[loc];
            if (items[itemId].ratio > solPositions[p].maxURatio) {
                if ((plan[itemId]==0) && 
                    (sumPickedWeight +  items[itemId].weight<=sumUnPickedWeight)) 
                {
                    sumPickedWeight += items[itemId].weight;
                    sumPickedProfit += items[itemId].profit;
                    addWeight2[p]   += items[itemId].weight;
                    flipItems.push_back(itemId);
                }       
            } 
            else
                break;
        }
    }
}

double     PGCH::evalVirChanges(void)
{
    int         *tour         = sol.getTour();
    SolPosition *solPositions = sol.getPositions();
    int          numCities    = ttp.getNumCities();   
    double       maxSpeed     = ttp.getMaxSpeed() ;
    double       rentRatio    = ttp.getRentRatio();
    double       velConst     = ttp.getVelConst() ;
    long         totalProfit;
    long         accWeight;
    double       accTime;
    
    totalProfit = sol.getTotalProfit() - sumUnPickedProfit + sumPickedProfit;
    if (0<posNumA && posNumA<posNumB) {      
        accTime   = solPositions[posNumA-1].accTime   - solPositions[posNumA-1].addTime  ;
        accWeight = solPositions[posNumA-1].accWeight - solPositions[posNumA-1].addWeight;
        for (int  p = posNumA-1; p <= posNumB; p++) {
            int cityId1 = tour2[p];
            int cityId2 = tour2[(p + 1) % numCities];
            accWeight  += addWeight2[p];
            accTime    += ttp.distLong(cityId1,cityId2) / 
                                       (maxSpeed - accWeight * velConst);
        }
        if (sumUnPickedWeight == sumPickedWeight)
            accTime += sol.getTotalTime() - solPositions[posNumB].accTime;
        else {
            for (int p = posNumB+1; p < numCities; p++) {
                int cityId1 = tour[p];
                int cityId2 = tour[(p + 1) % numCities];
                accWeight  += solPositions[p].addWeight;
                accTime    += ttp.distLong(cityId1,cityId2) / 
                                           (maxSpeed - accWeight * velConst);
            }
        }    
    }
    else {    
        accTime     = 0;
        accWeight   = 0;
        for (int  p = 0; p < numCities; p++) {
            int cityId1 = tour2[p];
            int cityId2 = tour2[(p + 1) % numCities];
            accWeight  += addWeight2[p];
            accTime    += ttp.distLong(cityId1,cityId2) / 
                                       (maxSpeed - accWeight * velConst);
        }
    }    
    return totalProfit - rentRatio*accTime;
}

void    PGCH::realAdjustPlan(vector<int> &FlipItems) {

    const Item  *items        = ttp.getItems();
    int         *plan         = sol.getPlan();
    SolCity     *solCities    = sol.getCities();
    SolPosition *solPositions = sol.getPositions();
    
    for (int flipItemId: FlipItems) {    
        int cityId = items[flipItemId].cityId;
        int posNum = solCities[cityId].posNum;
        if (plan[flipItemId] == 0) {
            plan[flipItemId] = 1;
            sol.remSpace    -= items[flipItemId].weight;
            sol.totalProfit += items[flipItemId].profit;
            solPositions[posNum].addWeight += items[flipItemId].weight;
        }
        else {
            plan[flipItemId] = 0;
            sol.remSpace    += items[flipItemId].weight;
            sol.totalProfit -= items[flipItemId].profit;
            solPositions[posNum].addWeight -= items[flipItemId].weight;
        }    
    }
}

void    PGCH::updateLowPickedHighUnPicked(vector<int> &FlipItems) {
    const Item  *items     = ttp.getItems();
    int         *plan      = sol.getPlan();
    SolCity     *solCities = sol.getCities();
    
    for (int flipItemId: FlipItems) {
        int cityId   = items[flipItemId].cityId;
        int location = items[flipItemId].location; 
        int      numAvailItems = ttp.getNumAvailItems(cityId);
        const int  *availItems = ttp.getAvailItems(cityId);
        if (plan[flipItemId] == 0) { // flipped from 1 to 0 adjusting the plan
            if (location > solCities[cityId].highUnPicked)
                solCities[cityId].highUnPicked = location;
            if (location == solCities[cityId].lowPicked) {
                int  loc;        
                for (loc = location+1; loc < numAvailItems; loc++) {
                    int itemId = availItems[loc];
                    if (plan[itemId] == 1)
                        break;
                }
                solCities[cityId].lowPicked = loc;
            }    
        }
        else {   // flipped from 0 to 1 adjusting the plan
            if (location < solCities[cityId].lowPicked)
                solCities[cityId].lowPicked = location;
            if (location == solCities[cityId].highUnPicked) {
                int  loc;
                for (loc = location-1; loc >= 0; loc--) {
                    int itemId = availItems[loc];
                    if (plan[itemId] == 0)
                        break;
                }
                solCities[cityId].highUnPicked = loc;
            }                
        }    
    }
}

void  PGCH::updateTotalGain(int PosNumA, int PosNumB) {
    
    int         *tour         = sol.getTour();
    SolPosition *solPositions = sol.getPositions();
    int          numCities    = ttp.getNumCities();   
    double       maxSpeed     = ttp.getMaxSpeed() ;
    double       rentRatio    = ttp.getRentRatio();
    double       velConst     = ttp.getVelConst() ;
    long         accWeight;
    double       accTime;
    double       addTime;
    
    if (PosNumA==0 || PosNumA>PosNumB) 
        PosNumA = 1;
    accTime   = solPositions[PosNumA-1].accTime - solPositions[PosNumA-1].addTime;
    accWeight = solPositions[PosNumA-1].accWeight - solPositions[PosNumA-1].addWeight;
    for (int p = PosNumA-1; p < numCities; p++) {
        int cityId1 = tour[p];
        int cityId2 = tour[(p + 1) % numCities];
        accWeight  += solPositions[p].addWeight;
        addTime     = ttp.distLong(cityId1,cityId2) / 
                                   (maxSpeed - accWeight * velConst);
        accTime    += addTime;
        solPositions[p].addTime   = addTime;
        solPositions[p].accTime   = accTime;
        solPositions[p].accWeight = accWeight;
    }           
       
    sol.totalTime = accTime;
    sol.totalGain = sol.totalProfit - rentRatio*accTime;
}

PGCH::~PGCH() {
    delete [] highUnPicked2;   
}
