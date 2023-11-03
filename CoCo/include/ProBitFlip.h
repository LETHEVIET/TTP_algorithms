/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   ProBitFlip.h
 * Author: Majid Namazi
 *
 * Created on 27 December 2019
 */

#ifndef PROBITFLIP_H
#define PROBITFLIP_H

#include "BitFlip.h"

// This move has been developed to flip state of an item while updating attributes of the solution
// related to the coordination moves, ie. lowPicked and highUnPicked values of cities and minPRatio
// and maxURatio values of positions. 


class ProBitFlip : public BitFlip {
    
    protected:
        
        void    updateLowPickedHighUnPicked(int ItemId);
        
        void    updateMinPRatioes(int ItemId);
        
        void    updateMaxURatioes(int ItemId);
        
        void    updateMinMaxRatioes(int ItemId) {
             
            updateMinPRatioes(ItemId);
            updateMaxURatioes(ItemId);
            
        };
        
    public:
        
        virtual void    makeMove() {                            // Flips state of the given item 
                                                                // and updates solution's parameters
                                                                // based on the latest checked move.
            realFlipState(flipItemId);
            updateLowPickedHighUnPicked(flipItemId);
            updateMinMaxRatioes(flipItemId);
            updateTotalGain(flipItemId);
            
        }
        
        virtual void    makeKeptMove() {                        // Flips state of the given item 
                                                                // and updates solution's parameters
                                                                // based on the latest kept move.
            realFlipState(flipItemId2);
            updateLowPickedHighUnPicked(flipItemId2);
            updateMinMaxRatioes(flipItemId2);
            updateTotalGain(flipItemId2);
            
        }
        
        ProBitFlip() {};
        
        ProBitFlip(TTPInstance &TTP,TTPSolution &Sol) : BitFlip(TTP,Sol){};
        
        ~ProBitFlip() {};

};

#endif /* PROBITFLIP_H */

