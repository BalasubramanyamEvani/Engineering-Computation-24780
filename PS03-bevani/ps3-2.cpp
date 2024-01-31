/**
 * @file ps3-2.cpp
 * @author Balasubramanyam Evani (bevani@andrew.cmu.edu)
 * @brief PS3-2 Solution
 *
 * @date 2023-09-16
 */

#define GL_SILENCE_DEPRECATION

#include <iostream>

#include "fssimplewindow.h"

using namespace std;

// Constants for screen dimensions and scaling factors
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const double ORIGIN_X = 50;
const double ORIGIN_Y = 1500;
const double SCALE_X = 5.0;
const double SCALE_Y = 0.6;

// Constants for colors
const uint8_t ENVELOPE_COLOR_R = 0;
const uint8_t ENVELOPE_COLOR_G = 0;
const uint8_t ENVELOPE_COLOR_B = 0;
const uint8_t RES_POINT_COLOR_R = 0;
const uint8_t RES_POINT_COLOR_G = 0;
const uint8_t RES_POINT_COLOR_B = 0;

// Constants for envelope vertices
const int ENVELOPE_NUM_VERTICES = 5;
const double ENVELOPE_X[5] = {52.0, 67.0, 97.0, 116.0, 71.0};
const double ENVELOPE_Y[5] = {1500.0, 1950.0, 2450.0, 2450.0, 1500.0};

// Constants for Cessna 172 specifications
const double CESSNA_172_BASIC_EMPTY_WEIGHT = 1639.0;
const double CESSNA_172_EMPTY_MOMENT = 64.4;
const double ARM_FRONT_SEAT = 37.0;
const double ARM_REAR_SEAT = 73.0;
const double ARM_BAGGAGE_AREA1 = 95.0;
const double ARM_BAGGAGE_AREA2 = 123.0;
const double ARM_FUEL_TANK = 48.0;
const double MAX_BAGGAGE_AREA1_WEIGHT = 120.0;
const double MAX_BAGGAGE_AREA2_WEIGHT = 50.0;
const double FUEL_WEIGHT_PER_GALLON = 6.0;
const double FUEL_ALLOWANCE_WEIGHT = 7.0;
const double FUEL_ALLOWANCE_MOMENT = 0.3;

/**
 * @brief Calculates scaled x-coordinate.
 *
 * @param x Input x-coordinate.
 *
 * @return Scaled x-coordinate.
 */
double calc_scaled_xd(double x) {
    double delta_x = 0.0, res = 0.0;
    delta_x = x - ORIGIN_X;
    res = SCALE_X * delta_x;
    return res;
}

/**
 * @brief Calculates scaled y-coordinate.
 *
 * @param y Input y-coordinate.
 *
 * @return Scaled y-coordinate.
 */
double calc_scaled_yd(double y) {
    double delta_y = 0.0, res = 0.0;
    delta_y = y - ORIGIN_Y;
    res = SCREEN_HEIGHT - (SCALE_Y * delta_y);
    return res;
}

/**
 * @brief Initializes the graphics window.
 */
