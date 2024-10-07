# CG--project
# 9/11 animation - OpenGL with GLFW

This project is a 2D OpenGL-based animation featuring planes crashing into buildings, with additional background elements like clouds and trees. It uses the GLFW library for handling windows and OpenGL contexts.

## Prerequisites

Before running the project, ensure that the following tools and libraries are installed on your machine:

1. **Visual Studio 2022**
   - Visual Studio 2022 must be installed. You can download it from [here](https://visualstudio.microsoft.com/vs/).
   - Ensure that the **Desktop development with C++** workload is installed with Visual Studio.
   
2. **GLFW**
   - This project uses the `GLFW` library for OpenGL context and window creation.
   - Install `GLFW` via the following steps:
     - Download `GLFW` from the official website: https://www.glfw.org/download.html
     - Follow the instructions provided by GLFW to include it in your Visual Studio project.

3. **OpenGL**
   - OpenGL comes pre-installed with Visual Studio as part of the system graphics drivers, but make sure your machine supports OpenGL.

4. **GLEW (optional)**
   - GLEW may be required for some OpenGL functionalities, though this code primarily uses core OpenGL functions that Visual Studio supports.
   - Download GLEW from: https://glew.sourceforge.net/

## Project Setup in Visual Studio 2022

### Step 1: Create a New Project
1. Open **Visual Studio 2022**.
2. Select `Create a new project`.
3. Choose the template: `Console Application` or `Empty Project` under the `C++` section.
4. Set your project name and location, then click `Create`.

### Step 2: Add Source Code
1. Copy and paste the provided C++ code into a new `.cpp` file inside the `Source Files` folder in your Visual Studio project.

### Step 3: Set Up Libraries (GLFW and OpenGL)

#### Setting Up GLFW
1. Download GLFW: [GLFW Download](https://www.glfw.org/download.html)
2. Unzip the GLFW archive.
3. Copy the **include** and **lib** folders from the unzipped GLFW folder to your project directory.
4. In Visual Studio:
   - Right-click on your project in Solution Explorer and select **Properties**.
   - Under **Configuration Properties** -> **VC++ Directories**:
     - Add the `include` folder path to **Include Directories**.
     - Add the `lib` folder path to **Library Directories**.
   - Under **Linker** -> **Input** -> **Additional Dependencies**, add the following:
     ```
     glfw3.lib
     opengl32.lib
     ```

#### Setting Up GLEW (if required)
1. Download GLEW: [GLEW Download](https://glew.sourceforge.net/)
2. Unzip the GLEW archive.
3. Add GLEW's **include** and **lib** directories to your project as done with GLFW.
4. Under **Linker** -> **Input** -> **Additional Dependencies**, add:

   
### Step 4: Build Configuration
1. Right-click on your project in Solution Explorer and go to **Properties**.
2. Under **Configuration Properties** -> **C/C++** -> **General**, add the `include` directory of GLFW to **Additional Include Directories**.
3. Under **Linker** -> **General**, add the `lib` directory to **Additional Library Directories**.
4. Under **Linker** -> **Input**, add `glfw3.lib`, `opengl32.lib` (and `glew32.lib` if using GLEW) to **Additional Dependencies**.

### Step 5: Build and Run
1. Build the project by selecting **Build** -> **Build Solution** or pressing `Ctrl+Shift+B`.
2. Run the application by pressing `F5` or selecting **Debug** -> **Start Debugging**.

### Step 6: Additional Configuration (if required)
If you encounter any issues with library linkage or include paths, make sure the **Additional Include Directories** and **Additional Library Directories** point correctly to the directories containing GLFW and OpenGL libraries.

## Controls

- The animation automatically plays on start.
- There is no interactive input required for controlling the planes or the environment.
- Once both planes crash, the scene will reset after a short delay.

## Troubleshooting

- **Error: Cannot open GLFW/glfw3.h**  
Make sure that the GLFW include directory is added correctly under **VC++ Directories** -> **Include Directories**.

- **Linking Errors**  
Ensure that the GLFW and OpenGL libraries are correctly linked under **Linker** -> **Input**.

---


