#pragma once

#include "StateMachine.h"

#include <iostream>
#include <unordered_map>
#include <functional>
#include <type_traits>

class TextNoteStateMachine : public StateMachine
{
public:
  // Creates the new states.
  void Init() override;

  void CreateTextNote();
  void DeleteTextNote();
};