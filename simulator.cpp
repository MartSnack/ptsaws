#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <future>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <filesystem>
#include <iomanip>
#include "pokemon.h"
#include "battle.h"
#include "tr_ai.h"
#include "config.h"
namespace fs = std::filesystem;
unsigned long totalSims = 160000000UL;
// unsigned long totalSims = 4294967295UL;
const int maxTurns = 32;
const int maxBranches = 32;
int parallel = 16;
// test comment
struct Range {
    unsigned long start;
    unsigned long stop;
    int index; // what chunk we're on (0 to maxThreads)
    int chunkSize; // how big the loss tracker chunk is (not the range chunk, which start/stop define)
    bool useSpecifics;
    std::vector<unsigned long> seeds;
};

struct LossLog {
    unsigned long seed;
    int turn;
    bool isLoss;
};

struct BattleReport {
    bool playerWin;
    int playerHp;
    int endTurn;
};

struct SimulationReport {
    unsigned long startingSeed; // the starting seed for the returned result (either the first seed we lose on, or the seed we have the lowest hp on if we win all of them)
    bool playerWin;
    int playerHp;
    unsigned long turnTracker[maxTurns]; // max_turns
};
struct CommandList {
    unsigned long defaultCommand;
    unsigned long commands[maxTurns][maxBranches]; // max_turns
};
unsigned long parseCommand(BattleContext *bc, PokeClient *pc, unsigned long commands[maxBranches]) {
    int com = 0;
    bool branched = true;
    while(branched) {
        com = commands[bc->branch];
        if (com & COMMAND_BRANCH_INC) {
            bc->branch++;
            com &= ~COMMAND_BRANCH_INC;
        } else if(com & COMMAND_BRANCH_DEC) {
            bc->branch--;
            com &= ~COMMAND_BRANCH_DEC;
        }
        branched = false; // default to breaking while loop
        switch(com) {
            case COMMAND_USE_GATEAU_OR_BITE:
                if(pc->team[pc->battler].bVal.condition > 0 || pc->team[pc->battler].bVal.volConditions & VOLATILE_CONDITION_CONFUSION) {
                    com = COMMAND_USE_ITEM_FULL_HEAL;
                } else {
                    com = COMMAND_MOVE_SLOT_2;
                }
                break;
            case COMMAND_USE_HYPER_OR_BITE:
                if(pc->team[pc->battler].bVal.bHp < pc->team[pc->battler].cHp) {
                    com = COMMAND_USE_ITEM_HYPER_POTION;
                } else {
                    com = COMMAND_MOVE_SLOT_2;
                }
                break;
            case COMMAND_USE_ITEM_FULL_HEAL:
                if(!pc->team[pc->battler].bVal.condition > 0 && !pc->team[pc->battler].bVal.volConditions & VOLATILE_CONDITION_CONFUSION) {
                    // full heal will have no use, branch
                    branched = true;
                    bc->branch++;
                }
                break;
            case COMMAND_USE_ITEM_HYPER_POTION:
                if(pc->team[pc->battler].bVal.bHp == pc->team[pc->battler].cHp) {
                    // hp is max, branch
                    branched = true;
                    bc->branch++;
                }
                break;
            default:
                break;
        }
    }
    return com;
}
void getCommand(BattleContext *bc, CommandList cList, int turnNumber) {
    int com = 0;
    if(bc->attacker.aiControl) {
        com = parseCommand(bc, &bc->defender, cList.commands[turnNumber]);
        if(com == 0) {
            com = cList.defaultCommand;
        }
        bc->defender.command = com;
    } else {
        com = parseCommand(bc, &bc->attacker, cList.commands[turnNumber]);
        if(com == 0) {
            com = cList.defaultCommand;
        }
        bc->attacker.command = com;
    }
}

