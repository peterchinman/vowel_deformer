#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

const std::unordered_set<std::string> cmuVowels = {
    "AA", "AE", "AH", "AO", "EH",
    "ER", "IH", "IY", "UH", "UW",
    "AW", "AY", "EY", "OW", "OY"
};

int countVowels(const std::string& pronunciation) {
    std::istringstream iss(pronunciation);
    std::string symbol;
    int vowel_count{};

    while (iss >> symbol) {
        if (!symbol.empty() && std::isdigit(symbol.back())) {
            symbol.pop_back();
        }

        if(cmuVowels.find(symbol) != cmuVowels.end()) {
            ++vowel_count;
        }
    }

    return vowel_count;
}

//returns an unordered_set of vowels from a pronunciation
std::unordered_set<std::string> listVowels(std::string& pronunciation) {
    std::istringstream iss(pronunciation);
    std::string symbol{};
    std::unordered_set<std::string> vowel_list{};
    
    while (iss >> symbol) {
        if (!symbol.empty() && std::isdigit(symbol.back())) {
            symbol.pop_back();
        }
        if(cmuVowels.find(symbol) != cmuVowels.end()) {
            vowel_list.insert(symbol);
        }
    }

    return vowel_list;

}


int main()
{
    // open CMU dict

    std::ifstream cmudict{"../data/cmudict-0.7b"};

    if (!cmudict.is_open()) {
        std::cerr << "Failed to open the dictionary." << std::endl;
        return 1;
    }



    // read CMU dict line-by-line into a std::unorderd_map with word, pronunciation

    std::unordered_map<std::string, std::vector<std::string>> dictionary{};
    std::string line;

    while (std::getline(cmudict, line)) {
        if (line.empty() || line[0] == ';') {
            continue;
        }

        std::istringstream iss(line);

        // extract word up to white space
        std::string word;
        iss >> word;

        // strip variation "(n)"
        std::size_t pos = word.find('(');
        if (pos != std::string::npos && pos!= 0) {
            word = word.substr(0, pos);
        }

         // pronunciations are ARPABET, separated by spaces
        // vowels end with a number indicating stress, 0 no stress, 1 primary stress, 2 secondary stress

        std::string pronunciation;
        std::getline(iss, pronunciation);
        // remove two leading spaces
        pronunciation = pronunciation.substr(2);

        dictionary[word].push_back(pronunciation);
    }


    // close cmudict

    cmudict.close();

    // test example

    std::string query{"FINANCE"};
    auto it = dictionary.find(query);
    if (it != dictionary.end()) {
        std::cout << query << ":" << std::endl;
        for (const auto& transcription : it->second) {
            std::cout << transcription << std::endl;
            std::cout << "vowel count: " << countVowels(transcription) << std::endl;
        }
    } else {
        std::cout << query << " not found in the dictionary." << std::endl;
    }

    // define a vowel transformation

    // get user-text

    // check each word against dict, get pronunciation

    // what to do with words not found in dict

    // get vowels

    // apply vowel transformation

    // transform ARPABET -> pronouncable text

    return 0;
}
