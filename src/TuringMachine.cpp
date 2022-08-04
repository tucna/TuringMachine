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
      head.value = CharToValue(string(f[0].at(1))[0]);

      FunctionBody body;
      body.newState = string(f[1].at(0))[0];
      body.newValue = CharToValue(string(f[1].at(1))[0]);
      body.moveHead = CharToMovement(string(f[1].at(2))[0]);

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
      m_tape[tapeStart++].value = CharToValue(ch[0]);
  }

  ReadInstruction();

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
  static constexpr uint8_t cellWidth = 17;
  static constexpr uint8_t cellHeight = 20;
  static const tDX::Pixel cellColor = tDX::DARK_BLUE;
  static const tDX::Pixel activeCellColor = tDX::DARK_RED;
  static const uint16_t tapeDrawX = (ScreenWidth() - m_tape.size() * cellWidth) / 2;
  static const uint16_t tapeDrawY = ScreenHeight() - cellHeight - 50;

  Clear(tDX::BLACK);

  DrawString(15, 15, "Program");
  DrawString(22, 28, m_programName, tDX::DARK_GREY);

  DrawString(15, 50, "States");
  DrawString(22, 63, m_states, tDX::DARK_GREY);

  DrawString(15, 180, "Current state");
  DrawString(22, 193, string(1, m_currentState), tDX::DARK_GREY);

  string functionStr = "d(" + string(1, m_currentHead.state) + ", " + ValueToStr(m_currentHead.value) + ") = (" +
    string(1, m_currentBody.newState) + ", " + ValueToStr(m_currentBody.newValue) + ", " + MovementToStr(m_currentBody.moveHead) + ")";

  DrawString(180, 180, "Current transition function");
  DrawString(187, 193, functionStr, tDX::DARK_GREY);

  m_tape[m_headPos].active = true;

  for (uint8_t id = 0; id < m_tape.size(); id++)
  {
    const Cell& cell = m_tape[id];
    const tDX::Pixel& fillColor = cell.active ? activeCellColor : cellColor;

    FillRect(tapeDrawX + (cellWidth * id) + 1, tapeDrawY + 1, cellWidth - 1, cellHeight - 1, fillColor);
    DrawRect(tapeDrawX + (cellWidth * id), tapeDrawY, cellWidth, cellHeight, tDX::DARK_GREY);
    DrawString(tapeDrawX + (cellWidth * id) + 5, tapeDrawY + 10, ValueToStr(cell.value));
  }

  m_tape[m_headPos].active = false;

  if (GetKey(tDX::Key::F1).bPressed) m_showUI = !m_showUI;

  if (GetKey(tDX::Key::SPACE).bPressed && m_currentBody.newState != 'H')
  {
    ExecuteInstruction();
    ReadInstruction();
  }
  else if (m_currentBody.newState == 'H')
  {
    DrawString(193, 120, "HALT", tDX::DARK_GREEN);
    DrawRect(130, 105, 156, 36);
  }
  else
  {
    DrawString(145, 120, "SPACEBAR to step");
    DrawRect(130, 105, 156, 36);
  }

  const uint16_t headPointPos = tapeDrawX + (cellWidth * m_headPos) + (cellWidth / 2);

  FillTriangle(headPointPos - 2, tapeDrawY + cellHeight + 13, headPointPos, tapeDrawY + cellHeight + 5, headPointPos + 2, tapeDrawY + cellHeight + 13, activeCellColor);

  return true;
}

bool TuringMachine::OnUserUpdateEndFrame(float fElapsedTime)
{
  if (!m_showUI)
    return true;

  // Start the Dear ImGui frame
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Transition functions");

  char lastState = m_deltas.begin()->first.state;

  for (const auto& f : m_deltas)
  {
    if (lastState != f.first.state)
    {
      lastState = f.first.state;
      ImGui::Separator();
    }

    string line = "d(" + string(1, f.first.state) + ", " + ValueToStr(f.first.value) + ") = (" +
      string(1, f.second.newState) + ", " + ValueToStr(f.second.newValue) + ", " + MovementToStr(f.second.moveHead) + ")";

    ImGui::Text(line.c_str());
  }

  ImGui::End();

  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

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

void TuringMachine::ExecuteInstruction()
{
  m_tape[m_headPos].value = m_currentBody.newValue;
  m_currentState = m_currentBody.newState;

  if (m_currentBody.moveHead == Movement::Right)
    m_headPos++;
  else
    m_headPos--;
}

void TuringMachine::ReadInstruction()
{
  m_currentHead.state = m_currentState;
  m_currentHead.value = m_tape[m_headPos].value;

  m_currentBody = m_deltas[m_currentHead];
}

std::string TuringMachine::ValueToStr(const Value& value)
{
  string result;

  switch (value)
  {
    case Value::Blank: result = '_'; break;
    case Value::Zero:  result = '0'; break;
    case Value::One:   result = '1'; break;
  };

  return result;
}

std::string TuringMachine::MovementToStr(const Movement& movement)
{
  string result;

  switch (m_currentBody.moveHead)
  {
    case Movement::Left:  result = 'L'; break;
    case Movement::Right: result = 'R'; break;
  };

  return result;
}

TuringMachine::Value TuringMachine::CharToValue(char state)
{
  Value result;

  switch (state)
  {
    case '0': result = Value::Zero; break;
    case '1': result = Value::One; break;
    case '_': result = Value::Blank; break;
  };

  return result;
}

TuringMachine::Movement TuringMachine::CharToMovement(char movement)
{
  Movement result;

  switch (movement)
  {
    case 'R': result = Movement::Right; break;
    case 'L': result = Movement::Left; break;
  };

  return result;
}
