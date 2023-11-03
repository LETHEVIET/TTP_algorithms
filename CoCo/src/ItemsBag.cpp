/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   ItemsBag.cpp
 * Author: Majid Namazi
 * 
 * Created on 27 December 2019
 */

#include <ItemsBag.h>

ItemsBag::ItemsBag(TTPInstance &TTP, TTPSolution &Sol): ttp(TTP), sol(Sol) {
    
    maxNumItems = ttp.getNumItems();
    numChecked  = 0;
    bag = new int [maxNumItems];
    loc = new int [maxNumItems];   
}

int  ItemsBag::randItem() {
    int index = rand()%(curNumItems-numChecked) + numChecked;
    return bag[index];
}

void ItemsBag::markChecked(int ItemId) {
    
    int l = loc[ItemId];
    swap(bag[numChecked],bag[l]);
    loc[ItemId] = numChecked++  ;
    loc[bag[l]] = l;
}

ItemsBag::~ItemsBag() {
    
    if (bag) 
        delete [] bag;
    if (loc) 
        delete [] loc;   
};
