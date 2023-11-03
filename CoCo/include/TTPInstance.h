/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   TTPInstance.h
 * Author: Majid Namazi
 *
 * Created on 27 December 2019
 */

#ifndef TTPINSTANCE_H
#define TTPINSTANCE_H
#include <string>
#include <cstdio>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <set>
#include <math.h>

using namespace std;

struct City {
        int     numAvailItems;
        int     numDelaNeighbs;
        int    *availItems;           /// Sorted based on the profitability ratios
        int    *delaNeighbs;          /// Neighbor cities in the delaunay triangulation
};

struct Item {
        int     cityId;               /// City in which item is located or available
        int     location;             /// Location in the availItems list of the involving city
        int     profit;               /// Profit of the item
        int     weight;               /// Weight of the item
        double  ratio;                /// Profitability ratio of the item
};

class TTPInstance {
        string  ttpName;              /// Such as a280_n279_bounded-strongly-corr_01
        int     norm;                 /// Edge weight type
        int     numCities;            /// Number of cities
        int     numItems;             /// Number of items
        long    capacity;             /// Capacity of the knapsack
        double  minSpeed;             /// Minimum speed, with full knapsack
        double  maxSpeed;             /// Maximum speed, with empty knapsack
        double  rentRatio;            /// Rent ratio of the knapsack
        double  velConst;             /// Velocity constant 
        double  maxRatio;             /// Maximum profitability ratio
    
        double  *xCoords;             /// City coordinates are needed to be kept in this way  
        double  *yCoords;             /// to be used by the Concorde TSP Solver based methods
                                       
        struct City    *cities;
        struct Item    *items ;
        
        void    readTTP(char *TTPName);
        void    fillCities();
        void    fillAvailItems();
        void    delaunayEdges(int &EdgesCount, int* &EdgesList);
        void    fillDelaNeighbs();
        
    public:  
        string  getTTPName() {return ttpName;};
        
        int     getNorm() {return norm;};
        
        int     getNumCities() {return numCities;};
        
        int     getNumItems() {return numItems;};
        
        long    getCapacity() {return capacity;};
        
        double  getMinSpeed() {return minSpeed;};
        
        double  getMaxSpeed() {return maxSpeed;};
        
        double  getRentRatio() {return rentRatio;};
        
        double  getVelConst() {return velConst;};
        
        double  getMaxRatio() {return maxRatio;};
        
        double  *getXCoords() {return xCoords;};
        
        double  *getYCoords() {return yCoords;};
        
        const Item    *getItems() {return items;};
        
        const int     *getAvailItems(int CityId) {
            
            if (0<=CityId && CityId<numCities)
                return cities[CityId].availItems;
            else {
                cout<<"City index="<<CityId<<" is out of range in getAvailItems."<<endl;
                return (int *)nullptr;
            }    
        };
        
        const int     *getDelaNeighbs(int CityId) {
            
            if (0<=CityId && CityId<numCities)
                return cities[CityId].delaNeighbs;
            else {
                cout<<"City index="<<CityId<<" is out of range in getDelaNeighbs."<<endl;
                return (int *)nullptr;
            }
        };
        
        int   getNumAvailItems(int CityId) {
        
            if (0<=CityId && CityId<numCities)
                return cities[CityId].numAvailItems;
            else {
                cout<<"City index="<<CityId<<" is out of range in getNumAvailItems."<<endl;
                return -1;
            }
        };
        int   getNumDelaNeighbs(int CityId) {
            
            if (0<=CityId && CityId<numCities)
                return cities[CityId].numDelaNeighbs;
            else {
                cout<<"City index="<<CityId<<" is out of range in getNumDelaNeighbs"<<endl;
                return -1;
            }
        }; 
        bool    compareItems(int ItemId1, int ItemId2);
        double  distDouble(int CityId1, int CityId2);
        long    distLong(int CityId1, int CityId2);
        TTPInstance(char *TTPName);
        ~TTPInstance();    
        
        friend ostream &operator<<(ostream &Out, TTPInstance &TTP);
};  

#endif /* TTPINSTANCE_H */

