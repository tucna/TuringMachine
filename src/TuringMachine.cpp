#define T_PGE_APPLICATION
#include "engine/tPixelGameEngine.h"

#include <fstream>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "TuringMachine.h"

using namespace std;
using json = nlohmann::json;

TuringMachine::TuringMachine()
{
  sAppName = "Turing machine";

  // Read JSON program
  {
    ifstream f("prg/addition.json");
    json program = json::parse(f);

    m_programName = program["function"];
    m_states = program["states"].dump();

    for (const auto& f : program["delta"])
    {
      FunctionHead head;
      head.state = string(f[0].at(0))[0];

      switch (string(f[0].at(1))[0])
      {
      case '0': head.value = Value::Zero; break;
      case '1': head.value = Value::One; break;
      case '_': head.value = Value::Blank; break;
      };

      FunctionBody body;
      body.newState = string(f[1].at(0))[0];

      switch (string(f[1].at(1))[0])
      {
      case '0': body.newValue = Value::Zero; break;
      case '1': body.newValue = Value::One; break;
      case '_': body.newValue = Value::Blank; break;
      };

      switch (string(f[1].at(2))[0])
      {
      case 'R': body.moveHead = Movement::Right; break;
      case 'L': body.moveHead = Movement::Left; break;
      };

      m_deltas[head] = body;
    }
  }

  // Read JSON data
  {
    ifstream f("dat/twoBinNumbers.json");
    json data = json::parse(f);

    uint8_t tapeStart = data["start_cell"];
    m_headPos = tapeStart;

    for (const string& ch : data["data"])
    {
      char value = ch[0];

      switch (value)
      {
      case '0': m_tape[tapeStart++].value = Value::Zero; break;
      case '1': m_tape[tapeStart++].value = Value::One; break;
      case '_': m_tape[tapeStart++].value = Value::Blank; break;
      };
    }
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
}

TuringMachine::~TuringMachine()
{
}

bool TuringMachine::OnUserCreate()
{
  // Setup Platform/Renderer bindings
  ImGui_ImplWin32_Init(GetHWND());
  ImGui_ImplDX11_Init(GetDevice(), GetContext());

  return true;
}

bool TuringMachine::OnUserUpdate(float fElapsedTime)
{
  static const tDX::Pixel cellColor = tDX::DARK_BLUE;
  static const tDX::Pixel activeCellColor = tDX::DARK_RED;
  static constexpr uint8_t cellWidth = 17;
  static constexpr uint8_t cellHeight = 20;
  static const uint16_t tapeDrawX = (ScreenWidth() - m_tape.size() * cellWidth) / 2;
  static const uint16_t tapeDrawY = ScreenHeight() - cellHeight - 50;

  Clear(tDX::BLACK);

  DrawString(15, 15, "Program");
  DrawString(22, 28, m_programName, tDX::DARK_GREY);

  DrawString(15, 50, "States");
  DrawString(22, 63, m_states, tDX::DARK_GREY);

  DrawString(15, 180, "Current state");
  DrawString(22, 193, string(1, m_currentState), tDX::DARK_GREY);

  DrawString(180, 180, "Current transition function");
  DrawString(187, 193, m_function, tDX::DARK_GREY);

  m_tape[m_headPos].active = true;

  for (uint8_t id = 0; id < m_tape.size(); id++)
  {
    const Cell& cell = m_tape[id];
    const tDX::Pixel& fillColor = cell.active ? activeCellColor : cellColor;

    string ch = "X";

    switch (cell.value)
    {
    case Value::Blank: ch = '_'; break;
    case Value::Zero:  ch = '0'; break;
    case Value::One:   ch = '1'; break;
    };

    FillRect(tapeDrawX + (cellWidth * id) + 1, tapeDrawY + 1, cellWidth - 1, cellHeight - 1, fillColor);
    DrawRect(tapeDrawX + (cellWidth * id), tapeDrawY, cellWidth, cellHeight, tDX::DARK_GREY);
    DrawString(tapeDrawX + (cellWidth * id) + 5, tapeDrawY + 10, ch);
  }

  m_tape[m_headPos].active = false;

  if (GetKey(tDX::Key::RIGHT).bPressed) m_headPos++;
  if (GetKey(tDX::Key::LEFT).bPressed) m_headPos--;
  if (GetKey(tDX::Key::SPACE).bPressed) Step();

  const uint16_t headPointPos = tapeDrawX + (cellWidth * m_headPos) + (cellWidth / 2);

  FillTriangle(headPointPos - 2, tapeDrawY + cellHeight + 13, headPointPos, tapeDrawY + cellHeight + 5, headPointPos + 2, tapeDrawY + cellHeight + 13, activeCellColor);

  return true;
}

bool TuringMachine::OnUserUpdateEndFrame(float fElapsedTime)
{
  if (!m_showUI)
    return true;

  return true;
}

bool TuringMachine::OnUserDestroy()
{
  // Cleanup ImGui
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  return true;
}

void TuringMachine::Step()
{
  FunctionHead head;
  head.state = m_currentState;
  head.value = m_tape[m_headPos].value;

  FunctionBody body = m_deltas[head];

  m_tape[m_headPos].value = body.newValue;
  m_currentState = body.newState;

  if (body.moveHead == Movement::Right)
    m_headPos++;
  else
    m_headPos--;

  char s;

  switch (head.value)
  {
  case Value::Blank: s = '_'; break;
  case Value::Zero:  s = '0'; break;
  case Value::One:   s = '1'; break;
  };

  char ns;

  switch (body.newValue)
  {
  case Value::Blank: ns = '_'; break;
  case Value::Zero:  ns = '0'; break;
  case Value::One:   ns = '1'; break;
  };

  char d;

  switch (body.moveHead)
  {
  case Movement::Left: d = 'L'; break;
  case Movement::Right: d = 'R'; break;
  };

  string functionStr = "d(" + string(1, head.state) + ", " + string(1, s) + ") = (" + string(1, body.newState) + ", " + string(1, ns) + ", " + string(1, d) + ")";

  m_function = functionStr;
}
