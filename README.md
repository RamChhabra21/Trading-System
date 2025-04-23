# Trading-System

docker container can be built with the command : docker build -t your_container_name . 

docker container can be run with the command : docker run -v your_path_to_app -it your_container_name bash

for eg :  path to the application (your_path_to_app)

after running the docker container all the required dependencies mentioned in the Dockerfile will be installed 

to build a cpp file use command :

for latency.cpp

clang++ -o latency latency.cpp ./src/*.cpp -I/usr/local/include/websocketpp -I headers -lcurl -ljsoncpp -lpthread -lssl -lcrypto   

for test.cpp

clang++ -o test test.cpp ./src/*.cpp -I/usr/local/include/websocketpp -I headers -lcurl -ljsoncpp -lpthread -lssl -lcrypto   

this ensures that the hpp files included in these files are also built together 

to run the executable file

./test (for test.cpp)
or ./latency (for latency.cpp)
