#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <future>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
struct Foo {
    unsigned long seed;
    unsigned short result;
};
struct Range {
    unsigned long start;
    unsigned long stop;
};
struct Seed {
    int cycles;
    unsigned long startingSeed;
};

struct BiggerSeed {
    int lowest;
    unsigned long lowestSeed;
    int highest;
    unsigned long highestSeed;
    int lowest2;
    unsigned long lowestSeed2;
    int highest2;
    unsigned long highestSeed2;
};
struct DmgCalc {
    int critRoll;
    int dmgRoll;
    int accRoll;
    Foo value;
};
struct WanderMatrix {
    int height;
    int width;
    int startX;
    int startY;
};
int seedCounts[800];

Foo advanceSeed(unsigned long seed){
    seed = (1103515245 * seed + 24691) & 0xFFFFFFFF;
    unsigned short result = seed / 65536;
    return {seed, result};
}
long long twice(long long m) {
    return 2 * m;
}
DmgCalc getDmgRoll(Foo value) {
    value = advanceSeed(value.seed);
    int critRoll =  value.result % 16;
    value = advanceSeed(value.seed);
    int damageRoll =  value.result % 16;
    value = advanceSeed(value.seed);
    int accuracyRoll =  value.result % 100;
    return {critRoll, damageRoll, accuracyRoll, value};
}
Seed getMax(Range r){
    Seed worstSeed = {0,0};
    int friendshipMax = 255;
    int currentFriendship = 0;
    int walkCycles = 0;
    Foo value = {0,0};
    unsigned long seed = 0;
    for(unsigned long i = r.start; i < r.stop; i++){
        seed = i;
        currentFriendship = 0;
        walkCycles = 0;
        while(currentFriendship < friendshipMax){
            walkCycles = walkCycles + 1;
            value = advanceSeed(seed);
            seed = value.seed;
            if(value.result & 0x1){
                currentFriendship = currentFriendship + 1;
            }
        }
        if(walkCycles > worstSeed.cycles){
            worstSeed = {walkCycles, i};
        }
        seedCounts[walkCycles] = seedCounts[walkCycles] + 1; 
    }
    return worstSeed;
}

Seed getMostBalls(Range r){
    Seed worstSeed = {0,0};
    bool uncaptured = true;
    unsigned long seed = 0;
    int ballsThrown = 0;
    int comparator = 49931;
    Foo value = {0,0};
    for(unsigned long i = r.start; i < r.stop; i++){
        seed = i;
        ballsThrown = 0;
        uncaptured = true;
        while(uncaptured){
            ballsThrown++;
            uncaptured = false;
            for(int j = 0; j < 4; j++){
                value = advanceSeed(seed);
                seed = value.seed;
                if(value.result >= comparator){
                    uncaptured = true;
                }
            }
            for(int k=0; k < 16; k++){
                value = advanceSeed(seed);
                seed = value.seed;
            }
        }
        if(ballsThrown > worstSeed.cycles) {
            worstSeed = {ballsThrown, i};
        }
        seedCounts[ballsThrown] = seedCounts[ballsThrown] + 1;
    }
    return worstSeed;
}

Seed getWanderTime(Range r){
    Seed worstSeed = {0,0};
    bool seen = false;
    unsigned long seed = 0;
    int framesPassed = 0;
    Foo value = {0,0};
    int waittime = 0;
    int currentPositionX = 2;
    int currentPositionY = 2;
    int arr[3][3] = {
            {0,0,0},
            {0,0,0},
            {0,0,1}
        };
    int direction = 0;
    for(unsigned long i = r.start; i < r.stop; i++){

        for(int j = 0; j < 3; j++){
            for(int k = 0; k < 3; k++){
                arr[j][k] = 0;
            }
        }
        arr[2][2] = 1;
        currentPositionX = 2;
        currentPositionY = 2;
        seed = i;
        framesPassed = 0;
        seen = false;
        while(!seen){
            arr[currentPositionY][currentPositionX] = 0;
            value = advanceSeed(seed);
            seed = value.seed;
            waittime = 30 + (32 * (value.result % 4));
            framesPassed = framesPassed + waittime;
            value = advanceSeed(seed);
            seed = value.seed;
            direction = value.result % 4;
            if(direction == 0){
                currentPositionY = currentPositionY + 1;
                if(currentPositionY > 2) {
                    currentPositionY = 2;
                    framesPassed = framesPassed + 4;
                } else {
                    framesPassed = framesPassed + 18;
                }
            } else if(direction == 1){
                currentPositionY = currentPositionY - 1;
                if(currentPositionY < 0){
                    currentPositionY = 0;
                    framesPassed = framesPassed + 4;
                } else {
                    framesPassed = framesPassed + 18;
                }
            } else if(direction == 3){
                currentPositionX = currentPositionX + 1;
                if(currentPositionX > 2) {
                    currentPositionX = 2;
                    framesPassed = framesPassed + 4;
                } else {
                    framesPassed = framesPassed + 18;
                }
            } else if(direction == 2){
                currentPositionX = currentPositionX - 1;
                if(currentPositionX < 0){
                    currentPositionX = 0;
                    framesPassed = framesPassed + 4;
                } else {
                    framesPassed = framesPassed + 18;
                }
            }
            if(currentPositionX == 0 & currentPositionY == 0 & direction == 1){
                seen = true;
            }
            arr[currentPositionY][currentPositionX] = direction + 1;
        }
        if(framesPassed >= worstSeed.cycles) {
            worstSeed = {framesPassed, i};
        }
        framesPassed = framesPassed/3600;
        seedCounts[framesPassed] = seedCounts[framesPassed] + 1;
    }
    return worstSeed;
}