BattleReport simulate(unsigned long startingSeed, CommandList cList, SimulationReport *sr) {
    BattleContext bc = setupVarFight(startingSeed);
    bc.terminate = false;
    bc.branch = 0;
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
    while(shouldContinue) {
        sr->turnTracker[i]++; // log this turn in the report
        getCommand(&bc, cList, i);
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
        br.playerHp = bc.defender.team[bc.defender.battler].bVal.bHp;
    } else {
        br.playerHp = bc.attacker.team[bc.attacker.battler].bVal.bHp;
    }
    br.playerWin = didPlayerWin;
    br.endTurn = i;
    // std::cout << bc.defender.team[bc->defender.battler].bVal.bHp << std::endl;
    return br;
};
SimulationReport runSimulations(Range r, CommandList cList, std::vector<LossLog>& losses ) {
    unsigned long i;
    unsigned long j = 0; // tracking loss array
    SimulationReport sr = {0,true,1000};
    for(int m = 0; m < 32; m++) { // max_turn
        sr.turnTracker[m] = 0; // init turn tracker
    }
    BattleReport br;
    unsigned long tempSeed = 0;
    for(i=r.start;i<r.stop;i++) {
        if(r.useSpecifics) {
            tempSeed = r.seeds[i];
            br = simulate(tempSeed, cList, &sr);
        } else {
            tempSeed = i;
            br = simulate(tempSeed, cList, &sr);
        }
        if(!br.playerWin){
            losses[j + r.index * r.chunkSize] = {tempSeed, br.endTurn, !br.playerWin};
            j++;
            sr.startingSeed = tempSeed;
            sr.playerHp = 0; // loss!
            sr.playerWin = br.playerWin;

        } else {
            if(WATCH_TURN > 0) {
                // watch all seeds past specified turn.
                if(br.endTurn >= WATCH_TURN) {
                    losses[j + r.index * r.chunkSize] = {tempSeed, br.endTurn, !br.playerWin};
                    j++;
                }
            }
            if(br.playerHp < sr.playerHp){
                sr.playerHp = br.playerHp;
                sr.startingSeed = tempSeed;
            }
        }
        if(EXIT_EARLY || (j >= (r.chunkSize - 1))) {
            if(EXIT_EARLY) {
                std::cout << "Exiting thread " << r.index << " early [ had " << r.stop - i << " sims left ]" << std::endl;
            } else {
                std::ofstream errorFile("errors.log");
                errorFile << "Exited thread " << r.index << " early due to overfilling seed array" << std::endl;
                errorFile.close();
            }
            return sr;
        }
    }
    return sr;
}
std::vector<unsigned long> getSpecificSeeds(std::string fileName) {
    // Open the input file
    std::ifstream inputFile(fileName);

    // Check if the file opened successfully
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the input file." << std::endl;
    } {
        // Vector to store integers
        std::vector<unsigned long> numbers;

        // Read each line from the file
        std::string line;
        while (std::getline(inputFile, line)) {
            // Create a stringstream from the line
            std::stringstream ss(line);
            
            // Temporary variable to store each integer
            unsigned long num;

            // Read integers separated by commas
            while (ss >> num) {
                // Push the integer into the vector
                numbers.push_back(num);

                // Check for a comma, ignore it
                if (ss.peek() == ',')
                    ss.ignore();
            }
        }

        // Close the input file
        inputFile.close();
        numbers.resize(numbers.size());
        return numbers;
    }

}
std::vector<unsigned long> getSubVector(const std::vector<unsigned long>& vec, size_t startIndex, size_t endIndex) {
    // Check if the indices are valid
    if (startIndex >= vec.size() || endIndex >= vec.size() || startIndex > endIndex) {
        throw std::out_of_range("Invalid start or end index");
    }

    // Create a sub-vector using iterators
    return std::vector<unsigned long>(vec.begin() + startIndex, vec.begin() + endIndex + 1);
}

