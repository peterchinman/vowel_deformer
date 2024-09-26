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



// map of arpa vowels from pair.first to pair.second for pair in VowelMap, needs to be alphabetical by pair.first
using VowelMap = std::vector<std::pair<std::string, std::string>>;
/* for VowelMap
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
    };*/


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

std::string respellArpa(const std::string& symbol) {
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



class CMU_Dict {
private:
    // map of words with CMU pronunciations
    std::unordered_map<std::string, std::vector<std::string>> m_dictionary {};

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
    std::vector<std::string> search_dictionary(std::string query) {
        // capitalize all queries
        std::transform(query.begin(), query.end(), query.begin(), ::toupper);
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
};

struct Word {
    std::string word{};
    std::string punct_prefix{};
    std::string punct_suffix{};
    std::vector<std::string> pronunciations{};
    std::vector<std::string> deformed_pronunciations{};
    std::vector<std::string> respellings{};
    std::vector<std::string> deformed_respellings{};
};

class Text {
private:
    std::string m_user_text{};
    std::vector<Word> m_words{};

public:
    void printDeformedRespelling(){
        for (auto& word : m_words) {
            if (word.pronunciations.empty()) {
                std::cout << word.punct_prefix << word.word << word.punct_suffix << ' ';
            }
            else {
                std::cout << word.punct_prefix << word.deformed_respellings.at(0) <<  word.punct_suffix << ' ';
            }
            
        }
        std::cout << '\n';
    }
    // TODO error verification
    bool getUserText(){
        std::cout << "Input text: ";
        std::string input{};
        std::getline(std::cin, input);
        m_user_text = input;
        return true;
    }
    bool splitUserText() {
        std::istringstream iss(m_user_text);
        std::string word{};

        while (iss >> word) {
            // deal with punctuation :,./()-]
            // regex match for words
            std::regex words_regex("[a-zA-Z]+");
            auto words_begin =
            std::sregex_iterator(word.begin(), word.end(), words_regex);
            auto words_end = std::sregex_iterator();
            
            std::string match_suffix{};
            std::vector<Word> temp_words{};

            // iterate thru matches
            for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
                std::smatch match = *i;
                std::string match_str = match.str();
                std::string match_prefix = match.prefix();
                match_suffix = match.suffix();
                // add prefix and word to array
                Word temp {match_str, match_prefix};
                temp_words.emplace_back(temp);
            }
            // add the remaining suffix to the last word match (only would come into place in strange cases, e.g. "dilly-dally.")
            temp_words.back().punct_suffix = match_suffix;


            m_words.insert(m_words.end(), temp_words.begin(), temp_words.end());
        }
        return true;
    }

    // unknown words are skipped
    std::vector<std::string> getPronunciation(const std::string& query, CMU_Dict& dict){
        std::vector<std::string> pronunciation{};

        try {
            pronunciation = dict.search_dictionary(query);
        }
        catch(const std::exception& exception) {
            std::cerr << exception.what() << " Word will be skipped." << '\n';
        }

        return pronunciation;
        
        
    }

    std::string deform(const std::string& arpa, VowelMap vowel_map) {
        std::istringstream iss(arpa);
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
                deformed_arpa += vowel_map[index].second + accent + " ";

            }
            // if not a vowel
            else {
                deformed_arpa += symbol + " ";
            }
        }
        return deformed_arpa;

    }

    // TODO handle accents
    std::string respell(const std::string& pronunciation) {
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
            respelling += respellArpa(symbol);
        }
        return respelling;
    }

    

    bool init(CMU_Dict& dict, VowelMap vowel_map) {
        getUserText();
        splitUserText();
    
        for (auto& word: m_words) {
            word.pronunciations = getPronunciation(word.word, dict);


            for (auto& instance : word.pronunciations) {
                // respell each pronunciation
                word.respellings.emplace_back(respell(instance));
                // deform each pronunciation
                word.deformed_pronunciations.emplace_back(deform(instance, vowel_map));
                // respell each deformed pronunciation
                word.deformed_respellings.emplace_back(respell(word.deformed_pronunciations.back()));
            }

        }

        return true;
    }
};

int main()
{
    // open CMU dict

    CMU_Dict dict{};

    dict.import_dictionary("../data/cmudict-0.7b");

    // define a vowel transformation
    
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

    Text text{};

    text.init(dict, compress_to_ah);

    text.printDeformedRespelling();

    return 0;
}
