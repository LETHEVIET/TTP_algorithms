/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   AllItemsBag.h
 * Author: Majid Namazi
 *
 * Created on 27 December 2019
 */

#ifndef ALLITEMSBAG_H
#define ALLITEMSBAG_H

#include "ItemsBag.h"

class AllItemsBag : public ItemsBag {
    
    public:
    
        virtual void markFlipped(int ItemId=0) { uncheckAll(); };
        
        AllItemsBag() {};
        
        AllItemsBag(TTPInstance &TTP, TTPSolution &Sol);
        
        ~AllItemsBag() {};
};

#endif /* ALLITEMSBAG_H */

