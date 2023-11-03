/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   TwoOPT.cpp
 * Author: Majid Namazi
 * 
 * Created on 27 December 2019
 */

#include "TwoOPT.h"

TwoOPT::TwoOPT(TTPInstance &TTP,TTPSolution &Sol): Move(TTP,Sol){

    int  numCities = ttp.getNumCities();
    tour2          = new int  [numCities];
    addWeight2     = new long [numCities];
}

void   TwoOPT::virtuallyReverseSegment(void) {

    int          numCities    = ttp.getNumCities();
    int         *tour         = sol.getTour();
    SolPosition *solPositions = sol.getPositions();
    
    if (0<posNumA && posNumA<posNumB) {    
        tour2[posNumA-1]      = tour[posNumA-1];
        addWeight2[posNumA-1] = solPositions[posNumA-1].addWeight;
        for (int p = posNumA; p <= posNumB; p++) {
            int  q = posNumB-p+posNumA;
            tour2[p]      = tour[q];
            addWeight2[p] = solPositions[q].addWeight;;
        }
        tour2[(posNumB+1)%numCities] = tour[(posNumB+1)%numCities];
    }
    else {
        int p = 1, last  ;
        tour2[0]      = 0;
        addWeight2[0] = 0;
        if (posNumA == 0)
            last = numCities-1;
        else {  // posNumB < posNumA    
            for (int q = numCities-1; q >= posNumA; q--, p++) {
                tour2[p]      = tour[q];
                addWeight2[p] = solPositions[q].addWeight;
            }
            last = posNumA-1; 
        }
        for (int q = posNumB+1; q <= last; q++, p++) {
            tour2[p]      = tour[q];
            addWeight2[p] = solPositions[q].addWeight;
        }
        for (int q = posNumB; q > 0; q--, p++) {
            tour2[p]      = tour[q];
            addWeight2[p] = solPositions[q].addWeight;
        }
    }    
}

double     TwoOPT::evalVirChanges(void) {
    SolPosition     *solPositions = sol.getPositions();
    int              numCities    = ttp.getNumCities();   
    double           maxSpeed     = ttp.getMaxSpeed();
    double           rentRatio    = ttp.getRentRatio();
    double           velConst     = ttp.getVelConst() ;
    long             accWeight;
    double           accTime;
    
    if (0<posNumA && posNumA<posNumB) {    
        accTime    = solPositions[posNumA-1].accTime - solPositions[posNumA-1].addTime;
        accWeight  = solPositions[posNumA-1].accWeight - solPositions[posNumA-1].addWeight;
        for (int p = posNumA-1; p <= posNumB; p++) {
            int cityId1 = tour2[p];
            int cityId2 = tour2[(p + 1) % numCities];
            accWeight  += addWeight2[p];
            accTime    += ttp.distLong(cityId1,cityId2) / 
                                       (maxSpeed - accWeight * velConst);
        }
        accTime += sol.getTotalTime() - solPositions[posNumB].accTime;    
    }
    else {    
        accTime    = 0; 
        accWeight  = 0;
        for (int p = 0; p<numCities; p++) {
            int cityId1 = tour2[p];
            int cityId2 = tour2[(p + 1) % numCities];
            accWeight  += addWeight2[p];
            accTime    += ttp.distLong(cityId1,cityId2) / 
                                       (maxSpeed - accWeight * velConst);
        }
    }    
    return sol.getTotalProfit() - rentRatio*accTime;
}

void    TwoOPT::realReverseSegment(int PosNumA, int PosNumB) {
    int              numCities     = ttp.getNumCities();
    int              *tour         = sol.getTour();
    SolPosition      *solPositions = sol.getPositions();
    SolCity          *solCities    = sol.getCities();
    
    if (0<PosNumA && PosNumA<PosNumB) {
        for (int p = 0; p < (PosNumB+1-PosNumA)/2; p++) {
            swap(tour[PosNumA+p],tour[PosNumB-p]);
            swap(solPositions[PosNumA+p].addWeight,solPositions[PosNumB-p].addWeight);
            swap(solCities[tour[PosNumA+p]].posNum,solCities[tour[PosNumB-p]].posNum);
        }
    }
    else {
        int p = 1, last  ;
        tour2[0]      = 0;
        addWeight2[0] = 0;
        
        if (PosNumA == 0)
            last = numCities-1;
        else {  // PosNumB<PosNumA    
            for (int q = numCities-1; q >= PosNumA; q--, p++) {
                tour2[p]      = tour[q];
                addWeight2[p] = solPositions[q].addWeight;
            }
            last = PosNumA-1; 
        }
        for (int q = PosNumB+1; q <= last; q++, p++) {
            tour2[p]      = tour[q];
            addWeight2[p] = solPositions[q].addWeight;
        }
        for (int q = PosNumB; q > 0; q--, p++) {
            tour2[p]      = tour[q];
            addWeight2[p] = solPositions[q].addWeight;
        }
        
        for (int p = 0; p < numCities; p++) {
            tour[p]                   = tour2[p];
            solPositions[p].addWeight = addWeight2[p];
            solCities[tour[p]].posNum = p;
        }
    }
}

void  TwoOPT::updateTotalGain(int PosNumA, int PosNumB) {
    
    int         *tour         = sol.getTour();
    SolPosition *solPositions = sol.getPositions();
    int          numCities    = ttp.getNumCities();   
    double       maxSpeed     = ttp.getMaxSpeed() ;
    double       rentRatio    = ttp.getRentRatio();
    double       velConst     = ttp.getVelConst() ;
    long         accWeight;
    double       accTime;
    double       addTime;
    
    if (0<PosNumA && PosNumA<PosNumB) {    
        accTime    = solPositions[PosNumA-1].accTime   - solPositions[PosNumA-1].addTime;
        accWeight  = solPositions[PosNumA-1].accWeight - solPositions[PosNumA-1].addWeight;
        for (int p = PosNumA-1; p <= PosNumB; p++) {
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
        for (int p = PosNumB+1; p < numCities; p++) {
            accTime += solPositions[p].addTime;
            solPositions[p].accTime = accTime;
        }           
    }
    else {
        accTime    = 0; 
        accWeight  = 0;
        for (int p = 0; p < numCities; p++) {
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
    }   
    sol.totalTime = accTime;
    sol.totalGain = sol.totalProfit - rentRatio*accTime;
}

TwoOPT::~TwoOPT() {
    delete [] tour2;
    delete [] addWeight2;    
}
