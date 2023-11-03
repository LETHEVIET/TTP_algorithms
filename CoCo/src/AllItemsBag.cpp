/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   AllItemsBag.cpp
 * Author: Majid Namazi
 * 
 * Created on 27 December 2019
 */

#include "AllItemsBag.h"


AllItemsBag::AllItemsBag(TTPInstance &TTP, TTPSolution &Sol):ItemsBag(TTP,Sol) {
    
    curNumItems = maxNumItems;
    for (int index = 0, itemId = 0; index < curNumItems; index++, itemId++)
    {
        bag[index]  = itemId;
        loc[itemId] = index ;
    } 
}
