/****************************************************************************

  Header file for LED service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef LEDService_H
#define LEDService_H

#include "ES_Types.h"
#include "DM_Display.h"
#include "PIC32_SPI_HAL.h"

// State definitions for use with the query function
typedef enum
{
  ScrollMsgMode,
  ScoreBoardMode,
} LED_MatrixState_t;

enum WhichMsgEnum
{
  WelcomeMsg,
  Player1WinMsg,
  Player2WinMsg,
  TieMsg,
  InactivityMsg,
  PlaceBallMsg,
  CoinInserted1Msg,
};
// Public Function Prototypes

bool InitLEDService(uint8_t Priority);
bool PostLEDService(ES_Event_t ThisEvent);
ES_Event_t RunLEDService(ES_Event_t ThisEvent);

#endif /* SerUltra_H */
