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
			else if (word.substr(word.length() - 2, 2) == "ed")
				pron = GetPronunciation(word.substr(0, word.length() - 2)) + "d";
			else
				pron = word;
		}
	}

	return pron;
}

string DecodePronunciation(string pron) {
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

int main() {

	string word;
	cin >> word;
	string lowerWord = "";
	for (char c : word) {
		lowerWord += tolower(c);
	}
	word = lowerWord;

	vector<string> result;
	unordered_map<string, string> pronMap;

	while (word != "1") {
		if (word == "0") {
			for (string word : result) {
				if (word != result.front())
					cout << " ";
				cout << word;
			}
			cout << endl;
			result.clear();
		}
		else {
			vector<string> noHyphens;
			// replace hyphens with spaces
			size_t hyphenPos = word.find("-");
			if (hyphenPos == string::npos)
				noHyphens.push_back(word);
			else
				noHyphens.push_back(word.substr(0, hyphenPos));
			while (hyphenPos != string::npos) {
				word.erase(0, hyphenPos + 1);
				hyphenPos = word.find("-");
				noHyphens.push_back(word.substr(0, hyphenPos));
			}

			for (string word : noHyphens) {
				if (pronMap.find(word) == pronMap.end())
					pronMap.emplace(word, GetPronunciation(word));
				string pron = pronMap[word];

				if (pron != "")
					result.push_back(DecodePronunciation(pron));
			}
		}

		cin >> word;
		string lowerWord = "";
		for (char c : word) {
			lowerWord += tolower(c);
		}
		word = lowerWord;
	}
	return 0;
}
