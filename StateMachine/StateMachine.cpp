#include "StateMachine.h"

std::shared_ptr<State> StateMachine::GetCurrentState() const {
  if (!StateExists(currentState_)) return nullptr;
  return states_.at(currentState_);
}

// Transition to new state if it exists.
void StateMachine::TransitionTo(const std::string& newState) {
  if (!StateExists(newState)) return;
  currentState_ = newState;
}

void StateMachine::AddState(std::shared_ptr<State> state) {
  std::string name = state->getName();
  states_[name] = state;
}

std::string State::getName() {
  return name_;
}

void StateMachine::HandleExternalEvent(ExternalEvent *event)
{
  if (states_.empty() || !StateExists(currentState_))
    return;

  InternalEvent internalEvent;
  internalEvent.type_ = InternalEventType::NoOp;
  switch (event->type_)
  {
  case 0:
    internalEvent.type_ = InternalEventType::NoOp;
    break;
  case 1:
    internalEvent.type_ = InternalEventType::TriggerClicked;
    break;
  case 2:
    internalEvent.type_ = InternalEventType::HomeClicked;
    break;
  case 3:
    internalEvent.type_ = InternalEventType::ControllerMoved;
    break;
  case 4:
    internalEvent.type_ = InternalEventType::RaycastHit;
    internalEvent.data_ = event->data_;
    break;
  };
  states_[currentState_]->HandleInternalEvent(internalEvent);
}

bool StateMachine::StateExists(const std::string &stateName) const {
  if (states_.count(stateName) == 0)
  {
    std::cout << "Error: State " << "\"" << stateName << "\"" << " being transitioned to does not exist." << std::endl;
    return false;
  }
  return true;
}