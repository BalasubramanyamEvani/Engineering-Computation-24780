/**
 * @file ps3-1.cpp
 * @author Balasubramanyam Evani (bevani@andrew.cmu.edu)
 * @brief PS3-1 Solution
 *
 * @date 2023-09-16
 */
#include "ps3.h"

// Main Function
int main(void) {
    // variables to store the used input and output result
    double altitude, temperature, climb_performance;

    // this is an infinite loop
    // need to give a ctrl + c (SIGINT) to exit the program
    while (true) {
        get_altitude_temperature(&altitude, &temperature);
        climb_performance = calculate_climb_performance(altitude, temperature);
        printf("Expected maximum rate of climb for the given condition is: %.2lf ft/min\n", climb_performance);
    }
    return 0;
}
