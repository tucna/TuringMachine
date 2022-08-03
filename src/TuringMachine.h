#pragma once

#include "engine/tPixelGameEngine.h"
#include "nlohmann/json.hpp"

#include <array>
#include <map>
#include <string>

class TuringMachine : public tDX::PixelGameEngine
{
public:
  TuringMachine();
  ~TuringMachine();

  // tDX create
  bool OnUserCreate() override;
  // tDX update
  bool OnUserUpdate(float fElapsedTime) override;
  // tDX end frame - used for ImGui
  bool OnUserUpdateEndFrame(float fElapsedTime) override;
  // tDX destroy
  bool OnUserDestroy() override;

private:
  enum class Value
  {
    Blank,
    Zero,
    One
  };

  enum class Movement
  {
    Left,
    Right
  };

  struct Cell
  {
    bool active;
    Value value;
  };

  struct FunctionHead
  {
    char state;
    Value value;

    bool operator<(const FunctionHead &o) const
    {
      return state < o.state || (state == o.state && value < o.value);
    };
  };

  struct FunctionBody
  {
    char newState;
    Value newValue;
    Movement moveHead;
  };

  std::array<Cell, 20> m_tape;

  std::string m_programName;
  std::string m_states;

  std::map<FunctionHead, FunctionBody> m_deltas;

  uint8_t m_headPos = 0;

  bool m_showUI = false;
};