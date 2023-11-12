project(catchTheChicken)
add_executable(${PROJECT_NAME} camera.cpp ground.cpp main.cpp window.cpp wall.cpp)
enable_abcg(${PROJECT_NAME})
