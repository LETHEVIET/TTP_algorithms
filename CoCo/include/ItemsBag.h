/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   ItemsBag.h
 * Author: Majid Namazi
 *
 * Created on 27 December 2019
 */

#ifndef ITEMSBAG_H
#define ITEMSBAG_H

#include "TTPInstance.h"
#include "TTPSolution.h"

class ItemsBag{
    
    protected:
        
        TTPInstance     &ttp = *(TTPInstance *)nullptr;
        TTPSolution     &sol = *(TTPSolution *)nullptr;
        int             *bag;                           /// Container for the items
        int             *loc;                           /// Location of the items in the bag
        int              maxNumItems;                   /// Maximum number of the items
        int              curNumItems;                   /// Current number of the items
        int              numChecked;                    /// Number of the already checked items
    
    public:
    
        bool allChecked() {return numChecked==curNumItems;};
        
        void uncheckAll() {numChecked = 0; };
        
        int  randItem();
        
        void markChecked(int ItemId);
        
        virtual void markFlipped(int ItemId) = 0;
        
        ItemsBag() {};
        
        ItemsBag(TTPInstance &TTP, TTPSolution &Sol);
        
        ~ItemsBag();
};


#endif /* ITEMSBAG_H */

