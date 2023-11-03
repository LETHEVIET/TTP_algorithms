/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   BitFlip.cpp
 * Author: Majid Namazi
 * 
 * Created on 27 December 2019
 */

#include "BitFlip.h"

void    BitFlip::virtuallyFlipState(void) {                         // Called by the checkMove method,
                                                                    // virtually flips state of the
                                                                    // given item.  
    int        *plan  = sol.getPlan();
    const Item *items = ttp.getItems();
    
    if (plan[flipItemId] == 0) {
        deltaProfit = items[flipItemId].profit;
        deltaWeight = items[flipItemId].weight;
    } 
    else {
        deltaProfit = -items[flipItemId].profit;
        deltaWeight = -items[flipItemId].weight;
    }    
}        

double  BitFlip::evalVirChanges() {                                 // Called by the checkMove method,
                                                                    // evaluates impacts of the virtual
                                                                    // changes.
    int             *tour          = sol.getTour();
    SolPosition     *solPositions  = sol.getPositions();
    SolCity         *solCities     = sol.getCities();
    int              numCities     = ttp.getNumCities();   
    double           maxSpeed      = ttp.getMaxSpeed() ;
    double           rentRatio     = ttp.getRentRatio();
    double           velConst      = ttp.getVelConst() ;
    const Item      *items         = ttp.getItems();
    long             totalProfit;
    long             accWeight;    // Accumulated weight
    double           accTime;      // Accumulated time
    
    totalProfit = sol.getTotalProfit() + deltaProfit;

    int  cityId = items[flipItemId].cityId;
    int  posNum = solCities[cityId].posNum;
    
    accTime     = solPositions[posNum-1].accTime;
    for (int p  = posNum; p < numCities; p++) {
        int cityId1 = tour[p];
        int cityId2 = tour[(p + 1) % numCities];
        accWeight   = solPositions[p].accWeight + deltaWeight;
        accTime    += ttp.distLong(cityId1,cityId2) / 
                                 (maxSpeed - accWeight * velConst);
    }
    return totalProfit - accTime * rentRatio;
}

void    BitFlip::realFlipState(int ItemId) {                        // Called by the makeMove method,
                                                                    // flips state of the given item.
    
    int         *plan         = sol.getPlan();
    SolPosition *solPositions = sol.getPositions();
    SolCity     *solCities    = sol.getCities();
    const Item  *items        = ttp.getItems();

    int   cityId = items[ItemId].cityId;
    int   posNum = solCities[cityId].posNum;

    plan[ItemId] = plan[ItemId] == 0  ?  1 : 0;
    
    if (plan[ItemId]) {
        sol.remSpace    -= items[ItemId].weight;
        sol.totalProfit += items[ItemId].profit;
        solPositions[posNum].addWeight += items[ItemId].weight;
    } 
    else {
        sol.remSpace    += items[ItemId].weight;
        sol.totalProfit -= items[ItemId].profit;
        solPositions[posNum].addWeight -= items[ItemId].weight;
    }
}        

void    BitFlip::updateTotalGain(int ItemId, int NotReq) {          // Called by the makeMove method,
                                                                    // updates solution's parameters
                                                                    // according to the real changes.
    int         *tour         = sol.getTour();
    SolPosition *solPositions = sol.getPositions();
    SolCity     *solCities    = sol.getCities();
    int          numCities    = ttp.getNumCities();   
    double       maxSpeed     = ttp.getMaxSpeed() ;
    double       rentRatio    = ttp.getRentRatio();
    double       velConst     = ttp.getVelConst() ;
    const Item  *items        = ttp.getItems();
    long         accWeight;
    double       addTime;
    double       accTime;

    int cityId = items[ItemId].cityId;
    int posNum = solCities[cityId].posNum;
    accTime    = solPositions[posNum-1].accTime;
    accWeight  = solPositions[posNum-1].accWeight;
    for (int p = posNum; p < numCities; p++) {
        int cityId1 = tour[p];
        int cityId2 = tour[(p + 1) % numCities];
        accWeight  += solPositions[p].addWeight;
        addTime     = ttp.distLong(cityId1, cityId2) / 
                                  (maxSpeed - accWeight * velConst);
        accTime    += addTime;
        solPositions[p].addTime   = addTime;
        solPositions[p].accTime   = accTime;
        solPositions[p].accWeight = accWeight;
    }
    
    sol.totalTime = accTime;
    sol.totalGain = sol.totalProfit - accTime * rentRatio;
}        
