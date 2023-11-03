/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   ProTwoOPT.h
 * Author: Majid Namazi
 *
 * Created on 27 December 2019
 */

#ifndef PROTWOOPT_H
#define PROTWOOPT_H

#include "TwoOPT.h"

// This move has been developed to reverse a segment while updating attributes of the solution
// related to the coordination moves, ie. minPRatio and maxURatio values of the positions. 
// As such, it does not adjust the plan according to the segment reversed and so it does 
// not need to update lowPicked and highUnPicked values of the cities.

class ProTwoOPT : public TwoOPT{
    
    protected:
        
        void updateMinPRatioes(int PosNumA, int PosNumB);
        
        void updateMaxURatioes(int PosNumA, int PosNumB);
        
        void updateMinMaxRatioes(int PosNumA, int PosNumB) {
             updateMinPRatioes(PosNumA,PosNumB);
             updateMaxURatioes(PosNumA,PosNumB);
        };
    
    public:
        
        virtual void   makeMove() {                               // Makes the real segment reversing
                                                                  // and updates solution's parameters
                                                                  // based on the last checked move.
            realReverseSegment(posNumA,posNumB);
            updateMinMaxRatioes(posNumA,posNumB);
            updateTotalGain(posNumA,posNumB);
        }
        
        virtual void   makeKeptMove() {                           // Makes the real segment reversing
                                                                  // and updates solution's parameters
                                                                  // based on the last kept move.
            realReverseSegment(posNumA2,posNumB2);
            updateMinMaxRatioes(posNumA2,posNumB2);
            updateTotalGain(posNumA2,posNumB2);
        }
        
        ProTwoOPT() {};
        
        ProTwoOPT(TTPInstance &TTP,TTPSolution &Sol) : TwoOPT(TTP,Sol) {};
        
        ~ProTwoOPT() {} ;

};

#endif /* PROTWOOPT_H */

