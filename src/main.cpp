#include <iostream>
#include <fstream>
#include <sstream>
#include <sfml/Graphics.hpp>
#include <unordered_map>
#include <utility>

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include "ImGui-SFML/imgui-SFML.h"

#include "WordConverter.h"
#include "LyricMatcher.h"

using std::string;

const int WINDOW_WIDTH = 1240;
const int WINDOW_HEIGHT = 800;

const int ICON_WIDTH = 128;
const int ICON_HEIGHT = 128;

const float BUTTON_WIDTH = 80;
const float SEARCH_XPADDING = 10;
const float RESULTS_YPADDING = 15;

const int NUM_RESULTS = 10;

enum SortType {STD, MERGE, QUICK};
struct SongResult {
    //Icon icon;
    string name;
    string artist;
    std::vector<string> lyricsExcerpt;
    double matchRatio;
    int matchingLineIndex = 1;
    SongResult() = default;
    SongResult(string name, string artist, std::vector<string> lyricsExcerpt, double matchRatio, int matchingLineIndex = 1) : name(std::move(name)), artist(std::move(artist)), lyricsExcerpt(std::move(lyricsExcerpt)), matchRatio(matchRatio), matchingLineIndex(matchingLineIndex) {}
};
std::vector<SongResult> search(const char lyrics[], SortType sort, std::vector<Lyric> &allLyrics,
                               std::unordered_map<std::string, std::string>& ipaMap,
                               std::unordered_map<std::string,std::string>& pronunciationDictionary,
                               std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>& allSongs);

