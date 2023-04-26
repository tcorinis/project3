#include <iostream>
#include <fstream>
#include <sfml/Graphics.hpp>
#include <unordered_map>
#include "SearchBar.h"
#include "ImGui/imgui.h"
#include "ImGui-SFML/imgui-SFML.h"
using std::string;

// Stuff with IPA; commenting this out for now.
/*const int NUM_IPA_SYMBOLS = 35;
int adjMatrix[NUM_IPA_SYMBOLS][NUM_IPA_SYMBOLS];
const char symbolsChar[NUM_IPA_SYMBOLS] = {'0', 'a', '1', '2', 'b', '3', 'd', 'e', '4', 'f', 'g', 'h',
                                         'i', '5', 'j', 'k', 'l', 'm', 'n', '6', '7', '8', '9',
                                         'p', 'r', 's', '!', 't', '@', '#', '$', 'v', 'w', 'y', 'z'};

const string symbols[NUM_IPA_SYMBOLS] = {"uh", "a", "ay", "ah", "b", "ch", "d", "e", "ee", "f", "g", "h",
                          "i", "iy", "j", "k", "l", "m", "n", "ng", "oh", "oy", "or",
                          "p", "r", "s", "sh", "t", "th", "oo", "ouh", "v", "w", "y", "z"};
std::unordered_map<string, int> symbolToIndex;
std::unordered_map<char, int> charToIndex;

void readIPAWeights(int matrix[NUM_IPA_SYMBOLS][NUM_IPA_SYMBOLS], std::fstream& ipaFile) {
    std::string instring;

    int index = 0;
    string currentLetter;
    while(ipaFile >> instring) {
        if(instring.find(':') != string::npos) {
            instring = instring.substr(0, instring.size()-1);
            currentLetter = instring;
        } else if (instring.find('?') != string::npos) {
            break;
        } else if (instring.find('\n') != string::npos) {
            continue;
        } else {
            int weight = stoi(instring);
            ipaFile >> instring;
            int &currentIndex = symbolToIndex[currentLetter];
            int &newIndex = symbolToIndex[instring];
            adjMatrix[currentIndex][newIndex] = weight;
            adjMatrix[newIndex][currentIndex] = weight;
        }
    }

    for(int i = 0; i < NUM_IPA_SYMBOLS; i++) {
        for(int j = 0; j < NUM_IPA_SYMBOLS; j++) {
            if (i == j) { // Same symbols have distance 0.
                adjMatrix[i][j] = 0;
            } else if(adjMatrix[i][j] == 0) {
                adjMatrix[i][j] = 7;
            }
        }
    }
}
int omissionCost(char c) {
    if(c == 's') return 2;
    return 6;
}*/

// Will be eclipsed by the use of fuzzy string later, anyway
/*int directDistance(string first, string second) {
    int sum = 0;
    for(int i = 0; i < first.length() && i < second.length(); i++) {
        int firstIndex = charToIndex[first[i]];
        int secondIndex = charToIndex[second[i]];
        sum += adjMatrix[firstIndex][secondIndex];
    }
    return sum;
}*/

// Incomplete
/*
const int THRESHOLD = 7;
int distance(string first, string second) {
    string shorter;
    string longer;
    if(first.length() < second.length()) {
        shorter = first;
        longer = second;
    } else {
        shorter = second;
        longer = first;
    }

    // Find longest "block"
    int index1;
    int index2;
    int maxLength = 0;
    for(int i = 0; i < shorter.length(); i++) {
        for(int j = 0; j < longer.length(); j++) {
            int length = 1;
            while(i + length < shorter.length() && j + length < longer.length() &&
            directDistance(shorter.substr(i,length), longer.substr(j, length)) < THRESHOLD) {
                length++;
            }
            if(length > maxLength) {
                index1 = i;
                index2 = j;
                maxLength = length;
            }
        }
    }

    // Left side
    int leftBestIndex1 = 0;
    int leftBestIndex2 = 0;
    for(int i = 0; i < index1; i++) {

    }

    std::cout << shorter.substr(index1, maxLength) << ", " << longer.substr(index2, maxLength) << std::endl;
    return 0;
}*/

bool search(char lyrics[]) {
    // Placeholder
    std::cout << "Beantastic!\n";

    return false;
}

int main() {
    const int WINDOW_WIDTH = 500;
    const int WINDOW_HEIGHT = 700;

    const int SEARCHBAR_HEIGHT = 80;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Song Searcher");
    window.setFramerateLimit(30); // Setting higher to 30 makes the refresh rate of the textbox go nutty. Can't backspace once without deleting everything.

    ImGui::SFML::Init(window);
    sf::Time deltaTime = sf::milliseconds(100); // Not exactly... Sure what this is used for, to be honest?

    static char inputLyrics[128] = ""; // Captures input (?)
    static char inputHint[128] = "Type your lyrics here!"; // Hint text

    bool justHereToMakeImGuiHappy;
    bool &isOpen = justHereToMakeImGuiHappy; // (This is a joke. We should remove this before submission.)

    bool hasSearched = false; // False at the start; To stop "no results" from showing up before searching.
    bool searchStatus = false; // Captures whether our search succeeded.

    while(window.isOpen()) { // Main loop
        sf::Event event;
        while (window.pollEvent(event)) { // Capture SFML events
            ImGui::SFML::ProcessEvent(window, event);
            if(event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.resetGLStates(); // Allegedly, we need this if we don't draw using SFML.

        window.clear(sf::Color::Black);

        ImGui::SFML::Update(window, deltaTime); // Re: No idea what this does?

        // Searchbar
        ImGui::SetNextWindowSize({WINDOW_WIDTH, SEARCHBAR_HEIGHT});
        ImGui::Begin("Help", &isOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::PushItemWidth(-1); // I think(?) this sets the width to max for the searchbar.
        // The "##" prevents the text "SearchBar" from appearing in front of the searchbar.
        if (ImGui::InputTextWithHint("##SearchBar", inputHint, inputLyrics, IM_ARRAYSIZE(inputLyrics), ImGuiInputTextFlags_EnterReturnsTrue)) {
            searchStatus = search(inputLyrics);
            hasSearched = true;
        }
        ImGui::PopItemWidth();

        //Search button
        if(ImGui::Button("Search")) {
            search(inputLyrics);
            hasSearched = true;
        }
        ImGui::End();


        // Results panel
        ImGui::SetNextWindowSize({WINDOW_WIDTH, WINDOW_HEIGHT - SEARCHBAR_HEIGHT}); // Fill rest of space
        ImGui::SetNextWindowPos({0, SEARCHBAR_HEIGHT}); // Start where the other panel ended
        ImGui::Begin("##ResultsWindow", &hasSearched, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        if(hasSearched && searchStatus == false) { // Writing "== false" explicitly bc i named searchStatus badly
            ImGui::Text("Sorry, no results found!");
        }
        ImGui::End();

        // Render
        ImGui::SFML::Render(window);
        window.display();
    }

    // Close everything
    ImGui::SFML::Shutdown();
    return 0;
}
