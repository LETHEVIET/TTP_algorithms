/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   TTPInstance.cpp
 * Author: Majid Namazi
 * 
 * Created on 27 December 2019
 */

#include <TTPInstance.h>

extern "C" {
#include "concorde.h"
}

double TTPInstance::distDouble(int CityId1, int CityId2) {

    double X = xCoords[CityId1]-xCoords[CityId2]; X *= X;
    double Y = yCoords[CityId1]-yCoords[CityId2]; Y *= Y;
    return sqrt(X+Y);
}
long TTPInstance::distLong(int CityId1, int CityId2) {

    double X = xCoords[CityId1]-xCoords[CityId2]; X *= X;
    double Y = yCoords[CityId1]-yCoords[CityId2]; Y *= Y;
    return ceil(sqrt(X+Y));                        
}

void TTPInstance::readTTP(char *TTPName) {
                    /// Imported and adapted from Concorde TSP Solver at 
                    /// http://www.math.uwaterloo.ca/tsp/concorde.html
    int ttpNameLen = strlen(TTPName);
    ttpName = TTPName;
    char    tspName[50];
    int     k,tspNameLen; 
    for (k = 0; TTPName[k] != '_'; k++)
        tspName[k] = TTPName[k];
    tspName[k] = '\0';
    tspNameLen = k;
    char *fileName = new char [ttpNameLen+tspNameLen+15];
    if (!fileName)
    {
        cout<<"Unable to allocate memory for fileName."<<endl;
        return;
    }
    strcpy(fileName,"data/");
    strcat(fileName,tspName);
    strcat(fileName,"-ttp/");
    strcat(fileName,TTPName);
    strcat(fileName,".ttp");
    char    buf[256], key[256], field[256];
    char    *p;
    FILE    *in;
    norm      = -1;
    numCities = -1;
    numItems  = -1;
    if ((in = fopen (fileName, "r")) == (FILE *) NULL) {
        cout<<"Unable to open "<<fileName<<endl;
        return;
    }
    while (fgets (buf, 254, in) != (char *) NULL) {
        p = buf;
        if (sscanf(p, "%s", key) != EOF) {
            p += strlen (key);
            if (!strcmp (key, "DIMENSION:")) {
                while (*p == ' ')
                    p++;
                if (sscanf (p, "%s", field) == EOF) {
                    cout<<"Error in DIMENSION line"<<endl;
                    return;
                }
                numCities = atoi(field);   
            }
            else if (!strcmp (key, "NUMBER")) {
                while (*p != ':')
                    p++;
                p++;
                while (*p == ' ')
                    p++;
                if (sscanf (p, "%s", field) == EOF) {
                    cout<<"Error in NUMBER OF ITEMS line"<<endl;
                    return;
		}                
		numItems = atoi (field);
            }
            else if (!strcmp (key, "CAPACITY")) {
                while (*p != ':')
                    p++;
                p++;
                while (*p == ' ')
                    p++;
                if (sscanf (p, "%s", field) == EOF) {
                    cout<<"Error in CAPACITY OF KNAPSACK line"<<endl;
                    return;
		}                
		capacity = atol (field);
            }
            else if (!strcmp (key, "MIN")) {
                while (*p != ':')
                    p++;
                p++;
                while (*p == ' ')
                    p++;
                if (sscanf (p, "%s", field) == EOF) {
                    cout<<"Error in MIN SPEED line"<<endl;
                    return;
		}                
		minSpeed = atof (field);
            }
            else if (!strcmp (key, "MAX")) {
                while (*p != ':')
                    p++;
                p++;
                while (*p == ' ')
                    p++;
                if (sscanf (p, "%s", field) == EOF) {
                    cout<<"Error in MAX SPEED line"<<endl;
                    return;
		}                
		maxSpeed = atof (field);
            }
            else if (!strcmp (key, "RENTING")) {
                while (*p != ':')
                    p++;
                p++;
                while (*p == ' ')
                    p++;
                if (sscanf (p, "%s", field) == EOF) {
                    cout<<"Error in RENTING RATIO line"<<endl;
                    return;
		}                
		rentRatio = atof (field);
            }
            else if (!strcmp (key, "EDGE_WEIGHT_TYPE:")) {
                while (*p == ' ')
                    p++;
                if (sscanf (p, "%s", field) == EOF) {
                    cout<<"ERROR in EDGE_WEIGHT_TYPE line"<<endl;
                    return;
                }
                if (!strcmp (field, "EXPLICIT")) 
                    norm = CC_MATRIXNORM;
                else if (!strcmp (field, "EUC_2D")) 
                    norm = CC_EUCLIDEAN;
                else if (!strcmp (field, "EUC_3D")) 
                    norm = CC_EUCLIDEAN_3D;
                else if (!strcmp (field, "MAX_2D")) 
                    norm = CC_MAXNORM;
                else if (!strcmp (field, "MAN_2D"))
                    norm = CC_MANNORM;
                else if (!strcmp (field, "GEO"))
                    norm = CC_GEOGRAPHIC;
                else if (!strcmp (field, "GEOM"))
                    norm = CC_GEOM;
                else if (!strcmp (field, "ATT"))
                    norm = CC_ATT;
                else if (!strcmp (field, "CEIL_2D"))
                    norm = CC_EUCLIDEAN_CEIL;
                else if (!strcmp (field, "DSJRAND"))
                    norm = CC_DSJRANDNORM;
                else {
                    cout<<"ERROR: Not set up for norm "<<field<<endl;
                    return;
                }
            } 
            else if (!strcmp (key, "NODE_COORD_SECTION")) { // Just 2D is supported here
                int i;
                if ( numCities <= 0) {
                    cout<<"ERROR: Dimension not specified"<<endl;
                    return;
                }
                if ((norm & CC_NORM_SIZE_BITS) == CC_D2_NORM_SIZE) {
                    xCoords = new double [numCities];
                    yCoords = new double [numCities];
                    fscanf (in, "%*d %lf %lf", xCoords, yCoords);
                    for (i = 1; i < numCities; i++) {
                        fscanf (in, "%*d %lf %lf", xCoords+i, yCoords+i);
                    }
                } 
                else 
                    cout<<"ERROR: Node coordinates with norm "<<norm<<"?\n";
            }
            else if (!strcmp (key, "ITEMS")) { // Just 2D is supported here
                int i;
                if ( numItems <= 0) {
                    cout<<"ERROR: Number of items not specified"<<endl;
                    return;
                }
                maxRatio = 0;
                items = new struct Item [numItems];
                for (int itemId = 0; itemId < numItems; itemId++) {
                    struct Item item;
                    fscanf(in, "%*d %d %d %d", &item.profit, &item.weight, &item.cityId);
                    item.cityId  -= 1;
                    item.ratio    = ((double)item.profit)/item.weight;
                    items[itemId] = item;
                    if (item.ratio>maxRatio)
                        maxRatio  = item.ratio;
                }    
            }
        }
    }
    velConst = (maxSpeed - minSpeed) / capacity;
    fclose(in);
    delete [] fileName;
}    
TTPInstance::TTPInstance(char *TTPName) {

    readTTP(TTPName);     
    fillCities();
}

