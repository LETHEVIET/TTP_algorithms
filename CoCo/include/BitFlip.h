/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   BitFlip.h
 * Author: Majid Namazi
 *
 * Created on 27 December 2019
 */

#ifndef BITFLIP_H
#define BITFLIP_H

#include "Move.h"

class BitFlip : public Move {
    
    protected:    
        
        int     flipItemId;           // Keeps the itemId of the flipping item
        
        int     flipItemId2;          // Keeps a copy of the itemId of the flipping item after calling
                                      // the keepMove method
    
        int     deltaWeight;          // Keeps weight of the item, virtually picked(+) or un-picked(-).  
        
        int     deltaProfit;          // Keeps profit of the item, virtually picked(+) or un-picked(-). 
        
        virtual void    virtuallyFlipState();                       // Called by the checkMove method,
                                                                    // virtually flips state of the 
                                                                    // given item, ie. flipItemId
        
        virtual double  evalVirChanges();                           // Called by the checkMove method,
                                                                    // evaluates impacts of the virtual
                                                                    // changes.
        
        virtual void    realFlipState(int FlipItemId);              // Called by the makeMove and 
                                                                    // makeKeptMove methods, makes real 
                                                                    // flipping state of the given item,
                                                                    // ie. flipItemId or flipItemId2   
        
        virtual void    updateTotalGain(int FlipItemId, int NotReq=0);  
                                                                    // Called by the makeMove and 
                                                                    // makeKeptMove methods, updates 
                                                                    // the total gain value and its 
                                                                    // related parameters according 
                                                                    // to the real changes.
   
    public:
        
        virtual double  checkMove(int FlipItemId, int NotReq=0) {   // Virtually flips state of the given 
                                                                    // item and evaluates impacts.    
            flipItemId = FlipItemId;
            virtuallyFlipState();
            return evalVirChanges();
            
        };
        
        virtual void     keepMove() {                           // Called after the checkMove method,
                                                                // keeps internal parameters related
                                                                // to the last checked move to facilitate
                                                                // making the best move later.
            flipItemId2 = flipItemId;
        }
        
        virtual void    makeMove() {                            // Flips state of the given item 
                                                                // and updates solution's parameters,
                                                                // based on the latest checked move.
            realFlipState(flipItemId);
            updateTotalGain(flipItemId);
        }
        
        virtual void    makeKeptMove() {                        // Flips state of the given item 
                                                                // and updates solution's parameters
                                                                // based on the latest kept move.
            realFlipState(flipItemId2);
            updateTotalGain(flipItemId2);
        }
        
        BitFlip() {};
        
        BitFlip(TTPInstance &TTP,TTPSolution &Sol) : Move(TTP,Sol) {};
        
        ~BitFlip() {}; 
        
};

#endif /* BITFLIP_H */

