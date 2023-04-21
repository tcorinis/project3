#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_map>
using namespace std;

string GetPronunciation(string word) {
	string pron = "";

	size_t punctPos = word.find_first_of("\"\'().,?!");
	while (punctPos != string::npos) {
		word.erase(punctPos, 1);
		punctPos = word.find_first_of("\"\'().,?!");
	}

	size_t hyphenPos = word.find("-");
	while (hyphenPos != string::npos) {
		string newWord = word.substr(0, hyphenPos);
		pron += GetPronunciation(newWord) + " ";
		word.erase(0, hyphenPos + 1);
		hyphenPos = word.find("-");
	}

	string input = "curl https://dictionaryapi.com/api/v3/references/collegiate/json/"
		+word
		+"?key=e0c99829-f7f5-4ad9-9f70-97534628f352 -o definition.txt";
	const char* cmd = input.c_str();
	system(cmd);

	ifstream def("definition.txt");

	string line;
	getline(def, line);

	if (line.find("\"id\"") == string::npos) {
		cout << "Word not found\n";
		return "";
	}
	

	/*string foundWord = line.substr(16, line.find(",") - 17);
	foundWord = foundWord.substr(0, foundWord.find(":"));

	if (foundWord != word) {
		input = "curl https://dictionaryapi.com/api/v3/references/collegiate/json/"
			+foundWord
			+"?key=e0c99829-f7f5-4ad9-9f70-97534628f352 -o definition.txt";
		cmd = input.c_str();
		system(cmd);

		ifstream newDef("definition.txt");
		getline(newDef, line);
	}*/

	size_t pronPos = line.find("\"mw\"");
	if (pronPos != string::npos) {
		size_t endPos = line.find(",", pronPos);
		pron += line.substr(pronPos + 6, endPos - pronPos - 7);
	}

	/*if (foundWord != word) {
		if (word.back() == 's')
			pron += 's';
		if (word.back() == 'd')
			pron += 'd';
	}*/

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

	vector<string> result;
	unordered_map<string, string> pronMap;

	while (word != "-1") {
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
			if (pronMap.find(word) == pronMap.end())
				pronMap.emplace(word, GetPronunciation(word));
			string pron = pronMap[word];
		
			if (pron != "") 
				result.push_back(DecodePronunciation(pron));
		}
		
		cin >> word;
	}
	return 0;
}