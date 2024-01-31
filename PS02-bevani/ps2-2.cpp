/**
 * @file ps2-1.cpp
 * @author Balasubramanyam Evani (AndrewId: bevani)
 * @brief
 *
 * Solution for the PS2-2 Digital Flashcards of Multiplication
 * The main function here does the following in sequential order:
 * 1. Sets random seed based on current time
 * 2. Gets the number of flashcards - this needs to be within predetermined range or else loop
 *    till user enters valid value
 * 3. Generates the needed number of flashcards
 * 4. Accepts user answer and compares with expected answer
 * 5. Finally calculates certain metrics related to questions correctly answered and
 *    time taken to answer the questions
 *
 * NOTE: One of the primary assumption is that the user will give appropriate int input to the
 * program. Giving anything other input will most probably result in the program crashing
 *
 * @date 2023-09-08
 */
#include <chrono>

#include "ps2.h"
using namespace std::chrono;

/**
 * @brief Main function
 * @return int exit status code
 */
int main(void) {
    // Initializes random seed for random number generation
    set_random_num_seed();

    // Initializes variables for the program
    int user_answer, total_questions;
    int total_correct_answers = 0;
    double correct_percentage = 0.0;
    double total_time_taken = 0.0;

    // Gets the number of flashcards and calculates total questions
    get_num_flashcards(&total_questions);

    // Generates random flashcards and checks for memory allocation error
    // if there's a memory allocation error then abnormally quit the program
    flashcard_t **cards = gen_rnd_flashcards(total_questions);
    if (cards == NULL) {
        printf("Abnormal Exit - Couldn't allocate memeory for flashcards\n");
        exit(1);
    }

    // Loops through the flashcards
    for (int i = 0; i < total_questions; ++i) {
        if (cards[i] == NULL) {
            destroy_flashcards(cards, total_questions);
            printf("Abnormal Exit - Invalid card encountered\n");
            exit(1);
        }
        // Measures time to answer the question and displays the effect flashcard number and question
        // and asks user to input answer for comparison
        auto start = steady_clock::now();
        if (cards[i]->number->high >= 1 && cards[i]->number->high <= 9) {
            printf("[%d. Card value: 0%d, Question] %d x %d = ", i + 1, cards[i]->number->val, cards[i]->number->high, cards[i]->number->low);
        } else {
            printf("[%d. Card value: %d, Question] %d x %d = ", i + 1, cards[i]->number->val, cards[i]->number->high, cards[i]->number->low);
        }
        scanf("%d", &user_answer);
        auto stop = steady_clock::now();
        duration<double> diff = (stop - start);
        total_time_taken += diff.count();
        if (is_answer_correct(cards[i]->answer, user_answer)) {
            // Correct Answer
            printf("[Result] Your answer %d is correct!\n", user_answer);
            ++total_correct_answers;
        } else {
            // Incorrect answer
            printf("[Result] Expected Correct Answer is %d, your answer was %d which is incorrect\n", cards[i]->answer, user_answer);
        }
        printf("------------\n");
    }

    // Destroy flashcards i.e. free up memory allocated to store flashcard_t struct
    destroy_flashcards(cards, total_questions);

    // Calculates the percentage of questions answered correctly
    // Would not result in divide by 0 since total questions always >= MIN_FLASH_CARDS (= 1)
    correct_percentage = ((double)total_correct_answers / (double)total_questions) * 100;

    // Print overall results
    printf("You answered %d problems in %.2lf seconds.", total_questions, total_time_taken);
    printf("You answered %d problems correctly (%.2lf%%)\n", total_correct_answers, correct_percentage);
    return 0;
}
