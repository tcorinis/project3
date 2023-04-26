#include <string>
#include <rapidfuzz/fuzz.hpp>

struct Lyric {

    std::string artistName;
    std::string songTitle;
    std::string phoneticLyric;
    std::string originalLyric;
    float fuzzRatio, fuzzPartialRatio, fuzzTokenSortRatio, fuzzTokenSetRatio;

    Lyric(std::string artistName, std::string songTitle,  std::string originalLyric, std::string phoneticLyric) : artistName(artistName), \
    songTitle(songTitle), phoneticLyric(phoneticLyric), originalLyric(originalLyric), fuzzRatio(-1.0), fuzzPartialRatio(-1.0), \
    fuzzTokenSortRatio(-1.0), fuzzTokenSetRatio(-1.0) {}
    
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
        fuzzTokenSetRatio= float(rapidfuzz::fuzz::ratio(userSearch, phoneticLyric));
    }
    void TokenSetFuzzRatio(std::string &userSearch)
    {
        fuzzTokenSetRatio = float(rapidfuzz::fuzz::ratio(userSearch, phoneticLyric));
    }

    bool operator<(Lyric const& rhs)
    {
        return rhs.fuzz
    }
};

