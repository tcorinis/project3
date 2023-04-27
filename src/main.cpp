#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <rapidfuzz/fuzz.hpp>
#include "LyricMatcher.h"

// FOR POINTER COMPARISONS, CURRENTLY NOT USING
// bool cmp(const Lyric* left, const Lyric* right)
// {
//     return left->fuzzRatio > right->fuzzRatio;
// }

int main()
{
	// READ IN DICTIONARY
	std::unordered_map<std::string, std::string> ipa_map;
	FillIpaMap(ipa_map);

	std::cout << ipa_map.size() << std::endl; // should be 16)
	//std::string dictFilePath;
	//std::cout << "Enter dictionary file path: " << std::endl; // for testing
	//std::cin >> dictFilePath;

	std::fstream stored_dict("programData/dictall.csv", std::ios::in);
    //std::fstream stored_dict(dictFilePath, std::ios::in);
    
    std::unordered_map<std::string,std::string> pronunciationMap;
    while(stored_dict.peek() != EOF)
	{
		std::string line, word, pronunciation;
		std::getline(stored_dict, line);
		std::stringstream ss(line);
		ss >> word;
		ss >> pronunciation;
		if(word.back() == '"')
		{
			word.pop_back();
		}
		//cout << word << " " << pronunciation << endl;
		pronunciationMap.emplace(word, pronunciation);
	}

    stored_dict.close();
    std::cout << pronunciationMap.size() << std::endl; // should be 160,000 (unique words)

	// READ IN DATA AND CREATE VECTOR OF UNIQUE LYRICS

	// for testing
	//std::string lyricsFilePath;
	//std::cout << "Enter file path of lyrics: " << std::endl;
	//std::cin >> lyricsFilePath;

	//std::fstream inFile(lyricsFilePath, std::ios::in);
	std::fstream inFile("programData/someconvertedlyrics.csv", std::ios::in);
	
	std::vector<Lyric> allLyrics;

	// std::regex validLyric = std::regex("^[a-zA-Z0-9\\s'\"\n]*$");
    // std::regex validWord = std::regex("^[a-zA-Z0-9']*\"?$");
	// std::regex numeric = std::regex("^[0-9]*$");

	if(inFile.is_open())
	{
		while(inFile.peek() != EOF)
		{
			std::string index, artistName, songTitle, junk, songLyrics, originalLyric, phoneticLyric;

			std::getline(inFile, index, ',');
			std::getline(inFile, artistName, ',');
			std::getline(inFile, songTitle, ',');
			std::getline(inFile, junk, '"'); // opening quote
			std::getline(inFile, songLyrics, '"');
			//songLyrics.push_back('"'); // like a god"

			std::stringstream songLyricsStream(songLyrics);
			bool isLastLine = false;
			std::unordered_set<std::string> uniqueLyrics; // stores the unique lyric within the current song

			while(std::getline(songLyricsStream, originalLyric)) // by new line character 
			{
				std::getline(songLyricsStream, phoneticLyric);


				if(uniqueLyrics.count(originalLyric) != 1 && originalLyric.front() != '[' && originalLyric.back() != ']') // if we haven't seen this lyric before in the current song
				{
					// create a new Lyric object and push it back into the vector
					uniqueLyrics.insert(originalLyric);
					//Lyric* lyric = new Lyric(artistName, songTitle, originalLyric, phoneticLyric);
					Lyric lyric(artistName, songTitle, originalLyric, phoneticLyric);
					allLyrics.push_back(lyric);
				}

			}
		}
		std::cout << "Vector Size: " << allLyrics.size() << std::endl; // should be 1890406, unique lyrics 

		inFile.close();

	}
	std::cin.ignore(100,'\n'); // clears the cin buffer

	// TODO: Everything above this is one time only, run on program start up (loading in our dictionary, and ipa_map)

	// TODO: Process: If the user clicks the search bar, take an input from standard input (cin) store as userSearch
	// TODO: call CalcFuzzRatio(vector<Lyric> &allLyrics, string userSearch) which will eventually return the sorted vector
	// TODO: CalcFuzzRatio(...) will use one of our implemented sorting algorithms based on user selection
	// TODO: This main program will display <x> amount of the top results and maybe do some filtering

	std::string userSearch;

	std::string userSearchPhonetic;
	
	return 0;

}

/*
	for(int i = 0; i < 15; i++)
	{
	

		std::cout << "Enter search: \n";
		std::string userSearch;
		//std::cin.clear();
		//std::cin.sync();
		std::getline(std::cin, userSearch);
		
		//std::cout << userSearch << std::endl;
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
				if(pronunciationMap.find(word) != pronunciationMap.end())
				{
					ipaWord = pronunciationMap[word];
				}
				else
				{
					ipaWord = GetPronunciation(word);
					ipaWord = DecodePronunciation(ipaWord, ipa_map);
					pronunciationMap.emplace(word, ipaWord);
				}
				userSearchPhonetic.append(ipaWord);
				userSearchPhonetic.push_back(' ');
			}
		}


		// for(Lyric lyric : allLyrics)
		// {
		// 	//lyric.PartialFuzzRatio(userSearchPhonetic);
		// 	//lyric.FuzzRatio(userSearchPhonetic);
		// 	lyric.AllFuzzRatios(userSearchPhonetic);
		// 	std::cout << lyric.originalLyric << std::endl;
		// 	std::cout << lyric.phoneticLyric << std::endl;
			
		// 	// std::cout << "partial: " << lyric.fuzzPartialRatio << std::endl;
		// 	// std::cout << "fuzzy normal: " << lyric.fuzzRatio << std::endl;
		// 	// std::cout << "fuzzy token set: " << lyric.fuzzTokenSetRatio << std::endl;
		// 	// std::cout << "fuzzy token sort: " << lyric.fuzzTokenSortRatio << std::endl;
		// 	// std::cout << "fuzzy token: " << lyric.fuzzTokenRatio << std::endl;
		// }
		for(int i = 0; i < allLyrics.size(); i++)
		{
			allLyrics.at(i).FuzzRatio(userSearchPhonetic);
		}

		std::cout << "Fuzz Ratio Calculated " << std::endl;

		std::sort(allLyrics.begin(), allLyrics.end(), std::greater<Lyric>());
		std::cout << "User's Phonetic Search: "<< userSearchPhonetic << std::endl;
		//std::cout << userSearchPhonetic << std::endl;
		std::cout << "Top 10 Results: \n" << std::endl;

		for(int i = 0; i < 10; i++)
		{
			std::cout << allLyrics.at(i).fuzzRatio << std::endl;
			allLyrics.at(i).PrintLyrics();
		}
	

		// int size = allLyrics.size();
		// for(int i = 0; i < size; i++)
		// {
		// 	delete allLyrics.at(i);
		// }
	
	}
	*/