int main() {
    // Initial setup
    // Initialize IPA map
    std::unordered_map<std::string, std::string> ipaMap;
    FillIpaMap(ipaMap);

    // Read in dictionary
    std::unordered_map<std::string,std::string> pronunciationDictionary;
    std::fstream stored_dict("data/dictall.csv", std::ios::in);

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
        pronunciationDictionary.emplace(word, pronunciation);
    }
    stored_dict.close();


    // Read in lyrics
    std::fstream inFile("data/someconvertedlyrics.csv", std::ios::in);

    std::vector<Lyric> allLyrics;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> allSongs; // artist -> song -> vector of lyrics

    if(inFile.is_open())
    {
        while(inFile.peek() != EOF)
        {
            // EACH SONG
            std::string index, artistName, songTitle, junk, songLyrics, originalLyric, phoneticLyric;

            std::getline(inFile, index, ',');
            std::getline(inFile, artistName, ',');
            std::getline(inFile, songTitle, ',');
            std::getline(inFile, junk, '"'); // opening quote
            std::getline(inFile, songLyrics, '"');

            std::stringstream songLyricsStream(songLyrics);
            bool isLastLine = false;
            std::unordered_set<std::string> uniqueLyrics; // stores the unique lyric within the current song
            std::vector<std::string> fullSongLyrics;

            int lyricLineIndex = 0;

            while(std::getline(songLyricsStream, originalLyric)) // by new line character
            {
                // EACH LYRIC
                std::getline(songLyricsStream, phoneticLyric);

                if(originalLyric.front() == '[' || originalLyric.front() == ']' || originalLyric.empty()) // not a valid lyric
                {
                    continue;
                }

                fullSongLyrics.push_back(originalLyric);

                if(uniqueLyrics.count(originalLyric) != 1 && !originalLyric.empty()) // if we haven't seen this lyric before in the current song
                {
                    // create a new Lyric object and push it back into the vector

                    uniqueLyrics.insert(originalLyric);
                    //Lyric* lyric = new Lyric(artistName, songTitle, originalLyric, phoneticLyric);
                    Lyric lyric(artistName, songTitle, originalLyric, phoneticLyric, lyricLineIndex);
                    allLyrics.push_back(lyric);
                }

                lyricLineIndex++;

            }

            if(allSongs.count(artistName) == 0)
            {
                // if the artist isn't included yet
                std::unordered_map<std::string, std::vector<std::string>> artistSongs;
                allSongs.emplace(artistName, artistSongs);
            }
            if(allSongs[artistName].count(songTitle) == 0)
            {
                // if this song from the artist isn't there yet
                allSongs[artistName].emplace(songTitle, fullSongLyrics);
            }



        }
        std::cout << "Vector Size: " << allLyrics.size() << std::endl; // should be 1890406, unique lyrics

        inFile.close();

    }
    //sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "LyricMatch", sf::Style::Titlebar | sf::Style::Close);
    window.setSize({WINDOW_WIDTH, WINDOW_HEIGHT});

    // Load icon
    auto image = sf::Image{};
    if (!image.loadFromFile("src/assets/songfinder_icon.png"))
    {
        std::cout << "Didn't find icon... Oh well!\n";
    }
    window.setIcon(ICON_WIDTH, ICON_HEIGHT, image.getPixelsPtr());

    // Load Merriam-Webster's logo
    sf::Texture logoTexture;
    if (!logoTexture.loadFromFile("src/assets/MWLogo.png"))
    {
        throw std::runtime_error("Merriam-Webster logo not found!");
    }
    sf::Sprite logo;
    logo.setTexture(logoTexture);

    window.setFramerateLimit(30); // Setting higher to 30 makes the refresh rate of the textbox go nutty. Can't backspace once without deleting everything.
    if(!ImGui::SFML::Init(window)) {
        throw std::runtime_error("Failed to initialize window.");
    }

    sf::Time deltaTime = sf::milliseconds(100); // Not exactly... Sure what this is used for, to be honest?

    static char inputLyrics[128] = ""; // Captures input (?)
    static char inputHint[128] = "Type your lyrics here!"; // Hint text

    bool isOpen; // The ImGui window wants a bool reference, but we're not really going to use it
    bool hasSearched = false; // False only at the start; To stop "no results" from showing up before searching.
    std::vector<SongResult> results; // For storing search results


    // Load fonts
    auto &io = ImGui::GetIO();

    io.Fonts->AddFontFromFileTTF("src/assets/arial.ttf", 24);
    io.Fonts->AddFontFromFileTTF("src/assets/arialbd.ttf", 24);
    io.Fonts->AddFontFromFileTTF("src/assets/framd.ttf", 30);
    io.Fonts->AddFontFromFileTTF("src/assets/FRAHV.ttf", 30);

    enum Fonts {ARIAL = 1, ARIAL_BOLD = 2, FRANK = 3, FRANK_BOLD = 4};

    if(!ImGui::SFML::UpdateFontTexture()) {
        throw std::runtime_error("Failed update font texture.");
    }

    SortType currentSort = STD;

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

        

        // Searchbar
        ImGui::SetNextWindowPos({0,0});
        ImGui::SetNextWindowSize({WINDOW_WIDTH, WINDOW_HEIGHT});
        ImGui::Begin("Help", &isOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus);
        ImGui::PushItemWidth(ImGui::GetWindowWidth() - 2*SEARCH_XPADDING - BUTTON_WIDTH); // I think(?) this sets the width to max for the searchbar.

        ImGui::SetCursorPosX(0);
        ImGui::SetCursorPosY(0);

        float search_yPos = ImGui::GetCursorPosY();

        // Input textbox
        // The "##" prevents the text "SearchBar" from appearing in front of the searchbar.
        ImGui::PushFont(io.Fonts->Fonts[ARIAL]);
        if (ImGui::InputTextWithHint("##SearchBar", inputHint, inputLyrics, IM_ARRAYSIZE(inputLyrics), ImGuiInputTextFlags_EnterReturnsTrue)) {
            results = search(inputLyrics, currentSort, allLyrics, ipaMap, pronunciationDictionary, allSongs);
            hasSearched = true;
        }
        ImGui::PopItemWidth();

        //Search button
        ImGui::SetCursorPosX(ImGui::GetWindowWidth()-BUTTON_WIDTH - SEARCH_XPADDING);
        ImGui::SetCursorPosY(search_yPos);
        ImGui::PushItemWidth(BUTTON_WIDTH);
        if(ImGui::Button("Search")) {
            results = search(inputLyrics, currentSort, allLyrics, ipaMap, pronunciationDictionary, allSongs);
            hasSearched = true;
        }
        ImGui::PopItemWidth();
        ImGui::PopFont();

        // Sort selection
        if(ImGui::RadioButton("STD::Sort (recommended)", currentSort == STD)) {
            currentSort = STD;
        }
        if(ImGui::RadioButton("QuickSort", currentSort == QUICK)) {
            currentSort = QUICK;
        }
        if(ImGui::RadioButton("MergeSort", currentSort == MERGE)) {
            currentSort = MERGE;
        }

        // Results panel
        ImGui::BeginChild("##ResultsPanel");
        if(hasSearched) {
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
                    string match = std::to_string(song.matchRatio).substr(0, 5);
                    string byArtistText = " by " + song.artist + " (" + match + "%% match)";
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

        logo.setPosition(20, WINDOW_HEIGHT - 260);
        window.draw(logo);

        // Render
        ImGui::SFML::Render(window);
        window.display();
    }

    // Close everything
    ImGui::SFML::Shutdown();
    return 0;
}

