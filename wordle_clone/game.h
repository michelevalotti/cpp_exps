#include <iostream>
#include <string>
#include <vector>


struct Guess {
    std::vector<std::string> submitted_guesses = {};  // only contains accepted guesses
    std::vector<int> scores_of_submitted = {};
    std::string current_guess;  // this can be an invalid submission

    bool create_guess() {current_guess = "abcde";};
};


struct HumanGuess : public Guess {
    bool create_guess();
};

struct RandomGuess : public Guess {
    std::vector<std::string> available_words;

    RandomGuess(std::vector<std::string> available_wrds);

    bool create_guess();
    virtual bool filter_guessable_list();  // virtual allows this to be changes in children
    virtual bool choose_from_filtered_list();
};


struct ArtificialHumanGuess : public RandomGuess {
    ArtificialHumanGuess(std::vector<std::string> available_wrds) : RandomGuess(available_wrds) {};

    bool filter_guessable_list();
    bool word_is_letter_matched(std::string word);
};


struct SortedHumanGuess: public ArtificialHumanGuess {
    std::vector<float> letter_importance;  // has to be float because these are weighed by repetition (see choose_from_filtered_list)
    std::vector<std::vector<float>> positional_letter_importance;
    SortedHumanGuess(std::vector<std::string> available_wrds) : ArtificialHumanGuess(available_wrds) {};

    bool choose_from_filtered_list();
    bool set_letter_importance();
    bool set_positional_letter_importance();
};


struct Wordle {

    std::string word_of_the_day;
    std::vector<std::string> every_word;
    std::string all_words_filepath;
    std::string guessable_words_filepath;

    Wordle(std::string all_words_fn, std::string guessable_words_fn);

    static bool get_words_from_file(std::string words_filepath, std::vector<std::string> & vecOfStrs);
    std::string get_word_of_the_day();
    bool guess_is_valid(std::string guessed_word);
    bool get_letters_rating(std::string guessed_word, std::vector<int> & ratings_array);

    template <typename T>
    int get_n_guesses(T &g);

};