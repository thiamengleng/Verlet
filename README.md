# Building the project
### NOTE: You need GCC or CLANG to compile or replace target_compile_options flag with something that suits your C++ compiler 
```
git pull github.com/thiaengleng/Verlet.git --recursive && cd ./Verlet
```
```
cmake -S . -B build
```
```
cmake --build build
```
---
## Project Description
physics engine
---
| Keybind   | Description       |
| ------------- |:-------------:| 
| F     | Spawns objects near the center of the container  | 
| C     | Prints debug text (Frametime and object count)   | 
| S     | Step one frame (buggy)                           |  
| Space | Pause                                            |  
