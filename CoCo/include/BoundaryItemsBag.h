/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   BoundaryItemsBag.h
 * Author: Majid Namazi
 *
 * Created on 27 December 2019
 */

#ifndef BOUNDARYITEMSBAG_H
#define BOUNDARYITEMSBAG_H

#include "ItemsBag.h"

class BoundaryItemsBag : public ItemsBag {
    
    protected:
        
        int     lowPicked;                       // Used to maintain the bag
    
        int     highUnPicked;                    // Used to maintain the bag
    
        void    insert(int ItemId);
        
        void    remove(int ItemId);
        
        void    initBag(void);
        
        void    fillBagBasedOnMinPRatioes(void);
        
        void    fillBagBasedOnMaxURatioes(void);
        
        void    updateLocalLowPickedHighUnPicked(int ItemId);
    
        void    updateBagBasedOnNewMinPRatioes(int ItemId);
        
        void    updateBagBasedOnNewMaxURatioes(int ItemId);
    
    public:
        
        virtual void markFlipped(int ItemId) {
            updateLocalLowPickedHighUnPicked(ItemId);
            updateBagBasedOnNewMinPRatioes(ItemId);    
            updateBagBasedOnNewMaxURatioes(ItemId);    
            uncheckAll();
        }
        
        BoundaryItemsBag() {};
        
        BoundaryItemsBag(TTPInstance &TTP, TTPSolution &Sol) : ItemsBag(TTP,Sol) {
            initBag();        
        };
        
        ~BoundaryItemsBag() {};
};

#endif /* BOUNDARYITEMSBAG_H */

