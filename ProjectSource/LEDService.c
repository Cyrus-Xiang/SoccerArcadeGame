/****************************************************************************
 Module
   LEDService.c

 Revision
   1.0.1

 Description
   This is a template file for implementing a simple service under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/16/12 09:58 jec      began conversion from TemplateFSM.c
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "LEDService.h"
#include "dbprintf.h"
#include "ES_Port.h"
#include "terminal.h"
#include "DM_Display.h"
#include "PIC32_SPI_HAL.h"
#include "FontStuff.h"
/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/
static void ChangeLongMsg(uint16_t);
/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;
#define ONE_SEC 1000
#define HALF_SEC (ONE_SEC / 2)
#define TWO_SEC (ONE_SEC * 2)
#define FIVE_SEC (ONE_SEC * 5)
static const char *LongMsg = "PLEASE INSERT COINS ";
static size_t MsgLength = 20;
static uint16_t msg_ind = 0;
static LED_MatrixState_t CurrentState;
static uint16_t Player1Score = 0;
static uint16_t Player2Score = 0;
static int8_t TimeLeft_s; //count down time for solenoid shot
static uint16_t TimePerRound_s;
#define ScrollTimeInterval_ms 300

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitLEDService

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, and does any
     other required initialization for this service
 Notes

 Author
     J. Edward Carryer, 01/16/12, 10:00
****************************************************************************/
bool InitLEDService(uint8_t Priority)
{
  ///Set needed non-constant variables////////
  //static uint8_t char_pos1 = 0; 
  // Setup SPI using SPI HAL

    SPISetup_DisableSPI(SPI_SPI1); // Turn off SPI
    //
    //  //Config everything
    SPISetup_BasicConfig(SPI_SPI1);
    SPISetup_SetLeader(SPI_SPI1, SPI_SMP_MID);
    SPISetup_MapSSOutput(SPI_SPI1, SPI_RPA0);
    SPISetup_MapSDOutput(SPI_SPI1, SPI_RPA1);
    SPISetup_SetClockIdleState(SPI_SPI1, SPI_CLK_HI);
    SPISetup_SetActiveEdge(SPI_SPI1, SPI_SECOND_EDGE);
    SPISetEnhancedBuffer(SPI_SPI1, true);
    SPISetup_SetXferWidth(SPI_SPI1, SPI_16BIT);
    SPISetup_SetBitTime(SPI_SPI1, 1e9 / 100e3);
    //  //Turn SPI back on
    SPISetup_EnableSPI(SPI_SPI1);
  while (!SPIOperate_HasSS1_Risen()){} // needed for the first two messages to be sperated
  //Setup LED matrix
while ( false == DM_TakeInitDisplayStep() ){}
// DM_AddChar2DisplayBuffer('I');//a initialization message
while (false == DM_TakeDisplayUpdateStep()){  }
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  /********************************************
   in here you write your initialization code
   *******************************************/

  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  DB_printf( "LED Service Initiated \n");
  if (ES_PostToService(MyPriority, ThisEvent) == true)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/****************************************************************************
 Function
     PostLEDService

 Parameters
     EF_Event_t ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
bool PostLEDService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunLEDService

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes

 Author
   J. Edward Carryer, 01/15/12, 15:23
****************************************************************************/
ES_Event_t RunLEDService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
 LED_MatrixState_t NextState = CurrentState;
  ES_Event_t Event2post;
  switch (CurrentState) {
    case ScrollMsgMode:{
      switch (ThisEvent.EventType)
      {
      case EnterScoreLED:{
        NextState = ScoreMode;
        TimePerRound_s = ThisEvent.EventParam;
        ES_Timer_StopTimer(LED_Timer);
        DM_ClearDisplayBuffer();
        //reset the timer and update display buffer
        TimeLeft_s = TimePerRound_s;
        //clear players scores and update display buffer
        Player1Score = 0; 
        Player2Score = 0;
        DM_AddNum2Buffer_Module(Player1Score,3);
        DM_AddNum2Buffer_Module(Player2Score,1);
        //add a VS on the 2nd module
        DM_AddChar2Buffer_Module('V',2);
        DM_ScrollModuleBuffer(4,2);
        DM_AddChar2Buffer_Module('S',2);
        Event2post.EventType = ES_LED_Disp_Need_Update;
        PostLEDService(Event2post);
      }
      break;
      case ES_TIMEOUT:{
        if (ThisEvent.EventParam == LED_Timer)
        {
          // DB_printf("LED Timer timed out \n");
          ES_Timer_InitTimer(LED_Timer, ScrollTimeInterval_ms);
          ES_Event_t event2post;
          event2post.EventType = ES_NEW_KEY;
          if (msg_ind < MsgLength)
          {
            event2post.EventParam = LongMsg[msg_ind];
            msg_ind++;
          }else{ 
            event2post.EventParam = LongMsg[0];
            msg_ind = 1;
          }   
          ES_PostToService(MyPriority, event2post);
        }
        
      }
      break;
      case ES_NEW_KEY:   
      {
        DM_ScrollDisplayBuffer(4);
        DM_AddChar2DisplayBuffer((char)ThisEvent.EventParam);
        //DB_printf("updating the LED display \n");
        ES_Event_t event2post;
        event2post.EventType = ES_LED_Disp_Need_Update;
        ES_PostToService(MyPriority, event2post);
      }
      break;
      default:
      break;
      }   
    }
    break;
    case ScoreMode:{
        switch (ThisEvent.EventType)
        {

        case ES_TIMEOUT:{
          if (ThisEvent.EventParam == LED_Timer4Player)
          {
            if (TimeLeft_s > 0)
            {
              TimeLeft_s --;//update time left
              DM_AddNum2Buffer_Module(TimeLeft_s,0);
              Event2post.EventType = ES_LED_Disp_Need_Update;
              PostLEDService(Event2post);
              ES_Timer_InitTimer(LED_Timer4Player,1000);
            }else // meaning there is no time left
            {
              ES_Timer_StopTimer(LED_Timer4Player);
            } 
          }
        }
        break;
        case BallShot:{
          TimeLeft_s = 1;
          
        }
        break;
        case LED_RestartTimer4Player:{
          TimeLeft_s = TimePerRound_s;
          DM_AddNum2Buffer_Module(TimeLeft_s,0);
          Event2post.EventType = ES_LED_Disp_Need_Update;
          PostLEDService(Event2post);
          //init local timer for counting time left for player for display purpose
          ES_Timer_InitTimer(LED_Timer4Player,1000);
        }
        break;
        
        case LED_ScoreUpdate:{
          //event param contains the information of which player's score to update
          if (ThisEvent.EventParam == 1)
          {
            Player1Score++;
            DM_AddNum2Buffer_Module(Player1Score,3);
          }else if (ThisEvent.EventParam == 2)
          {
            Player2Score++;
            DM_AddNum2Buffer_Module(Player2Score,1);
          }
          Event2post.EventType = ES_LED_Disp_Need_Update;
          PostLEDService(Event2post);
        }
        break;

        
        default:
          break;
        }
    }
    break;
    
    default:
    break;
  }
  CurrentState = NextState;

   
  //below is independent of the FSM and doing its own thing
  
  
  switch (ThisEvent.EventType)
  {
    case ES_INIT:
    {
      CurrentState = ScrollMsgMode;
      DM_ClearDisplayBuffer();
      ES_Timer_InitTimer(LED_Timer,ScrollTimeInterval_ms);
      DB_printf("Scroll message mode in LED Service\n");
    }break;
    //the LED matrix's row by row update runs in parallel in the background
    case ES_LED_Disp_Need_Update:
    {
      if (DM_TakeDisplayUpdateStep() == false)
      {
        ES_Event_t event2post;
        event2post.EventType = ES_LED_Disp_Need_Update;
        ES_PostToService(MyPriority, event2post);
      }
      
    }break;
    //check user inactivity
    case UserInactivity:{
        CurrentState = ScrollMsgMode;
        ChangeLongMsg(InactivityMsg);
        //the inactivity msg is only displayed for some period of time before we return to welcome msg
        ES_Timer_InitTimer(LED_InactivityMsgTimer,10000);
    }
    break;
    case ES_TIMEOUT:{
    //the inactivity msg is only displayed for some period of time before we return to welcome msg
    if (ThisEvent.EventParam == LED_InactivityMsgTimer)
        {
          //change back to the welcome message
         ChangeLongMsg(WelcomeMsg);
        }
    }break;
    //command from the main FSM to change the current long msg being displayed
    case LED_ChangeMsg:{
        ChangeLongMsg(ThisEvent.EventParam);
    }break;
    default:
    break;
  
  }
  return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/
//The function will clear the module, init LED Timer send LED_needs_update event
void ChangeLongMsg(uint16_t WhichMsg){
  //stops the current scrolling of the message
  ES_Timer_StopTimer(LED_Timer);
  DM_ClearDisplayBuffer();
  switch (WhichMsg)
  {
  case  WelcomeMsg:
    LongMsg = "PLEASE INSERT COINS ";
    MsgLength = 20;
    break;
  case InactivityMsg:
    LongMsg = "TIMEOUT DUE TO USER INACTIVITY ";
    MsgLength = 31;      
    break;
  case Player1WinMsg:
    LongMsg = "PLAYER 1 WINS ";
    MsgLength = 14;
    break;
  case Player2WinMsg:
    LongMsg = "PLAYER 2 WINS ";
    MsgLength = 14;
    break;
  case TieMsg:
    LongMsg = "GAME IS A TIE ";
    MsgLength = 14;
    break;
  case PlaceBallMsg:
    LongMsg = "P1 PLEASE PLACE BALL ";
    MsgLength = 21;
    break;
  case CoinInserted1Msg:
    LongMsg = "COIN INSERTED 1/2 ";
    MsgLength = 18;
    break;
  default:
    break;
  }
  msg_ind = 0;
  ES_Timer_InitTimer(LED_Timer,ScrollTimeInterval_ms);
  //update the LED matrix
  ES_Event_t Event2post;
  Event2post.EventType = ES_LED_Disp_Need_Update;
  PostLEDService(Event2post);
  
}
/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

