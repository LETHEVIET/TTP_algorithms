/******************************************************************************/
/*                                                                            */
/*  Copyright 2019 Majid Namazi.                                              */
/*                                                                            */
/*  This file is part of CoCo TTP solver released under the New BSD license.  */
/*                                                                            */
/******************************************************************************/

/* 
 * File:   PGCH.h
 * Author: Majid Namazi
 *
 * Created on 27 December 2019
 */

#ifndef PGCH_H
#define PGCH_H

#include "ProTwoOPT.h"

class PGCH : public ProTwoOPT {

    protected:
        
        int     *highUnPicked2;     // Keeps the location of the highest-ratio un-picked item
                                    // in the availItems list of each city located inside the
                                    // virtually reversed segment
        
        long     sumUnPickedWeight; // Keeps sum of the weights of the low profitable items 
                                    // virtually un-picked by the virUnPickLowProfitableItems 
                                    // method. 
        
        long     sumUnPickedProfit; // Keeps sum of the profits of the low profitable items 
                                    // virtually un-picked by the virUnPickLowProfitableItems
                                    // method.
        
        long     sumPickedWeight;   // Keeps sum of the weights of the high profitable items 
                                    // virtually picked by the virPickHighProfitableItems method.
         
        long     sumPickedProfit;   // Keeps sum of the profits of the high profitable items 
                                    // virtually picked by the virPickHighProfitableItems method.
        
    
        vector<int>  flipItems;     // Filled in the checkMove method, keeps the list of items whose 
                                    // status must be flipped when the makeMove method needs    
                                    // to be done, to ensure that exactly the same items will 
                                    // be considered in the makeMove method as in the checkMove 
                                    // method.
    
        vector<int>  flipItems2;    // Copied from flipItems in the keepMove method, keeps the 
                                    // list of items whose status must be flipped when the 
                                    // makeKeptMove method needs to be done, to ensure that 
                                    // exactly the same items will be considered in the 
                                    // makeKeptMove method as in the checkMove method.                              
        
        void   virUnPickLowProfitableItems();           // Called by the virAdjustPlan method,
                                                        // virtually un-picks low profitable
                                                        // items from the cities located in the 
                                                        // virtually reversed segment.
        
        void   virPickHighProfitableItems();            // Called by the virAdjustPlan method,
                                                        // virtually picks high profitable
                                                        // items from the cities located in the 
                                                        // virtually reversed segment
        
        void    virAdjustPlan() {                       // Called by the checkMove method, virtually 
                                                        // adjusts the plan in coordination with
                                                        // the virtual segment reversing. 
            flipItems.clear();                        
            virUnPickLowProfitableItems();
            virPickHighProfitableItems();
        }

        virtual double  evalVirChanges();               // Called by the checkMove method, 
                                                        // evaluates potential impacts of the 
                                                        // virtual segment reversing and the  
                                                        // corresponding plan adjustment

        void    realAdjustPlan(vector<int> &FlipItems); // Called by the makeMove and makeKeptMove 
                                                        // methods, adjusts the plan in coordination with
                                                        // the real segment reversing.
        
        void    updateLowPickedHighUnPicked(vector<int> &FlipItems);          
                                                        // Called by the makeMove and makeKeptMove
                                                        // methods, updates lowPicked and highUnPicked 
                                                        // values for each city located in the 
                                                        // reversed segment after adjusting the plan.
                 
        virtual void    updateTotalGain(int PosA, int PosB);    // Called by the makeMove and 
                                                                // makeKeptMove methods, updates
                                                                // total gain values and its 
                                                                // related parameters.
        
    public: 
        
        virtual double  checkMove(int PosNumA, int PosNumB) {  // Virtually reverses the segment,
                                                         // adjusts the plan and evaluates impacts.
                                                      
            posNumA = PosNumA; posNumB = PosNumB;
            virtuallyReverseSegment();
            virAdjustPlan();
            return evalVirChanges();
            
        };
        
        virtual void    keepMove()
        {
            posNumA2   = posNumA;
            posNumB2   = posNumB;
            flipItems2 = flipItems;
        }
        
        
        virtual void    makeMove() {                     // Makes the real segment reversing and 
                                                         // adjustment of the plan accordingly
                                                         // based-on the last checked move.
            
            realReverseSegment(posNumA,posNumB);
            realAdjustPlan(flipItems);
            updateLowPickedHighUnPicked(flipItems);
            updateMinMaxRatioes(posNumA,posNumB);
            updateTotalGain(posNumA,posNumB);
          
        };    
        
        virtual void    makeKeptMove() {                 // Makes the real segment reversing and 
                                                         // adjustment of the plan accordingly
                                                         // based-on the last kept move.
            
            realReverseSegment(posNumA2,posNumB2);
            realAdjustPlan(flipItems2);
            updateLowPickedHighUnPicked(flipItems2);
            updateMinMaxRatioes(posNumA2,posNumB2);
            updateTotalGain(posNumA2,posNumB2);
          
        };    

        PGCH() {};
        
        PGCH(TTPInstance &TTP,TTPSolution &Sol);
        
        ~PGCH();
};

#endif /* PGCH_H */

