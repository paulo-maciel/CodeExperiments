#pragma once

#include <iostream>
#include <unordered_map>
#include <functional>
#include <memory>

// Note: ExternalEvent is lumin::ServerEvent
// This is a simulation.
struct ExternalEvent
{
  ExternalEvent(uint32_t type)
      : type_(type) {}

  uint32_t type_ = 0;
  // 0, unknown
  // 1, trigger click
  // 2, home click
  // 3, 6dof moved
  // 4, ray hit object (e.g. ServerEventType::kRayCastEvent)
  // The server event, in the case of a kRayCastEvent type, it contains the raycast hit data, i.e. getHitData
  // which is a RaycastNodeResult, which contains the hit node id.
  void* data_ = nullptr;
};

class State;

class StateMachine {
public:
  StateMachine() {}
  ~StateMachine() {}

  // Creates the new states.
  virtual void Init() = 0;

  // Adds a state to the state machine.
  void AddState(std::shared_ptr<State> state);

  // TODO: Replace ExternalEvent with lumin::ServerEvent
  void HandleExternalEvent(ExternalEvent* event);

  // Transition to new state.
  void TransitionTo(const std::string& state);

  // Returns the pointer to the current state.
  std::shared_ptr<State> GetCurrentState() const;

protected:
  // State functions
  std::unordered_map<std::string, std::shared_ptr<State>> states_;
  std::string currentState_;

private:
  bool StateExists(const std::string& stateName) const;
};

#define TOSTRING(ENUM) \
  if (val == ENUM)     \
    return #ENUM;

enum class InternalEventType
{
  NoOp,
  TriggerClicked,
  HomeClicked,
  ControllerMoved,
  RaycastHit,
};

static std::string InternalEventTypeToString(InternalEventType val)
{
  TOSTRING(InternalEventType::NoOp);
  TOSTRING(InternalEventType::TriggerClicked);
  TOSTRING(InternalEventType::HomeClicked);
  TOSTRING(InternalEventType::ControllerMoved);
  return "Undefined";
}

struct InternalEvent {
  InternalEventType type_;
  void* data_ = nullptr;
};

class State
{
public:
  State(StateMachine &sm)
  : sm_(sm)
  , name_("Unnamed") {}

  virtual void HandleInternalEvent(InternalEvent internalEvent) = 0;
  std::string getName();

protected:
  StateMachine &sm_;
  std::string name_;
};

#define STATE_DECLARE(NAME) \
  class NAME : public State \
  {                         \
  public:                   \
    NAME(StateMachine &sm)  \
        : State(sm)         \
    {                       \
      name_ = #NAME;        \
    }                       \
    void HandleInternalEvent(InternalEvent internalEvent) override;
