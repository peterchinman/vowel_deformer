#include <iostream>
#include <exception>
#include <fstream>
#include <iterator>
#include <limits>
#include <map>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

// type aliases


using Dictionary = std::unordered_map<std::string, std::vector<std::string>>;
// array of pairs, pair.first is the word pair.second is pronunciations
using PronunciationArray = std::vector<std::pair<std::string, std::vector<std::string>>>;
// map of arpa vowels from pair.first to pair.second for pair in VowelMap
using VowelMap = std::vector<std::pair<std::string, std::string>>;


// All vowels used in CMU
const std::unordered_set<std::string> cmuVowels = {
    "AA", "AE", "AH", "AO", "EH",
    "ER", "IH", "IY", "UH", "UW", // 10 monopthongs
    "AW", "AY", "EY", "OW", "OY" // 5 dipthongs
};

// converts arpa symbol to VowelMap index
std::size_t arpaToIndex(const std::string symbol) {
    std::vector<std::string> vowel_keys{
        "AA", "AE", "AH", "AO", "AW", "AY", 
        "EH", "ER", "EY", "IH", "IY", "OW", 
        "OY", "UH", "UW"
    };
    for (std::size_t i{}; i < vowel_keys.size(); ++i ) {
        if (vowel_keys[i] == symbol) {
            return i;
        }
    }
    throw std::runtime_error(symbol + " not found in vowel list");
}

std::string checkSymbol(const std::string& symbol) {
    std::unordered_map<std::string, std::string> respelling_map = {
        {"AA", "aw"},
        {"AE", "ah"},
        {"AH", "uh"},
        {"AO", "aw"},
        {"EH", "eh"},
        {"ER", "ur"},
        {"IH", "ih"},
        {"IY", "ee"},
        {"UH", "uu"},
        {"UW", "oo"},
        {"AW", "ow"},
        {"AY", "eye"},
        {"EY", "ay"},
        {"OW", "oh"},
        {"OY", "oy"},
        {"B", "b"},
        {"CH", "ch"},
        {"D", "d"},
        {"DH", "dh"},
        {"F", "f"},
        {"G", "g"},
        {"HH", "h"},
        {"JH", "j"},
        {"K", "k"},
        {"L", "l"},
        {"M", "m"},
        {"N", "n"},
        {"NG", "ng"},
        {"P", "p"},
        {"R", "r"},
        {"S", "s"},
        {"SH", "sh"},
        {"T", "t"},
        {"TH", "th"},
        {"V", "v"},
        {"W", "w"},
        {"WH", "wh"},
        {"Y", "y"},
        {"Z", "z"},
        {"ZH", "zh"}
    };

    return respelling_map.at(symbol);
}

// TODO handle accents
std::string arpaRespell(const std::string& pronunciation) {
    std::string respelling{};
    
    std::istringstream iss(pronunciation);
    std::string symbol;
    while (iss >> symbol) {
        std::string accent{};
        // check vowel, pop accent;
        if (!symbol.empty() && std::isdigit(symbol.back())) {
            accent = symbol.back();
            symbol.pop_back();
        }
        respelling += checkSymbol(symbol);
    }

    return respelling;
}



class CMU_Dict {
private:
    Dictionary m_dictionary {};

public:
    bool import_dictionary(std::string file_path) {
        std::ifstream cmudict{file_path};
        if (!cmudict.is_open()) {
            std::cerr << "Failed to open the dictionary." << '\n';
            return false;
        }
        std::string line;
        while (std::getline(cmudict, line)) {
            if(line.empty() || line[0] == ';') {
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

            m_dictionary[word].push_back(pronunciation);
        }

        cmudict.close();
        return true;
    }

    // throws a std::exception if query not found
    std::vector<std::string> search_dictionary(const std::string& query) {
        auto it = m_dictionary.find(query);
        if (it != m_dictionary.end()) {
            return it->second;
        }
        else {
            throw std::runtime_error(query + " not found in dictionary.");
        }
    }

    // vowels in cmudict end with a number as an accent indicator
    // we *could* just use that in this function as an indicator that a symbol is a vowel
    // instead here we strip the digit and then check against a list of vowels
    // is that helpfully robust, or just an extra step? you tell me
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

    std::vector<std::string> deform(PronunciationArray pronunciations, VowelMap deformation) {
        // PronunciationArray deformed_array{};
        std::vector<std::string> deformed_array{};

        for (auto pair : pronunciations) {
            // take the first pronunciation, if there are multiple
            std::istringstream iss(pair.second[0]);
            std::string symbol;
            std::string deformed_arpa{};

            while (iss >> symbol) {
                // record and remove vowel accent
                std::string accent{};
                // also functions as an if vowel
                if (!symbol.empty() && std::isdigit(symbol.back())) {
                    accent = symbol.back();
                    symbol.pop_back();
                    
                    // get vowel index, index into the deformation
                    std::size_t index = arpaToIndex(symbol);
                    std::string new_vowel{deformation[index].second};
                    deformed_arpa += deformation[index].second + accent + " ";

                }
                // if not a vowel
                else {
                    deformed_arpa += symbol + " ";
                }
            }
            deformed_array.emplace_back(deformed_arpa);

        }

        return deformed_array;
    }

    // TODO handle punctuation and capitalization and numbers, etc!
    // unknown words are skipped
    PronunciationArray words_to_pronunciations(std::vector<std::string> words) {
        PronunciationArray pronunciation_array{};
        for (const auto& word : words) {
            try {
                pronunciation_array.emplace_back(word, search_dictionary(word));
            }
            catch(const std::exception& exception) {
                std::cerr << exception.what() << " Word will be skipped." << '\n';
            }
        }

        return pronunciation_array;
    }
};

std::string getUserText(){
    std::cout << "Input text: ";
    std::string input{};
    std::getline(std::cin, input);
    return input;
}

std::vector<std::string> splitTextToWords(const std::string& text) {
    std::istringstream iss(text);
    std::string word{};
    std::vector<std::string> words{};

    while (iss >> word) {
        words.push_back(word);
    }

    return words;
}


int main()
{
    // open CMU dict

    CMU_Dict cmu_dict{};

    cmu_dict.import_dictionary("../data/cmudict-0.7b");

    // define a vowel transformation

    // here's my test that compresses all vowels to "UH"
    
    VowelMap compress_to_ah{
        {"AA", "AH"},
        {"AE", "AH"},
        {"AH", "AH"},
        {"AO", "AH"},
        {"AW", "AH"},
        {"AY", "AH"},
        {"EH", "AH"},
        {"ER", "AH"},
        {"EY", "AH"},
        {"IH", "AH"},
        {"IY", "AH"},
        {"OW", "AH"},
        {"OY", "AH"},
        {"UH", "AH"},
        {"UW", "AH"},
    };

    // get user-text
    std::vector<std::string> user_words{splitTextToWords(getUserText())};

    // get pronunciation array from user text
    PronunciationArray user_pronunciations{cmu_dict.words_to_pronunciations(user_words)};

    // get deformed text
    std::vector<std::string> deformed_text{cmu_dict.deform(user_pronunciations, compress_to_ah)};

    // transform ARPABET -> pronouncable text
    std::vector<std::string> deformed_text_respelled{};
    for (const auto& word : deformed_text){
        deformed_text_respelled.emplace_back(arpaRespell(word));
    }

    // Print it
    for (const auto& word : deformed_text_respelled) {
        std::cout << word << " ";
    }
    std::cout << std::endl;

    return 0;
}