void TTPInstance::fillCities() {

    cities = new struct City [numCities];
    fillAvailItems();
    fillDelaNeighbs(); 
}

bool TTPInstance::compareItems(int ItemId1, int ItemId2) { 

    return ( items[ItemId1].ratio<items[ItemId2].ratio  ||
            (items[ItemId1].ratio==items[ItemId2].ratio &&
             items[ItemId1].profit<items[ItemId2].profit)); 
}

void TTPInstance::fillAvailItems() {
 
    vector<int> *availItemsLists = new vector<int> [numCities];
    
    for (int itemId = 0; itemId < numItems; itemId++) {
        int  cityId = items[itemId].cityId;
        availItemsLists[cityId].push_back(itemId);
    }    
    
    for (int cityId = 0; cityId < numCities; cityId++) {
        int numAvailItems = availItemsLists[cityId].size();
        cities[cityId].numAvailItems = numAvailItems;
        cities[cityId].availItems = new int [numAvailItems];
        numAvailItems = 0;
        for (int itemId: availItemsLists[cityId]) {
            int loc;
            for (loc = numAvailItems-1; loc >= 0; loc--) {
                int itemId2 = cities[cityId].availItems[loc];
                if (compareItems(itemId,itemId2))
                   cities[cityId].availItems[loc+1] = cities[cityId].availItems[loc];
                else
                    break;
            }
            cities[cityId].availItems[loc+1] = itemId;
            numAvailItems++;
        }
        for (int loc = 0; loc < numAvailItems; loc++) {
            int itemId = cities[cityId].availItems[loc];
            items[itemId].location = loc;
        }       
    }
    delete [] availItemsLists;    
}

