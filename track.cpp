#include "track.h"

// Constructor Implementation
Track::Track(int trackId, std::string t, std::string a, std::string path, int duration, std::string cover, bool liked) {
    id = trackId;
    title = t;
    artist = a;
    filePath = path;
    durationSeconds = duration;
    coverPath = cover;
    isLiked = liked;
}

// Getter Implementations
int Track::getId() const { return id; }
std::string Track::getTitle() const { return title; }
std::string Track::getArtist() const { return artist; }
std::string Track::getFilePath() const { return filePath; }
int Track::getDuration() const { return durationSeconds; }
std::string Track::getCoverPath() const { return coverPath; }
bool Track::getIsLiked() const { return isLiked; }

// Setter
void Track::setIsLiked(bool liked) {
    isLiked = liked;
}
