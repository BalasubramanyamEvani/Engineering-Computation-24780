#include <stdio.h>

// Constants

#define MIN_ALTITUDE 0
#define MAX_ALTITUDE 10000
#define MIN_TEMPERATURE -20
#define MAX_TEMPERATURE 40

#define NUM_ALTITUDES 6
#define NUM_TEMPS 4

const double rate_of_climb_table[6][4] = {
    {830.0, 770.0, 705.0, 640.0},
    {720.0, 655.0, 595.0, 535.0},
    {645.0, 585.0, 525.0, 465.0},
    {530.0, 475.0, 415.0, 360.0},
    {420.0, 365.0, 310.0, 250.0},
    {310.0, 255.0, 200.0, 145.0},
};

const double altitudes[6] = {0.0, 2000.0, 4000.0, 6000.0, 8000.0, 10000.0};
const double temperatures[4] = {-20.0, 0.0, 20.0, 40.0};

/**
 * @brief Checks if the given altitude and temperature fall within valid ranges
 *
 * @param altitude The altitude to check
 * @param temperatue The temperature to check
 * @return true, if both altitude and temperature are within valid ranges
 * @return false, otherwise
 */
bool is_valid_ps31_input(double altitude, double temperatue) {
    if (altitude < MIN_ALTITUDE || altitude > MAX_ALTITUDE) {
        return false;
    }
    if (temperatue < MIN_TEMPERATURE || temperatue > MAX_TEMPERATURE) {
        return false;
    }
    return true;
}

/**
 * @brief Gets the user input - altitude and temperature
 *
 * @param altitude Pointer to store the valid altitude value
 * @param temperature Pointer to store the valid temperature value
 */
void get_altitude_temperature(double *altitude, double *temperature) {
    while (true) {
        printf("Enter Altitude [space] Temperature: ");
        scanf("%lf %lf", altitude, temperature);
        if (is_valid_ps31_input(*altitude, *temperature)) {
            break;
        }
        printf("Invalid Values Entered, re-enter the values\n");
    }
}

/**
 * @brief Searches for a value in the consts array declared above
 *
 * @param val The value to search for
 * @param arr Pointer to either const altitudes/temperatures array
 * @param len The length of the array
 * @return int The index of the first element in the array greater than or equal to val
 */
int search(double val, const double *arr, int len) {
    for (int i = 0; i < len; ++i) {
        if (arr[i] >= val) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Performs linear interpolation to estimate a y-value for a given x-value within a range
 *
 * @param x The x-value for which to estimate the y-value
 * @param x1 The lower bound of the x-range
 * @param y1 The corresponding y-value at x1
 * @param x2 The upper bound of the x-range
 * @param y2 The corresponding y-value at x2
 * @return double The estimated y-value at 'x'
 */
double interpolate(double x, double x1, double y1, double x2, double y2) {
    double m = (y2 - y1) / (x2 - x1);
    return y1 + m * (x - x1);
}

/**
 * @brief Calculates Climb Performance using bilinear-interpolation
 *
 * @param altitude
 * @param temperature
 * @return double
 */
double calculate_climb_performance(double altitude, double temperature) {
    double climb_rate_prev, climb_rate_next;

    // search index of altitude and temperature
    // if not present then it'll store next highest values
    int alt_idx = search(altitude, altitudes, NUM_ALTITUDES);
    int temp_idx = search(temperature, temperatures, NUM_TEMPS);

    // If altitude and temperature is in the climb rate table
    if ((altitudes[alt_idx] == altitude) && (temperatures[temp_idx] == temperature)) {
        return rate_of_climb_table[alt_idx][temp_idx];
    }

    // if the altitude is present but the temperature is not present in the climb rate table
    // iterpolate along column
    if (altitudes[alt_idx] == altitude) {
        return interpolate(
            temperature,
            temperatures[temp_idx - 1],
            rate_of_climb_table[alt_idx][temp_idx - 1],
            temperatures[temp_idx],
            rate_of_climb_table[alt_idx][temp_idx]);
    }

    // at this point altitude is not there is climb rate table
    // alt_idx would point to next highest altitude

    // if the temperature is not in the climb rate table
    // interpolate along column at sea level i.e. index 0, and
    // interpolate along column at next highest altitude
    if (temperatures[temp_idx] != temperature) {
        climb_rate_prev = interpolate(
            temperature,
            temperatures[temp_idx - 1],
            rate_of_climb_table[alt_idx - 1][temp_idx - 1],
            temperatures[temp_idx],
            rate_of_climb_table[alt_idx - 1][temp_idx]);

        climb_rate_next = interpolate(
            temperature,
            temperatures[temp_idx - 1],
            rate_of_climb_table[alt_idx][temp_idx - 1],
            temperatures[temp_idx],
            rate_of_climb_table[alt_idx][temp_idx]);
    } else {
        // temperature column is there in climb rate table but the altitude doesn't match
        climb_rate_prev = rate_of_climb_table[alt_idx - 1][temp_idx];
        climb_rate_next = rate_of_climb_table[alt_idx][temp_idx];
    }
    // interpolate along row
    return interpolate(
        altitude,
        altitudes[alt_idx - 1],
        climb_rate_prev,
        altitudes[alt_idx],
        climb_rate_next);
}
