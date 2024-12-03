/****************************************************************************
 Module
   EventCheckers.c

 Revision
   1.0.1

 Description
   This is the sample for writing event checkers along with the event
   checkers used in the basic framework test harness.

 Notes
   Note the use of static variables in sample event checker to detect
   ONLY transitions.

 History
 When           Who     What/Why
 -------------- ---     --------
 08/06/13 13:36 jec     initial version
****************************************************************************/

// this will pull in the symbolic definitions for events, which we will want
// to post in response to detecting events
#include "ES_Configure.h"
// This gets us the prototype for ES_PostAll
#include "ES_Framework.h"
// this will get us the structure definition for events, which we will need
// in order to post events in response to detecting events
#include "ES_Events.h"
// if you want to use distribution lists then you need those function
// definitions too.
#include "ES_PostList.h"
// This include will pull in all of the headers from the service modules
// providing the prototypes for all of the post functions
#include "ES_ServiceHeaders.h"
// this test harness for the framework references the serial routines that
// are defined in ES_Port.c
#include "ES_Port.h"
// include our own prototypes to insure consistency between header &
// actual functionsdefinition
#include "EventCheckers.h"

// This is the event checking function sample. It is not intended to be
// included in the module. It is only here as a sample to guide you in writing
// your own event checkers
#if 0
/****************************************************************************
 Function
   Check4Lock
 Parameters
   None
 Returns
   bool: true if a new event was detected
 Description
   Sample event checker grabbed from the simple lock state machine example
 Notes
   will not compile, sample only
 Author
   J. Edward Carryer, 08/06/13, 13:48
****************************************************************************/
bool Check4Lock(void)
{
  static uint8_t  LastPinState = 0;
  uint8_t         CurrentPinState;
  bool            ReturnVal = false;

  CurrentPinState = LOCK_PIN;
  // check for pin high AND different from last time
  // do the check for difference first so that you don't bother with a test
  // of a port/variable that is not going to matter, since it hasn't changed
  if ((CurrentPinState != LastPinState) &&
      (CurrentPinState == LOCK_PIN_HI)) // event detected, so post detected event
  {
    ES_Event ThisEvent;
    ThisEvent.EventType   = ES_LOCK;
    ThisEvent.EventParam  = 1;
    // this could be any of the service post functions, ES_PostListx or
    // ES_PostAll functions
    ES_PostAll(ThisEvent);
    ReturnVal = true;
  }
  LastPinState = CurrentPinState; // update the state for next time

  return ReturnVal;
}

#endif

/****************************************************************************
 Function
   Check4Keystroke
 Parameters
   None
 Returns
   bool: true if a new key was detected & posted
 Description
   checks to see if a new key from the keyboard is detected and, if so,
   retrieves the key and posts an ES_NewKey event to TestHarnessService0
 Notes
   The functions that actually check the serial hardware for characters
   and retrieve them are assumed to be in ES_Port.c
   Since we always retrieve the keystroke when we detect it, thus clearing the
   hardware flag that indicates that a new key is ready this event checker
   will only generate events on the arrival of new characters, even though we
   do not internally keep track of the last keystroke that we retrieved.
 Author
   J. Edward Carryer, 08/06/13, 13:48
****************************************************************************/

bool Check4BallPlacement(void){
      ES_Event_t ThisEvent;
  //initialize current local variables
    bool ReturnVal = false;
    bool CurrentPlacementState= PORTBbits.RB4; //defining to read input of placement sensor on RB4
    static bool LastPlacementState = 1;
    //check to see if different coin state (switch from low to high indicates the end of coin passing through)
    if(CurrentPlacementState != LastPlacementState){
        //if it is returning high coin just passed through
        if(!CurrentPlacementState){
            ThisEvent.EventType= BallPlaced;
            PostSoccerFSM(ThisEvent);
           DB_printf("ball place detected in event checker \n");
           ReturnVal = true;
        }
    LastPlacementState= CurrentPlacementState;
    }     
  
  return ReturnVal;
}
//need to check the high to low input to determine coin insert
bool Check4Coin(void)
{
    ES_Event_t ThisEvent;
  //initialize current local variables
    bool ReturnVal = false;
    bool CurrentCoinState= PORTBbits.RB8; //defining to read input of coin sensor on RB6, and maybe include FSM in this event checker
    static bool LastCoinState = 1;
    //check to see if different coin state (switch from low to high indicates the end of coin passing through)
    if(CurrentCoinState != LastCoinState){
        //if it is returning high coin just passed through
        if(!CurrentCoinState){
            ThisEvent.EventType= CoinDetect;
            PostSoccerFSM(ThisEvent);
//            DB_printf("coin detected in event checker \n");
        }
        ReturnVal = true;
    }     
  LastCoinState= CurrentCoinState;
  return ReturnVal;
}


