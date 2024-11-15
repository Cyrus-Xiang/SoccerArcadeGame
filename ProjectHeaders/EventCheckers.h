/****************************************************************************
 Module
     EventCheckers.h
 Description
     header file for the event checking functions
 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 10/18/15 11:50 jec      added #include for stdint & stdbool
 08/06/13 14:37 jec      started coding
*****************************************************************************/

#ifndef EventCheckers_H
#define EventCheckers_H

// the common headers for C99 types
#include <stdint.h>
#include <stdbool.h>

// prototypes for event checkers

bool Check4Coin(void);
bool Check4Shot(void);
bool Check4Goal(void); //has both goal and no goal states in this event
bool Check4Pot(void);
bool Check4Keystroke(void);
bool Check4BallPlacement(void);


//do i need one for a timer? doubt it

#endif /* EventCheckers_H */
