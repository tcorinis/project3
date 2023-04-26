#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <rapidfuzz/fuzz.hpp>
#include "wordConverter.h"
#include "Lyric.h"

int main()
{
	// READ IN DICTIONARY
	std::unordered_map<std::string, std::string> ipa_map;
	FillIpaMap(ipa_map);
	std::cout << ipa_map.size() << std::endl;
	std::string dictFilePath;
	std::cout << "Enter dictionary file path: " << std::endl;
	std::cin >> dictFilePath;
    std::fstream stored_dict(dictFilePath, std::ios::in);
    
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
    std::cout << pronunciationMap.size() << std::endl;
    //double score = rapidfuzz::fuzz::ratio("gaht ay lahng with", "gaht ay lahng list uhv eks luh-vuhrs");
    //std::cout << score << std::endl;

	// create a vector of lyric objects that store artistName, songTitle, phoneticLyric and originalLyric
	// after a user inputs a search
	// iterate through the vector, call ComputeFuzzRatio which sets the fuzz ratio for all the lyrics

	// READ IN DATA AND CREATE VECTOR OF UNIQUE LYRICS

	std::string lyricsFilePath;
	std::cout << "Enter file path of lyrics: " << std::endl;
	std::cin >> lyricsFilePath;

	std::fstream inFile(lyricsFilePath, std::ios::in);
	std::unordered_set<std::string> uniqueLyrics;
	std::vector<Lyric> allLyrics;

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
			
			while(std::getline(songLyricsStream, originalLyric)) // by new line character 
			{
				//std::cout << originalLyric << std::endl;
				
				std::getline(songLyricsStream, phoneticLyric);
				//std::cout << phoneticLyric << std::endl;
				// if(phoneticLyric.back() == '"')
				// {
				// 	phoneticLyric.pop_back();
				// 	isLastLine = true; // might not NEED
				// }
				
				if(uniqueLyrics.count(originalLyric) != 1) // if we haven't seen this lyric before
				{
					// create a new Lyric object and push it back into the vector
					
					Lyric lyric(artistName, songTitle, originalLyric, phoneticLyric);
					allLyrics.push_back(lyric);
				}
				else
				{
					uniqueLyrics.insert(originalLyric); // set of all the unique lyrics, 
				}

				//if(isLastLine)
			}
		}

	// ACTUAL PROGRAM THAT READS USER INPUT AND DOES COMPUTE FUZZ ON EVERY SONG
	inFile.close();
	}

	std::cout << "Vector Size: " << allLyrics.size() << std::endl;

	std::cout << "Enter search: \n";
	std::string userSearch;
	//std::cin.clear();
	//std::cin.sync();
	std::cin.ignore(1000, '\n');
	std::getline(std::cin, userSearch);
	//std::cout << userSearch << std::endl;

	// convert the user search to phonetic pronunciation
	std::regex validLyric = std::regex("^[a-zA-Z0-9\\s'\"\n\\[\\]]*$"); // lowercase a-z, nums, spaces, apostrpohes, quotes, square brackets
    std::regex validWord = std::regex("^[a-zA-Z0-9']*\"?$");
	std::regex numeric = std::regex("^[0-9]*$");

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


	for(Lyric lyric : allLyrics)
	{
		lyric.PartialFuzzRatio(userSearchPhonetic);
		
		std::cout << lyric.originalLyric << std::endl;
		std::cout << lyric.phoneticLyric << std::endl;
		std::cout << lyric.fuzzPartialRatio << std::endl;
	}

	std::cout << userSearchPhonetic << std::endl;
	//std::cout << userSearchPhonetic << std::endl;
    return 0;
}