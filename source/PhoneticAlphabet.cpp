#include "PhoneticAlphabet.hpp"
#include <../external/nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>


const std::unordered_map<std::string, PhoneticTransliteration>& PhoneticAlphabet::getTranslationMap(const std::string& from) {
      if (from == "IPA") return IPA_to_transliteration;
      else if (from == "ARPABET") return ARPABET_to_transliteration;
      else if (from == "XSAMPA") return XSAMPA_to_transliteration;
   }

bool PhoneticAlphabet::loadFromFile() {

   std::ifstream file("../data/transliteration_data.json");
   if (!file.is_open()) {
      std::cerr << "Error opening file" << std::endl;
      return false;
   }

   // Parse JSON from file
   nlohmann::json data;
   try {
      file >> data;
   } catch (const std::exception& e) {
      std::cerr << "Error parsing JSON: " << e.what() << std::endl;
      return false;
   }

   // Load each phonetic letter from the JSON data
   for (auto& [key, value] : data.items()) {
      PhoneticTransliteration letter = {
            value["IPA"].get<std::string>(),
            value["ARPABET"].get<std::string>(),
            value["XSAMPA"].get<std::string>()
      };
      addLetter(letter);
   }

   return true;
}

void PhoneticAlphabet::addLetter(const PhoneticTransliteration& letter) {
   phonetic_letters.push_back(letter);
   IPA_to_transliteration[letter.IPA] = letter;
   ARPABET_to_transliteration[letter.ARPABET] = letter;
   XSAMPA_to_transliteration[letter.XSAMPA] = letter;
}

// takes a word pronunciation, i.e. vector<std::string> in a particular phonetic alphabet and transliterates to another phonetic alphabet
std::vector<std::string> PhoneticAlphabet::transliterate(const std::vector<std::string>& word, const std::string& from, const std::string& to) {
   const auto& transliteration_map = getTranslationMap(from);
   std::vector<std::string> transliterated_word{};

   for (const auto& symbol : word) {
      if (transliteration_map.find(symbol) != transliteration_map.end()) {
         if(to == "IPA") transliterated_word.emplace_back(transliteration_map.at(symbol).IPA);
         else if(to == "ARPABET") transliterated_word.emplace_back(transliteration_map.at(symbol).ARPABET);
         else if(to == "XSAMPA") transliterated_word.emplace_back(transliteration_map.at(symbol).XSAMPA);
      } else {
         transliterated_word.emplace_back("[Unknown Symbol]");
      }
   }

   return transliterated_word;
   
}

// TODO need to include stress markers
std::string PhoneticAlphabet::simple_transliterate(const std::string& pronunciation, const std::string& from, const std::string& to) {
   std::string transliteration{};
   std::istringstream iss(pronunciation);
   std::string symbol;

   while (iss >> symbol) {
      for (const auto& letter : phonetic_letters) {
            if (letter.ARPABET == symbol) {
               if (to == "XSAMPA") {
                  transliteration += letter.XSAMPA;
               }
            } 
            
         }
   }

   return transliteration;
}

