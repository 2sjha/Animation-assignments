# Assignment 0

## Setup on Manjaro (Any Arch based distro) 

1. Install All dependencies mentioned in the pdf, but in AUR and Arch repositories, they have different names. (I also use `yay` for AUR packages)

  - C++17 (The pdf says 11, but I had a lot of issues compiling with C++11, and after a bit of searching on Stackoverflow. I ended up using C++17)

  - Graphic cards which support OpenGL 3.3+
  
  - CMake 2.8+ (you probably already have this, but check using `cmake --version`)
  
  - glew (`sudo pacman -S glew`)
  
  - glfw/glut/freeglut (`yay glfw3`)
  
  - glm (`sudo pacman -S glm`)
  
  - NanoGUI (`yay nanogui`)
  
  - stb_image (`sudo pacman -S soil`)

2. Compile

  - Running `g++ -o helloworld main.cpp -std=c++17 -lGL -lglfw -lGLEW -lnanogui` will produce some errors.

    - Use `-std=c++17` instead of `c++11`.

    - Use `-lnanogui` as an extra argument in the compile command. It is not in the original PDF but was added later on eLearning.

    - Don't use `sudo` while compiling. It is unnecessary.

  - The majority of the errors will be like this `main.cpp:93:14: error: ‘class nanogui::FormHelper’ has no member named ‘addButton’; did you mean ‘add_button’?`. I don't know the reason, but there are a lot of camelCase function calls that don't work.
    - I changed all such camelCase function calls to snake_case function calls.

  - Another error may be due to an ambiguous class name for the provided shader. `main.cpp:148:9: error: reference to ‘Shader’ is ambiguous`. There are 2 candidates, `class nanogui::Shader` and `class Shader` in the provided `shader.h`.

    - I changed the class name in the provided `shader.h` to `class OurShader`. You'll need to change the constructor as well.
    
    - Of course, the class name needs to be changed from `Shader` to `OurShader` at `main.cpp:148`.
  
  - Another error may be due to the use of Eigen::Vector2i `main.cpp:78:60: error: cannot convert ‘Eigen::Vector2i’ {aka ‘Eigen::Matrix<int, 2, 1>’} to ‘const nanogui::Vector2i&’ {aka ‘const nanogui::Array<int, 2>&’}`
    - I changed `Eigen::Vector2i` to `Array<int, 2>`.

3. Run `./helloworld`