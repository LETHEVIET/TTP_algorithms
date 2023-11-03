/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   TTPSolution.cpp
 * Author: majid
 * 
 * Created on 27 December 2019
 */

#include "TTPSolution.h"

void TTPSolution::setPlan(int *Plan) {
    if (plan)
        delete [] plan;
    plan = Plan;
}

void TTPSolution::setTour(int *Tour) {
    if (tour)
        delete [] tour;
    tour = Tour;
}
void TTPSolution::init()
{
    tour         = nullptr;
    plan         = nullptr;
    positions    = nullptr;
    cities       = nullptr;
}
TTPSolution::TTPSolution()
{
    init();
}
TTPSolution::TTPSolution(int NumCities, int NumItems, int *Tour, int *Plan) {
    tour      = Tour;
    plan      = Plan;
    numCities = NumCities;
    numItems  = NumItems;
    positions = new SolPosition [numCities];
    if (!positions)
    {
        cout<<"Error in allocating memory for positions"<<endl;
        return;
    }    
    cities = new SolCity [numCities];
    if (!cities)
    {
        cout<<"Error in allocating memory for cities"<<endl;
        return;
    } 
}
void TTPSolution::copy(const TTPSolution &Sol){
    destroy();
    numCities   = Sol.numCities;          
    numItems    = Sol.numItems;           
    totalProfit = Sol.totalProfit;        
    totalTime   = Sol.totalTime;           
    totalGain   = Sol.totalGain;          
    remSpace    = Sol.remSpace;                 
    tour        = new int [numCities];
    plan        = new int [numItems] ;
    positions   = new SolPosition [numCities];
    cities      = new SolCity     [numCities];
    for (int posNum = 0; posNum < numCities; posNum++) {
        int cityId        = Sol.tour[posNum];
        tour[posNum]      = Sol.tour[posNum];
        positions[posNum] = Sol.positions[posNum];
        cities[cityId]    = Sol.cities[cityId];
    }
    for (int  itemId = 0; itemId < numItems; itemId++)
        plan[itemId] = Sol.plan[itemId];
}

TTPSolution::TTPSolution(const TTPSolution &Sol) {
    init();
    copy(Sol);
}

TTPSolution &TTPSolution::operator=(TTPSolution &Sol){
    copy(Sol);
    return Sol;
}

void TTPSolution::destroy()
{
    if (tour)
        delete [] tour;
    if (plan)
        delete [] plan;
    if (positions)
        delete [] positions;      
    if (cities)
        delete [] cities;  
}
TTPSolution::~TTPSolution(){
    destroy();
}

ostream &operator<<(ostream &Out, TTPSolution &Sol)
{
    int  numCities = Sol.numCities;
    int  numItems  = Sol.numItems;
    int  *tour     = Sol.getTour();
    int  *plan     = Sol.getPlan();
    
    
    Out << endl << "Tour: " << endl;
    for (int posNum = 0; posNum < numCities; posNum++)
        Out << tour[posNum] << " ";
    Out << endl << endl;
    Out << "Plan: " << endl;
    for (int itemId = 0; itemId < numItems; itemId++)
        Out << plan[itemId] << " ";
    Out << endl << endl;
    Out << "Total gain: " << endl;
    Out << Sol.totalGain;
}
