/**
 * @file ps2-1.cpp
 * @author Balasubramanyam Evani (AndrewId: bevani)
 * @brief
 *
 * Solution for the PS2-1 Weekly Lunch Planner
 * The main function here does the following in sequential order:
 * 1. Sets random seed based on current time
 * 2. Randomly shuffles predetermined food choices array
 * 3. Outputs the shuffled food choices for the week
 *
 * @date 2023-09-08
 */
#include "ps2.h"

/**
 * @brief Main function
 * @return int exit status code
 */
int main(void) {
    set_random_num_seed();
    rnd_shuffle(food_choices, NUM_DAYS);
    ps2_1_output(day_names, food_choices, NUM_DAYS);
    return 0;
}
