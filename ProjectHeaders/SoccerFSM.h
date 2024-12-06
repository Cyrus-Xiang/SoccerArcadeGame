/****************************************************************************

  Header file for template Flat Sate Machine
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef FSMSoccer_H
#define FSMSoccer_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
  InitPState,
  Wait4Coin,
  Wait4PlayerShot,
  Wait4BallReturn,
  Wait4BallPlacement,
} SoccerState_t;

// Public Function Prototypes

bool InitSoccerFSM(uint8_t Priority);
bool PostSoccerFSM(ES_Event_t ThisEvent);
ES_Event_t RunSoccerFSM(ES_Event_t ThisEvent);
bool CheckMissBeamSensor(void);
bool CheckBallReturnPlayer1();
bool CheckBallReturnPlayer2();
SoccerState_t QuerySoccerFSM(void);

#endif /* FSMTemplate_H */