// rng cheatsheet
// +2 at start of round of combat
// +2 at start of your turn
    // +3 for using an attack (crit/dmg/acc)
    // +1 for using a status (acc) and +1 if the status has a random component (eg taunt)
    // +0 for using an item with no random component
    // +2 if you hit (CONDITIONAL on using something that hits)
// +2 at start of opponents turn
// +3/1/0 based on their move/action
    // +2 if they hit 
// AI advances
    // +4 if the AI has a guarenteed action (using a potion)
    // +10 if the AI is just selecting a move
    // +9 if the AI just swapped in a new pokemon
Foo advanceSeedBy(Foo oSeed, int cycles) {
    for(int i = 0; i < cycles; i++){
        oSeed = advanceSeed(oSeed.seed);
    }
    return oSeed;
}
// simulation of the roark fight
Seed getDoubleCrit(Range r) {
    Seed worstSeed = {0,0};
    unsigned long seed = 0;
    Foo value = {0,0};
    int critRoll = 0;
    int damageRoll = 0;
    int accuracyRoll = 0;
    for(unsigned long i = r.start; i < r.stop; i++){
        seed = i;
        for(int j = 0; j < 6; j++){
            // advance 6 for turn start and geodude turn start nohit
            value = advanceSeed(seed);
            seed = value.seed;
            // using x attack this turn
        }
        value = advanceSeed(seed);
        critRoll = value.result % 16;
        value = advanceSeed(value.seed);
        damageRoll = value.result % 16;
        value = advanceSeed(value.seed);
        accuracyRoll = value.result % 100;
        if(accuracyRoll < 90) {
            // hits
            if(critRoll == 0 & damageRoll == 0){
                // max damage crit
                for(int j = 0; j < 16; j++){
                    // advance 16 for ai advances, hitting, and beginning of next turn
                    value = advanceSeed(value.seed);
                }
                value = advanceSeed(value.seed);
                critRoll = value.result % 16;
                value = advanceSeed(value.seed);
                damageRoll = value.result % 16;
                value = advanceSeed(value.seed);
                accuracyRoll = value.result % 100;
                // mach punch will hit
                if(critRoll != 0 & damageRoll > 5){
                    // mach punch does min damage
                    for(int j = 0; j < 4; j++){
                        // advance 4 for geodude start and getting hit
                        value = advanceSeed(value.seed);
                    }
                    value = advanceSeed(value.seed);
                    critRoll = value.result % 16;
                    value = advanceSeed(value.seed);
                    damageRoll = value.result % 16;
                    value = advanceSeed(value.seed);
                    accuracyRoll = value.result % 100;
                    if(critRoll == 0 & damageRoll == 0 & accuracyRoll < 90){
                        value = advanceSeedBy(value, 51); // using potions for a turn
                        value = advanceSeed(value.seed);
                        critRoll = value.result % 16;
                        value = advanceSeed(value.seed);
                        damageRoll = value.result % 16;
                        value = advanceSeed(value.seed);
                        accuracyRoll = value.result % 100;
                        if(critRoll == 0 & accuracyRoll < 90){
                            if(worstSeed.cycles == 0){
                                // worstSeed = {5, i};
                            }
                        }
                    }
                }

            }


        } else {
            value = advanceSeedBy(value, 26);
            accuracyRoll = value.result % 100;
            if(accuracyRoll > 90){
                if(worstSeed.cycles == 0){
                    worstSeed = {5, i};
                }
            }
        }



    }
    return worstSeed;
}
// spinning on a tile, how many moves must we make
// before guarenteed encounter

