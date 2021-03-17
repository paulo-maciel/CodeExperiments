#include "TextNoteStateMachine.h"

using namespace std;

int main()
{
  TextNoteStateMachine sm;
  sm.Init();

  sm.CreateTextNote();

  // We should now be in CreateAnchorMoving mode.
  // Send 6dof events.
  auto* event = new ExternalEvent(3);
  for(int i = 0; i < 3; ++i) {
    sm.HandleExternalEvent(event);
  }
  delete event;

  // Send a trigger clicked event
  event = new ExternalEvent(1);
  sm.HandleExternalEvent(event);
  delete event;

  // We should now be in CreateTextMoving mode.
  // Send 6dof events.
  event = new ExternalEvent(3);
  for (int i = 0; i < 3; ++i) {
    sm.HandleExternalEvent(event);
  }

  // Send a trigger clicked event to finish creating the text note.
  event = new ExternalEvent(1);
  sm.HandleExternalEvent(event);
  delete event;

  // Note has been created and we should be back in Idle state.
  std::string finalState = sm.GetCurrentState()->getName();
  cout << "Final state is: " << finalState << endl;

  // Create another text note and bail out.
  sm.CreateTextNote();

  // We should now be in CreateAnchorMoving mode.
  // Send 6dof events.
  event = new ExternalEvent(3);
  for (int i = 0; i < 3; ++i)
  {
    sm.HandleExternalEvent(event);
  }
  delete event;

  // bail out by pressing the home button.
  event = new ExternalEvent(2);
  sm.HandleExternalEvent(event);

  event = new ExternalEvent(3);
  for (int i = 0; i < 3; ++i)
  {
    sm.HandleExternalEvent(event);
  }
  delete event;

  finalState = sm.GetCurrentState()->getName();
  cout << "Final state is: " << finalState << endl;

  // Find a note to delete.
  sm.DeleteTextNote();

  return 1;
}