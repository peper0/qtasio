# QtAsio

[![Build and Test](https://github.com/peper0/qtasio/actions/workflows/build.yml/badge.svg)](https://github.com/peper0/qtasio/actions/workflows/build.yml)

QtAsio is a library that integrates Qt's event loop with Boost.Asio's io_service, allowing both frameworks to work together seamlessly.

## Features

- Integrates Qt's event loop with Boost.Asio
- Handles timers, socket notifications, and other events
- Compatible with multiple Qt versions (5.9 through 5.15)
- Allows you to use Boost.Asio's asynchronous I/O within a Qt application

## Requirements

- C++14 compatible compiler
- Qt 5.9 or later
- Boost 1.66.0 or later (with Asio)
- CMake 3.14 or later

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Running tests

```bash
cd build
ctest
```

## Installation

```bash
cd build
make install
```

## Usage

After installation, you can use QtAsio in your CMake project:

```cmake
find_package(qtasio REQUIRED)
target_link_libraries(your_target PRIVATE qtasio::qtasio)
```

In your code:

```cpp
#include <boost/asio/io_service.hpp>
#include <qtasio/qasioeventdispatcher.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    boost::asio::io_service io_service;
    
    // Set QAsioEventDispatcher as Qt's event dispatcher
    QApplication::setEventDispatcher(new QAsioEventDispatcher(io_service));
    
    QApplication app(argc, argv);
    
    // Now you can use both Qt and Boost.Asio
    io_service.post([]() {
        // Asio handler
        qDebug() << "Hello from Asio!";
    });
    
    return app.exec();
}
```

## License

See the [LICENSE](LICENSE) file for license rights and limitations.
