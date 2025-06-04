FROM ubuntu:22.04

# Set noninteractive installation
ENV DEBIAN_FRONTEND=noninteractive

# Install basic build tools
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    make \
    git \
    cmake \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Install Qt dependencies for Ubuntu 22.04
# Note: qt5-default package is not available in Ubuntu 22.04
RUN apt-get update && apt-get install -y \
    qtbase5-dev \
    qtbase5-private-dev \
    qt5-qmake \
    qtchooser \
    libqt5core5a \
    libqt5gui5 \
    libqt5widgets5 \
    libqt5network5 \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Install boost dependencies
RUN apt-get update && apt-get install -y \
    libboost-all-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Set Qt environment variables
ENV QT_SELECT=qt5

# Copy source code
COPY . /app
WORKDIR /app

# Build the project
RUN mkdir -p build && cd build && qmake .. && make

# If test executable exists, run tests
CMD cd build-qmake-all/tests && if [ -f qtasio_test ]; then ./qtasio_test; elif [ -f ../tests/qtasio_test ]; then ../tests/qtasio_test; else echo "Tests not found, build completed successfully"; fi