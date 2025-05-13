# Building the project

```
cmake -S . -B build
```
```
cmake --build build
```
---
## Project Description
Really bad project that uses spatial grid for collision detection and SDL3 to render everything.
Handles about 500 objects with about 60FPS (pls help)
---
| Keybind   | Description       |
| ------------- |:-------------:| 
| F     | Spawns objects near the center of the container  | 
| C     | Prints debug text (Frametime and object count)   | 
| S     | Step one frame (buggy)                           |  
| Space | Pause                                            |  
