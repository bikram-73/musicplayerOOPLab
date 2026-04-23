#include "libraryscanner.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

bool LibraryScanner::parseID3Tag(const QString& filePath, QString& title, QString& artist) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;

    // Check for ID3v2
    char header[10];
    if (file.read(header, 10) != 10) return false;

    if (header[0] == 'I' && header[1] == 'D' && header[2] == '3') {
        int tagSize = ((header[6] & 0x7F) << 21) | ((header[7] & 0x7F) << 14) |
                      ((header[8] & 0x7F) << 7) | (header[9] & 0x7F);

        QByteArray tagData = file.read(tagSize);
        int pos = 0;

        while (pos < tagSize - 10) {
            char frameId[5] = {0};
            memcpy(frameId, tagData.data() + pos, 4);
            
            if (frameId[0] == 0) break; // Padding reached

            int frameSize = (tagData[pos + 4] << 24) | ((tagData[pos + 5] & 0xFF) << 16) |
                            ((tagData[pos + 6] & 0xFF) << 8) | (tagData[pos + 7] & 0xFF);
            
            // Safe guard against corrupt tags
            if (frameSize <= 0 || pos + 10 + frameSize > tagSize) break;

            if (strcmp(frameId, "TIT2") == 0 || strcmp(frameId, "TPE1") == 0) {
                // The first byte of the frame content is the text encoding
                char encoding = tagData[pos + 10];
                QString text;
                if (encoding == 0 || encoding == 3) {
                    // ISO-8859-1 or UTF-8
                    text = QString::fromUtf8(tagData.mid(pos + 11, frameSize - 1));
                } else if (encoding == 1 || encoding == 2) {
                    // UTF-16
                    text = QString::fromUtf16(reinterpret_cast<const char16_t*>(tagData.constData() + pos + 11), (frameSize - 1) / 2);
                }

                // Clean up string: remove trailing null terminators that render as box icons in Qt
                while (text.endsWith(QChar('\0'))) {
                    text.chop(1);
                }
                text = text.trimmed();
                
                // Clean up web anomalies embedded in ID3 tag
                text.replace("&#039;", "'");
                text.replace("&amp;", "&");
                text.replace("&quot;", "\"");

                // We are purposely ignoring "TIT2" to enforce literal filesystem names for Track Titles
                if (strcmp(frameId, "TPE1") == 0) artist = text;
            }
            
            pos += 10 + frameSize; // Move to next frame
        }
        return true;
    }
    return false;
}

std::vector<Track> LibraryScanner::scanDirectory(const QString& directoryPath) {
    std::vector<Track> detectedTracks;
    QDir dir(directoryPath);
    if (!dir.exists()) return detectedTracks;

    QStringList filters;
    filters << "*.mp3";
    dir.setNameFilters(filters);
    
    QFileInfoList fileList = dir.entryInfoList(QDir::Files);
    for (const QFileInfo& fileInfo : fileList) {
        QString title = fileInfo.baseName();
        QString artist = "Unknown Artist";
        
        parseID3Tag(fileInfo.absoluteFilePath(), title, artist);
        
        // Find matching cover art inside the same directory
        QString coverPath = "";
        QStringList extList = {".jpg", ".jpeg", ".png"};
        for (const QString& ext : extList) {
            if (QFile::exists(dir.absoluteFilePath(fileInfo.baseName() + ext))) {
                coverPath = fileInfo.baseName() + ext; // Store just the filename
                break;
            }
        }
        
        // Use a dummy ID for now, will get proper ID in database
        Track t(0, title.toStdString(), artist.toStdString(), fileInfo.fileName().toStdString(), 200, coverPath.toStdString(), false);
        detectedTracks.push_back(t);
    }
    
    return detectedTracks;
}
