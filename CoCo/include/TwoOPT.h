/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   TwoOPT.h
 * Author: Majid Namazi
 *
 * Created on 27 December 2019
 */

#ifndef TWOOPT_H
#define TWOOPT_H

#include "Move.h"

// This move has been developed for reversing a segment indicated by two positions PosNumA and 
// PosNumB. The segment starting at PosNumA, going always forward and ending at PosNumB is reversed.
// For example, if there is 10 cities in the tour and PosNumA=3, PosNumB=6, the segment including 
// positions 3, 4, 5 and 6 will be reversed. However, if PosNumA=6 and PosNumB=3, the segment
// including positions 6,7,8,9,0,1,2 and 3 will be reversed.

class TwoOPT : public Move {
    
    protected:
        
        int      posNumA;              // Starting position number of the segment to be reversed 
                                          
        int      posNumB;              // Ending position number of the segment to be reversed
        
        int      posNumA2;             // Keeps a copy of the starting position number of the segment
                                       // to be reversed after calling the keepMove Method
        
        int      posNumB2;             // Keeps a copy of the ending position number of the segment
                                       // to be reversed after calling the keepMove Method 
        
        int     *tour2;                // Keeps virtually reversed segment.
        
        long    *addWeight2;           // Keeps added weight to the knapsack in each position of 
                                       // the virtually reversed segment.

        virtual void    virtuallyReverseSegment();                   // Called by the checkMove method, 
                                                                     // virtually reverses the segment.     
        
        virtual double  evalVirChanges();                            // Called by the checkMove method,
                                                                     // evaluates potential impacts of 
                                                                     // the virtual changes.
        
        virtual void    realReverseSegment(int PosNumA, int PosNumB);// Called by the makeMove and 
                                                                     // makeKeptMove methods, makes the
                                                                     // real segment reversing.
        
        virtual void    updateTotalGain(int PosNumA, int PosNumB);   // Called by the makeMove and 
                                                                     // makeKeptMove methods, updates
                                                                     // solution's parameters according
                                                                     // to the real changes.

    public:
        
        virtual double  checkMove(int PosNumA, int PosNumB) {   // Virtually reverses the segment 
                                                                // and evaluates impacts.
            posNumA = PosNumA; posNumB = PosNumB;
            virtuallyReverseSegment();         
            return evalVirChanges();
            
        };                
        virtual void    keepMove()                              // Called after the checkMove method,
                                                                // keeps internal parameters related
                                                                // to the last checked move to facilitate
                                                                // making the best move later.
        {
            posNumA2 = posNumA;
            posNumB2 = posNumB;
        }
        virtual void    makeMove(){                             // Makes the real segment reversing 
                                                                // and updates solution's parameters
                                                                // according to the latest checked move.
            realReverseSegment(posNumA,posNumB); 
            updateTotalGain(posNumA,posNumB);
            
        }
        
        virtual void    makeKeptMove(){                         // Makes the real segment reversing 
                                                                // and updates solution's parameters
                                                                // according to the latest checked move.
            realReverseSegment(posNumA2,posNumB2);
            updateTotalGain(posNumA2,posNumB2);
            
        }
        
        TwoOPT() {};
        
        TwoOPT(TTPInstance &TTP,TTPSolution &Sol);
        
        ~TwoOPT();
};

#endif /* TWOOPT_H */

