#include "playlist.h"

Playlist::Playlist(int playlistId, std::string playlistName) {
    id = playlistId;
    name = playlistName;
    currentIndex = -1;
}

int Playlist::getId() const { return id; }
std::string Playlist::getName() const { return name; }
void Playlist::setName(const std::string& newName) { name = newName; }

void Playlist::addTrack(const Track& track) {
    tracks.push_back(track);
    if (currentIndex == -1) {
        currentIndex = 0; // Select the first track added
    }
}

void Playlist::removeTrack(int index) {
    if (index >= 0 && index < static_cast<int>(tracks.size())) {
        tracks.erase(tracks.begin() + index);
        if (currentIndex >= static_cast<int>(tracks.size())) {
            currentIndex = tracks.size() - 1;
        }
    }
}

void Playlist::clear() {
    tracks.clear();
    currentIndex = -1;
}

Track* Playlist::getCurrentTrack() {
    if (tracks.empty() || currentIndex < 0 || currentIndex >= static_cast<int>(tracks.size())) {
        return nullptr;
    }
    return &tracks[currentIndex];
}

void Playlist::nextTrack() {
    if (!tracks.empty()) {
        currentIndex++;
        if (currentIndex >= static_cast<int>(tracks.size())) {
            currentIndex = 0;
        }
    }
}

void Playlist::previousTrack() {
    if (!tracks.empty()) {
        currentIndex--;
        if (currentIndex < 0) {
            currentIndex = tracks.size() - 1;
        }
    }
}

int Playlist::getTrackCount() const {
    return tracks.size();
}

bool Playlist::isEmpty() const {
    return tracks.empty();
}

void Playlist::setCurrentIndex(int index) {
    if (index >= 0 && index < static_cast<int>(tracks.size())) {
        currentIndex = index;
    }
}

int Playlist::getCurrentIndex() const {
    return currentIndex;
}

const std::vector<Track>& Playlist::getTracks() const {
    return tracks;
}