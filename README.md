# Building the project
Really bad project that uses O(n^2) collision detection and SDL3 to render everything.
Handles about 500 objects with about 50FPS
---
```
cmake -S . -B build
```
```
cmake --build build
```
---
| Keybind   | Description       |
| ------------- |:-------------:| 
| F     | Spawns objects near the center of the container  | 
| C     | Prints debug text (Frametime and object count)   | 
| S     | Step one frame (buggy)                           |  
| Space | Pause                                            |  