TTPInstance::~TTPInstance() {

    delete [] xCoords;
    delete [] yCoords;
    delete [] items;
    for (int cityId = 0; cityId < numCities; cityId++) {
        delete [] cities[cityId].availItems;
        delete [] cities[cityId].delaNeighbs;
    }    
    delete [] cities;
}
void TTPInstance::delaunayEdges(int &EdgesCount, int* &EdgesList) {
                    /// Imported from Concorde TSP Solver at 
                    /// http://www.math.uwaterloo.ca/tsp/concorde.html
    CCdatagroup  dat;
    int   wantList = 1;
    CCutil_init_datagroup (&dat);
    dat.x = xCoords;
    dat.y = yCoords;
    if (CCutil_dat_setnorm (&dat, norm)) {
        cout<< "ERROR: Couldn't set norm "<<norm<<endl;
        return;
    }
    CCedgegen_delaunay(numCities, &dat, wantList, &EdgesCount, &EdgesList);
    dat.x = (double *)nullptr;
    dat.y = (double *)nullptr;
    CCutil_freedatagroup (&dat);
}
void TTPInstance::fillDelaNeighbs() {                    

    int   edgesCount;
    int   *edgesList;
    delaunayEdges(edgesCount,edgesList);
    
    vector<int> *delaNeighbsLists = new vector<int> [numCities];
    
    for (int edgeIndex = 0; edgeIndex < edgesCount; edgeIndex++) {
        int cityId1 = edgesList[2*edgeIndex];
        int cityId2 = edgesList[2*edgeIndex+1];
        delaNeighbsLists[cityId1].push_back(cityId2);
        delaNeighbsLists[cityId2].push_back(cityId1);
    }
    
    for (int cityId = 0; cityId < numCities; cityId++) {
        int numDelaNeighbs = delaNeighbsLists[cityId].size();
        cities[cityId].numDelaNeighbs = numDelaNeighbs;
        cities[cityId].delaNeighbs    = new int [numDelaNeighbs];
        numDelaNeighbs = 0;
        for (int cityId2: delaNeighbsLists[cityId])
            cities[cityId].delaNeighbs[numDelaNeighbs++] = cityId2;
    }
    delete [] delaNeighbsLists;
    free(edgesList);
}

ostream &operator<<(ostream &Out, TTPInstance &TTP) {
    
    int       numCities = TTP.getNumCities();
    int       numItems  = TTP.getNumItems();
    const Item  *items  = TTP.getItems();
    
    Out<<TTP.getTTPName()<<endl;
    Out<<TTP.getNumCities()<<" "<<TTP.getNumItems()<<endl;
    for (int cityId = 0; cityId < numCities; cityId++) {
        Out<<cityId<<":"<<endl;
        int     numAvailItems = TTP.getNumAvailItems(cityId);
        const int *availItems = TTP.getAvailItems(cityId);
        Out<<numAvailItems<<" : ";
        for (int loc = 0; loc < numAvailItems; loc++) {
            int itemId = availItems[loc];
            Out<<itemId<<" ";
        }
        Out<<endl;        
        int     numDelaNeighbs = TTP.getNumDelaNeighbs(cityId);
        const int *delaNeighbs = TTP.getDelaNeighbs(cityId);
        Out<<numDelaNeighbs<<" : ";
        for (int loc = 0; loc < numDelaNeighbs; loc++) {
            int cityId2 = delaNeighbs[loc];    
            Out<<cityId2<<" ";
        }
        Out<<endl<<endl;
    }
    for (int itemId = 0; itemId < numItems; itemId++) {
        Out<<itemId<<" : "<<items[itemId].cityId<<" "<<items[itemId].location<<" ";
        Out<<items[itemId].profit<<" "<<items[itemId].weight<<" "<<items[itemId].ratio<<endl;
    }    
}
