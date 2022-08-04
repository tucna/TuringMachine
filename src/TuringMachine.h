#pragma once

#include "engine/tPixelGameEngine.h"
#include "nlohmann/json.hpp"

#include <array>
#include <map>
#include <string>

class TuringMachine : public tDX::PixelGameEngine
{
public:
  // Constructor
  TuringMachine();
  // Destructor
  ~TuringMachine() {}

  // tDX create
  bool OnUserCreate() override;
  // tDX update
  bool OnUserUpdate(float fElapsedTime) override;
  // tDX end frame - used for ImGui
  bool OnUserUpdateEndFrame(float fElapsedTime) override;
  // tDX destroy
  bool OnUserDestroy() override;

private:
  // Possible values what tape cell can have
  enum class Value
  {
    Blank, // _
    Zero,  // 0
    One    // 1
  };

  // Where head should move
  enum class Movement
  {
    Left,
    Right
  };

  // Information about cell
  struct Cell
  {
    // Flag to control colors for debug draw
    bool active;
    // Value of the cell
    Value value;
  };

  // Function declaration "d(X, X) ="
  struct FunctionHead
  {
    // State for which the rule applies
    char state;
    // Value for which the rule applies
    Value value;

    // Operator to allow FunctionHead as a key for std::map
    bool operator<(const FunctionHead &o) const
    {
      return state < o.state || (state == o.state && value < o.value);
    };
  };

  // Function definition "= (X, X, X)"
  struct FunctionBody
  {
    // State which will replace the current one
    char newState;
    // Value which will replace the current one on the tape
    Value newValue;
    // Where the head should move
    Movement moveHead;
  };

  // Read instruction specified by head position
  void ReadInstruction();
  // Execute instruction fetched by ReadInstruction()
  void ExecuteInstruction();
  // Draw visualization of the machine and the process
  void DrawScreen();

  // Convert Value to text
  std::string ValueToStr(const Value& value);
  // Convert Movement to text
  std::string MovementToStr(const Movement& movement);

  // Convert text to Value
  Value CharToValue(char state);
  // Convert text to Movement
  Movement CharToMovement(char movement);

  // Tape for Turing machine (20 elements fixed)
  std::array<Cell, 20> m_tape;

  // Debug string to print name of the program
  std::string m_programName;
  // Debug string to print states of the program
  std::string m_states;

  // Map of all transition functions
  std::map<FunctionHead, FunctionBody> m_deltas;

  // Head of the fetched instruction (map key)
  FunctionHead m_currentHead;
  // Body of the fetched instruction (map value)
  FunctionBody m_currentBody;

  // Position of the head
  uint8_t m_headPos = 0;

  // Current state of the machine
  char m_currentState = '0';

  // Flag to show/hide debug UI
  bool m_showUI = false;
};
