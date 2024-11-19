/****************************************************************************

  Header file for Buzzer service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef ServBuzzer_H
#define ServBuzzer_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitBuzzerService(uint8_t Priority);
bool PostBuzzerService(ES_Event_t ThisEvent);
ES_Event_t RunBuzzerService(ES_Event_t ThisEvent);

#endif /* ServBuzzer_H */

