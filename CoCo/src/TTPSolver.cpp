/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   TTPSolver.cpp
 * Author: Majid Namazi
 * 
 * Created on 27 December 2019
 */

#include "CLKSolver.h"
#include "TTPSolver.h"
#include "TTPSolution.h"
#include "PGCH.h"
#include "TwoOPT.h"
#include "AllItemsBag.h"
#include "BoundaryItemsBag.h"
#include "BitFlip.h"
#include "ProBitFlip.h"

double get_wall_time() {
    struct timeval time;
    if (gettimeofday(&time,NULL)) {
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
double get_cpu_time() {
    return (double)clock() / CLOCKS_PER_SEC;
}

int *TTPSolver::chainedLinKernTour() {
    CLKSolver clkSolver;
    int     numCities = ttp.getNumCities();
    int     norm      = ttp.getNorm();   
    double *xCoords   = ttp.getXCoords();
    double *yCoords   = ttp.getYCoords();
    double  timeBound = 1+(rand()%101)/100.0;
    int     inRepeats = rand()%(numCities + 1);
    int    *tour      = clkSolver.tour(numCities,xCoords,yCoords,norm,timeBound,inRepeats);
    return  tour;
}
TTPSolver::TTPSolver(TTPInstance &TTP, double StartWallTime, int TimeLimit) : ttp(TTP) {
    int     numCities    = ttp.getNumCities();
    int     numItems     = ttp.getNumItems() ;
    
    timeLimit            = TimeLimit;           
    startWallTime        = StartWallTime;
    bestOverAllTotalGain = -INFINITY;       
    bestOverAllTour      = new int [numCities];
    bestOverAllPlan      = new int [numItems];
    acceptedRevCount     = 0;
    acceptedRevLenAvg    = 0;
   
}

void TTPSolver::updateBestOverAll(int *Tour,int *Plan, double TotalGain) {
    int numCities = ttp.getNumCities();
    int numItems  = ttp.getNumItems();
    for (int posNum = 0; posNum < numCities; posNum++)
        bestOverAllTour[posNum] = Tour[posNum];
    for (int itemId = 0; itemId < numItems; itemId++)
        bestOverAllPlan[itemId] = Plan[itemId];
    bestOverAllTotalGain = TotalGain;
}

double TTPSolver::computeTotalGain(TTPSolution &Sol) {
    int         *tour         = Sol.getTour();
    int         *plan         = Sol.getPlan(); 
    SolCity     *solCities    = Sol.getCities();
    SolPosition *solPositions = Sol.getPositions(); 
    int          numCities    = ttp.getNumCities();
    long         capacity     = ttp.getCapacity() ;
    double       maxSpeed     = ttp.getMaxSpeed() ;
    double       rentRatio    = ttp.getRentRatio();
    double       velConst     = ttp.getVelConst() ;
    const Item  *items        = ttp.getItems();
    long         accWeight    = 0;     
    long         accProfit    = 0;     
    double       accTime      = 0;
    
    for (int posNum = 0; posNum < numCities; posNum++) {
        double  addTime;
        long    addWeight = 0;
        int     cityId    = tour[posNum];       
        int     numAvailItems = ttp.getNumAvailItems(cityId);
        const int *availItems = ttp.getAvailItems(cityId);    
        for (int loc = 0; loc < numAvailItems; loc++) {
            int itemId = availItems[loc]; 
            if (plan[itemId]) {
                accProfit += items[itemId].profit;
                addWeight += items[itemId].weight;
            }
        }
        accWeight  += addWeight;
        int cityId2 = tour[(posNum+1)%numCities];
        addTime     = ttp.distLong(cityId,cityId2) / 
                                   (maxSpeed - accWeight * velConst);
        accTime    += addTime;
        solPositions[posNum].addTime   = addTime;
        solPositions[posNum].accTime   = accTime;
        solPositions[posNum].addWeight = addWeight;
        solPositions[posNum].accWeight = accWeight;
        solCities[cityId].posNum       = posNum;
    }
    Sol.totalTime   = accTime;
    Sol.totalProfit = accProfit;
    Sol.remSpace    = capacity  - accWeight;
    Sol.totalGain   = accProfit - rentRatio * accTime;
    return Sol.totalGain;
}

double TTPSolver::computeTotalGain(int *Tour, int *Plan)
{ 
    int          numCities    = ttp.getNumCities();
    double       maxSpeed     = ttp.getMaxSpeed() ;
    double       velConst     = ttp.getVelConst() ;  
    double       rentRatio    = ttp.getRentRatio();
    const Item  *items        = ttp.getItems();
    long         accWeight    = 0;     
    long         accProfit    = 0;     
    double       accTime      = 0;
    
    for (int posNum = 0; posNum < numCities; posNum++) {
        long    addWeight = 0;
        int     cityId = Tour[posNum];
        int     numAvailItems = ttp.getNumAvailItems(cityId);
        const int *availItems = ttp.getAvailItems(cityId);    
        for (int loc = 0; loc < numAvailItems; loc++) {
            int itemId = availItems[loc];    
            if (Plan[itemId]) {
                accProfit += items[itemId].profit;
                addWeight += items[itemId].weight;
            }
        }
        accWeight  += addWeight;
        int cityId2 = Tour[(posNum+1)%numCities];
        accTime    += ttp.distLong(cityId,cityId2) / 
                                  (maxSpeed - accWeight * velConst);
    }
    return accProfit - rentRatio * accTime;
}

void    TTPSolver::initLowPickedHighUnPicked(TTPSolution &Sol) {
    int       numCities    = ttp.getNumCities();   
    int      *plan         = Sol.getPlan()  ;
    SolCity  *solCities    = Sol.getCities();
    
    for (int cityId = 0; cityId < numCities; cityId++) {
        int     numAvailItems = ttp.getNumAvailItems(cityId);
        const int *availItems = ttp.getAvailItems(cityId);    
        solCities[cityId].lowPicked    = numAvailItems;
        solCities[cityId].highUnPicked = -1;
        for (int loc = 0; loc < numAvailItems; loc++)
        {
            int itemId = availItems[loc];
            if (!plan[itemId])
                solCities[cityId].highUnPicked = loc;
            else if (solCities[cityId].lowPicked == numAvailItems)
                solCities[cityId].lowPicked = loc;
        }    
    }
}

void   TTPSolver::initMinPRatioes(TTPSolution &Sol) {
    int          numCities    = ttp.getNumCities();
    double       maxRatio     = ttp.getMaxRatio();
    const Item  *items        = ttp.getItems();
    int         *tour         = Sol.getTour()  ;
    SolCity     *solCities    = Sol.getCities();
    SolPosition *solPositions = Sol.getPositions();
    double       minPRatio    = maxRatio+1;
    
    for (int posNum = 0; posNum < numCities; posNum++) {
        double  ratio;
        int     cityId = tour[posNum];
        int     loc    = solCities[cityId].lowPicked;
        int     numAvailItems = ttp.getNumAvailItems(cityId);
        const int *availItems = ttp.getAvailItems(cityId);    
        if (loc < numAvailItems) {
            int itemId = availItems[loc];
            ratio      = items[itemId].ratio;
        }
        else
            ratio      = maxRatio + 1;
        if (ratio < minPRatio)
            minPRatio  = ratio;
        solPositions[posNum].minPRatio = minPRatio;
    }
}

void   TTPSolver::initMaxURatioes(TTPSolution &Sol) {
    int          numCities    = ttp.getNumCities();
    const Item  *items        = ttp.getItems();
    int         *tour         = Sol.getTour()  ;
    SolCity     *solCities    = Sol.getCities();
    SolPosition *solPositions = Sol.getPositions();
    double       maxURatio    = 0;
    
    for (int posNum = numCities-1; posNum > 0; posNum--) {
        double  ratio;
        int     cityId = tour[posNum];
        int     loc    = solCities[cityId].highUnPicked;
        const int *availItems = ttp.getAvailItems(cityId);   
        if (loc >= 0) {
            int itemId = availItems[loc];
            ratio      = items[itemId].ratio;
        }
        else
            ratio      = 0;
        if (ratio > maxURatio)
            maxURatio  = ratio;
        solPositions[posNum].maxURatio = maxURatio;
    }
}

void TTPSolver::quickSort(double *Scores, int *Indices, int Low, int High) {
    
    if (Low >= High)
        return;
    int randPos = rand()%(High-Low+1)+Low;
    swap(Indices[Low],Indices[randPos]);
    double pivot = Scores[Indices[Low]];
    
    int i = Low; 
    for (int j = i+1; j <= High; j++)
    {
        if (Scores[Indices[j]] >= pivot)
            swap(Indices[++i],Indices[j]);
    }
    swap(Indices[Low],Indices[i]);    
    quickSort(Scores,Indices,Low,i-1);
    quickSort(Scores,Indices,i+1,High);
}

int *TTPSolver::sortItems(double *Scores) {
    int     numItems = ttp.getNumItems(); 
    int     *indices = new int [numItems];
    for (int index = 0; index < numItems; index++)
        indices[index] = index;
    quickSort(Scores,indices,0,numItems-1);
    return indices;
}

void TTPSolver::packItems(int *Tour, double *Len, double Alpha, int *Plan) {
    int         numItems = ttp.getNumItems();
    const Item *items    = ttp.getItems(); 
    int         muItems  = numItems/100;
    bool        jFlag;
    if (muItems > 1)
        jFlag   = true;
    else {    
        jFlag   = false;
        muItems = 2;
        
    }
    long    remSpace = ttp.getCapacity();
    double  *scores  = new double [numItems];
    for (int itemId  = 0; itemId < numItems; itemId++) {
        Plan[itemId] = 0;
        int  cityId  = items[itemId].cityId;
        //if (Len[cityId]>0)    
        scores[itemId] = pow(items[itemId].ratio,Alpha) / Len[cityId];
        //else
        //    scores[itemId] = std::numeric_limits<double>::infinity();
    }
    double  totalGain = computeTotalGain(Tour,Plan);
    double  localBestTotalGain = totalGain; 
    
    int   *sortedItems = sortItems(scores);
           
    int     index = 0, numPicked = 0;
    while (index<numItems && remSpace>0 && muItems>=2) {
        int itemId = sortedItems[index];
        if (remSpace >= items[itemId].weight) {
            Plan[itemId] = 1;
            remSpace    -= items[itemId].weight;
        }
        if (index%muItems==0 || !jFlag) {
            totalGain = computeTotalGain(Tour,Plan);
            if (totalGain >= localBestTotalGain) {
                numPicked  = index + 1;
                localBestTotalGain = totalGain;
            }
            else {
                while (index >= numPicked)
                {
                    itemId = sortedItems[index];
                    if (Plan[itemId]!=0) {
                        Plan[itemId] = 0;
                        remSpace    += items[itemId].weight;
                    }
                    index--;
                }
                muItems = ceil(muItems/2.0);
            }
        }    
        index++;
    }
    delete [] scores;
    delete [] sortedItems;
}


int *TTPSolver::packIterative(int *Tour, double C, double D, int Q) { /// c=5, d=2.5, q=20
    
    double   epsilon   = 0.1;
    int      numCities = ttp.getNumCities();
    int      numItems  = ttp.getNumItems() ;
    int     *leftPlan  = new int    [numItems] ;
    int     *midlePlan = new int    [numItems] ;
    int     *rightPlan = new int    [numItems] ;
    double  *length    = new double [numCities];

    //length[Tour[numCities-1]] = 0;
    length[Tour[numCities-1]] = ttp.distLong(Tour[numCities-1],Tour[0]);
    for (int posNum = numCities-2; posNum >= 0; posNum--)
        //length[Tour[posNum]] = length[Tour[posNum+1]] + ttp.distDouble(Tour[posNum],Tour[posNum+1]);
        length[Tour[posNum]] = length[Tour[posNum+1]] + ttp.distLong(Tour[posNum],Tour[posNum+1]);
    
    packItems(Tour,length,C-D,leftPlan); 
    double leftTotalGain  = computeTotalGain(Tour,leftPlan);  
    
    packItems(Tour,length,C,midlePlan); 
    double midleTotalGain = computeTotalGain(Tour,midlePlan);
    
    packItems(Tour,length,C+D,rightPlan); 
    double rightTotalGain = computeTotalGain(Tour,rightPlan);
    
    int   *bestPlan = midlePlan;
    
    for (int i = 0; i < Q; i++) {
        
        if (leftTotalGain>midleTotalGain && rightTotalGain>midleTotalGain) {
            if (leftTotalGain > rightTotalGain) {
                midleTotalGain = leftTotalGain;  
                bestPlan = leftPlan;
                C = C - D;
            }
            else {
                midleTotalGain = rightTotalGain;  
                bestPlan = rightPlan;
                C = C + D;
            }
        }
        else if (leftTotalGain > midleTotalGain) {
            midleTotalGain = leftTotalGain;  
            bestPlan = leftPlan;
            C = C - D;
        } 
        else if (rightTotalGain > midleTotalGain) {
            midleTotalGain = rightTotalGain;  
            bestPlan = rightPlan;
            C = C + D;
        }
        
        D /= 2;
        
        packItems(Tour,length,C-D,leftPlan) ; 
        leftTotalGain  = computeTotalGain(Tour,leftPlan);  
        
        packItems(Tour,length,C+D,rightPlan); 
        rightTotalGain = computeTotalGain(Tour,rightPlan);
        
        if (abs(leftTotalGain  - midleTotalGain) < epsilon && 
            abs(rightTotalGain - midleTotalGain) < epsilon)
            break;
    }    
    
    delete [] length;
    if (leftPlan  != bestPlan)
        delete [] leftPlan;
    if (midlePlan != bestPlan)
        delete [] midlePlan;
    if (rightPlan != bestPlan)
        delete [] rightPlan;    
    
    return bestPlan;
} 

int *TTPSolver::insertion(int *Tour) {
                                            /// Imported from https://github.com/yafrani/ttplab
    int         numCities = ttp.getNumCities();
    int         numItems  = ttp.getNumItems() ;
    double      maxSpeed  = ttp.getMaxSpeed() ;
    long        capacity  = ttp.getCapacity() ;
    double      rentRatio = ttp.getRentRatio();
    double      velConst  = ttp.getVelConst() ;
    const Item *items     = ttp.getItems()    ;
    long       *len       = new long [numCities];
    int        *location  = new int  [numCities];
    int        *outPlan   = new int  [numItems] ;
    
    // time approximations
    double t1, t2, t3, a, b1, b2;
    
    location[Tour[numCities-1]] = numCities-1;
    len[numCities-1] = ttp.distLong(Tour[numCities-1],0);
    for (int posNum  = numCities-2; posNum >= 0; posNum--) 
    {
        len[posNum]  = len[posNum+1] + ttp.distLong(Tour[posNum],Tour[posNum+1]);
        location[Tour[posNum]] = posNum;
    }

    // sort item according to score
    double   *scores = new double [numItems];
    for (int itemId  = 0; itemId < numItems; itemId++) {
      // index where Bit-Flip happened
      int origBF = location[items[itemId].cityId];
      // calculate time approximations
      t1 = len[origBF]*(1/(maxSpeed-velConst*items[itemId].weight) - 1/maxSpeed);
      // affect score to item
      scores[itemId]  = (items[itemId].profit-rentRatio*t1) / items[itemId].weight;
      // empty the knapsack
      outPlan[itemId] = 0;
    }
    // evaluate solution after emptying knapsack
    // computeTotalGain(Tour,outPlan);

    // sort items according to score
    int   *sortedItems = sortItems(scores);

    // loop & insert items
    long currentWeight = 0;
    int v2 = 0, v3 = 0;
    for (int index = 0; index < numItems; index++) {

        int itemId = sortedItems[index];

        // check if new weight doesn't exceed knapsack capacity
        if (currentWeight + items[itemId].weight > capacity) {
            continue;
        }

        // index where Bit-Flip happened
        int origBF = location[items[itemId].cityId];

        /* insert item if it has a potential gain */
        // time approximations t2 (worst-case time)
        t2 = len[origBF] * (1/(maxSpeed-velConst*(currentWeight+items[itemId].weight)) - 
                            1/(maxSpeed-velConst*currentWeight));
        if (items[itemId].profit > rentRatio*t2) {
            v2++;
            outPlan[itemId] = 1;
            currentWeight  += items[itemId].weight;
        }
        else {
            // time approximations t3 (expected time)
            a  = currentWeight / len[0];
            b1 = maxSpeed - velConst * (currentWeight + items[itemId].weight);
            b2 = maxSpeed - velConst * currentWeight;
            t3 = (1 / a) * 
                 log( ( (a * len[0] + b1) * (a * (len[0] - len[origBF]) + b2) ) /
                      ( (a * (len[0] - len[origBF]) + b1) * (a * len[0] + b2) ) );
            if (items[itemId].profit > rentRatio*t3) {
                v3++;
                outPlan[itemId] = 1;
                currentWeight  += items[itemId].weight;
            }
            else 
                continue;
        }
    }

    //computeTotalGain(Tour,outPlan);
    delete [] len;
    delete [] location;
    delete [] scores;
    delete [] sortedItems;
    return outPlan;
}

int *TTPSolver::initPlan(int *Tour)
{
    int *plan1 = insertion(Tour);
    int *plan2 = packIterative(Tour);
    if (computeTotalGain(Tour,plan1) > computeTotalGain(Tour,plan2)) {
        delete [] plan2;
        return    plan1;
    } 
    else {
        delete [] plan1;
        return    plan2;
    }
}

TTPSolution &TTPSolver::hillClimbingKPSolverAllItems(TTPSolution &Sol) {

    int        *plan  = Sol.getPlan();
    const Item *items = ttp.getItems();

    computeTotalGain(Sol);
    AllItemsBag  itemsBag(ttp,Sol);
    BitFlip      bitFlip(ttp,Sol);
    
    while (!itemsBag.allChecked())  
    {
        double currentWallTime = get_wall_time();
        double elapsedWallTime = currentWallTime-startWallTime;      
        if (elapsedWallTime >= timeLimit)
            break;

        int itemId = itemsBag.randItem();
        
        if (!plan[itemId] && items[itemId].weight>Sol.getRemSpace()) {
            itemsBag.markChecked(itemId);
            continue;
        }
        
        double totalGain = bitFlip.checkMove(itemId);

        if (totalGain > Sol.getTotalGain()) 
        {
            itemsBag.markFlipped(itemId);
            bitFlip.makeMove();
        }
        else
            itemsBag.markChecked(itemId);    
    };
    return Sol;
}

TTPSolution &TTPSolver::hillClimbingKPSolverBoundaryItems(TTPSolution &Sol) {

    int        *plan  = Sol.getPlan();
    const Item *items = ttp.getItems();

    computeTotalGain(Sol);
    prepareForCoordMoves(Sol);
    BoundaryItemsBag  itemsBag(ttp,Sol);
    ProBitFlip        bitFlip(ttp,Sol);
    
    while (!itemsBag.allChecked())  
    {
        double currentWallTime = get_wall_time();
        double elapsedWallTime = currentWallTime-startWallTime;      
        if (elapsedWallTime >= timeLimit)
            break;

        int itemId = itemsBag.randItem();
        
        if (!plan[itemId] && items[itemId].weight>Sol.getRemSpace()) {
            itemsBag.markChecked(itemId);
            continue;
        }
        
        double totalGain = bitFlip.checkMove(itemId);

        if (totalGain > Sol.getTotalGain()) 
        {
            itemsBag.markFlipped(itemId);
            bitFlip.makeMove();            
        }
        else
            itemsBag.markChecked(itemId);    
    };
    return Sol;
}


TTPSolution &TTPSolver::steepestAscentHillClimbingTspSolver(TTPSolution &Sol) {
        
    int     numCities  = ttp.getNumCities();
    int     *tour      = Sol.getTour();
    SolCity *solCities = Sol.getCities();
    bool     improved;
    
    computeTotalGain(Sol);
    TwoOPT   twoOPT(ttp,Sol);
    
    double   bestTotalGain = Sol.getTotalGain();    
    
    do {
        double currentWallTime = get_wall_time();
        double elapsedWallTime = currentWallTime-startWallTime;
        if (elapsedWallTime>timeLimit)
            break;
        
        improved = false;
          
        for (int posNumA = 0; posNumA < numCities; posNumA++) {
            currentWallTime = get_wall_time();
            elapsedWallTime = currentWallTime-startWallTime;
            if (elapsedWallTime>timeLimit)
                break;
        
            int     cityIdA = tour[posNumA];
            int     numDelaNeighbs = ttp.getNumDelaNeighbs(cityIdA);
            const int *delaNeighbs = ttp.getDelaNeighbs(cityIdA);
            for (int index = 0; index < numDelaNeighbs; index++) {    
                int    cityIdB   = delaNeighbs[index];   
                int    posNumB   = solCities[cityIdB].posNum;
                double totalGain = twoOPT.checkMove(posNumA,posNumB); 
                if (totalGain - bestTotalGain > 0.01)
                {
                    twoOPT.keepMove();
                    bestTotalGain = totalGain;
                    improved      = true;
                    int revLen    = posNumA<posNumB ? posNumB-posNumA+1 : numCities-posNumA+posNumB+1; 
                    acceptedRevCount  += 1;
                    acceptedRevLenAvg += (revLen-acceptedRevLenAvg)/acceptedRevCount;
                }                
            } 
        }
        if (improved) 
        {
            double prevTotalGain = Sol.getTotalGain();  
            twoOPT.makeKeptMove();
            if (Sol.getTotalGain()-prevTotalGain < 0.0001*prevTotalGain)
                improved = false;
        }    
    } while (improved);
    return Sol;
}

TTPSolution &TTPSolver::coordSteepestAscentHillClimbingTspSolver(TTPSolution &Sol) {
        
    int     numCities  = ttp.getNumCities();
    int     *tour      = Sol.getTour();
    SolCity *solCities = Sol.getCities();
    bool     improved;
    
    computeTotalGain(Sol);
    prepareForCoordMoves(Sol);
    PGCH     pgch(ttp,Sol);
    
    double   bestTotalGain = Sol.getTotalGain();    
    do {
        double currentWallTime = get_wall_time();
        double elapsedWallTime = currentWallTime-startWallTime;
        if (elapsedWallTime>timeLimit)
            break;
        
        improved = false;
          
        for (int posNumA = 1; posNumA < numCities; posNumA++) {
            currentWallTime = get_wall_time();
            elapsedWallTime = currentWallTime-startWallTime;
            if (elapsedWallTime>timeLimit)
                break;
        
            int     cityIdA = tour[posNumA];
            int     numDelaNeighbs = ttp.getNumDelaNeighbs(cityIdA);
            const int *delaNeighbs = ttp.getDelaNeighbs(cityIdA);
            for (int index = 0; index < numDelaNeighbs; index++) {    
                int    cityIdB     = delaNeighbs[index];   
                int    posNumB     = solCities[cityIdB].posNum;
                if (posNumB < posNumA)
                    continue;
                double totalGain   = pgch.checkMove(posNumA,posNumB); 
                if (totalGain - bestTotalGain > 0.01)
                {
                    pgch.keepMove();
                    bestTotalGain = totalGain;
                    improved      = true;
                    int revLen    = posNumA<posNumB ? posNumB-posNumA+1 : numCities-posNumA+posNumB+1; 
                    acceptedRevCount  += 1;
                    acceptedRevLenAvg += (revLen-acceptedRevLenAvg)/acceptedRevCount;
                }                
            } 
        }
        if (improved) 
        {
            double prevTotalGain = Sol.getTotalGain();
            
            pgch.makeKeptMove();
            
            if (Sol.getTotalGain()-prevTotalGain<0.0001*prevTotalGain)
                improved = false;
        }    
    } while (improved);
    return Sol;
}


TTPSolution TTPSolver::solve(int Alg)
{
    int numCities = ttp.getNumCities();
    int numItems  = ttp.getNumItems();
    int tryNum    = 0;
   
    while (true)
    {
        
        double currentWallTime = get_wall_time();
        double elapsedWallTime = currentWallTime-startWallTime;
        if (elapsedWallTime > timeLimit)
            break;
        
        tryNum++;
        //cout<<"before tour"<<endl;
        
        int  *tour = chainedLinKernTour();
        //cout<<"after tour"<<endl;
        
        int  *plan = initPlan(tour);
        //cout<<"plan"<<endl;
        
        TTPSolution sol(numCities,numItems,tour,plan);

        computeTotalGain(sol);
        cout<<tryNum<<" : "<<sol.getTotalGain()<<" ";
        
        if (sol.getTotalGain() > bestOverAllTotalGain)
            updateBestOverAll(sol.getTour(),sol.getPlan(),sol.getTotalGain());
        
        //cout<<bestTotalGain;
        
        int iterNum = 0;
        
        do {
                     
            double currentWallTime = get_wall_time();
            double elapsedWallTime = currentWallTime-startWallTime;
            if (elapsedWallTime > timeLimit)
                break;
            
            iterNum++;
            
            cout<<endl<<"    "<<iterNum<<" ";
            
            double prevTotalGain = sol.getTotalGain();
            
            if (Alg==1 || Alg==2)
                steepestAscentHillClimbingTspSolver(sol);
            else
                coordSteepestAscentHillClimbingTspSolver(sol);
            
            
            cout<<sol.getTotalGain()<<" ";
            
            if (sol.getTotalGain() <= prevTotalGain && iterNum>1)
                break;
            
            if (sol.getTotalGain() > bestOverAllTotalGain)
                updateBestOverAll(sol.getTour(),sol.getPlan(),sol.getTotalGain());    
            
            prevTotalGain = sol.getTotalGain();
            if (Alg==1 || Alg==3)
                hillClimbingKPSolverAllItems(sol);
            else
                hillClimbingKPSolverBoundaryItems(sol);
            cout<<sol.getTotalGain()<<" ";
            if (sol.getTotalGain()<prevTotalGain)
                cout<<"KP reducing error"<<endl;
            if (sol.getTotalGain() <= prevTotalGain)
                break;
            
            if (sol.getTotalGain() > bestOverAllTotalGain)
                updateBestOverAll(sol.getTour(),sol.getPlan(),sol.getTotalGain()); 
            
        } while (true);
        cout<<endl<<"        "<<sol.getTotalGain()<<" "<<bestOverAllTotalGain<<endl;
    }
    double currentWallTime = get_wall_time();
    double elapsedWallTime = currentWallTime-startWallTime;
    cout<< elapsedWallTime << " " <<timeLimit<<endl;   
    TTPSolution bestSol(numCities,numItems,bestOverAllTour,bestOverAllPlan);   
    computeTotalGain(bestSol);
    return bestSol;
}
