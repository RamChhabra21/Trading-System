FROM ubuntu:20.04

# Update apps on the base image
RUN apt-get -y update && apt-get install -y

# Install the Clang compiler
RUN apt-get -y install clang

# Install required libraries for WebSocket++ and your dependencies
RUN apt-get install -y libcurl4-openssl-dev
RUN apt-get install -y curl
RUN apt-get install -y libjsoncpp-dev
RUN apt-get install -y libssl-dev
RUN apt-get install -y libasio-dev  # Asio library for WebSocket++
RUN apt-get install -y libpthread-stubs0-dev
RUN apt-get install -y zlib1g-dev
RUN apt-get install -y wget
RUN apt-get install -y git
RUN apt-get install -y cmake
RUN apt-get install -y make

# Clone WebSocket++ from GitHub into the correct path for easy access
RUN git clone https://github.com/zaphoyd/websocketpp.git /usr/local/include/websocketpp

# Download nlohmann::json header-only library
RUN wget https://github.com/nlohmann/json/releases/download/v3.10.5/json.hpp -P /usr/local/include/nlohmann/

# Copy your project files into the container
COPY . .

# Set the working directory to /app
WORKDIR /app