void init_window() {
    FsOpenWindow(50, 50, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * @brief Draws a point on the graphics window.
 *
 * @param cx x-coordinate of the point.
 * @param cy y-coordinate of the point.
 */
void draw_point(double cx, double cy) {
    glPointSize(4);
    glBegin(GL_POINTS);
    glVertex2d(cx, cy);
    glEnd();
}

/**
 * @brief Draws the envelope and a point on it.
 *
 * @param px x-coordinate of the point.
 * @param py y-coordinate of the point.
 */
void draw_envelope_and_point(double px, double py) {
    glColor3ub(ENVELOPE_COLOR_R, ENVELOPE_COLOR_G, ENVELOPE_COLOR_B);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < ENVELOPE_NUM_VERTICES; ++i) {
        glVertex2d(
            calc_scaled_xd(ENVELOPE_X[i]),
            calc_scaled_yd(ENVELOPE_Y[i]));
    }
    glEnd();

    glColor3ub(RES_POINT_COLOR_R, RES_POINT_COLOR_G, RES_POINT_COLOR_B);
    draw_point(calc_scaled_xd(px), calc_scaled_yd(py));
}

/**
 * @brief Takes user input for weight and fuel.
 *
 * @param weight_front_seat Weight of the front-seat pilot/passenger.
 * @param weight_rear_seat_passenger Weight of the rear-seat passenger.
 * @param weight_baggage_area1 Weight of baggage in area 1.
 * @param weight_baggage_area2 Weight of baggage in area 2.
 * @param gallons_fuel Amount of fuel in gallons.
 */
void take_user_input(
    double &weight_front_seat,
    double &weight_rear_seat_passenger,
    double &weight_baggage_area1,
    double &weight_baggage_area2,
    double &gallons_fuel
) {
    cout << "Enter Weight of the Front-Seat Pilot/Passenger in pounds: ";
    cin >> weight_front_seat;

    cout << "Enter Weight of the Rear-Seat Passenger in pounds: ";
    cin >> weight_rear_seat_passenger;

    cout << "Enter Weight of the Baggage in Area 1 in pounds: ";
    cin >> weight_baggage_area1;

    cout << "Enter Weight of the Baggage in Area 2 in pounds: ";
    cin >> weight_baggage_area2;

    cout << "Enter fuel in Gallons: ";
    cin >> gallons_fuel;
}

/**
 * @brief Calculates the plane's takeoff weight.
 *
 * @param weight_front_seat Weight of the front-seat pilot/passenger.
 * @param weight_rear_seat_passenger Weight of the rear-seat passenger.
 * @param weight_baggage_area1 Weight of baggage in area 1.
 * @param weight_baggage_area2 Weight of baggage in area 2.
 * @param gallons_fuel Amount of fuel in gallons.
 *
 * @return Plane's takeoff weight.
 */
double calculate_plane_takeoff_weight(
    double weight_front_seat,
    double weight_rear_seat_passenger,
    double weight_baggage_area1,
    double weight_baggage_area2,
    double gallons_fuel) {
    double weight_take_off = 0.0;
    gallons_fuel *= FUEL_WEIGHT_PER_GALLON;
    weight_take_off = CESSNA_172_BASIC_EMPTY_WEIGHT + weight_front_seat + weight_rear_seat_passenger + weight_baggage_area1 + weight_baggage_area2 + gallons_fuel;

    weight_take_off = weight_take_off - FUEL_ALLOWANCE_WEIGHT;
    return weight_take_off;
}


/**
 * @brief Calculates the plane's takeoff moment.
 *
 * @param weight_front_seat Weight of the front-seat pilot/passenger.
 * @param weight_rear_seat_passenger Weight of the rear-seat passenger.
 * @param weight_baggage_area1 Weight of baggage in area 1.
 * @param weight_baggage_area2 Weight of baggage in area 2.
 * @param gallons_fuel Amount of fuel in gallons.
 *
 * @return Plane's takeoff moment.
 */
double calculate_plane_takeoff_moment(
    double weight_front_seat,
    double weight_rear_seat_passenger,
    double weight_baggage_area1,
    double weight_baggage_area2,
    double gallons_fuel) {
    double moment_fuel = (ARM_FUEL_TANK * gallons_fuel * FUEL_WEIGHT_PER_GALLON) / 1000;
    double moment_front_seat = (ARM_FRONT_SEAT * weight_front_seat) / 1000;
    double moment_rear_seat = (ARM_REAR_SEAT * weight_rear_seat_passenger) / 1000;
    double moment_baggage_area1 = (ARM_BAGGAGE_AREA1 * weight_baggage_area1) / 1000;
    double moment_baggage_area2 = (ARM_BAGGAGE_AREA2 * weight_baggage_area2) / 1000;
    double moment_take_off = CESSNA_172_EMPTY_MOMENT + moment_fuel + moment_front_seat + moment_rear_seat + moment_baggage_area1 + moment_baggage_area2 - FUEL_ALLOWANCE_MOMENT;

    return moment_take_off;
}

// Main Function
int main(void) {
    double weight_front_seat = 0.0, weight_rear_seat_passenger = 0.0;
    double weight_baggage_area1 = 0.0, weight_baggage_area2 = 0.0, gallons_fuel = 0.0;
    double weight_take_off = 0.0, moment_take_off = 0.0;

    take_user_input(
        weight_front_seat,
        weight_rear_seat_passenger,
        weight_baggage_area1,
        weight_baggage_area2,
        gallons_fuel);

    weight_take_off = calculate_plane_takeoff_weight(
        weight_front_seat,
        weight_rear_seat_passenger,
        weight_baggage_area1,
        weight_baggage_area2,
        gallons_fuel);

    moment_take_off = calculate_plane_takeoff_moment(
        weight_front_seat,
        weight_rear_seat_passenger,
        weight_baggage_area1,
        weight_baggage_area2,
        gallons_fuel);

    init_window();
    draw_envelope_and_point(moment_take_off, weight_take_off);
    
    // Finally Draw since using single buffer mode
    glFlush();
    
    // Polls Device to listen for ESC Key
    for (;;) {
        FsPollDevice();
        auto key = FsInkey();
        if (key == FSKEY_ESC) {
            break;
        }
        FsSleep(10);
    }
    return 0;
}
