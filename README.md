<p align="center">
  <a href="https://github.com/Chowen233/LART">
    <img width="200" height="200" alt="logo" src="https://github.com/user-attachments/assets/50f2808a-e449-4490-9dc3-88f67c44d3bd">
  </a>
  <h3 align="center">LART</h3>
<p align="center">
    Linear Algebra-based Ray Tracing
  <br />
  
## About The Project
纯C++编写的光线追踪渲染引擎

<img width="4000" height="4000" alt="img" src="https://github.com/user-attachments/assets/50f2808a-e449-4490-9dc3-88f67c44d3bd">

### Built With
This project is built with following third parties.
* [stb_image](https://github.com/nothings/stb) for image I/O
* [OpenMP](https://github.com/openmp) for multithread rendering
* [OIDN](https://github.com/RenderKit/oidn) for render result denoising

## Getting Started

### Prerequisites

 I build this project on Windows platform. Please make sure your system is equipped with the following softwares.  

- [cmake](https://cmake.org/)：at least version 3.12

* Microsoft visual studio 2026
  

### Installation

Please compile the project for **x64 platform**.

1. Clone the repo
   ```sh
   git clone --recurse-submodules https://github.com/Chowen233/LART.git
   ```
   
2. Use cmake to build the project：
   
   ```
   mkdir build
   cd build
   cmake ..
   make
   ```
   
   or using cmake-gui is ok.

Please make sure that clone the reposity with `--recurse-submodules` option. If you foget to run `git clone` with it, just type the command `git submodule update --init --recursive`.