BiggerSeed getWildSpin(Range r) {
    BiggerSeed worstSeed = {0, 0};
    unsigned long seed = 0;
    Foo value = {0,0};
    int encRate = 30; // cave
    int moveRate = 30; // walking (spinning)
    bool battleStarted = false;
    for(unsigned long i = r.start; i < r.stop; i++){
        int cycles = 0;
        seed = i;
        battleStarted = false;
        while(!battleStarted) {
            value = advanceSeed(seed);
            seed = value.seed;
            if(value.result/0x290 < moveRate) {
                value = advanceSeed(seed);
                seed = value.seed;
                if(value.result/0x290 < encRate) {
                    battleStarted = true;
                }
            }
            cycles++;
        }
        if(cycles > worstSeed.highest) {
            worstSeed.highest = cycles;
            worstSeed.highestSeed = i;
        }
    }
    return worstSeed;
}

BiggerSeed getWildXp(Range r) {
    BiggerSeed worstSeed = {0, 0, 0, 0};
    unsigned long seed = 0;
    Foo value = {0,0};
    int encRate = 30; // ravaged path // grass is 30
    int moveRate = 30; // walking (spinning)
    int maxSteps = 5;
    int totalXp = 0;
    int totalEv = 0;
    int battles = 0;
    int stepCounter = 0;
    int slot = 0;
    int nature = 0;
    bool loop = true;
    unsigned long pidTop = 0;
    unsigned long pidBottom = 0;
    unsigned long pid = 0;
    // int xpSlots[12] = {30, 45, 38, 57, 23, 23, 68, 46, 46, 46, 46, 46}; // ravaged path
    int xpSlots[12] = {16, 16, 24, 24, 24, 24, 32, 33, 32, 33, 32, 33}; // lake verity
    int evSlots[12] = {0,1,0,1,0,1,0,1,0,1,0,1}; // lake verity
    for(unsigned long i = r.start; i < r.stop; i++){
        seed = i;
        totalXp = 0;
        totalEv = 0;
        battles = 0;
        stepCounter = 0;
        value = advanceSeed(seed);
        slot = 0;
        nature = 0;
        while(battles < 70) {
            if(stepCounter >= maxSteps | value.result/0x290 < 5){
                if(stepCounter < maxSteps){
                    value = advanceSeed(value.seed);
                }
                if(value.result/0x290 < moveRate) {
                    value = advanceSeed(value.seed);
                    if(value.result/0x290 < encRate) {
                        value = advanceSeed(value.seed);
                        slot = value.result/656;
                        if(slot < 20){
                            slot = 0;
                        } else if(slot < 40) {
                            slot = 1;
                        } else if(slot < 50) {
                            slot = 2;
                        } else if(slot < 60) {
                            slot = 3;
                        } else if(slot < 70) {
                            slot = 4;
                        } else if(slot < 80) {
                            slot = 5;
                        } else if(slot < 85) {
                            slot = 6;
                        } else if(slot < 90) {
                            slot = 7;
                        } else if(slot < 94){
                            slot = 8;
                        } else if(slot < 98) {
                            slot = 9;
                        } else if(slot == 98) {
                            slot = 10;
                        } else {
                            slot = 11;
                        }
                    value = advanceSeed(value.seed);
                    nature = value.result / 0xA3E; // 0-24
                    loop = true;
                    while(loop) {
                        value = advanceSeed(value.seed);
                        pidBottom = value.result;
                        value = advanceSeed(value.seed);
                        pidTop = value.result * 65536;

                        pid = pidTop + pidBottom;
                        if(pid % 25 == nature){
                            loop = false;
                        }
                    }
                    value = advanceSeed(value.seed); // iv
                    value = advanceSeed(value.seed); // iv2
                    value = advanceSeed(value.seed);
                    value = advanceSeed(value.seed);
                    stepCounter = -1;
                    battles++;
                    totalXp = totalXp + xpSlots[slot];
                    totalEv = totalEv + evSlots[slot];
                    // std::cout << "slot: " << slot << std::endl;

                    }
                }
            }
            stepCounter++;
            value= advanceSeed(value.seed);

        }
        if(totalXp > worstSeed.highest | worstSeed.highest == 0){
            worstSeed.highest = totalXp;
            worstSeed.highestSeed = i;
        } else if(totalXp < worstSeed.lowest | worstSeed.lowest == 0 ) {
            worstSeed.lowest = totalXp;
            worstSeed.lowestSeed = i;
        }

        if(totalEv > worstSeed.highest2 | worstSeed.highest2 == 0){
            worstSeed.highest2 = totalEv;
            worstSeed.highestSeed2 = i;
        } else if(totalEv < worstSeed.lowest2 | worstSeed.lowest2 == 0) {
            worstSeed.lowest2 = totalEv;
            worstSeed.lowestSeed2 = i;
        }
    }
    std::cout<< worstSeed.highest <<std::endl;

    return worstSeed;
}
BiggerSeed getShiny(Range r) {
    BiggerSeed worstSeed = {0, 0, 0, 0};
    unsigned long seed = 0;
    Foo value = {0,0};
    int encRate = 30; // ravaged path // grass is 30
    int moveRate = 30; // walking (spinning)
    int maxSteps = 5;
    int sid = 63631;
    int tid = 10064;
    int battles = 0;
    int stepCounter = 0;
    int slot = 0;
    int nature = 0;
    bool loop = true;
    int shinyCount = 0;
    unsigned long pidTop = 0;
    unsigned long pidTopOriginal = 0;
    unsigned long pidBottom = 0;
    unsigned long pid = 0;
    // int xpSlots[12] = {30, 45, 38, 57, 23, 23, 68, 46, 46, 46, 46, 46}; // ravaged path
    int xpSlots[12] = {16, 16, 24, 24, 24, 24, 32, 33, 32, 33, 32, 33}; // lake verity
    int evSlots[12] = {0,1,0,1,0,1,0,1,0,1,0,1}; // lake verity
    for(unsigned long i = r.start; i < r.stop; i++){
        seed = i;
        battles = 0;
        stepCounter = 0;
        shinyCount = 0;
        value = advanceSeed(seed);
        slot = 0;
        nature = 0;
        while(battles < 4) {
            if(stepCounter >= maxSteps | value.result/0x290 < 5){
                if(stepCounter < maxSteps){
                    value = advanceSeed(value.seed);
                }
                if(value.result/0x290 < moveRate) {
                    value = advanceSeed(value.seed);
                    if(value.result/0x290 < encRate) {
                        value = advanceSeed(value.seed);
                        slot = value.result/656;
                        if(slot < 20){
                            slot = 0;
                        } else if(slot < 40) {
                            slot = 1;
                        } else if(slot < 50) {
                            slot = 2;
                        } else if(slot < 60) {
                            slot = 3;
                        } else if(slot < 70) {
                            slot = 4;
                        } else if(slot < 80) {
                            slot = 5;
                        } else if(slot < 85) {
                            slot = 6;
                        } else if(slot < 90) {
                            slot = 7;
                        } else if(slot < 94){
                            slot = 8;
                        } else if(slot < 98) {
                            slot = 9;
                        } else if(slot == 98) {
                            slot = 10;
                        } else {
                            slot = 11;
                        }
                    value = advanceSeed(value.seed);
                    nature = value.result / 0xA3E; // 0-24
                    loop = true;
                    while(loop) {
                        value = advanceSeed(value.seed);
                        pidBottom = value.result;
                        value = advanceSeed(value.seed);
                        pidTopOriginal = value.result;
                        pidTop = pidTopOriginal * 65536;

                        pid = pidTop + pidBottom;
                        if(pid % 25 == nature){
                            loop = false;
                        }
                    }
                    if((tid ^ sid ^ pidTopOriginal ^ pidBottom) < 8) {
                        shinyCount++;
                    }
                    value = advanceSeed(value.seed); // iv
                    value = advanceSeed(value.seed); // iv2
                    value = advanceSeed(value.seed);
                    value = advanceSeed(value.seed);
                    stepCounter = -1;
                    battles++;

                    // std::cout << "slot: " << slot << std::endl;

                    }
                }
            }
            stepCounter++;
            value= advanceSeed(value.seed);

        }
        if(shinyCount > worstSeed.highest | worstSeed.highest == 0){
            worstSeed.highest = shinyCount;
            worstSeed.highestSeed = i;
        }
    }

    return worstSeed;
}
BiggerSeed getPuruglyCrits(Range r) {
    BiggerSeed worstSeed = {0,0,0,0};
    unsigned long seed = 0;
    Foo value = {0,0};
    int critRoll = 0;
    int damageRoll = 0;
    int accuracyRoll = 0;
    DmgCalc dmg = {0,0,0,value};
    for(unsigned long i = r.start; i < r.stop; i++){
        seed = i;
        value.seed = i;
        value = advanceSeedBy(value, 6);
        dmg = getDmgRoll(value);
        value = dmg.value;
        if(dmg.critRoll == 0 & dmg.dmgRoll == 0){
            value = advanceSeedBy(value, 23); // go to next turn
            dmg = getDmgRoll(value);
            value = dmg.value;
            if(dmg.critRoll == 0 & dmg.dmgRoll == 0){
                value = advanceSeedBy(value, 17);
                dmg = getDmgRoll(value);
                value = dmg.value;
                if(dmg.critRoll == 0 & dmg.dmgRoll < 16) {
                    worstSeed.highestSeed = i;
                    worstSeed.highest = 5;
                }

            }

        }
    }
    return worstSeed;
}

