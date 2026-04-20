#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <vector>
#include <string>
#include "track.h"

class Playlist {
private:
    int id;
    std::string name;
    std::vector<Track> tracks;
    int currentIndex;

public:
    // Constructor
    Playlist(int playlistId = 0, std::string playlistName = "Default Playlist");

    int getId() const;
    std::string getName() const;
    void setName(const std::string& newName);

    // Core functionality
    void addTrack(const Track& track);
    void removeTrack(int index);
    void clear();

    // Navigation
    Track* getCurrentTrack();
    void nextTrack();
    void previousTrack();

    // Status
    int getTrackCount() const;
    bool isEmpty() const;

    void setCurrentIndex(int index);
    int getCurrentIndex() const;
    const std::vector<Track>& getTracks() const;
};

#endif // PLAYLIST_H