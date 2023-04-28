#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include "WordConverter.h"

int main()
{
    std::unordered_map<std::string, std::string> ipa_map;
	FillIpaMap(ipa_map);

	std::string lyricsToConvertFilePath, dictionaryToReadFilePath, dictionaryToWriteFilePath, convertedLyricsToWriteFilePath;
	std::cout << "Enter File Path of Lyrics To Convert: " << std::endl;
	std::cin >> lyricsToConvertFilePath;
	std::cout << "Enter File Path of the Stored Dictionary: " << std::endl;
	std::cin >> dictionaryToReadFilePath;
	std::cout << "Enter File Path of the OUTputted Dictionary: " << std::endl;
	std::cin >> dictionaryToWriteFilePath;
	std::cout << "Enter File Path of the OUTputted Converted Lyrics: " << std:: endl;
	std::cin >> convertedLyricsToWriteFilePath;

	// input/<filename>.csv
	// output/<filename>.csv

    std::fstream inFile(lyricsToConvertFilePath, std::ios::in); // the songs to convert, delete and paste where left off
	std::fstream stored_dict(dictionaryToReadFilePath, std::ios::in); // read in all the words we have seen

	std::fstream dictionaryFile(dictionaryToWriteFilePath, std::ios::out); // the new dictionary, should be all the old words + new
	std::fstream convertedLyrics(convertedLyricsToWriteFilePath, std::ios::out); // converted lyrics

    std::string artistName, songTitle, songLyrics, lyricsLine, junk, index;
    std::getline(inFile, junk);
    std::regex validLyric = std::regex("^[a-zA-Z0-9\\s'\"\n\\[\\]]*$"); // lowercase a-z, nums, spaces, apostrpohes, quotes, square brackets
    std::regex validWord = std::regex("^[a-zA-Z0-9']*\"?$");
	std::regex numeric = std::regex("^[0-9]*$");


	long long repeatedWords = 0; // performance indicator
	//long long validLinecount = 0; // performance indicator

	// TODO write converted lyrics file
	convertedLyrics << ",artist" << ",title" << ",lyrics\n"; // header

	// avoid querying Merriam Webster (takes time) for words we already
	// know the pronunciation of
    std::unordered_map<std::string, std::string> pronunciationMap; 

	auto start = std::chrono::system_clock::now();

	// load in the dictionary (locally stored words)
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
	//cout << pronunciationMap.size() << endl;
	stored_dict.close();

	// read the file containing lyrics to convert
	// write the file containing the lyrics converted
    if(inFile.is_open())
    {
        while(inFile.peek() != EOF)
        {

            std::getline(inFile, index, ',');
            std::getline(inFile, artistName, ',');
            std::getline(inFile, songTitle, ',');
			std::getline(inFile, junk, '"');
            std::getline(inFile, songLyrics, '"');

			convertedLyrics << index << "," << artistName << "," << songTitle << ",\n\""; // (21 Savage, Supply,\n\"")
            
			songLyrics.push_back('"');
            std:: stringstream songLyricsStream(songLyrics);
            bool isLastLine = false;
			std::cout << artistName << " " << songTitle << std::endl;

			
            while(std::getline(songLyricsStream, lyricsLine)) // for each line in one song's lyrics , parsed by newline
            {

				std::string phoneticLyricsLine = "";
                if(std::regex_match(lyricsLine, validLyric)) // if its a valid lyric line
                {
                    std::string lyricsLineLowercase = ""; // convert the line of lyrics to lowercase for standardization in pronunciation
                    for(char c : lyricsLine)
                    {
                        lyricsLineLowercase += tolower(c);
                    }
                    lyricsLine = lyricsLineLowercase;
                    
                    std::string word, ipaWord;
                    std::stringstream lineStream(lyricsLine);

                    while(std::getline(lineStream, word, ' ')) // each word in the current line
                    {
						
                        if(word.back() == '"') // must be the last line of the song
                        {
                            isLastLine = true;
							word.pop_back();
                        }
						if (std::regex_match(word, numeric) && word.length() > 0 && word.length() < 19)
						{
							const char* numberString = word.c_str();
							char* pEnd;
							long long num = strtoll(numberString, &pEnd, 10);
							std::string numName = numToName(num);

							std::stringstream numStream(numName);
							std::string digitName;

							// Numeric strings to pronunciation
							// ex. 104 -> one hundred four
							while (numStream >> digitName) {
								if (pronunciationMap.find(digitName) != pronunciationMap.end()) // if the has already been searched
								{
									// 101 -> one hundred one
									// each loop will get one part of the combined pronunciation
									ipaWord = pronunciationMap[digitName];
									repeatedWords++;
								}
								else
								{
									ipaWord = GetPronunciation(digitName);
									ipaWord = DecodePronunciation(ipaWord, ipa_map);
									pronunciationMap.emplace(digitName, ipaWord); // we need to search this word, it's not in our dictionary
								}
								// std::cout << DecodePronunciation(pronunciationMap[digitName], ipa_map) << endl; (ex. )
								//ipaWord = DecodePronunciation(pronunciationMap[word], ipa_map);
								phoneticLyricsLine.append(ipaWord);
								phoneticLyricsLine.push_back(' ');

							}
						}
                        else if(std::regex_match(word, validWord))
                        {
                            //std::cout << word << endl;
							// avoid query if we have already seen this word
                            if(pronunciationMap.find(word) != pronunciationMap.end())
                            {
								
                                ipaWord = pronunciationMap[word];
								repeatedWords++;
                            }
                            else
                            {
								// gotta query, store it in case we see the same word later
								ipaWord = GetPronunciation(word);
								ipaWord = DecodePronunciation(ipaWord, ipa_map);
                                pronunciationMap.emplace(word, ipaWord);
                            }
    
							phoneticLyricsLine.append(ipaWord);

							if(!isLastLine)
							{
								phoneticLyricsLine.push_back(' '); // keep separation between words
							}
                            
                        }
                        if(isLastLine)
                        {
							break;
                        }
                        
                    }
                }
				if(isLastLine)
				{
					phoneticLyricsLine.push_back('"'); // how we format our files
				}
				
				if(!phoneticLyricsLine.empty()) // if there is a lyric to write to the output file 
				{
					
					if(!isLastLine)
					{
						phoneticLyricsLine.push_back('\n');
					}
					if(lyricsLine.back() == '"')
					{
						lyricsLine.pop_back();
					}
					convertedLyrics << lyricsLine << "\n"; // original
					convertedLyrics << phoneticLyricsLine; // converted
					
				}

                if(isLastLine)
				{
					break;
				}
				
            }

            // get artist
            // get song title
            // get lyrics line convert to string stream then parse by \n
		
        }

    }
	for(auto it = pronunciationMap.begin(); it != pronunciationMap.end(); it++)
	{
		dictionaryFile << it->first << " " << it->second << "\n";
	}
	std::cout << "Map Size: " << pronunciationMap.size() << std::endl;
	std::cout << "Repeated Words: " << repeatedWords << std::endl;

	auto end = std::chrono::system_clock::now();
	std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout << "Started at: " << std::ctime(&start_time) << std::endl;
	std::cout << "Ended at: " << std::ctime(&end_time) << std::endl;
	inFile.close();
	dictionaryFile.close();
	convertedLyrics.close();

    // Objective of this file:
    // 1) Generate a file that can be used to recreate the map, <word, phonetic spelling>
    // 2) Generate a csv file that represents the data base, Artist, Song Title, Lyrics converted to phonetics
    // 3) Actual program will need to search pronunciation or ideally use map to convert the user input
    // 3)(cont) its phonetic spelling
    // 4) 
	
    return 0;
};