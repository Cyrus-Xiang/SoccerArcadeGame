/****************************************************************************

  Header file for Servo service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef ServServo_H
#define ServServo_H
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"
#include "dbprintf.h"
#include "PIC32_AD_Lib.h"

// Public Function Prototypes

bool InitServoService(uint8_t Priority);
bool PostServoService(ES_Event_t ThisEvent);
ES_Event_t RunServoService(ES_Event_t ThisEvent);

#endif /* ServServo_H */

