#include <string>
#include <sstream>
#include <regex>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include "wordConverter.h"
#include "Lyric.h"
#include <rapidfuzz/fuzz.hpp>
const float FUZZ_THRESHOLD = 50.0;

// QuickSort() helper function
int Partition(std::vector<Lyric>& lyrics, int low, int high) {
    //Lyric pivot = lyrics.at(low);
    int index = (high + low) / 2;
    
    Lyric pivot = lyrics.at(index);
    int up = low, down = high;

    while (up < down) {
        for (int i = up; i < high; i++) {
            if (lyrics.at(up) > pivot)
                break;
            up++;
        }
        for (int i = high; i > low; i--) {
            if (lyrics.at(down) < pivot)
                break;
            down--;
        }
        if (up < down) {
            Lyric temp = lyrics.at(up);
            lyrics.at(up) = lyrics.at(down);
            lyrics.at(down) = temp;
        }
    }
    Lyric temp = lyrics.at(low);
    lyrics.at(low) = lyrics.at(down);
    lyrics.at(down) = temp;
    return down;
}

// QuickSort function
void QuickSort(std::vector<Lyric> &lyrics, int low, int high) {
    if (low < high) {
        int pivot = Partition(lyrics, low, high);
        if(lyrics.at(pivot).fuzzRatio >= FUZZ_THRESHOLD)
        {
            QuickSort(lyrics, low, pivot - 1);
        }
        QuickSort(lyrics, pivot + 1, high);
    }
}

// MergeSort Helper Function
void Merge(std::vector<Lyric> &lyrics, int left, int mid, int right)
{
    int n1 = mid - left + 1; // left sub-vector
    int n2 = right - mid; // right sub-vector

    std::vector<Lyric> leftSubset;
    std::vector<Lyric> rightSubset;
    for(int i = 0; i < n1; i++)
    {
        leftSubset.push_back(lyrics.at(left + i)); // fill left sub vector 
    }
    for(int j = 0; j < n2; j++)
    {
        rightSubset.push_back(lyrics.at(mid + 1 + j)); // fill right sub vector
    }

    int i, j, k;
    i = 0;
    j = 0;
    k = left;

    while(i < n1 && j < n2) 
    {
        if(leftSubset.at(i) <= rightSubset.at(j))
        {
            lyrics.at(k) = leftSubset.at(i); // place the items from the left sub vector
            i++;
        }
        else
        {
            lyrics.at(k) = rightSubset.at(j); // place the items from the right sub vector
            j++;
        }

        k++;
    }

    // fill remaining vector
    while(i < n1)
    {
        lyrics.at(k) = leftSubset.at(i);
        i++;
        k++;
    }
    while(j < n2)
    {
        lyrics.at(k) = rightSubset.at(j);
        j++;
        k++;
    }

}

// Merge Sort Function
void MergeSort(std::vector<Lyric> &lyrics, int left, int right)
{
    if(left < right)
    {
        int mid = left + (right - left) / 2;
        MergeSort(lyrics, left, mid);
        MergeSort(lyrics, mid + 1, right);

        Merge(lyrics, left, mid, right);
    }
}

std::string ConvertSearchToPhonetic(std::string userSearch, std::unordered_map<std::string, std::string> &ipa_map, std::unordered_map<std::string,std::string> &pronunciationMap)
{
    // convert search to lowercase to search local dictionary of phonetic pronunciation
    // ex. 'Cool' and 'cool' should have same pronunciation
    std::string lowercaseUserSearch;
    for(char c : userSearch)
    {
        lowercaseUserSearch.push_back(std::tolower(c));
    }

    userSearch = lowercaseUserSearch;

    // convert the user search to phonetic pronunciation
    //std::regex validLyric = std::regex("^[a-zA-Z0-9\\s'\"\n\\[\\]]*$"); // lowercase a-z, nums, spaces, apostrpohes, quotes, square brackets

    std::string userSearchPhonetic, word, ipaWord;

    std::stringstream userSearchStream(userSearch);

    while(userSearchStream >> word) // for each word in the users search
    {
        std::regex validLyric = std::regex("^[a-zA-Z0-9\\s'\"\n]*$");
        std::regex validWord = std::regex("^[a-zA-Z0-9']*\"?$");
        std::regex numeric = std::regex("^[0-9]*$");

        // for pronunciation of numeric strings/words 
        // ex. 104 -> one hundred four
        if(std::regex_match(word, numeric) && word.length() > 0 && word.length() < 19) 
        {
            const char* numberString = word.c_str();
            char* pEnd;
            long long num = strtoll(numberString, &pEnd, 10);
            std::string numName = numToName(num);

            std::stringstream numStream(numName);
            std::string digitName;
            while(numStream >> digitName)
            {
                if(pronunciationMap.find(digitName) != pronunciationMap.end())
                {
                    ipaWord = pronunciationMap[digitName];
                }
                else
                {
                    ipaWord = GetPronunciation(digitName);
                    ipaWord = DecodePronunciation(ipaWord, ipa_map);
                    pronunciationMap.emplace(digitName, ipaWord);
                }
                userSearchPhonetic.append(ipaWord);
                userSearchPhonetic.push_back(' ');
            }
        }
        else if(std::regex_match(word, validWord))
        {
            // words we have the pronunciation of stored locally
            if(pronunciationMap.find(word) != pronunciationMap.end())
            {
                ipaWord = pronunciationMap[word];
            }
            else
            // query Merriam Webster
            {
                ipaWord = GetPronunciation(word);
                ipaWord = DecodePronunciation(ipaWord, ipa_map);
                pronunciationMap.emplace(word, ipaWord);
            }
            userSearchPhonetic.append(ipaWord);
            userSearchPhonetic.push_back(' ');
        }
    }

    return userSearchPhonetic;

}

void CalcFuzzRatio(std::vector<Lyric> &allLyrics, std::string userSearchPhonetic)
{
    // Compute a fuzz ratio using fuzzy string matching
    for(int i = 0; i < allLyrics.size(); i++)
    {
        allLyrics.at(i).FuzzRatio(userSearchPhonetic);
    }
}