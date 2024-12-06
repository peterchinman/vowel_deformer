#ifndef TRANSLITERATION_HPP
#define TRANSLITERATION_HPP

#include <string>
#include <unordered_map>
#include <vector>


struct PhoneticTransliteration {
   std::string IPA {};
   std::string ARPABET {};
   std::string XSAMPA {};
};

class PhoneticAlphabet {
private:
   std::vector<PhoneticTransliteration> phonetic_letters{};

   // QUESTION is it overkill to set these up as unordered maps? How much time do I save on look-up given that there are only, a few dozen symbols??
   std::unordered_map<std::string, PhoneticTransliteration> IPA_to_transliteration{};
   std::unordered_map<std::string, PhoneticTransliteration> ARPABET_to_transliteration{};
   std::unordered_map<std::string, PhoneticTransliteration> XSAMPA_to_transliteration{};

   const std::unordered_map<std::string, PhoneticTransliteration>& getTranslationMap(const std::string& from);

public:

   // Method to init class with data from transliteration_data.json
   bool loadFromFile();

   // Add each letter in all the various maps
   void addLetter(const PhoneticTransliteration& letter);

   // transliterate a pronunciation (vector<std::string>) from one alphabet to another.
   // alphabets are "IPA", "ARPABET", "XSAMPA"
   std::vector<std::string> transliterate(const std::vector<std::string>& word, const std::string& from, const std::string& to);

   std::string simple_transliterate(const std::string& pronunciation, const std::string& from, const std::string& to);
};

#endif  // TRANSLITERATION_HPP
