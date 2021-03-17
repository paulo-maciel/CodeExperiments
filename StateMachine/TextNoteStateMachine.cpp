#include "TextNoteStateMachine.h"

#include <cassert>

void TextNoteStateMachine::CreateTextNote() {
  // Create anchor, leader line, and UiTextEdit.
  // Make leader line and UiTextEdit invisible for now.
  // Use the grab manipulator to put the anchor in movement.
  // Transition to the CreateAnchorMoving state.
  std::cout << "Started creation of a text note. current state: " << GetCurrentState()->getName() << std::endl;
  assert(GetCurrentState()->getName() == "IdleState");
  TransitionTo("CreateAnchorMovingState");
}

void TextNoteStateMachine::DeleteTextNote()
{
  std::cout << "Looking for note to delete." << std::endl;
  assert(GetCurrentState()->getName() == "IdleState");

  TransitionTo("SelectAnchorTextState");
}

// States
STATE_DECLARE(IdleState) };
void IdleState::HandleInternalEvent(InternalEvent internalEvent)
{
  std::cout << "HandleInternalEvent in Idle state." << std::endl;
}

STATE_DECLARE(CreateAnchorMovingState) };
void CreateAnchorMovingState::HandleInternalEvent(InternalEvent internalEvent)
{
  switch (internalEvent.type_)
  {
  case InternalEventType::TriggerClicked:
    // Stop manipulating anchor.
    // Make leaderline and UiTextEdit visible.
    // Attach the grab manipulator to the UiTextEdit and start moving (How?)
    std::cout << "Trigger clicked handled in CreateAnchorMovingState" << std::endl;
    sm_.TransitionTo("CreateTextMovingState");
    break;
  case InternalEventType::HomeClicked:
    std::cout << "Home clicked handled in CreateAnchorMovingState" << std::endl;
    // TODO: Delete textnote.
    sm_.TransitionTo("IdleState");
    break;
  case InternalEventType::ControllerMoved:
    std::cout << "Controller moved event in CreateAnchorMovingState" << std::endl;
    break;
  default:
    break;
  }
}

STATE_DECLARE(CreateTextMovingState) };
void CreateTextMovingState::HandleInternalEvent(InternalEvent internalEvent)
{
  switch (internalEvent.type_)
  {
  case InternalEventType::TriggerClicked:
  case InternalEventType::HomeClicked:
    // Stop manipulating text.
    std::cout << "Trigger clicked handled in CreateTextMovingState" << std::endl;
    sm_.TransitionTo("IdleState");
    break;
  case InternalEventType::ControllerMoved:
    std::cout << "Controller moved event in CreateTextMovingState" << std::endl;
    break;
  default:
    break;
  }
}

STATE_DECLARE(SelectAnchorTextState) };
void SelectAnchorTextState::HandleInternalEvent(InternalEvent internalEvent)
{
  switch (internalEvent.type_) {
    case InternalEventType::RaycastHit:
      // Something got hit.  Check to see if it is a text note, i.e. either
      // the anchor, leader line, or the textedit.
      // If one of those are hit, highlight all of them, get the note number,and transition to
      // AnchorTextSelectedState.

      // Then, get a pointer to the AnchorTextSelectedState and set the number of the
      // note to be deleted.
      // i.e. sm_.TransitionTo("AnchorTextSelectedState");
      // auto *selectedState = sm_.GetCurrentState();
      // static_cast<AnchorTextSelectedState>(selectedState)->noteNumber_ = noteNumber;
      break;
    default:
    break;
  };
}

STATE_DECLARE(AnchorTextSelectedState)
  uint32_t noteNumber_;
};
void AnchorTextSelectedState::HandleInternalEvent(InternalEvent internalEvent)
{
  switch (internalEvent.type_)
  {
    case InternalEventType::RaycastHit:
      // We continue to hit the text.  Do nothing.
      break;
    case InternalEventType::TriggerClicked:
      // User decided this note may be deleted.
      // Pop up the delete node dialog and if confirmed,
      // delete the note and sm_.TransitionTo("IdleState");
      // Otherwise, stay in this state.
      sm_.TransitionTo("IdleState");

      // Invalidate the noteNumber.
      noteNumber_ = -1;
      break;
    case InternalEventType::HomeClicked:
      // Dehighlight the text and move back to idle.
      sm_.TransitionTo("IdleState");

      // Invalidate the noteNumber.
      noteNumber_ = -1;
      break;
    default:
      // We are no longer hitting the text note.
      // Unhighlight and transition back to SelectAnchorTextState so another
      // text note (or the same) can be selected again.
      sm_.TransitionTo("SelectAnchorTextState");
      break;
  };
}
// End of states

void TextNoteStateMachine::Init()
{
  std::shared_ptr<IdleState> idleState = std::make_shared<IdleState>(*this);
  AddState(idleState);
  std::shared_ptr<CreateAnchorMovingState> createAnchortMovingState = std::make_shared<CreateAnchorMovingState>(*this);
  AddState(createAnchortMovingState);
  std::shared_ptr<CreateTextMovingState> createTextMovingState = std::make_shared<CreateTextMovingState>(*this);
  AddState(createTextMovingState);
  std::shared_ptr<SelectAnchorTextState> selectAnchorTextState = std::make_shared<SelectAnchorTextState>(*this);
  AddState(selectAnchorTextState);
  std::shared_ptr<AnchorTextSelectedState> anchorTextSelectedState = std::make_shared<AnchorTextSelectedState>(*this);
  AddState(anchorTextSelectedState);

  TransitionTo("IdleState");
}