std::vector<SongResult> search(const char lyrics[], SortType sort, std::vector<Lyric> &allLyrics, std::unordered_map<std::string,
                               std::string>& ipaMap, std::unordered_map<std::string,std::string>& pronunciationDictionary,
                               std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>& allSongs) {
    string input = lyrics;
    string inputPhonetic = ConvertSearchToPhonetic(input, ipaMap, pronunciationDictionary);
    if(inputPhonetic.empty()) return {};
    CalcFuzzRatio(allLyrics, inputPhonetic);

    // For printing out time; wouldn't be used in the actual program.
    auto start = std::chrono::system_clock::now();

    // Choose sort
    switch(sort) {
        case STD:
            std::sort(allLyrics.begin(), allLyrics.end(), std::less<Lyric>());
            break;
        case MERGE:
            MergeSort(allLyrics, 0, allLyrics.size()-1);
            break;
        case QUICK:
            QuickSort(allLyrics, 0, allLyrics.size()-1);
            break;
    }

    auto end = std::chrono::system_clock::now();

    // Time things copied from en.ccpreference.com via "Frederick the Fool" on stackoverflow
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Sorted in " << elapsed_seconds.count() << "s" << std::endl;

    std::vector<SongResult> results;
    for(int i = allLyrics.size()-1; i >= allLyrics.size()-NUM_RESULTS; i--) {
        SongResult song;
        song.name = allLyrics.at(i).songTitle;
        song.artist = allLyrics.at(i).artistName;
        song.matchingLineIndex = 0; // Fix later
        int &index = allLyrics.at(i).positionIndex;
        auto &songLyrics = allSongs[song.artist][song.name];

        if(songLyrics.size() < 4) {
            song.lyricsExcerpt = songLyrics;
        } else if (index == 0) {
            song.matchingLineIndex = 0;
            song.lyricsExcerpt = {songLyrics[index], songLyrics[index+1], songLyrics[index+2], songLyrics[index+3]};
        } else if(index < songLyrics.size()-2) {
            song.matchingLineIndex = 1;
            song.lyricsExcerpt = {songLyrics[index-1], songLyrics[index], songLyrics[index+1], songLyrics[index+2]};
        } else {
            int linesLeft = allSongs[song.artist][song.name].size() - index - 1;
            song.matchingLineIndex = 3 - linesLeft;
            song.lyricsExcerpt = {songLyrics[index-3 + linesLeft], songLyrics[index-2 + linesLeft],
                                  songLyrics[index-1 + linesLeft], songLyrics[index+linesLeft]};
        }
        song.matchRatio = allLyrics.at(i).fuzzRatio;
        results.push_back(song);
    }
    return results;
}