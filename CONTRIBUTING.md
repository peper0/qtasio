# Contributing to qtasio

Thank you for considering contributing to qtasio! This document provides guidelines and instructions for contributing to this project.

## Development Environment Setup

### Prerequisites

- Qt 5.x (5.3 or higher recommended)
- Boost (for boost::asio)
- GLib 2.0
- C++11 compatible compiler

### Building the Project

This project uses qmake as its build system. To build the project:

```bash
# Clone the repository
git clone https://github.com/yourusername/qtasio.git
cd qtasio

# Create a build directory
mkdir build
cd build

# Run qmake and make
qmake ../qtasio.pro
make
```

### Project Structure

- `src/` - Contains the library source code
- `include/qtasio/` - Public header files
- `tests/` - Test cases for the library
- `examples/` - Example applications demonstrating usage

## Testing

Run the tests after building:

```bash
cd build/tests
./qtasio_test
```

## Pull Request Process

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Run the tests to ensure they pass
5. Commit your changes (`git commit -m 'Add some amazing feature'`)
6. Push to the branch (`git push origin feature/amazing-feature`)
7. Open a Pull Request

## Coding Style

- Use camelCase for variable and function names
- Use PascalCase for class names
- Add comments for complex logic
- Follow Qt's coding style for consistency

## License

By contributing, you agree that your contributions will be licensed under the project's license.