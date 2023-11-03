/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   Move.h
 * Author: Majid Namazi
 *
 * Created on 27 December 2019
 */

#ifndef MOVE_H
#define MOVE_H

#include "TTPInstance.h"
#include "TTPSolution.h"

class Move {
    
    protected:    
    
        TTPInstance     &ttp = *(TTPInstance *)nullptr;
        TTPSolution     &sol = *(TTPSolution *)nullptr;
        
        
        virtual double  evalVirChanges(void) = 0;                   // Called by the checkMove method,
                                                                    // evaluates impacts of the virtual
                                                                    // changes.
                
        virtual void    updateTotalGain(int IndexA, int IndexB) = 0;// Called by the makeMove and 
                                                                    // makeKeptMove methods, updates
                                                                    // the total gain value and its 
                                                                    // related parameters according 
                                                                    // to the real changes.
        
    public:
        
        virtual double  checkMove(int IndexA, int IndexB) = 0;  // Virtually changes the solution and
                                                                // evaluates impacts. 
        
        virtual void    keepMove(void) = 0;                     // Called after the checkMove method,
                                                                // keeps internal parameters related
                                                                // to the last checked move to facilitate
                                                                // making the best move later.
                                                                   
        virtual void    makeMove(void) = 0;                     // Makes real changes on the solution 
                                                                // and updates solution's parameters
                                                                // based on the latest checked move.
        
        virtual void    makeKeptMove(void) = 0;                 // Makes real changes on the solution 
                                                                // and updates solution's parameters
                                                                // based-on the latest kept move.

        
        Move(){};
        
        Move(TTPInstance &TTP, TTPSolution &Sol): ttp(TTP),sol(Sol) {};
        
        ~Move() {};    
};

#endif /* MOVE_H */