bool Check4Goal(void)
{
    ES_Event_t ThisEvent;
  //initialize current local variables
    bool ReturnVal = false;
    
    //initializing goal sensor readings
    bool CurrentGoalState= PORTBbits.RB9; //defining to read input of goal sensor on RB9, and maybe include FSM in this event checker
    static bool LastGoalState = 1;

    
    //initializing miss sensor readings on pin RB13
//<<<<<<< HEAD
    bool CurrentMissState= PORTBbits.RB13;
    static bool LastMissState = 1;
    //initialize miss sensor 2 on pin RA4
    bool CurrentMissState1= PORTAbits.RA4;
    static bool LastMissState1 = 1;

    if (CurrentMissState1 != LastMissState1)
    {
      if(!CurrentMissState1){
            ThisEvent.EventType= GoalBeamBroken;
            PostSoccerFSM(ThisEvent);
            PostTestHarnessService0(ThisEvent);
        }
        ReturnVal = true;
    }
    LastMissState1 = CurrentMissState1;

    //check to see if different goal state (switch from low to high indicates the end of ball passing through goal)
    if(CurrentGoalState != LastGoalState){
        //if it is returning high, ball just passed through
        if(!CurrentGoalState){
            ThisEvent.EventType= GoalBeamBroken;
            PostSoccerFSM(ThisEvent);
            PostTestHarnessService0(ThisEvent);
          ReturnVal = true;
        }
        
    }
    else if(CurrentMissState != LastMissState){
        if(!CurrentMissState){
            ThisEvent.EventType= MissBeamBroken;
            PostSoccerFSM(ThisEvent);
            PostTestHarnessService0(ThisEvent);
            ReturnVal = true;
        }
    }else if (CurrentMissState1 != LastMissState1)
    {
            ThisEvent.EventType= MissBeamBroken;
            PostSoccerFSM(ThisEvent);
            PostTestHarnessService0(ThisEvent);
            ReturnVal = true;
    }
  LastMissState1 = CurrentMissState1;
  LastGoalState= CurrentGoalState;
  LastMissState= CurrentMissState;
  return ReturnVal;
}


bool Check4Shot(void)
{
    ES_Event_t ThisEvent;
  //initialize current local variables
    bool ReturnVal = false;
    bool CurrentShotState= PORTBbits.RB5; //defining to read input of shot sensor
    static bool LastShotState = 0;
    
    //check to see if different Shot state (switch from low to high indicates the button is pressed)
    if(CurrentShotState != LastShotState){
        //if it is returning high coin just passed through
        if(CurrentShotState){
            ThisEvent.EventType= ShotButtonDown;
            PostSoccerFSM(ThisEvent);
            ReturnVal = true;
        }
        
    }     
  LastShotState= CurrentShotState;
  return ReturnVal;
}

bool Check4Keystroke(void)
{
  if (IsNewKeyReady()) // new key waiting?
  {
    ES_Event_t ThisEvent;
    ThisEvent.EventType = ES_NEW_KEY;
    ThisEvent.EventParam = GetNewKey();
    PostTestHarnessService0(ThisEvent);
    //PostLEDService(ThisEvent);
    return true;
  }
  return false;
}

// check for potentiometer change
bool Check4Pot(void)
{
  int32_t diff_AD1;
  static uint32_t Curr_AD_Val1[1];
  static uint32_t Last_AD_Val1[] ={0};
  ADC_MultiRead(Curr_AD_Val1);//10 bits, 0-1023 corresponding to 0-3.3V
  diff_AD1 = abs( Curr_AD_Val1[0] - Last_AD_Val1[0]);
  if (diff_AD1 > 20){
      Last_AD_Val1[0] = Curr_AD_Val1[0];
      ES_Event_t ThisEvent;
      ThisEvent.EventType = Pot_Val_Update;
      ThisEvent.EventParam = (uint16_t) Curr_AD_Val1[0];
      PostServoService(ThisEvent);
      PostSoccerFSM(ThisEvent);
      return true;
      }
  return false;
}


