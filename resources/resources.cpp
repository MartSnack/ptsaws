#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <future>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
int bingo() {
    int b = 0;
    for(int i=0;i<1000000000;i++){
        b++;
    }
    return b;
}
int bongo() {
    int b = 0;
    for(int i=0;i<10000000000;i++){
        b++;
    }
    return b;
}
int main(int argc, char* argv[]) {
    int i;
    std::string arg;
    int j;
    for (i = 0; i < argc; i++) {
        arg = argv[i];
        std::cout << arg << std::endl;
        if(arg == "-r") {
            j = std::atoi(argv[i+1]);
            std::cout << j * 7 << std::endl;

        }
    }

    return 0;
}