#ifndef LIBRARYSCANNER_H
#define LIBRARYSCANNER_H

#include <QString>
#include <vector>
#include "track.h"

class LibraryScanner {
public:
    static std::vector<Track> scanDirectory(const QString& directoryPath);
    static bool parseID3Tag(const QString& filePath, QString& title, QString& artist);
};

#endif // LIBRARYSCANNER_H
