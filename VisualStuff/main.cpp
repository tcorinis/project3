#include <iostream>
#include <fstream>
#include <sfml/Graphics.hpp>
#include <unordered_map>
#include <utility>
#include "ImGui/imgui.h"
#include "ImGui-SFML/imgui-SFML.h"
#include "ImGui/imgui_internal.h"

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

struct SongResult {
    //Icon icon;
    string name;
    string artist;
    std::vector<string> lyricsExcerpt;
    int matchingLineIndex = 1;
    SongResult() = default;
    SongResult(string name, string artist, std::vector<string> lyricsExcerpt, int matchingLineIndex = 1) : name(std::move(name)), artist(std::move(artist)), lyricsExcerpt(std::move(lyricsExcerpt)), matchingLineIndex(matchingLineIndex) {}
};

std::vector<SongResult> search(const char lyrics[]) {
    if(lyrics[0] == '\0') return {};

    std::vector<SongResult> results;
    for(int i = 0; i < 10; i++) {
        SongResult song("Blank Space", "Taylor Swift", {"Isn't it weird", "All of these", "Are the same", "More or less?"});
        results.push_back(song);
    }
    return results;
}

int main() {
    const int WINDOW_WIDTH = 500;
    const int WINDOW_HEIGHT = 700;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Song Searcher");
    const int ICON_WIDTH = 128;
    const int ICON_HEIGHT = 128;

    auto image = sf::Image{};
    if (!image.loadFromFile("assets/songfinder_icon.png"))
    {
        std::cout << "Didn't find icon... Oh well!\n";
    }
    window.setIcon(ICON_WIDTH, ICON_HEIGHT, image.getPixelsPtr());

    window.setFramerateLimit(30); // Setting higher to 30 makes the refresh rate of the textbox go nutty. Can't backspace once without deleting everything.

    ImGui::SFML::Init(window);
    sf::Time deltaTime = sf::milliseconds(100); // Not exactly... Sure what this is used for, to be honest?

    static char inputLyrics[128] = ""; // Captures input (?)
    static char inputHint[128] = "Type your lyrics here!"; // Hint text

    bool justHereToMakeImGuiHappy;
    bool &isOpen = justHereToMakeImGuiHappy; // (This is a joke. We should remove this before submission.)

    bool hasSearched = false; // False at the start; To stop "no results" from showing up before searching.
    std::vector<SongResult> results; // Search results

    // Load fonts
    auto &io = ImGui::GetIO();

    io.Fonts->AddFontFromFileTTF("assets/arial.ttf", 24);
    io.Fonts->AddFontFromFileTTF("assets/arialbd.ttf", 24);
    io.Fonts->AddFontFromFileTTF("assets/framd.ttf", 30);
    io.Fonts->AddFontFromFileTTF("assets/FRAHV.ttf", 30);

    enum Fonts {ARIAL = 1, ARIAL_BOLD = 2, FRANK = 3, FRANK_BOLD = 4};

    ImGui::SFML::UpdateFontTexture();

    // Main loop
    while(window.isOpen()) {
        // Process events; we only really use ImGui events.
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if(event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.resetGLStates(); // Allegedly, we need this if we don't draw using SFML.

        window.clear(sf::Color::Black); // Clear screen for refresh

        ImGui::SFML::Update(window, deltaTime); // Re: Not sure what this does?

        // Disable CTRL+Tab shortcuts (global); these two lines via "ocornut" (https://github.com/ocornut/imgui/issues/5641)
        ImGui::SetShortcutRouting(ImGuiMod_Ctrl | ImGuiKey_Tab, ImGuiKeyOwner_None);
        ImGui::SetShortcutRouting(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Tab, ImGuiKeyOwner_None);

        const float BUTTON_WIDTH = 80;
        const float SEARCH_XPADDING = 10;

        // Searchbar
        ImGui::SetNextWindowSize({WINDOW_WIDTH, WINDOW_HEIGHT});
        ImGui::Begin("Help", &isOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus);
        ImGui::PushItemWidth(ImGui::GetWindowWidth() - 2*SEARCH_XPADDING - BUTTON_WIDTH); // I think(?) this sets the width to max for the searchbar.

        float search_yPos = ImGui::GetCursorPosY();

        // Input textbox
        // The "##" prevents the text "SearchBar" from appearing in front of the searchbar.
        ImGui::PushFont(io.Fonts->Fonts[ARIAL]);
        if (ImGui::InputTextWithHint("##SearchBar", inputHint, inputLyrics, IM_ARRAYSIZE(inputLyrics), ImGuiInputTextFlags_EnterReturnsTrue)) {
            results = search(inputLyrics);
            hasSearched = true;
        }
        ImGui::PopItemWidth();

        //Search button
        ImGui::SetCursorPosX(ImGui::GetWindowWidth()-BUTTON_WIDTH - SEARCH_XPADDING);
        ImGui::SetCursorPosY(search_yPos);
        ImGui::PushItemWidth(BUTTON_WIDTH);
        if(ImGui::Button("Search")) {
            results = search(inputLyrics);
            hasSearched = true;
        }
        ImGui::PopItemWidth();
        ImGui::PopFont();

        // Results panel
        ImGui::BeginChild("##ResultsPanel");
        if(hasSearched) {
            const float RESULTS_YPADDING = 15;
            if (results.empty()) {
                // No results, just draw error text.
                ImGui::PushFont(io.Fonts->Fonts[ARIAL]);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + RESULTS_YPADDING);
                string errorText = "Sorry, no results found!";
                ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(errorText.c_str()).x)*0.5f);
                ImGui::Text(errorText.c_str());
                ImGui::PopFont();
            } else {
                // Draw song result boxes
                ImGui::SetCursorPosY(RESULTS_YPADDING); // Initial padding, so the first song box matches the others
                for(auto &song : results) {
                    // Song title and "by Artist"
                    float titleY = ImGui::GetCursorPosY();

                    // Need to calculate how to center
                    ImGui::PushFont(io.Fonts->Fonts[FRANK_BOLD]);
                    auto songName = song.name.c_str();
                    float nameWidth = ImGui::CalcTextSize(songName).x;

                    ImGui::PushFont(io.Fonts->Fonts[FRANK]);
                    string byArtistText = " by " + song.artist;
                    float byArtistWidth = ImGui::CalcTextSize(byArtistText.c_str()).x;

                    // Drawing byArtist first because of the stack structure.
                    ImGui::SetCursorPosX((WINDOW_WIDTH - nameWidth - byArtistWidth)*0.5f + nameWidth);
                    ImGui::Text(byArtistText.c_str());
                    ImGui::PopFont();

                    // Draw song title.
                    ImGui::SetCursorPosX((WINDOW_WIDTH - nameWidth - byArtistWidth)*0.5f);
                    ImGui::SetCursorPosY(titleY); // Restore this because drawing moved it
                    ImGui::Text(songName);
                    ImGui::PopFont();

                    // Add padding before lyrics
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + RESULTS_YPADDING);

                    // Lyrics
                    auto &excerpt = song.lyricsExcerpt;
                    for(int i = 0; i < excerpt.size(); i++) {
                        // Bold the matching line
                        ImGui::PushFont(i == song.matchingLineIndex ? io.Fonts->Fonts[ARIAL_BOLD] : io.Fonts->Fonts[ARIAL]);

                        // Centering
                        auto xPos = (WINDOW_WIDTH - ImGui::CalcTextSize(excerpt[i].c_str()).x)*0.5;

                        // Appending ellipses
                        string line;
                        if(i == 0) {
                            line = "...";
                            xPos -= ImGui::CalcTextSize("...").x; // Center without ellipsis
                        }
                        line += excerpt[i];
                        if(i == excerpt.size()-1) line += "...";

                        // Put on screen
                        ImGui::SetCursorPosX(xPos);
                        ImGui::Text(line.c_str());
                        ImGui::PopFont();
                    }

                    // Add closing line and padding
                    float yPos = ImGui::GetCursorPosY();
                    yPos += RESULTS_YPADDING; // Padding
                    ImGui::SetCursorPosY(yPos);
                    ImGui::DrawLine({0, 0}, {WINDOW_WIDTH, 0}, ImGui::GetStyle().Colors[ImGuiCol_Border], 2);
                    yPos += RESULTS_YPADDING; // More padding
                    ImGui::SetCursorPosY(yPos);
                }
            }
        }
        ImGui::EndChild();
        ImGui::End();

        // Render
        ImGui::SFML::Render(window);
        window.display();
    }

    // Close everything
    ImGui::SFML::Shutdown();
    return 0;
}