std::string parseCommandString(int cmd) {
    switch(cmd){
        case COMMAND_MOVE_SLOT_1:
            return "M1";
        case COMMAND_MOVE_SLOT_2:
            return "M2";
        case COMMAND_MOVE_SLOT_3:
            return "M3";
        case COMMAND_MOVE_SLOT_4:
            return "M4";
        case COMMAND_USE_GATEAU_OR_BITE:
            return "FH";
        case COMMAND_USE_HYPER_OR_BITE:
            return "SP";
        default:
            return "na";
    }
    return "??";
}
int main(int argc, char* argv[]) {
    DEBUG = 0;
    EXIT_EARLY = 0;
    int i;
    int j;
    std::string arg;
    std::string outputDirName = "default";
    std::string inputSeeds = "seeds.csv";
    unsigned long simCount = 0;
    int threads = 0;
    bool runSpecificSeeds = false;
    bool runIndividualSeed = false;
    int individualSeed = 0;
    for (i = 0; i < argc; i++) {
        arg = argv[i];
        // example command: `output.exe -s 16000000 -t 16 -w 7`
        // will simulate 16 million seeds (1-16mil) on 16 threads and log any seeds that make it to turn 7
        if(arg == "-s") {
            // how many seeds to run, will run from 0 to N where N is the number after -s
            // eg. `output.exe -s 10000` will run 10000 simulations of seeds 0-9999
            char* endPtr;
            simCount = std::strtoul(argv[i+1], &endPtr, 10);
        }
        if(arg == "-t") {
            // how many threads to use (equal to CPU cores to run with 100% cpu power)
            // eg. `output.exe -t 16` will use 16 threads
            char* endPtr;
            threads = std::strtol(argv[i+1], &endPtr, 10);
        }
        if(arg == "-l") {
            // run all the seeds from seeds.csv
            // not compatible with -s
            // eg. `output.exe -l` will run the simulation only with seeds from seeds.csv
            runSpecificSeeds = true;
        }
        if(arg == "-i") {
            // use input csv file for seeds
            runSpecificSeeds = true;
            inputSeeds = argv[i+1];
        }
        if(arg == "-d") {
            // debug options
            // 1 - basic info
            // 3 - advanced info
            // 7 - dump everything
            // this is not a gradient, 2/4/5/6 do not produce variable amounts of debug output
            // not recommended for sims of more than 1 seed
            // eg. `output.exe -d 3` will show debug information in the console
            char* endPtr;
            DEBUG = std::strtol(argv[i+1], &endPtr, 10);
        }
        if(arg == "-z") {
            // run a specific seed by itself
            char* endPtr;
            individualSeed = std::strtol(argv[i+1], &endPtr, 10);
            runIndividualSeed = true;
            if(DEBUG == 0) {
                DEBUG = 1; // automatically set the debug level to 1
            }
        }
        if(arg == "-e") {
            // exit early, will cause threads to terminate on the first seed they encounter that loses
            // prevents use of excess computer power during research phase
            // when running a full simulation (all 4 billion seeds) better not to use this
            // we want to find ALL losing seeds for a given strategy to see if we can handle them
            // eg. `output.exe -e` will enable early exiting
            EXIT_EARLY = 1;
        }
        if(arg == "-w") {
            // watch seeds that get to turn N, where N is specified number
            // eg. `output.exe -w 5` will log all seeds that make it to turn 5 in the results
            char* endPtr;
            WATCH_TURN = std::strtol(argv[i+1], &endPtr, 10);
        }
        if(arg == "-o"){
            // name output directory
            outputDirName = argv[i+1];
        }
    }
    if(simCount == 0) {
        simCount = totalSims;
    }
    if(threads == 0){
        threads = parallel;
    }

    // return 0;
    SimulationReport val;
    std::cout<<"<booting>"<<std::endl;
    std::vector<unsigned long> seeds;
    if(runSpecificSeeds) {
        seeds = getSpecificSeeds(inputSeeds);
        std::cout << seeds.size() << " seeds from file" << std::endl;
    }
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    auto t1 = high_resolution_clock::now();
    // unsigned long end = 4294967295UL;
    unsigned long end = simCount;
    if(runSpecificSeeds) {
        end = static_cast<unsigned long>(seeds.size());
    }
    std::vector<LossLog> losses(lossTrackerSize, {0,0});
    int divisor = threads; // how many chunks to use
    int losingSeedsIndexSize = lossTrackerSize/threads;

    int offset = 0; // how far to offset
    unsigned long chunkSize = end/divisor;
    int remainder = end % divisor; // extra seeds
    std::vector<Range> ranges;
    CommandList cList = {};
    for(i=0;i<maxTurns ;i++) { // max_turns
        for(j=0;j<maxBranches; j++) {
            cList.commands[i][j] = 0;
        }
    }
    cList.defaultCommand = COMMAND_MOVE_SLOT_2;
    cList.commands[0][0] = COMMAND_USE_ITEM_GUARD_SPEC;
    cList.commands[1][0] = COMMAND_SWITCH_2;
    cList.commands[2][0] = COMMAND_USE_ITEM_X_ACCURACY;
    cList.commands[3][0] = COMMAND_USE_ITEM_HYPER_POTION;
    cList.commands[4][0] = COMMAND_USE_ITEM_FULL_HEAL;
    cList.commands[4][1] = COMMAND_MOVE_SLOT_2;
    cList.commands[5][0] = COMMAND_MOVE_SLOT_2;
    cList.commands[5][1] = COMMAND_TERMINATE;
    cList.commands[6][0] = COMMAND_TERMINATE;
    // cList.commands[1] = COMMAND_MOVE_SLOT_3;

    // cList.commands[1] = COMMAND_USE_ITEM_GUARD_SPEC;
    // cList.commands[3] = COMMAND_USE_ITEM_HYPER_POTION;

    // cList.commands[2] = COMMAND_USE_GATEAU_OR_BITE;
    // cList.commands[3] = COMMAND_USE_HYPER_OR_BITE;

    // simulate a specific seed
    if(runIndividualSeed) {
        simulate(individualSeed, cList, &val);
        return 0;
    }

    unsigned long startIndex = 0;
    unsigned long endIndex = 0;
    int extra = 0;
    for(i = 0; i < divisor; i++){
        if(runSpecificSeeds) {
            extra = (i < remainder) ? 1 : 0;
            endIndex = startIndex + chunkSize + extra;
            ranges.push_back({0, endIndex-startIndex, i, losingSeedsIndexSize, true, getSubVector(seeds, startIndex, endIndex - 1)});
            startIndex = endIndex;
        } else {
            ranges.push_back({i * chunkSize, (i+1) * chunkSize, i, losingSeedsIndexSize, false});
        }
    }
    std::vector<std::future<SimulationReport>> futures;
    for(auto &e : ranges){
        futures.push_back(std::async(runSimulations, e, cList, std::ref(losses)));
    }
    std::string dirName = "./simulationResults/" + outputDirName;
    std::string resultsFilename = dirName + "/simulationResults.txt";
    std::string lossSeedsFilename = dirName + "/losses.csv";
    std::string seedsPastTurnFilename = dirName + "/seeds_past_turn_" + std::to_string(WATCH_TURN) + ".csv";
    fs::create_directories(dirName);
    std::ofstream resultsFile(resultsFilename);
    int turnTotals[maxTurns]= {0}; // max_turns
    for(auto &e : futures) {
        val = e.get();
        if(!val.playerWin){
            resultsFile << "Loss -> Seed: " << val.startingSeed << "\n";
            std::cout << val.startingSeed << "\n";
        } else {
            resultsFile << "Player won. Lowest hp -> " << val.playerHp << " // Seed: " << val.startingSeed << "\n";
            std::cout << "Player won. Lowest hp -> " << val.playerHp << " // Seed: " << val.startingSeed << std::endl;
        }

        for(i = 0;i<maxTurns;i++){ // max_turns
            turnTotals[i] += val.turnTracker[i];
        }

    }
    for(i = 0;i<maxTurns;i++){ // max_turns
        resultsFile << "Turn " << i+1 << "\t" << turnTotals[i] << "\n";
    }
    auto t2 = high_resolution_clock::now();
     /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    resultsFile << ms_int.count() << "ms\n";
    resultsFile << ms_double.count() << "ms\n";
    int lossSeedCount = 0;
    for(i = 0; i < lossTrackerSize; i++) {
        if(losses[i].seed > 0 && losses[i].isLoss) {
            lossSeedCount++;
            resultsFile << losses[i].seed << " [" << losses[i].turn << "]" << ",";
        }
    }
    resultsFile << "Number of losses: " << lossSeedCount << std::endl;
    resultsFile << std::endl;
    for(i=0; i < maxTurns ; i++ ) {
        if(cList.commands[i] == 0) {
            resultsFile << parseCommandString(cList.defaultCommand);
        } else {
            resultsFile << parseCommandString(cList.commands[i][0]);
        }
        resultsFile << " / ";
    }
    resultsFile << std::endl;
    // lossesfile
    std::ofstream lossSeedsFile(lossSeedsFilename);
    for(i = 0; i < lossTrackerSize; i++) {
        if(losses[i].seed > 0 && losses[i].isLoss) {
            lossSeedsFile << losses[i].seed << ",";
        }
    }
    lossSeedsFile.close();
    if(WATCH_TURN > 0) {
        // get one of the seeds which made it to the furthest turn in the battle
        int furthestSeedTurn = 0;
        unsigned long furthestSeed = 0;
        int numSeeds = 0;
        std::ofstream seedsPastTurnFile(seedsPastTurnFilename);
        for(i = 0; i < lossTrackerSize; i++) {
            if(losses[i].seed > 0) {
                numSeeds++;
                if(losses[i].turn > furthestSeedTurn) {
                    furthestSeedTurn = losses[i].turn;
                    furthestSeed = losses[i].seed;
                }
                seedsPastTurnFile << losses[i].seed << ",";
            }
        }
        resultsFile << "furthest seed: " << furthestSeed << std::endl;
        resultsFile << "number of seeds past turn number: " << numSeeds << std::endl;
        seedsPastTurnFile.close();
    }
    resultsFile.close();

    return 0;
}