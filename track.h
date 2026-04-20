#ifndef TRACK_H
#define TRACK_H

#include <string>

class Track {
private:
    int id;
    std::string title;
    std::string artist;
    std::string filePath;
    int durationSeconds;
    std::string coverPath;
    bool isLiked;

public:
    // Constructor
    Track(int id, std::string t, std::string a, std::string path, int duration, std::string cover, bool liked);

    // Getters
    int getId() const;
    std::string getTitle() const;
    std::string getArtist() const;
    std::string getFilePath() const;
    int getDuration() const;
    std::string getCoverPath() const;
    bool getIsLiked() const;

    // Setter
    void setIsLiked(bool liked);
};

#endif // TRACK_H