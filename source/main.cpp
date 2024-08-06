#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

// type aliases

using Words = std::vector<std::string>;
using Pronunciations = std::vector<std::string>;
using Dictionary = std::unordered_map<std::string, Pronunciations>;
using PronunciationArray = std::vector<std::pair<std::string, Pronunciations>>;
using VowelMap = std::unordered_map<std::string, std::string>;
using VowelSet = std::unordered_set<std::string>;


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
VowelSet listVowels(const std::string& pronunciation) {
    std::istringstream iss(pronunciation);
    std::string symbol{};
    VowelSet vowel_set{};
    
    while (iss >> symbol) {
        if (!symbol.empty() && std::isdigit(symbol.back())) {
            symbol.pop_back();
        }
        if(cmuVowels.find(symbol) != cmuVowels.end()) {
            vowel_set.insert(symbol);
        }
    }

    return vowel_set;

}

void testQuery(const std::string& query, const Dictionary& dictionary) {
    auto it = dictionary.find(query);
    if (it != dictionary.end()) {
        std::cout << query << ":" << std::endl;
        for (const auto& transcription : it->second) {
            std::cout << transcription << std::endl;
            std::cout << "vowel count: " << countVowels(transcription) << std::endl;
            auto set{listVowels(transcription)};
            std::cout << "contains these vowels:";
            for (const auto& a : set) {
                std::cout << ' ' << a;
            }
            std::cout << std::endl;
        }
    } else {
        std::cout << query << " not found in the dictionary." << std::endl;
    }
    
}

std::string getUserText(){
    std::cout << "Input text: ";
    std::string input{};
    std::getline(std::cin, input);
    return input;
}

Words splitTextToWords(const std::string& text) {
    std::istringstream iss(text);
    std::string word{};
    Words words{};

    while (iss >> word) {
        words.push_back(word);
    }

    return words;
}

PronunciationArray wordsToPronunciations(Words words, Dictionary dictionary) {
    PronunciationArray pronunciation_array{};
    for (const auto& word : words) {
        Pronunciations pronunciations{};
        auto it = dictionary.find(word);
        if (it!= dictionary.end()) {
            for (const auto& pronunciation : it->second) {
                pronunciations.emplace_back(pronunciation);
            }
        }
        pronunciation_array.emplace_back(word, pronunciations);
    }

    return pronunciation_array;


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

    testQuery("FINANCE", dictionary);


    // define a vowel transformation

    // here's my test that compresses all vowels to "UH"
    
    VowelMap test_transformation{
        {"AA", "UH"},
        {"AE", "UH"},
        {"AH", "UH"},
        {"AO", "UH"},
        {"EH", "UH"},
        {"ER", "UH"},
        {"IH", "UH"},
        {"IY", "UH"},
        {"UH", "UH"},
        {"UW", "UH"},
        {"AW", "UH"},
        {"AY", "UH"},
        {"EY", "UH"},
        {"OW", "UH"},
        {"OY", "UH"},

    };


    // get user-text

    std::string user_text{getUserText()};
    auto words{splitTextToWords(user_text)};
    // check each word against dict, get pronunciation

    PronunciationArray pronunciation_array{wordsToPronunciations(words, dictionary)};

    // test print out array

    for (const auto& pair : pronunciation_array) {
        std::cout << pair.first << ":" << std::endl;
        for (const auto& a : pair.second) {
            std::cout << a << std::endl;
        }
    }

    // what to do with words not found in dict

    // get vowels

    // apply vowel transformation

    // transform ARPABET -> pronouncable text

    return 0;
}
