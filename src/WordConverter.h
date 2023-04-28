#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>

#pragma once

void FillIpaMap(std::unordered_map<std::string, std::string> &ipa_map)
{
	// translation from unicode phonetic character to standard ascii character
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
}

std::string GetPronunciation(std::string word)
{
	std::string pron = "";

	// contractions
	if (word.length() > 3 && word[word.length() - 2] == '\'') {
		if (word.substr(word.length() - 3, 3) == "n\'t") {
			pron = GetPronunciation(word.substr(0, word.length() - 3)) + "-nt";
		}
		else
			pron = GetPronunciation(word.substr(0, word.length() - 2)) + word.back();
		return pron;
	}
	// contractions
	else if (word.length() > 4 && word[word.length() - 3] == '\'') {
		pron = GetPronunciation(word.substr(0, word.length() - 3)) + word.substr(word.length() - 2, 2);
		return pron;
	}
	// words ending in -in
	else if (word.length() > 3 && word.substr(word.length() - 3, 3) == "in\'") {
        pron = GetPronunciation(word.substr(0, word.length() - 1) + "g");
        return pron;
    }

	// remove remaining punctuation
	size_t punctPos = word.find_first_of("\"'().,?!");
	while (punctPos != std::string::npos) {
		word.erase(punctPos, 1);
		punctPos = word.find_first_of("\"\'().,?!");
	}

	if (word.length() == 0)
		return "";

	// use curl to run command with word and write definition to file
	std::string input = "curl https://dictionaryapi.com/api/v3/references/collegiate/json/"
		+ word
		+ "?key=e0c99829-f7f5-4ad9-9f70-97534628f352 -o definition.txt";
	const char* cmd = input.c_str();
	system(cmd);

	std::ifstream def("definition.txt");

	std::string line;
	std::getline(def, line);

	size_t startPos = 0;
	size_t prsPos = line.find("\"prs\"");
	size_t closeQPos = line.rfind("\"", prsPos - 2);
	size_t openQPos = line.rfind("\"", closeQPos - 1) + 1;
	std::string possibleWord = line.substr(openQPos, closeQPos - openQPos);

	size_t starPos = possibleWord.find("*");
	while (starPos != std::string::npos) {
		possibleWord.erase(starPos, 1);
		starPos = possibleWord.find("*");
	}

	if (possibleWord == word) {
		size_t pronPos = line.find("\"mw\"", prsPos) + 6;
		size_t endPos = line.find(",", pronPos);
		pron = line.substr(pronPos, endPos - pronPos - 1);
	}
	else {
		while (prsPos != std::string::npos) {
			closeQPos = line.rfind("\"", prsPos - 2);
			openQPos = line.rfind("\"", closeQPos - 1) + 1;
			possibleWord = line.substr(openQPos, closeQPos - openQPos);

			size_t starPos = possibleWord.find("*");
			while (starPos != std::string::npos) {
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

std::string DecodePronunciation(std::string pron, std::unordered_map<std::string,std::string> &ipa_map) {

	std::string result;

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



std::string numToName(long num) {

	std::vector<std::string> ones{ "","one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
	std::vector<std::string> teens{ "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen","sixteen", "seventeen", "eighteen", "nineteen" };
	std::vector<std::string> tens{ "", "", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety" };

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