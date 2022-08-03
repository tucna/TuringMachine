#include "TuringMachine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  TuringMachine machine;

  if (machine.Construct(420, 300, 0, 0, 2, 2))
    machine.Start();

  return 0;
}