BiggerSeed getPsyDuckCatch(Range r) {
    BiggerSeed worstSeed = {0,0,0,0};
    unsigned long seed = 0;
    Foo value = {0,0};
    int roll1 = 0;
    int roll2 = 0;
    int roll3 = 0;
    int roll4 = 0;
    bool captured = false;
    for(unsigned long i = r.start; i < r.stop; i++){
        value.seed = i;
        value = advanceSeedBy(value, 4);
        value = advanceSeed(value.seed);
        roll1 = value.result;
        if(roll1 < 65535){
            value = advanceSeed(value.seed);
            roll2 = value.result;
            if(roll2 < 65535){
                value = advanceSeed(value.seed);
                roll3 = value.result;
                if(roll3 < 65535){
                    value = advanceSeed(value.seed);
                    roll4 = value.result;
                    captured = true;
                }
            }
        }

        value = advanceSeed(value.seed);

    }
    return worstSeed;
}

int main()
{
    std::cout<<"<booting>"<<std::endl;

    // getWildXp({116050380,116050381});
    // return 0;

    for(int i = 0; i < 800; i++){
        seedCounts[i] = 0;
    }
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    auto t1 = high_resolution_clock::now();
    unsigned long end = 4294967295UL;
    // unsigned long end = 169496720UL;
    // unsigned long end = 400000000UL;

    int divisor = 16; // how many chunks to use
    unsigned long neow = end/divisor;
    BiggerSeed max = {0,0};
    BiggerSeed min = {0,0};
    BiggerSeed max2 = {0,0};
    BiggerSeed min2 = {0,0};
    BiggerSeed val = {0,0};
    std::vector<Range> ranges;
    for(int i = 0; i < divisor; i++){
        ranges.push_back({i * neow, (i+1) * neow});
    }
    // bigger seed for high/low stuff
    std::vector<std::future<BiggerSeed>> futures;
    for(auto &e : ranges){
        futures.push_back(std::async(getShiny, e));
    }
    //retrive and print the value stored in the future
    for(auto &e : futures) {
        val = e.get();
        std::cout << "starting seed: " << val.lowestSeed << "  lowest: " << val.lowest << "| starting seed: " << val.highestSeed << "  highest: " << val.highest << std::endl;
        std::cout << "starting seed: " << val.lowestSeed2 << "  lowest2: " << val.lowest2 << "| starting seed: " << val.highestSeed2 << "  highest2: " << val.highest2 << std::endl;
        if(val.highest > max.highest){
            max = val;
        }
        if(val.lowest < min.lowest | min.lowest == 0){
            min = val;
        }
        if(val.highest2 > max2.highest2){
            max2.highest2 = val.highest2;
            max2.highestSeed2 = val.highestSeed2;
        }
        if(val.lowest2 < min2.lowest2 | min2.lowest2 == 0){
            min2 = val;
        }
    }
    std::cout << "______" << std::endl;
    std::cout << "highest cycles: " << max.highest << std::endl;
    std::cout << "starting seed: "<<max.highestSeed << std::endl;
    std::cout << "lowest cycles: " << min.lowest << std::endl;
    std::cout << "starting seed: "<< min.lowestSeed << std::endl;
    std::cout << "highest2 cycles: " << max2.highest2 << "  starting seed: "<<max2.highestSeed2 << std::endl;
    std::cout << "lowest2 cycles: " << min2.lowest2 << "  starting seed: "<< min2.lowestSeed2 << std::endl;
    auto t2 = high_resolution_clock::now();
     /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    std::cout << ms_int.count() << "ms\n";
    std::cout << ms_double.count() << "ms\n";
    for(int i = 0; i < 800; i++){
        int val = seedCounts[i];
        if(val > 0){
            std::cout << i << ": " << seedCounts[i] << "\n";
        }
    }
    return 0;

}

