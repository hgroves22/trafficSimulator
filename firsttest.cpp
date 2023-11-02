#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include "Animator.h"
#include "VehicleBase.h"
#include "randomNumberGenerator.cpp"

using namespace std;

map<string, string> readFile(string fileName) {
    map<string,string> inputs;
    std::ifstream infile {fileName}; // opens the input file
    if(!infile) {
        cerr << "Unable to open input file" << endl;
    }

    string helperData;
    string numbers;

    while(infile >> helperData && infile >> numbers) {
        inputs[helperData] = numbers;
    }
    infile.close(); //closes the input file
    return inputs;
}

int main(int argc, char* argv[]) {

    if(argc != 3) // checking for the right amount of arguments
    {
        cerr << "Incorrect Input. Example: " << argv[0] 
        << " <Input File> <Initial Seed>" 
        << endl; // error message showing correct amount and type of statements needed
        exit(0);
    }

    map<string, string> inputs = readFile(argv[1]); //map to store input files so order does not matter
    randomNumberGenerator rng = randomNumberGenerator(stoi(argv[2])); //random number generator from command line

    int maxSimTime = stoi(inputs.at("maximum_simulated_time:")); //maximum simulated time
    //int maxSimTime = 50;
    int halfSize = stoi(inputs.at("number_of_sections_before_intersection:")); //number of sections before intersection

    //green, yellow, red time for NS and EW
    int greenNS = stoi(inputs.at("green_north_south:"));
    int yellowNS = stoi(inputs.at("yellow_north_south:"));
    int greenEW = stoi(inputs.at("green_east_west:"));
    int yellowEW = stoi(inputs.at("yellow_east_west:"));
    int redEW = greenNS + yellowNS;
    int redNS = greenEW + yellowEW;

    //probability of vehicles each way
    float vehicleProbNorth = stof(inputs.at("prob_new_vehicle_northbound:"));
    float vehicleProbSouth = stof(inputs.at("prob_new_vehicle_southbound:"));
    float vehicleProbEast = stof(inputs.at("prob_new_vehicle_eastbound:"));
    float vehicleProbWest = stof(inputs.at("prob_new_vehicle_westbound:"));

    //proportion of each vehicle
    float carProb = stof(inputs.at("proportion_of_cars:"));
    float suvProb = stof(inputs.at("proportion_of_SUVs:"));
    float truckProb = 1 - carProb - suvProb;

    //proportion of right turn/straight cars
    float carRight = stof(inputs.at("proportion_right_turn_cars:"));
    float carLeft = stof(inputs.at("proportion_left_turn_cars:"));
    float carStraight = 1- carRight;

    //proportion of right turn/left turn/straight suvs
    float suvRight = stof(inputs.at("proportion_right_turn_SUVs:"));
    float suvLeft = stof(inputs.at("proportion_left_turn_SUVs:"));
    float suvStraight = 1 - suvRight;

    //proportion of right turn/left turn/straight trucks
    float truckRight = stof(inputs.at("proportion_right_turn_trucks:"));
    float truckLeft = stof(inputs.at("proportion_left_turn_trucks:"));
    float truckStraight = 1 - truckRight;

    //construct vectors of VehicleBase* of appropriate size, init to nullptr
    vector<VehicleBase*> northBound(halfSize * 2 + 2, nullptr);
    vector<VehicleBase*> southBound(halfSize * 2 + 2, nullptr);
    vector<VehicleBase*> eastBound(halfSize * 2 + 2, nullptr);
    vector<VehicleBase*> westBound(halfSize * 2 + 2, nullptr);

    //all possible vehicle/direction combinations (removed and put in the for loop)
    /*
    VehicleBase vbCarEast(VehicleType::car, Direction::east, WillTurn::no, Finished::no);
    VehicleBase vbSuvEast(VehicleType::suv, Direction::east, WillTurn::no, Finished::no);
    VehicleBase vbTruckEast(VehicleType::truck, Direction::east, WillTurn::no, Finished::no);
    VehicleBase vbCarWest(VehicleType::car, Direction::west, WillTurn::no, Finished::no);
    VehicleBase vbSuvWest(VehicleType::suv, Direction::west, WillTurn::no, Finished::no);
    VehicleBase vbTruckWest(VehicleType::truck, Direction::west, WillTurn::no, Finished::no);

    VehicleBase vbCarSouth(VehicleType::car, Direction::south, WillTurn::no, Finished::no);
    VehicleBase vbSuvSouth(VehicleType::suv, Direction::south, WillTurn::no, Finished::no);
    VehicleBase vbTruckSouth(VehicleType::truck, Direction::south, WillTurn::no, Finished::no);
    VehicleBase vbCarNorth(VehicleType::car, Direction::north, WillTurn::no, Finished::no);
    VehicleBase vbSuvNorth(VehicleType::suv, Direction::north, WillTurn::no, Finished::no);
    VehicleBase vbTruckNorth(VehicleType::truck, Direction::north, WillTurn::no, Finished::no);
    */

    //create the animator and set the lights
    Animator anim(halfSize);
    anim.setLightNorthSouth(LightColor::green);
    anim.setLightEastWest(LightColor::red);

    //character to take char input to pass time
    char timeInput;

    //randomNumber
    float randomNumber = 0;

    //int that holds the time left on yellow
    int yellowTime = 0;

    //to change the vehicleList
    VehicleBase* westNorth = nullptr;
    VehicleBase* northEast = nullptr;
    VehicleBase* eastSouth = nullptr;
    VehicleBase* southWest = nullptr;

    //make sure things only move once (after turn)
    bool westNorthMoved = false;
    bool northEastMoved = false;
    bool eastSouthMoved = false;
    bool southWestMoved = false;
    

    //to see if the turn is fully complete
    bool turnComplete = false;

    //vector to store all the vehicles
    vector<VehicleBase*> eastVehicleList(0, nullptr);
    vector<VehicleBase*> westVehicleList(0, nullptr);
    vector<VehicleBase*> northVehicleList(0, nullptr);
    vector<VehicleBase*> southVehicleList(0, nullptr);

    //for loop of the simulation
    for(int i = 0; i < maxSimTime; i++) 
    {
        for(int j = 0; j < greenNS + yellowNS + redNS; j++) 
        {
            if(i == maxSimTime + 1)
            {
                break;
            }

            randomNumber = rng.getRandomNumber();
            if((randomNumber <= vehicleProbWest) and westBound[0] == nullptr) { //west probability if there is a vehicle, what type it would be, will it turn
                randomNumber = rng.getRandomNumber();

                if(randomNumber <= carProb) {
                    VehicleBase* vbCarWest = new VehicleBase(VehicleType::car, Direction::west, WillTurn::no, Finished::no);
                    randomNumber = rng.getRandomNumber();
                    if(randomNumber <= carRight) {
                        vbCarWest->changeTurn();
                    }
                    westVehicleList.insert(westVehicleList.begin(), vbCarWest);
                }
                else if(randomNumber <= carProb + suvProb) {
                    VehicleBase* vbSuvWest = new VehicleBase(VehicleType::suv, Direction::west, WillTurn::no, Finished::no);
                    randomNumber = rng.getRandomNumber();
                    if(randomNumber <= suvRight) {
                        vbSuvWest->changeTurn();
                    }
                    westVehicleList.insert(westVehicleList.begin(), vbSuvWest);
                }
                else {
                    VehicleBase* vbTruckWest = new VehicleBase(VehicleType::truck, Direction::west, WillTurn::no, Finished::no);
                    randomNumber = rng.getRandomNumber();
                    if(randomNumber <= truckRight) {
                        vbTruckWest->changeTurn();
                    }
                    westVehicleList.insert(westVehicleList.begin(), vbTruckWest);
                }
            }

            randomNumber = rng.getRandomNumber();
            if((randomNumber <= vehicleProbEast) and eastBound[0] == nullptr) { //east probability if there is a vehicle, what type it would be, will it turn
                randomNumber = rng.getRandomNumber();

                if(randomNumber <= carProb) {
                    VehicleBase* vbCarEast = new VehicleBase(VehicleType::car, Direction::east, WillTurn::no, Finished::no);
                    randomNumber = rng.getRandomNumber();
                    if(randomNumber <= carRight) {
                        vbCarEast->changeTurn();
                    }
                    eastVehicleList.insert(eastVehicleList.begin(), vbCarEast);
                }
                else if(randomNumber <= carProb + suvProb) {
                    VehicleBase* vbSuvEast = new VehicleBase(VehicleType::suv, Direction::east, WillTurn::no, Finished::no);
                    randomNumber = rng.getRandomNumber();
                    if(randomNumber <= suvRight) {
                        vbSuvEast->changeTurn();
                    }
                    eastVehicleList.insert(eastVehicleList.begin(), vbSuvEast);
                }
                else {
                    VehicleBase* vbTruckEast = new VehicleBase(VehicleType::truck, Direction::east, WillTurn::no, Finished::no);
                    randomNumber = rng.getRandomNumber();
                    if(randomNumber <= truckRight) {
                        vbTruckEast->changeTurn();
                    }
                    eastVehicleList.insert(eastVehicleList.begin(), vbTruckEast);
                }
            }

            randomNumber = rng.getRandomNumber();
            if((randomNumber <= vehicleProbNorth) and northBound[0] == nullptr) { //north probability if there is a vehicle, what type it would be, will it turn
                randomNumber = rng.getRandomNumber();

                if(randomNumber <= carProb) {
                    VehicleBase* vbCarNorth = new VehicleBase(VehicleType::car, Direction::north, WillTurn::no, Finished::no);
                    randomNumber = rng.getRandomNumber();
                    if(randomNumber <= carRight) {
                        vbCarNorth->changeTurn();
                    }
                    northVehicleList.insert(northVehicleList.begin(), vbCarNorth);
                }
                else if(randomNumber <= carProb + suvProb) {
                    VehicleBase* vbSuvNorth = new VehicleBase(VehicleType::suv, Direction::north, WillTurn::no, Finished::no);
                    randomNumber = rng.getRandomNumber();
                    if(randomNumber <= suvRight) {
                        vbSuvNorth->changeTurn();
                    }
                    northVehicleList.insert(northVehicleList.begin(), vbSuvNorth);
                }
                else {
                    VehicleBase* vbTruckNorth = new VehicleBase(VehicleType::truck, Direction::north, WillTurn::no, Finished::no);
                    randomNumber = rng.getRandomNumber();
                    if(randomNumber <= truckRight) {
                        vbTruckNorth->changeTurn();
                    }
                    northVehicleList.insert(northVehicleList.begin(), vbTruckNorth);
                }
            }

            randomNumber = rng.getRandomNumber();
            if((randomNumber <= vehicleProbSouth) and southBound[0] == nullptr) { //south probability if there is a vehicle, what type it would be, will it turn
                randomNumber = rng.getRandomNumber();

                if(randomNumber <= carProb) {
                    VehicleBase* vbCarSouth = new VehicleBase(VehicleType::car, Direction::south, WillTurn::no, Finished::no);
                    randomNumber = rng.getRandomNumber();
                    if(randomNumber <= carRight) {
                        vbCarSouth->changeTurn();
                    }
                    southVehicleList.insert(southVehicleList.begin(), vbCarSouth);
                }
                else if(randomNumber <= carProb + suvProb) {
                    VehicleBase* vbSuvSouth = new VehicleBase(VehicleType::suv, Direction::south, WillTurn::no, Finished::no);
                    randomNumber = rng.getRandomNumber();
                    if(randomNumber <= suvRight) {
                        vbSuvSouth->changeTurn();
                    }
                    southVehicleList.insert(southVehicleList.begin(), vbSuvSouth);
                }
                else {
                    VehicleBase* vbTruckSouth = new VehicleBase(VehicleType::truck, Direction::south, WillTurn::no, Finished::no);
                    randomNumber = rng.getRandomNumber();
                    if(randomNumber <= truckRight) {
                        vbTruckSouth->changeTurn();
                    }
                    southVehicleList.insert(southVehicleList.begin(), vbTruckSouth);
                }
            }

            //moving of the cars

            if(j < greenNS) //NorthSouth = Green and EastWest = Red
            {
                anim.setLightNorthSouth(LightColor::green);
                anim.setLightEastWest(LightColor::red);

                yellowTime = 0;

                westNorth = nullptr;
                for(int k = westVehicleList.size()-1; k > -1; k--) {
                    westNorthMoved = false;
                    if(westVehicleList[k]->getTurn() == true and westBound[westBound.size()/2 - 2] == westVehicleList[k]) {
                        northBound = westVehicleList[k]->moveTurn(northBound, Direction::north, LightColor::red, LightColor::green, yellowTime);
                        westBound = westVehicleList[k]->moveStraight(westBound, Direction::west, LightColor::red, LightColor::green, yellowTime);
                        westNorth = westVehicleList[k];
                        westNorth->nowFinished();
                    }
                    else {
                        if(westVehicleList[k] == southWest and southWestMoved == false) {
                            southWestMoved = false;
                        }
                        else {
                            westBound = westVehicleList[k]->moveStraight(westBound, Direction::west, LightColor::red, LightColor::green, yellowTime);
                        }
                    }
                }
                if(westBound[westBound.size()/2 -2] != westNorth and westNorth != nullptr) {
                    northVehicleList.insert(northVehicleList.begin(), northBound[northBound.size()/2 + 1]);
                    westNorthMoved = true;
                }

                eastSouth = nullptr;
                for(int k = eastVehicleList.size()-1; k > -1; k--) {
                    eastSouthMoved = false;
                    if(eastVehicleList[k]->getTurn() == true and eastBound[eastBound.size()/2 - 2] == eastVehicleList[k]) {
                        southBound = eastVehicleList[k]->moveTurn(southBound, Direction::south, LightColor::red, LightColor::green, yellowTime);
                        eastBound = eastVehicleList[k]->moveStraight(eastBound, Direction::east, LightColor::red, LightColor::green, yellowTime);
                        eastSouth = eastVehicleList[k];
                        eastSouth->nowFinished();
                    }
                    else {
                        if(eastVehicleList[k] == northEast and northEastMoved == false) {
                            northEastMoved = false;
                        }
                        else {
                            eastBound = eastVehicleList[k]->moveStraight(eastBound, Direction::east, LightColor::red, LightColor::green, yellowTime);
                        }
                    }
                }
                if(eastBound[eastBound.size()/2 -2] != eastSouth and eastSouth != nullptr) {
                    southVehicleList.insert(southVehicleList.begin(), southBound[southBound.size()/2 + 1]);
                    eastSouthMoved = true;
                }

                northEast = nullptr;
                for(int k = northVehicleList.size()-1; k > -1; k--) {
                    northEastMoved = false;
                    if(northVehicleList[k]->getTurn() == true and northBound[northBound.size()/2 - 2] == northVehicleList[k]) {
                        eastBound = northVehicleList[k]->moveTurn(eastBound, Direction::east, LightColor::red, LightColor::green, yellowTime);
                        northBound = northVehicleList[k]->moveStraight(northBound, Direction::north, LightColor::red, LightColor::green, yellowTime);
                        northEast = northVehicleList[k];
                        northEast->nowFinished();
                    }
                    else {
                        if(northVehicleList[k] == westNorth and westNorthMoved == true) {
                            westNorthMoved = false;
                        }
                        else {
                            northBound = northVehicleList[k]->moveStraight(northBound, Direction::north, LightColor::red, LightColor::green, yellowTime);
                        }
                    }
                }
                if(northBound[northBound.size()/2 -2] != northEast and northEast != nullptr) {
                    eastVehicleList.insert(eastVehicleList.begin(), eastBound[eastBound.size()/2 + 1]);
                    northEastMoved = true;
                }

                southWest = nullptr;
                for(int k = southVehicleList.size()-1; k > -1; k--) {
                    southWestMoved = false;
                    if(southVehicleList[k]->getTurn() == true and southBound[southBound.size()/2 - 2] == southVehicleList[k]) {
                        westBound = southVehicleList[k]->moveTurn(westBound, Direction::west, LightColor::red, LightColor::green, yellowTime);
                        southBound = southVehicleList[k]->moveStraight(southBound, Direction::south, LightColor::red, LightColor::green, yellowTime);
                        southWest = southVehicleList[k];
                        southWest->nowFinished();
                    }
                    else {
                        if(southVehicleList[k] == eastSouth and eastSouthMoved == true) {
                            eastSouthMoved = false;
                        }
                        else {
                            southBound = southVehicleList[k]->moveStraight(southBound, Direction::south, LightColor::red, LightColor::green, yellowTime);
                        }
                    }
                }
                if(southBound[southBound.size()/2 -2] != southWest and southWest != nullptr) {
                    westVehicleList.insert(westVehicleList.begin(), westBound[westBound.size()/2 + 1]);
                    southWestMoved = true;
                }
            }

            else if(j < greenNS + yellowNS) //NorthSouth = Yellow and EastWest = Red
            {
                anim.setLightNorthSouth(LightColor::yellow);
                anim.setLightEastWest(LightColor::red);

                yellowTime = yellowNS + greenNS - j;

                westNorth = nullptr;
                for(int k = westVehicleList.size()-1; k > -1; k--) {
                    westNorthMoved = false;
                    if(westVehicleList[k]->getTurn() == true and westBound[westBound.size()/2 - 2] == westVehicleList[k]) {
                        northBound = westVehicleList[k]->moveTurn(northBound, Direction::north, LightColor::red, LightColor::yellow, yellowTime);
                        westBound = westVehicleList[k]->moveStraight(westBound, Direction::west, LightColor::red, LightColor::yellow, yellowTime);
                        westNorth = westVehicleList[k];
                        westNorth->nowFinished();
                    }
                    else {
                        if(westVehicleList[k] == southWest and southWestMoved == false) {
                            southWestMoved = false;
                        }
                        else {
                            westBound = westVehicleList[k]->moveStraight(westBound, Direction::west, LightColor::red, LightColor::yellow, yellowTime);
                        }
                    }
                }
                if(westBound[westBound.size()/2 -2] != westNorth and westNorth != nullptr) {
                    northVehicleList.insert(northVehicleList.begin(), northBound[northBound.size()/2 + 1]);
                    westNorthMoved = true;
                }
                
                eastSouth = nullptr;
                for(int k = eastVehicleList.size()-1; k > -1; k--) {
                    eastSouthMoved = false;
                    if(eastVehicleList[k]->getTurn() == true and eastBound[eastBound.size()/2 - 2] == eastVehicleList[k]) {
                        southBound = eastVehicleList[k]->moveTurn(southBound, Direction::south, LightColor::red, LightColor::yellow, yellowTime);
                        eastBound = eastVehicleList[k]->moveStraight(eastBound, Direction::east, LightColor::red, LightColor::yellow, yellowTime);
                        eastSouth = eastVehicleList[k];
                        eastSouth->nowFinished();
                    }
                    else {
                        if(eastVehicleList[k] == northEast and northEastMoved == false) {
                            northEastMoved = false;
                        }
                        else {
                            eastBound = eastVehicleList[k]->moveStraight(eastBound, Direction::east, LightColor::red, LightColor::yellow, yellowTime);
                        }
                    }
                }
                if(eastBound[eastBound.size()/2 -2] != eastSouth and eastSouth != nullptr) {
                    southVehicleList.insert(southVehicleList.begin(), southBound[southBound.size()/2 + 1]);
                    eastSouthMoved = true;
                }

                northEast = nullptr;
                for(int k = northVehicleList.size()-1; k > -1; k--) {
                    northEastMoved = false;
                    if(northVehicleList[k]->getTurn() == true and northBound[northBound.size()/2 - 2] == northVehicleList[k]) {
                        eastBound = northVehicleList[k]->moveTurn(eastBound, Direction::east, LightColor::red, LightColor::yellow, yellowTime);
                        northBound = northVehicleList[k]->moveStraight(northBound, Direction::north, LightColor::red, LightColor::yellow, yellowTime);
                        northEast = northVehicleList[k];
                        northEast->nowFinished();
                    }
                    else {
                        if(northVehicleList[k] == eastSouth and westNorthMoved == true) {
                            westNorthMoved = false;
                        }
                        else {
                            northBound = northVehicleList[k]->moveStraight(northBound, Direction::north, LightColor::red, LightColor::yellow, yellowTime);
                        }
                    }
                }
                if(northBound[northBound.size()/2 -2] != northEast and northEast != nullptr) {
                    eastVehicleList.insert(eastVehicleList.begin(), eastBound[eastBound.size()/2 + 1]);
                    northEastMoved = true;
                }

                southWest = nullptr;
                for(int k = southVehicleList.size()-1; k > -1; k--) {
                    southWestMoved = false;
                    if(southVehicleList[k]->getTurn() == true and southBound[southBound.size()/2 - 2] == southVehicleList[k]) {
                        westBound = southVehicleList[k]->moveTurn(westBound, Direction::west, LightColor::red, LightColor::yellow, yellowTime);
                        southBound = southVehicleList[k]->moveStraight(southBound, Direction::south, LightColor::red, LightColor::yellow, yellowTime);
                        southWest = southVehicleList[k];
                        southWest->nowFinished();
                    }
                    else {
                        if(southVehicleList[k] == eastSouth and eastSouthMoved == true) {
                            eastSouthMoved = false;
                        }
                        else {
                            southBound = southVehicleList[k]->moveStraight(southBound, Direction::south, LightColor::red, LightColor::yellow, yellowTime);
                        }
                    }
                }
                if(southBound[southBound.size()/2 -2] != southWest and southWest != nullptr) {
                    westVehicleList.insert(westVehicleList.begin(), westBound[westBound.size()/2 + 1]);
                    southWestMoved = true;
                }
            }

            else if(j < greenNS + yellowNS + greenEW) //NorthSouth = Red and EastWest = Green
            {
                anim.setLightNorthSouth(LightColor::red);
                anim.setLightEastWest(LightColor::green);

                yellowTime = 0;

                westNorth = nullptr;
                for(int k = westVehicleList.size()-1; k > -1; k--) {
                    westNorthMoved = false;
                    if(westVehicleList[k]->getTurn() == true and westBound[westBound.size()/2 - 2] == westVehicleList[k]) {
                        northBound = westVehicleList[k]->moveTurn(northBound, Direction::north, LightColor::green, LightColor::red, yellowTime);
                        westBound = westVehicleList[k]->moveStraight(westBound, Direction::west, LightColor::green, LightColor::red, yellowTime);
                        westNorth = westVehicleList[k];
                        westNorth->nowFinished();
                    }
                   else {
                        if(westVehicleList[k] == southWest and southWestMoved == false) {
                            southWestMoved = false;
                        }
                        else {
                            westBound = westVehicleList[k]->moveStraight(westBound, Direction::west, LightColor::green, LightColor::red, yellowTime);
                        }
                    }
                }
                if(westBound[westBound.size()/2 -2] != westNorth and westNorth != nullptr) {
                    northVehicleList.insert(northVehicleList.begin(), northBound[northBound.size()/2 + 1]);
                    westNorthMoved = true;
                }

                eastSouth = nullptr;
                for(int k = eastVehicleList.size()-1; k > -1; k--) {
                    eastSouthMoved = false;
                    if(eastVehicleList[k]->getTurn() == true and eastBound[eastBound.size()/2 - 2] == eastVehicleList[k]) {
                        southBound = eastVehicleList[k]->moveTurn(southBound, Direction::south, LightColor::green, LightColor::red, yellowTime);
                        eastBound = eastVehicleList[k]->moveStraight(eastBound, Direction::east, LightColor::green, LightColor::red, yellowTime);
                        eastSouth = eastVehicleList[k];
                        eastSouth->nowFinished();
                    }
                    else {
                        if(eastVehicleList[k] == northEast and northEastMoved == false) {
                            northEastMoved = false;
                        }
                        else {
                            eastBound = eastVehicleList[k]->moveStraight(eastBound, Direction::east, LightColor::green, LightColor::red, yellowTime);
                        }
                    }
                }
                if(eastBound[eastBound.size()/2 -2] != eastSouth and eastSouth != nullptr) {
                    southVehicleList.insert(southVehicleList.begin(), southBound[southBound.size()/2 + 1]);
                    eastSouthMoved = true;
                }
                

                northEast = nullptr;
                for(int k = northVehicleList.size()-1; k > -1; k--) {
                    northEastMoved = false;
                    if(northVehicleList[k]->getTurn() == true and northBound[northBound.size()/2 - 2] == northVehicleList[k]) {
                        eastBound = northVehicleList[k]->moveTurn(eastBound, Direction::east, LightColor::green, LightColor::red, yellowTime);
                        northBound = northVehicleList[k]->moveStraight(northBound, Direction::north, LightColor::green, LightColor::red, yellowTime);
                        northEast = northVehicleList[k];
                        northEast->nowFinished();
                    }
                    else {
                        if(northVehicleList[k] == westNorth and westNorthMoved == true) {
                            westNorthMoved = false;
                        }
                        else {
                            northBound = northVehicleList[k]->moveStraight(northBound, Direction::north, LightColor::green, LightColor::red, yellowTime);
                        }
                    }
                }
                if(northBound[northBound.size()/2 -2] != northEast and northEast != nullptr) {
                    eastVehicleList.insert(eastVehicleList.begin(), eastBound[eastBound.size()/2 + 1]);
                    northEastMoved = true;
                }

                southWest = nullptr;
                for(int k = southVehicleList.size()-1; k > -1; k--) {
                    southWestMoved = false;
                    if(southVehicleList[k]->getTurn() == true and southBound[southBound.size()/2 - 2] == southVehicleList[k]) {
                        westBound = southVehicleList[k]->moveTurn(westBound, Direction::west, LightColor::green, LightColor::red, yellowTime);
                        southBound = southVehicleList[k]->moveStraight(southBound, Direction::south, LightColor::green, LightColor::red, yellowTime);
                        southWest = southVehicleList[k];
                        southWest->nowFinished();
                    }
                    else {
                        if(southVehicleList[k] == eastSouth and eastSouthMoved == true) {
                            eastSouthMoved = false;
                        }
                        else {
                            southBound = southVehicleList[k]->moveStraight(southBound, Direction::south, LightColor::green, LightColor::red, yellowTime);
                        }
                    }
                }
                if(southBound[southBound.size()/2 -2] != southWest and southWest != nullptr) {
                    westVehicleList.insert(westVehicleList.begin(), westBound[westBound.size()/2 + 1]);
                    southWestMoved = true;
                }
            }

            else //NorthSouth = Red and EastWest = Yellow
            {
                anim.setLightNorthSouth(LightColor::red);
                anim.setLightEastWest(LightColor::yellow);

                yellowTime = greenNS + yellowNS + greenEW + yellowEW - j;

                westNorth = nullptr;
                for(int k = westVehicleList.size()-1; k > -1; k--) {
                    westNorthMoved = false;
                    if(westVehicleList[k]->getTurn() == true and westBound[westBound.size()/2 - 2] == westVehicleList[k]) {
                        northBound = westVehicleList[k]->moveTurn(northBound, Direction::north, LightColor::yellow, LightColor::red, yellowTime);
                        westBound = westVehicleList[k]->moveStraight(westBound, Direction::west, LightColor::yellow, LightColor::red, yellowTime);
                        westNorth = westVehicleList[k];
                        westNorth->nowFinished();
                    }
                    else {
                        if(westVehicleList[k] == southWest and southWestMoved == false) {
                            southWestMoved = false;
                        }
                        else {
                            westBound = westVehicleList[k]->moveStraight(westBound, Direction::west, LightColor::yellow, LightColor::red, yellowTime);
                        }
                    }
                }
                if(westBound[westBound.size()/2 -2] != westNorth and westNorth != nullptr) {
                    northVehicleList.insert(northVehicleList.begin(), northBound[northBound.size()/2 + 1]);
                    westNorthMoved = true;
                }
                
                eastSouth = nullptr;
                for(int k = eastVehicleList.size()-1; k > -1; k--) {
                    eastSouthMoved = false;
                    if(eastVehicleList[k]->getTurn() == true and eastBound[eastBound.size()/2 - 2] == eastVehicleList[k]) {
                        southBound = eastVehicleList[k]->moveTurn(southBound, Direction::south, LightColor::yellow, LightColor::red, yellowTime);
                        eastBound = eastVehicleList[k]->moveStraight(eastBound, Direction::east, LightColor::yellow, LightColor::red, yellowTime);
                        eastSouth = eastVehicleList[k];
                        eastSouth->nowFinished();
                    }
                    else {
                        if(eastVehicleList[k] == northEast and northEastMoved == false) {
                            northEastMoved = false;
                        }
                        else {
                            eastBound = eastVehicleList[k]->moveStraight(eastBound, Direction::east, LightColor::yellow, LightColor::red, yellowTime);
                        }
                    }
                }
                if(eastBound[eastBound.size()/2 -2] != eastSouth and eastSouth != nullptr) {
                    southVehicleList.insert(southVehicleList.begin(), southBound[southBound.size()/2 + 1]);
                    eastSouthMoved = true;
                }
                

                northEast = nullptr;
                for(int k = northVehicleList.size()-1; k > -1; k--) {
                    northEastMoved = false;
                    if(northVehicleList[k]->getTurn() == true and northBound[northBound.size()/2 - 2] == northVehicleList[k]) {
                        eastBound = northVehicleList[k]->moveTurn(eastBound, Direction::east, LightColor::yellow, LightColor::red, yellowTime);
                        northBound = northVehicleList[k]->moveStraight(northBound, Direction::north, LightColor::yellow, LightColor::red, yellowTime);
                        northEast = northVehicleList[k];
                        northEast->nowFinished();
                    }
                    else {
                        if(northVehicleList[k] == westNorth and westNorthMoved == true) {
                            westNorthMoved = false;
                        }
                        else {
                            northBound = northVehicleList[k]->moveStraight(northBound, Direction::north, LightColor::yellow, LightColor::red, yellowTime);
                        }
                    }
                }
                if(northBound[northBound.size()/2 -2] != northEast and northEast != nullptr) {
                    eastVehicleList.insert(eastVehicleList.begin(), eastBound[eastBound.size()/2 + 1]);
                    northEastMoved = true;
                }

                southWest = nullptr;
                for(int k = southVehicleList.size()-1; k > -1; k--) {
                    southWestMoved = false;
                    if(southVehicleList[k]->getTurn() == true and southBound[southBound.size()/2 - 2] == southVehicleList[k]) {
                        westBound = southVehicleList[k]->moveTurn(westBound, Direction::west, LightColor::yellow, LightColor::red, yellowTime);
                        southBound = southVehicleList[k]->moveStraight(southBound, Direction::south, LightColor::yellow, LightColor::red, yellowTime);
                        southWest = southVehicleList[k];
                        southWest->nowFinished();
                    }
                    else {
                        if(southVehicleList[k] == eastSouth and eastSouthMoved == true) {
                            eastSouthMoved = false;
                        }
                        else {
                            southBound = southVehicleList[k]->moveStraight(southBound, Direction::south, LightColor::yellow, LightColor::red, yellowTime);
                        }
                    }
                }
                if(southBound[southBound.size()/2 -2] != southWest and southWest != nullptr) {
                    westVehicleList.insert(westVehicleList.begin(), westBound[westBound.size()/2 + 1]);
                    southWestMoved = true;
                }
            }
            

            anim.setVehiclesNorthbound(northBound);
            anim.setVehiclesWestbound(westBound);
            anim.setVehiclesSouthbound(southBound);
            anim.setVehiclesEastbound(eastBound);

            anim.draw(i);
            std::cin.get(timeInput);
            

            //did not reset the vectors and instead kept them filled each loop
            //southBound.assign(halfSize * 2 + 2, nullptr); // reset
            //northBound.assign(halfSize * 2 + 2, nullptr); // reset
            //eastBound.assign(halfSize * 2 + 2, nullptr); // reset
            //westBound.assign(halfSize * 2 + 2, nullptr); // reset
            i++;
        }
        i--;
    }
}