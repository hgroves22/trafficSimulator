/* This class generates a random floating point number between 0 and 1. It takes nothing
 and returns a floating point number. */

#include <iostream>
#include <random>

class randomNumberGenerator {
    private: 
        std::mt19937 rng;
        
        //std::uniform_real_distribution<double> rand_double(0,1);

    public:
        randomNumberGenerator(int seed) {
            
            int initialSeed = seed;
            rng.seed(initialSeed);
        }

        float getRandomNumber() {
            std::uniform_real_distribution<float> rand_float(0,1);
            return rand_float(rng);
        }

        ~randomNumberGenerator() {};

};
/*
int main() {
    //This code shouldn't go into the final version but rather by implemented in our main simulation file
    randomNumberGenerator randomCreator = randomNumberGenerator(860836); //starts the engine
    double myRandomNumber = randomCreator.getRandomNumber();
    std::cout << myRandomNumber << "\n";
    for(int i = 0; i < 10; i++) {
        myRandomNumber = randomCreator.getRandomNumber();
        std::cout << myRandomNumber << std::endl;
    }
}
*/