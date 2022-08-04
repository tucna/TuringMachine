# Turing machine
Program to process and visualize Turing machine.

<p align="center">  
  <img src="doc/3.jpg">
</p>

The project is built on [tPixelGameEngine](https://github.com/tucna/tPixelGameEngine) and [Dear ImGui](https://github.com/ocornut/imgui) for debug purposes. The debug window shows all delta functions.

# Controls
- `Space` - step.
- `F1` - debug window.

# Features
Simulation supports step by step program processing fetched via JSON files. You can see visualization of tape, head and transition function behavior.
<p align="center">  
  <img src="doc/steps.gif" width="400px">
</p>

# Data input
```
{
    "function": "Addition of binary numbers.",
    "states": ["0", "1", "2", "3", "4", "5", "H"],
    "start_state": "0",
    "delta": [
        [["0", "0"], ["0", "0", "R"]],
        [["0", "1"], ["0", "1", "R"]],
        [["0", "_"], ["1", "_", "R"]],
        [["1", "0"], ["1", "0", "R"]],
        [["1", "1"], ["1", "1", "R"]],
        [["1", "_"], ["2", "_", "L"]],
        [["2", "0"], ["2", "1", "L"]],
        [["2", "1"], ["3", "0", "L"]],
        [["2", "_"], ["5", "_", "R"]],
        [["3", "0"], ["3", "0", "L"]],
        [["3", "1"], ["3", "1", "L"]],
        [["3", "_"], ["4", "_", "L"]],
        [["4", "0"], ["0", "1", "R"]],
        [["4", "1"], ["4", "0", "L"]],
        [["4", "_"], ["0", "1", "R"]],
        [["5", "1"], ["5", "_", "R"]],
        [["5", "_"], ["H", "_", "L"]]
    ]
}
```

# Screenshots
<p align="center">  
  <img src="doc/1.jpg" width="400px">&nbsp; &nbsp; <img src="doc/2.jpg" width="400px">
</p>
