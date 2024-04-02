#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <future>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include "pokemon.h"
#include "battle.h"
#include "tr_ai.h"

// test comment
struct Range {
    unsigned long start;
    unsigned long stop;
};

struct BattleReport {
    bool playerWin;
    int playerHp;
};

struct SimulationReport {
    unsigned long startingSeed;
    bool playerWin;
    int playerHp;
};
struct CommandList {
    int defaultCommand;
    int commands[32];
};


BattleReport simulate(unsigned long startingSeed, CommandList cList) {
    BattleContext bc = setupVarFight(startingSeed);
    // BattleContext bc = setupJupiterFight(startingSeed);

    // this command doesn't matter, it gets replaced later
    bc.attacker.command = COMMAND_MOVE_SLOT_2; // attacker is us
    bc.defender.command = COMMAND_MOVE_SLOT_2; // defender is them

    // idk why but the seed advances 2 here
    for(int i = 0; i < 2; i++){
        advanceSeed(&bc);
    }
    processAI(&bc); // calculate which command to use on first turn
    bool shouldContinue = true;
    int i = 0;
    int command = 0;

    while(shouldContinue) {
        command = cList.commands[i];
        if(command == 0) {
            command = cList.defaultCommand;
        }
        if(bc.attacker.aiControl){
            bc.defender.command = command;
        } else {
            bc.attacker.command = command;
        }
        shouldContinue = doTurn(&bc);
        i++;
        if(i > 31) {
            shouldContinue = false;
        }
    }
    BattleReport br;
    bool didPlayerWin = true;
    if(bc.attacker.isWinner && bc.attacker.aiControl) {
        didPlayerWin = false;
    } else if(bc.defender.isWinner && bc.defender.aiControl) {
        didPlayerWin = false;
    } else if(!bc.attacker.isWinner && !bc.defender.isWinner) {
        didPlayerWin = false; // somehow the fight didn't finish
    } else if(bc.attacker.isWinner && !bc.attacker.aiControl) {
        br.playerHp = bc.attacker.team[bc.attacker.battler].bVal.bHp;
    } else {
        br.playerHp = bc.defender.team[bc.defender.battler].bVal.bHp;
    }
    br.playerWin = didPlayerWin;
    // std::cout << bc.defender.team[bc->defender.battler].bVal.bHp << std::endl;
    return br;
};
SimulationReport runSimulations(Range r, CommandList cList) {
    unsigned long i;
    SimulationReport sr = {0,true,1000};
    BattleReport br;
    for(i=r.start;i<r.stop;i++) {
        br = simulate(i, cList);
        if(!br.playerWin){
            sr.startingSeed = i;
            sr.playerWin = br.playerWin;
            return sr;
        } else {
            if(br.playerHp < sr.playerHp){
                sr.playerHp = br.playerHp;
                sr.startingSeed = i;
            }
        }
    }
    return sr;
}

int main(int argc, char* argv[]) {
    int i;
    std::string arg;
    for (i = 0; i < argc; i++) {
        arg = argv[i];
        std::cout << arg << std::endl;
    }

    // return 0;
    SimulationReport val;
    std::cout<<"<booting>"<<std::endl;
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    auto t1 = high_resolution_clock::now();
    // unsigned long end = 4294967295UL;
    unsigned long end = 10000UL;

    int divisor = 16; // how many chunks to use
    int offset = 0; // how far to offset
    unsigned long chunkSize = end/divisor;
    std::vector<Range> ranges;
    CommandList cList = {};
    for(i=0;i<31;i++) {
        cList.commands[i] = 0;
    }
    cList.defaultCommand = COMMAND_MOVE_SLOT_2;
    // cList.commands[0] = COMMAND_MOVE_SLOT_3;
    // cList.commands[1] = COMMAND_MOVE_SLOT_3;
    // cList.commands[2] = COMMAND_MOVE_SLOT_3;
    // cList.commands[1] = COMMAND_USE_ITEM_GUARD_SPEC;
    cList.commands[3] = COMMAND_USE_ITEM_HYPER_POTION;
    cList.commands[6] = COMMAND_USE_ITEM_HYPER_POTION;
    // cList.commands[9] = COMMAND_USE_ITEM_HYPER_POTION;

    // simulate a specific seed
    // simulate(1758469, cList);
    // return 0;
    for(i = 0; i < divisor; i++){
        ranges.push_back({i * chunkSize, (i+1) * chunkSize});
    }
    std::vector<std::future<SimulationReport>> futures;
    for(auto &e : ranges){
        futures.push_back(std::async(runSimulations, e, cList));
    }
    std::ofstream myfile;
    myfile.open("./simulatorResults.txt");
    for(auto &e : futures) {
        val = e.get();
        if(!val.playerWin){
            myfile << val.startingSeed << "\n";
            std::cout << val.startingSeed << "\n";
        } else {
            myfile << "Player won" << "\n";
            std::cout << "Player won. Lowest hp -> " << val.playerHp << " // Seed: " << val.startingSeed << std::endl;
        }
    }
    auto t2 = high_resolution_clock::now();
     /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    myfile << ms_int.count() << "ms\n";
    myfile << ms_double.count() << "ms\n";
    myfile.close();
    return 0;
}