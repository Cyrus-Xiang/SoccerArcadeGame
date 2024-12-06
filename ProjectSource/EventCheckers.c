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

/****************************************************************************
****************************************************************************/
// need to check the high to low input to determine coin insert
bool Check4Coin(void)
{
  ES_Event_t Event2Post;
  // initialize current local variables
  bool ReturnVal = false;
  bool CurrentCoinState = PORTBbits.RB8; // defining to read input of coin sensor on RB6, and maybe include FSM in this event checker
  static bool LastCoinState = 1;
  // check to see if different coin state (switch from low to high indicates the end of coin passing through)
  if (CurrentCoinState != LastCoinState)
  {
    // if it is returning high coin just passed through
    if (!CurrentCoinState)
    {
      Event2Post.EventType = CoinDetected;
      PostSoccerFSM(Event2Post);
    }
    ReturnVal = true;
  }
  LastCoinState = CurrentCoinState;
  return ReturnVal;
}

bool Check4BallPlacement(void)
{
  ES_Event_t Event2Post;
  // initialize current local variables
  bool ReturnVal = false;
  bool CurrentPlacementState = PORTBbits.RB4; // defining to read input of placement sensor on RB4
  static bool LastPlacementState = 1;
  if (CurrentPlacementState != LastPlacementState)
  {
    // if it is returning high coin just passed through
    if (!CurrentPlacementState)
    {
      Event2Post.EventType = BallPlaced;
      PostSoccerFSM(Event2Post);
      DB_printf("ball place detected in event checker \n");
      ReturnVal = true;
    }
    LastPlacementState = CurrentPlacementState;
  }

  return ReturnVal;
}

bool Check4Goal(void)
{
  ES_Event_t Event2Post;
  // initializing goal sensor readings
  bool GoalSensorState = PORTBbits.RB9; // defining to read input of goal sensor on RB9, and maybe include FSM in this event checker
  static bool LastGoalState = 1;        // HIGH means nothing is detected

  // initializing miss sensor readings on pin RB13
  bool MissSensorState = PORTBbits.RB13;
  static bool LastMissState = 1;

  // initialize miss sensor 2 on pin RA4
  bool MissSensorState1 = PORTAbits.RA4;
  static bool LastMissState1 = 1;

  // check if the goal sensor reads a different value from last time (switch from low to high indicates the end of ball passing through goal)
  if (GoalSensorState != LastGoalState)
  {
    // if it is returning high, ball just passed through
    if (!GoalSensorState)
    {
      Event2Post.EventType = GoalBeamBroken;
      return true;
    }
  }
  // check miss sensor
  if (MissSensorState != LastMissState)
  {
    if (!MissSensorState)
    {
      Event2Post.EventType = MissBeamBroken;
    }
    return true;
  }
  // check the other miss sensor
  if (MissSensorState1 != LastMissState1)
  {
    if (!MissSensorState1)
    {
      Event2Post.EventType = MissBeamBroken;
    }
    return true;
  }
  LastGoalState = GoalSensorState;
  LastMissState = MissSensorState;
  LastMissState1 = MissSensorState1;
  return false;
}

bool Check4ShotButton(void)
{
  ES_Event_t Event2Post;
  // initialize current local variables
  bool ReturnVal = false;
  bool CurrentShotState = PORTBbits.RB5; // defining to read input of shot sensor
  static bool LastShotState = 0;

  // check to see if different Shot state (switch from low to high indicates the button is pressed)
  if (CurrentShotState != LastShotState)
  {
    // if it is returning high coin just passed through
    if (CurrentShotState)
    {
      Event2Post.EventType = ShotButtonPushed;
      PostSoccerFSM(Event2Post);
      ReturnVal = true;
    }
  }
  LastShotState = CurrentShotState;
  return ReturnVal;
}

bool Check4Keystroke(void)
// This is for debugging purposes and allows keyboard interactions
{
  if (IsNewKeyReady()) // new key waiting?
  {
    ES_Event_t Event2Post;
    Event2Post.EventType = ES_NEW_KEY;
    Event2Post.EventParam = GetNewKey();
    PostTestHarnessService0(Event2Post);
    // PostLEDService(Event2Post);
    return true;
  }
  return false;
}

// check for potentiometer change
bool Check4Pot(void)
{
  int32_t diff_AD1;
  static uint32_t Curr_AD_Val1[1];
  static uint32_t Last_AD_Val1[] = {0};
  ADC_MultiRead(Curr_AD_Val1); // 10 bits, 0-1023 corresponding to 0-3.3V
  diff_AD1 = abs(Curr_AD_Val1[0] - Last_AD_Val1[0]);
  // we only send event to ther service if the change in reading is significant
  if (diff_AD1 > 20)
  {
    Last_AD_Val1[0] = Curr_AD_Val1[0];
    ES_Event_t Event2Post;
    Event2Post.EventType = Pot_Val_Update;
    Event2Post.EventParam = (uint16_t)Curr_AD_Val1[0];
    PostServoService(Event2Post);
    PostSoccerFSM(Event2Post);
    return true;
  }
  return false;
}
