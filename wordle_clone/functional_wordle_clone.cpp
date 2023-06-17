#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;

bool get_word_list(std::string fileName, vector<string> & vecOfStrs)
{
    // Open the File
    std::ifstream in(fileName.c_str());
    // Check if object is valid
    if(!in)
    {
        std::cerr << "Cannot open the File : "<<fileName<<std::endl;
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
    //Close The File
    in.close();

    return true;
}

bool get_letters_rating(string guessed_word, string word_of_the_day, vector<int> & ratings_array) {
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

int play_game() {
    vector<string> wordle_words;
    vector<string> all_words;
    bool wordle_result = get_word_list("/home/michele/Documents/personal/experiments/Wordles.txt", wordle_words);
    bool result = get_word_list("/home/michele/Documents/personal/experiments/five_letter_words_stanford.txt", all_words);  // /usr/share/dict/british-english

    srand(time(0));  // change the seed each time or we get the same result
    int random = rand() % wordle_words.size();
    string word_of_the_day = wordle_words[random];
    // string word_of_the_day = "naval";

    // cout << word_of_the_day << '\n';

    if (!result) {
        cout << "could not find dictionary of words, double check your path";
        return 0;
    }

    cout << "You are playing THE GAME!\n 0 means you suck\n 1 means I'm not angry just disappointed\n 2 means you've done your job, do you want me to clap for you? you've still not guessed the word\n Go on enter your first guess bet you'll get it wrong anyway:\n";
    for (int i=0; i<6; i++) {
        string my_guess;

        bool acceptable_guess = false;
        while (!acceptable_guess) {
            cin >> my_guess;

            if (my_guess.size() != 5) {
                cout << "maybe try writing a word with 5 letters to start with eh\n";
                continue;
            }

            bool word_in_voc = false;
            for (int j=0; j<all_words.size(); j++) {
                if (my_guess == all_words[j]) {
                    word_in_voc = true;
                    break;
                }
            }
            if (!word_in_voc) {
                cout << "THAT'S NOT EVEN A WORD!\n";
                continue;
            }

            acceptable_guess = true;
        }

        vector<int> ratings_array;
        get_letters_rating(my_guess, word_of_the_day, ratings_array);

        if (( all_of(begin(ratings_array), end(ratings_array), [](int i){return i == 2;}) )) {
            cout << "You won, Jane. Enjoy the money, I hope it makes you happy. Dear lord, what a sad little life, Jane. You ruined my night completely so you could have the money and I hope now you can spend it on lessons in grace and decorum. Because you have all the grace of a reversing dump truck without any tyres on. So Jane, take your money and get off my property\n";
            return 1;
        }

        cout << "HAHA WRONG!\n";
        for (int i=0; i<5; i++) {
            cout << ratings_array[i];
        }
        cout << '\n';
    }

    cout << "you dummy you couldn't even guess a five letter word hahahah the word was OBVIOUSLY " << word_of_the_day << '\n';
}



int main() {
    play_game();
}
