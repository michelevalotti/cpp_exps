#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "game.h"
using namespace std;




bool HumanGuess::create_guess()
{
    int size_of_v = scores_of_submitted.size();
    if (size_of_v > 0) {
        for (int i=0; i<5; i++){
            std::cout << scores_of_submitted[size_of_v-5+i];
        }
    }
    std::cout << "\n";
    std::cin >> current_guess;
    return true;
}



RandomGuess::RandomGuess(std::vector<std::string> available_wrds)
{
    available_words = available_wrds;
}


bool RandomGuess::choose_from_filtered_list()
{
    int random = rand() % available_words.size();
    current_guess = available_words[random];

    return true;
}

bool RandomGuess::filter_guessable_list()
{
    std::vector<std::string> words_no_guess;
    for (int i=0; i<available_words.size(); i++) {
        if (available_words[i] != current_guess) {
            words_no_guess.push_back(available_words[i]);
        }
    }
    available_words = words_no_guess;

    return true;
}

bool RandomGuess::create_guess()
{
    if (submitted_guesses.size() > 0) {
        filter_guessable_list();
    }
    choose_from_filtered_list();
    
    return true;
}



bool ArtificialHumanGuess::filter_guessable_list()
{
    std::vector<std::string> filtered_words;        


    for (int i=0; i<available_words.size(); i++) {

        std::string current_word = available_words[i];

        if (current_word == current_guess) {continue;}
        if (!word_is_letter_matched(current_word)) {continue;}
        
        filtered_words.push_back(available_words[i]);
    }

    available_words = filtered_words;

    return true;
}

bool ArtificialHumanGuess::word_is_letter_matched(std::string word)
{
    std::vector<int> current_scores;
    for (int s=0; s<5; s++) {
        current_scores.push_back(scores_of_submitted[scores_of_submitted.size()-5+s]);
    }
    vector<char> required_chars;
    vector<char> not_allowed_chars;
    for (int l=0; l<5; l++) {
        if (current_scores[l] == 0) {not_allowed_chars.push_back(current_guess[l]);}
        if (current_scores[l] > 0) {required_chars.push_back(current_guess[l]);}
    }
    // remove from not allowed chars that in both arrays (e.g. when guess has two same letters but right word only one)
    vector<char> not_allowed_chars_remove_req_chars;
    for (int t=0; t<not_allowed_chars.size(); t++) {
        bool keep_in_array = true;
        for (int l=0; l<required_chars.size(); l++) {
            if (not_allowed_chars[t] == required_chars[l]) {keep_in_array=false;}
        }
        if (keep_in_array) {not_allowed_chars_remove_req_chars.push_back(not_allowed_chars[t]);}
    }
    not_allowed_chars = not_allowed_chars_remove_req_chars;

    // if word has letter with score 0, return false
    for (int l=0; l<5; l++) {
        for (int t=0; t<not_allowed_chars.size(); t++) {
            if (not_allowed_chars[t] == word[l]) {return false;}
        }
    }

    // if word does not have letter with score > 0, return false
    for (int l=0; l<required_chars.size(); l++) {
        bool required_char_in_word = false;
        for (int t=0; t<5; t++) {
            if (required_chars[l] == word[t]) {required_char_in_word=true; break;}
        }
        if (!required_char_in_word) {return false;}
    }

    // if letters with score 2 are in different place, or letters with score 1 are in the same place, return false
    for (int gli=0; gli<5; gli++) {
        if (current_scores[gli] == 2) {
            if (word[gli] != current_guess[gli]) {return false;}
        }
        if (current_scores[gli] == 1) {
            if (word[gli] == current_guess[gli]) {return false;}
        }
    }

    return true;
}



bool SortedHumanGuess::set_letter_importance(){
    for (int _=0; _<26; _++) {letter_importance.push_back(0);}
    for (int w=0; w<available_words.size(); w++) {
        for (int l=0; l<5; l++) {
            int letter_pos = int(tolower(available_words[w][l])) - 97;
            letter_importance[letter_pos] += 1;
        }
    }

    return true;
}

bool SortedHumanGuess::set_positional_letter_importance(){
    std::vector<std::vector<float>>plp(5, std::vector<float>(26,0));
    positional_letter_importance = plp;
    for (int w=0; w<available_words.size(); w++) {
        for (int l=0; l<5; l++) {
            int letter_pos = int(tolower(available_words[w][l])) - 97;
            positional_letter_importance[l][letter_pos] += 1;
        }
    }

    return true;
}

bool SortedHumanGuess::choose_from_filtered_list() {
    // if (submitted_guesses.size() == 0) {set_letter_importance();}
    if (submitted_guesses.size() == 0) {set_positional_letter_importance();}

    int max_score = -1;
    std::vector<string> best_words;
    for (int w=0; w<available_words.size(); w++) {
        int word_score = 0;
        int letter_scaling_facor = 2;
        std::vector<int> letter_score_scaling(26,1);  // this is so that if a word has more than one common letter, repeat letters do not carry as much weight
        for (int l=0; l<5; l++) {
            int letter_pos = int(tolower(available_words[w][l])) - 97;

            // word_score += letter_importance[letter_pos]/letter_score_scaling[letter_pos];
            word_score += positional_letter_importance[l][letter_pos]/letter_score_scaling[letter_pos];

            letter_score_scaling[letter_pos] *= letter_scaling_facor;
        }
        if (word_score > max_score) {
            best_words = {available_words[w]};
            max_score = word_score;
        }
        else if (word_score == max_score) {
            best_words.push_back(available_words[w]);
        }
    }

    int random = rand() % best_words.size();
    current_guess = best_words[random];

    return true;
}