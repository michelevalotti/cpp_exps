#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "guesser.cpp"
using namespace std;


Wordle::Wordle(std::string all_words_fn, std::string guessable_words_fn)
{
    all_words_filepath = all_words_fn;
    guessable_words_filepath = guessable_words_fn;
    bool _ = get_words_from_file(all_words_filepath, every_word);
}


bool Wordle::get_words_from_file(std::string words_filepath, vector<string> & vecOfStrs)
{
    std::ifstream in(words_filepath.c_str());
    // Check if object is valid
    if(!in)
    {
        std::cerr << "Cannot open the File : "<<words_filepath<<std::endl;
        return false;
    }
    std::string str;
    // Read the next line from File untill it reaches the end.
    while (std::getline(in, str))
    {
        if (str.size() != 5) {
            continue;
        }
        bool add_to_list = true;
        for (int i=0; i<str.size(); i++) {
            if (not (str[i] >= 97) && (str[i] <= 122)) {
                add_to_list = false;
                break;
            }
        }
        if (add_to_list) {
            vecOfStrs.push_back(str);
        }
    }
    in.close();

    return true;
}


std::string Wordle::get_word_of_the_day()
{
    vector<string> wordle_words;
    bool wordle_result = get_words_from_file(guessable_words_filepath, wordle_words);
    int random = rand() % wordle_words.size();
    string word_of_the_day = wordle_words[random];

    return word_of_the_day;
}


bool Wordle::guess_is_valid(std::string guessed_word)
{
    if (guessed_word.length() != 5){
        return false;
    }
    bool word_in_voc = false;
    for (int j=0; j<every_word.size(); j++) {
        if (guessed_word == every_word[j]) {
            word_in_voc = true;
            break;
        }
    }

    return word_in_voc;
}


bool Wordle::get_letters_rating(std::string guessed_word, vector<int> & ratings_array)
{
    ratings_array = {0,0,0,0,0};

    for (int i=0; i<5; i++) {
        char correct_letter = word_of_the_day[i];            
        int max_rating = 0;
        int right_pos = 0;
        for (int j=0; j<5; j++) {
            char my_letter = guessed_word[j];
            if (my_letter == correct_letter) {
                if (i==j) {
                    max_rating = 2;
                    right_pos = j;
                    break;
                }
                if (ratings_array[j] < 1) {
                    max_rating = 1;
                    right_pos = j;
                }
            }
        }
        if (max_rating>ratings_array[right_pos]) {
            ratings_array[right_pos] = max_rating;
        }
    }

    return true;
}

template <typename T>
int Wordle::get_n_guesses(T &g)  // pass ref or changes to the guess will be only in local scope -- this was causing the memory leak since these local vars were not reset when Guess was constructed again in the main loop
{
    word_of_the_day = get_word_of_the_day();

    int n_guesses_taken = 0;
    bool found_word = false;

    vector<int> guess_ratings = {-1,-1,-1,-1,-1};

    for (int i=0; i<6; i++){
        n_guesses_taken++;

        std::string this_guess;
        bool acceptable_guess = false;
        while (!acceptable_guess) {
            bool guess_success = g.create_guess();
            this_guess = g.current_guess;
            acceptable_guess = guess_is_valid(this_guess);
            if (!acceptable_guess) {cout << "invalid guess, you still have " << 6-i << " guesses left\n";}
        }

        g.submitted_guesses.push_back(this_guess);
        bool score_success = get_letters_rating(this_guess, guess_ratings);
        for (int k=0; k<5; k++) {g.scores_of_submitted.push_back(guess_ratings[k]);}

        int score_sum = 0;
        for (int j=0; j<guess_ratings.size() ; j++) {
            score_sum+=guess_ratings[j];
        }
        if (score_sum == 10) {
            found_word = true;
            break;
        }
    }

    if (!found_word) {
        return -1;
    }
    return n_guesses_taken;
}



int main(int argc, char *argv[])
{
    std::string help_str = "use --human to play the game\n/use --random to let a random solver play it for you\nuse --artificial-human to play using the artificial-human solver, which plays as a human would\nuse --sorted-human to make the artificail-human solver sort words by most important (common) letters (repeat letters in a word become less important)\nuse -h or --help to display this message\n";

    if (argc != 2) {
        cout << help_str;
        return 0;
    }

    std::string player_mode = argv[1];
    if ((argc==2 && player_mode=="-h") || (argc==2 && player_mode=="--help")) {
        cout << help_str;
        return 0;
    }

    srand(time(0));  // set random seed so we don't get same rand() output each rerun -- this has to be run only once at the start of the prog or it will produce the same rand() o/p

    std::string dict_fn = "./words_12792.txt";  // "./five_letter_words_stanford.txt";
    std::string guessable_words_fn = "./words_2315.txt";  // "./Wordles.txt";

    Wordle game(dict_fn, guessable_words_fn);

    if (player_mode=="--random" || player_mode=="--artificial-human" || player_mode=="--sorted-human") {

        float total_guesses = 1000;
        float correct_guesses = 0;
        float avg_n_guess = 0;
        for (int i=0; i<total_guesses; i++) {
            int n;
            if (player_mode=="--random") {
                RandomGuess guess(game.every_word);
                n = game.get_n_guesses<RandomGuess>(guess);
            }
            if (player_mode=="--artificial-human") {
                ArtificialHumanGuess guess(game.every_word);
                n = game.get_n_guesses<ArtificialHumanGuess>(guess);
            }
            if (player_mode=="--sorted-human") {
                SortedHumanGuess guess(game.every_word);
                n = game.get_n_guesses<SortedHumanGuess>(guess);
            }
            cout << "\rprogress: " << float ((int ((i+1)/total_guesses*1000)))/10 << '%' << " (" << i+1 << '/' << total_guesses << " guesses)  " << std::flush;  // spaces at the end are to make sure whole line is overwritten

            if (n != -1) {
                correct_guesses ++;
                avg_n_guess += n;
            }
        }
        cout << "\naccuracy: " << correct_guesses/total_guesses << "    avg number of guesses when correct: " << avg_n_guess/correct_guesses << '\n';

    }
    else if (player_mode=="--human") {
        HumanGuess guess;
        std::cout << "enter your first 5 letter word guess\n";
        int n = game.get_n_guesses<HumanGuess>(guess);
        if (n==-1) {
            cout << "unlucky! the word I was looking for was: " << game.word_of_the_day << '\n';
        }
        else {
            cout << "Congratulations! That's the word I was looking for!" << '\n';
        }
    }
    else {
        cout << "player option not recognised, use one of --human, --random, --artificial-human, --sorted-human\n";
    }
    return 0;
}