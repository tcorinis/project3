#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include "wordConverter.h"

int main()
{
    std::unordered_map<std::string, std::string> ipa_map;
	
	ipa_map.emplace("\\u02c8", "");
	ipa_map.emplace("\\u02cc", "");
	ipa_map.emplace("\\u0259", "uh");
	ipa_map.emplace("\\u1d4a", "");
	ipa_map.emplace("\\u0101", "ay");
	ipa_map.emplace("\\u00e4", "ah");
	ipa_map.emplace("u\\u0307", "ouh");
	ipa_map.emplace("\\u0113", "ee");
	ipa_map.emplace("\\u012b", "iy");
	ipa_map.emplace("\\u014b", "ng");
	ipa_map.emplace("\\u014d", "oh");
	ipa_map.emplace("\\u022f", "ah");
	ipa_map.emplace("\\u022fi", "oy");
	ipa_map.emplace("\\u022fr", "or");
	ipa_map.emplace("\\u035f", "");
	ipa_map.emplace("\\u00fc", "oo");

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
	long long validLinecount = 0; // performance indicator

	// TODO write converted lyrics file
	convertedLyrics << ",artist" << ",title" << ",lyrics\n"; // header

    std::unordered_map<std::string, std::string> pronunciationMap; // 

	auto start = std::chrono::system_clock::now();


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
            //cout << songLyrics << endl;
			songLyrics.push_back('"');
            std:: stringstream songLyricsStream(songLyrics);
            bool isLastLine = false;
			std::cout << artistName << " " << songTitle << std::endl;

			
            while(std::getline(songLyricsStream, lyricsLine)) // for each line in one song's lyrics , parsed by newline
            {
                //cout << lyricsLine << endl;

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
                            if(pronunciationMap.find(word) != pronunciationMap.end())
                            {
                                ipaWord = pronunciationMap[word];
								repeatedWords++;
                            }
                            else
                            {
								ipaWord = GetPronunciation(word);
								ipaWord = DecodePronunciation(ipaWord, ipa_map);
                                pronunciationMap.emplace(word, ipaWord);
                            }
                            //std::cout << DecodePronunciation(pronunciationMap[word], ipa_map) << endl;
							phoneticLyricsLine.append(ipaWord);
							if(!isLastLine)
							{
								phoneticLyricsLine.push_back(' ');
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
					phoneticLyricsLine.push_back('"');
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

/*
string GetPronunciation(string word) {
	string pron = "";

	if (word.length() > 3 && word[word.length() - 2] == '\'') {
		if (word.substr(word.length() - 3, 3) == "n\'t") {
			pron = GetPronunciation(word.substr(0, word.length() - 3)) + "-nt";
		}
		else
			pron = GetPronunciation(word.substr(0, word.length() - 2)) + word.back();
		return pron;
	}
	else if (word.length() > 4 && word[word.length() - 3] == '\'') {
		pron = GetPronunciation(word.substr(0, word.length() - 3)) + word.substr(word.length() - 2, 2);
		return pron;
	}
	else if (word.length() > 3 && word.substr(word.length() - 3, 3) == "in\'") {
        pron = GetPronunciation(word.substr(0, word.length() - 1) + "g");
        return pron;
    }

	// remove remaining punctuation
	size_t punctPos = word.find_first_of("\"'().,?!");
	while (punctPos != string::npos) {
		word.erase(punctPos, 1);
		punctPos = word.find_first_of("\"\'().,?!");
	}

	if (word.length() == 0)
		return "";

	// use curl to run command with word and write definition to file
	string input = "curl https://dictionaryapi.com/api/v3/references/collegiate/json/"
		+ word
		+ "?key=e0c99829-f7f5-4ad9-9f70-97534628f352 -o definition.txt";
	const char* cmd = input.c_str();
	system(cmd);

	ifstream def("definition.txt");

	string line;
	getline(def, line);

	size_t startPos = 0;
	size_t prsPos = line.find("\"prs\"");
	size_t closeQPos = line.rfind("\"", prsPos - 2);
	size_t openQPos = line.rfind("\"", closeQPos - 1) + 1;
	string possibleWord = line.substr(openQPos, closeQPos - openQPos);

	size_t starPos = possibleWord.find("*");
	while (starPos != string::npos) {
		possibleWord.erase(starPos, 1);
		starPos = possibleWord.find("*");
	}

	if (possibleWord == word) {
		size_t pronPos = line.find("\"mw\"", prsPos) + 6;
		size_t endPos = line.find(",", pronPos);
		pron = line.substr(pronPos, endPos - pronPos - 1);
	}
	else {
		while (prsPos != string::npos) {
			closeQPos = line.rfind("\"", prsPos - 2);
			openQPos = line.rfind("\"", closeQPos - 1) + 1;
			possibleWord = line.substr(openQPos, closeQPos - openQPos);

			size_t starPos = possibleWord.find("*");
			while (starPos != string::npos) {
				possibleWord.erase(starPos, 1);
				starPos = possibleWord.find("*");
			}

			if (possibleWord == word) {
				size_t pronPos = line.find("\"mw\"", prsPos) + 6;
				size_t endPos = line.find(",", pronPos);
				pron = line.substr(pronPos, endPos-pronPos - 1);
				break;
			}

			startPos = prsPos + 1;
			prsPos = line.find("\"prs\"", startPos);
		}
		if (possibleWord != word) {
			if (word.back() == 's')
				pron = GetPronunciation(word.substr(0, word.length() - 1)) + "s";
			else if (word.length() > 1 && word.substr(word.length() - 2, 2) == "ed")
				pron = GetPronunciation(word.substr(0, word.length() - 2)) + "d";
			else
				pron = word;
		}
	}

	return pron;
}

string DecodePronunciation(string pron, unordered_map<string,string> &ipa_map) {

	string result = "";

	for (int i = 0; i < pron.length(); i++) {
		if (pron[i] == '\"')
			continue;
		else if (pron[i] == '(')
			i += pron.substr(i, pron.find(")") - i).length();
		else if (pron[i] == 'u' && pron.substr(i, 7) == "u\\u0307") {
			result += ipa_map["u\\u0307"];
			i += 6;
		}
		else if (pron[i] == '\\') {
			if (pron.substr(i, 7) == "\\u022fi") {
				result += ipa_map["\\u022fi"];
				i++;
			}
			else if (pron.substr(i, 7) == "\\u022fr") {
				result += ipa_map["\\u022fr"];
				i++;
			}
			else
				result += ipa_map[pron.substr(i, 6)];
			i += 5;
		}
		else
			result += pron[i];
	}

	return result;
}



string numToName(long num) {

	vector<string> ones{ "","one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
	vector<string> teens{ "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen","sixteen", "seventeen", "eighteen", "nineteen" };
	vector<string> tens{ "", "", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety" };

	if (num < 10) {
		return ones[num];
	}
	else if (num < 20) {
		return teens[num - 10];
	}
	else if (num < 100) {
		return tens[num / 10] + ((num % 10 != 0) ? " " + numToName(num % 10) : "");
	}
	else if (num < 1000) {
		return numToName(num / 100) + " hundred" + ((num % 100 != 0) ? " " + numToName(num % 100) : "");
	}
	else if (num < 1000000) {
		return numToName(num / 1000) + " thousand" + ((num % 1000 != 0) ? " " + numToName(num % 1000) : "");
	}
	else if (num < 1000000000) {
		return numToName(num / 1000000) + " million" + ((num % 1000000 != 0) ? " " + numToName(num % 1000000) : "");
	}
	else if (num < 1000000000000) {
		return numToName(num / 1000000000) + " billion" + ((num % 1000000000 != 0) ? " " + numToName(num % 1000000000) : "");
	}
	return "error";
}
*/
