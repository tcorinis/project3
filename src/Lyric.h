#include <string>
#include <cmath>
#include <iostream>
#include <rapidfuzz/fuzz.hpp>

struct Lyric {

    std::string artistName;
    std::string songTitle;
    std::string phoneticLyric;
    std::string originalLyric;
    float fuzzRatio, fuzzPartialRatio, fuzzTokenSortRatio, fuzzTokenSetRatio, fuzzTokenRatio;

    Lyric(std::string artistName, std::string songTitle,  std::string originalLyric, std::string phoneticLyric) : artistName(artistName), \
    songTitle(songTitle), phoneticLyric(phoneticLyric), originalLyric(originalLyric), fuzzRatio(-1.0), fuzzPartialRatio(-1.0), \
    fuzzTokenSortRatio(-1.0), fuzzTokenSetRatio(-1.0), fuzzTokenRatio(-1.0) {};
    
    void PartialFuzzRatio(std::string &userSearch)
    {
        fuzzPartialRatio = float(rapidfuzz::fuzz::partial_ratio(userSearch, phoneticLyric));
    }
    void FuzzRatio(std::string &userSearch)
    {
        fuzzRatio = float(rapidfuzz::fuzz::ratio(userSearch, phoneticLyric));
    }
    void TokenSortFuzzRatio(std::string &userSearch)
    {
        fuzzTokenSortRatio= float(rapidfuzz::fuzz::token_sort_ratio(userSearch, phoneticLyric));
    }
    void TokenSetFuzzRatio(std::string &userSearch)
    {
        fuzzTokenSetRatio = float(rapidfuzz::fuzz::token_set_ratio(userSearch, phoneticLyric));
    }
    void TokenFuzzRatio(std::string &userSearch)
    {
        // if the word is in the string, return 100
        // dont use this
        fuzzTokenRatio = float(rapidfuzz::fuzz::token_ratio(userSearch, phoneticLyric));
    }
    void AllFuzzRatios(std::string &userSearch)
    {
        fuzzPartialRatio = float(rapidfuzz::fuzz::partial_ratio(userSearch, phoneticLyric));
        fuzzRatio = float(rapidfuzz::fuzz::ratio(userSearch, phoneticLyric));
        fuzzTokenSetRatio= float(rapidfuzz::fuzz::token_set_ratio(userSearch, phoneticLyric));
        fuzzTokenSortRatio = float(rapidfuzz::fuzz::token_sort_ratio(userSearch, phoneticLyric));
        fuzzTokenRatio = float(rapidfuzz::fuzz::token_ratio(userSearch, phoneticLyric));
    }
    void PrintFuzzRatios()
    {
        std::cout << "Partial Ratio: " << fuzzPartialRatio << std::endl;
        std::cout << "Fuzz Ratio: " << fuzzRatio << std::endl;
        std::cout << "Token Sort Ratio: " << fuzzTokenSortRatio << std::endl;
        std::cout << "Token Set Ratio: " << fuzzTokenSetRatio << std::endl;
        std::cout << "Token Ratio: " << fuzzTokenRatio << std::endl;
    }
    void PrintLyrics()
    {
        std::cout << "Song Name: " << songTitle << std::endl;
        std::cout << "Original Lyric: " << originalLyric << std::endl;
        std::cout << "Phonetic Lyric: " << phoneticLyric << std::endl;
        
    }
    bool operator<(const Lyric &rhs) const
    {
        return this->fuzzRatio < rhs.fuzzRatio;
    }
    bool operator>(const Lyric &rhs) const
    {
        return this->fuzzRatio > rhs.fuzzRatio;
    }
    bool operator==(const Lyric &rhs) const
    {
        return abs(this->fuzzRatio - rhs.fuzzRatio) < 0.01f;
    }
    bool operator<=(const Lyric &rhs) const
    {
        return this->fuzzRatio <= rhs.fuzzRatio;
    }
    bool operator>=(const Lyric &rhs) const
    {
        return this->fuzzRatio >= rhs.fuzzRatio;
    }
    

};

