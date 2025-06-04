cmake_minimum_required(VERSION 3.14)

# Create example executable
add_executable(qtasio_basic_example
    main.cpp
)

# Link with required libraries
target_link_libraries(qtasio_basic_example
    PRIVATE
        qtasio
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

# Set output directory
set_target_properties(qtasio_basic_example
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)