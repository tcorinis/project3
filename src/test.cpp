#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_map>

using namespace std;
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
int main()
{
    unordered_map<string, string> ipa_map;

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

    fstream inFile("oneArtist.csv", ios::in);
    string artistName, songTitle, songLyrics, lyricsLine, junk, index;
    getline(inFile, junk);
    regex validLyric = regex("^[a-zA-Z0-9\\s'\"\n\\[\\]]*$"); // lowercase a-z, numbers, spaces, apostrpohes, quotes
    regex validWord = regex("^[a-zA-Z0-9']*$");
    
    unordered_map<string, string> pronunciationMap;
    if(inFile.is_open())
    {
        while(inFile.peek() != EOF)
        {

            getline(inFile, index, ',');
            getline(inFile, artistName, ',');
            getline(inFile, songTitle, ',');
            getline(inFile, songLyrics, ',');
            //cout << songLyrics << endl;
            stringstream songLyricsStream(songLyrics);
            bool isLastLine = false;
            while(getline(songLyricsStream, lyricsLine)) // for each line in the lyrics
            {
                //cout << lyricsLine << endl;
                if(regex_search(lyricsLine, validLyric))
                {
                    string lyricsLineLowercase = ""; // convert the line of lyrics to lowercase
                    for(char c : lyricsLine)
                    {
                        lyricsLineLowercase += tolower(c);
                    }
                    lyricsLine = lyricsLineLowercase;
                    cout << lyricsLine << endl;
                    string word, ipaWord;
                    
                    stringstream lineStream(lyricsLine);
                    while(getline(lineStream, word, ' '))
                    {
                        if(word.back() == '"')
                        {
                            isLastLine = true;
                        }
                        if(regex_search(word, validWord))
                        {
                            
                            cout << word << endl;

                            if(pronunciationMap.find(word) != pronunciationMap.end())
                            {
                                ipaWord = pronunciationMap[word];
                            }
                            else
                            {
                                pronunciationMap.emplace(word, GetPronunciation(word));
                            }
                            cout << pronunciationMap[word] << endl;
                            
                        }
                        if(isLastLine)
                        {
                            break;
                        }
                        
                    }
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

    // Objective of this file:
    // 1) Generate a file that can be used to recreate the map, <word, phonetic spelling>
    // 2) Generate a csv file that represents the data base, Artist, Song Title, Lyrics converted to phonetics
    // 3) Actual program will need to search pronunciation or ideally use map to convert the user input
    // 3)(cont) its phonetic spelling
    // 4) 

    return 0;
};