/**
 * @file ps2.hs
 * @author Balasubramanyam Evani (AndrewId: bevani)
 * @brief
 *
 * Header file for ps2 assignment for 24-780
 * Contains utility functions used in q1 and q2
 *
 * @date 2023-09-08
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constants
#define MIN_FLASH_CARDS 1
#define MAX_FLASH_CARDS 144
#define NUM_DAYS 7

// ps2-1 day names
const char *day_names[NUM_DAYS] = {
    "Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"};

// ps2-1 food choices
const char *food_choices[NUM_DAYS] = {
    "Pizza from Mercurio's",
    "Dosa from Tamarind",
    "Spring Roll from Ocha Thai",
    "Sub from Uncle Subway",
    "Fried rice from Hunan",
    "Sandwiches from Skibo Cafe",
    "Find free food on campus"};

/**
 * Struct for storing high and low two digits of a 4 digit number
 * if high is a single digit then assume a leading zero
 */
typedef struct four_digit_number {
    int val;
    int low;
    int high;
} four_digit_number_t;

/**
 * Struct for representing a flashcard
 */
typedef struct flashcard {
    four_digit_number_t *number;
    int answer;  // The correct answer associated with the flashcard
} flashcard_t;

/**
 * @brief Set the seed for generating random numbers based on
 * current time
 */
void set_random_num_seed() {
    srand((unsigned int)time(NULL));
}

/**
 * @brief Prints the output required by Q1 of PS2
 *
 * @param days const array of names of days in a week
 * @param foods const array of names of food items
 * @param len equal to number of days in a week
 */
void ps2_1_output(const char *days[], const char *foods[], const int len) {
    for (int i = 0; i < len; ++i) {
        printf("%s %s\n", days[i], foods[i]);
    }
}

/**
 * @brief Swap the memory addresses which points to two const strings.
 *
 * @param a pointer to pointer which points to a const string
 * @param b pointer to pointer which points to a const string
 */
void swap_char(const char **a, const char **b) {
    const char *tmp = *a;
    *a = *b;
    *b = tmp;
}

/**
 * @brief Randomly Shuffle an array of const string
 *
 * @param arr string array
 * @param len length of string array
 */
void rnd_shuffle(const char *arr[], const int len) {
    for (int i = 0; i < len; ++i) {
        int rnd_index = rand() % len;  // won't crash because its a fixed value > 0 passed from main
        swap_char(&arr[i], &arr[rnd_index]);
    }
}

/**
 * @brief Generates a random number for elementary multiplications in range 1x1 to 12 x 12
 *
 * @return returns a pointer to random 4 digit int (for 3 digit an implied leading zero is assumed)
 */
four_digit_number_t *generate_random_4_digit_number() {
    int low, high;
    low = rand() % 12 + 1;   // won't crash since taking mod with respect to const 12
    high = rand() % 12 + 1;  // won't crash since taking mod with respect to const 12
    four_digit_number_t *number = (four_digit_number_t *)malloc(sizeof(four_digit_number_t));
    number->low = low;
    number->high = high;
    number->val = high * 100 + low;
    return number;
}

/**
 * @brief Swap the memory addresses which points to two flashcard_t structs.
 *
 * @param a pointer to pointer which points to a flashcard_t struct
 * @param b pointer to pointer which points to a flashcard_t struct
 */
void swap_flashcard(flashcard_t **a, flashcard_t **b) {
    flashcard_t *tmp = *a;
    *a = *b;
    *b = tmp;
}

/**
 * @brief Randomly Shuffle an array of flashcard structs
 *
 * @param cards flashcard_t struct array
 * @param len length of flashcard_t struct array
 */
void shuffle_flashcards(flashcard_t **cards, int len) {
    for (int i = 0; i < len; ++i) {
        int rnd_index = rand() % len;  // won't crash because the number of needed cards is checked before this is called
        swap_flashcard(&cards[i], &cards[rnd_index]);
    }
}

/**
 * @brief Checks if a random number is unique among the existing flashcards
 *
 * @param rnd_num The random number to check for uniqueness (high and low will be compared)
 * @param cards An array of flashcard_t pointers to compare against
 * @param len The length of the array
 * @return true if rnd_num is unique among the existing flashcards, false otherwise
 */
bool is_unique(four_digit_number_t *rnd_num, flashcard_t **cards, int len) {
    int i = 0;
    while (cards[i] != NULL && i < len) {
        if (cards[i]->number->val == rnd_num->val) {
            return false;
        }
        ++i;
    }
    return true;
}

/**
 * @brief Generates an array of random flashcards with unique 4 digit numbers (for 3 digit an implied leading
 * 0 is assumed) for elementary multiplication
 *
 * @param needed The number of flashcards to generate
 * @return A pointer to an array of flashcard_t pointers containing the generated flashcards
 *         Returns NULL if memory allocation fails
 */
flashcard_t **gen_rnd_flashcards(const int needed) {
    flashcard_t **cards = (flashcard_t **)malloc(sizeof(flashcard_t *) * needed);
    if (cards == NULL) {
        return NULL;
    }
    for (int i = 0; i < needed; ++i) {
        cards[i] = NULL;
    }
    int i = 0;
    while (i < needed) {
        four_digit_number_t *rnd_number = generate_random_4_digit_number();
        if (is_unique(rnd_number, cards, needed)) {
            cards[i] = (flashcard_t *)malloc(sizeof(flashcard_t));
            cards[i]->number = rnd_number;
            cards[i]->answer = cards[i]->number->high * cards[i]->number->low;
            ++i;
        } else {
            // Free allocated space for rnd number if already present in flashcards deck
            free(rnd_number);
        }
    }
    shuffle_flashcards(cards, needed);
    return cards;
}

/**
 * @brief Destroys an array of flashcards and frees the allocated memory
 *
 * @param cards An array of flashcard_t pointers to be destroyed
 * @param needed The number of flashcards in the array
 */
void destroy_flashcards(flashcard_t **cards, const int needed) {
    for (int i = 0; i < needed; ++i) {
        if (cards[i] != NULL) {
            free(cards[i]->number);
            free(cards[i]);
        }
    }
    free(cards);
}

/**
 * @brief Checks if a given number of flashcards is within the valid range
 *
 * @param num The number of flashcards to generate
 * @return true if the number is within the valid range, false otherwise
 */
bool is_valid_flashcard_count(int num) {
    return num >= MIN_FLASH_CARDS && num <= MAX_FLASH_CARDS;
}

/**
 * @brief Prompts the user for the number of flashcards to work on until a valid input is provided
 *
 * @param num_cards_to_show A pointer to an integer where the selected number of flashcards will be stored
 */
void get_num_flashcards(int *num_cards_to_show) {
    while (true) {
        printf("How many flash cards to work on? ");
        scanf("%d", num_cards_to_show);
        if (is_valid_flashcard_count(*num_cards_to_show)) {
            break;
        }
        printf("The number of cards must be between 1 and 144.\n");
    }
}

/**
 * @brief Checks if the entered answer matches the expected answer for a flashcard
 *
 * @param expected The expected answer
 * @param entered The entered answer by the user
 * @return true if the entered answer is correct, false otherwise
 */
bool is_answer_correct(int expected, int entered) {
    return expected == entered;
}
