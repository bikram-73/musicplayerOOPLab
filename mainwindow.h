#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTableWidgetItem>
#include <QListWidgetItem>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include "playlist.h"
#include "user.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(User user, QWidget *parent = nullptr);
    ~MainWindow() override;
    
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void on_btnPlayPause_clicked();
    void on_btnNext_clicked();
    void on_btnPrev_clicked();
    void on_trackTable_itemDoubleClicked(QTableWidgetItem *item);
    void on_volumeSlider_valueChanged(int value);

    void on_btnHome_clicked();
    void on_btnSearch_clicked();
    void on_btnLibrary_clicked();
    
    void on_btnLike_clicked();
    void on_btnAddPlaylist_clicked();
    void on_listPlaylists_itemClicked(QListWidgetItem *item);
    
    void on_lineSearch_textChanged(const QString &arg1);
    void on_listHomeItems_itemDoubleClicked(QListWidgetItem *item);
    void on_searchTable_itemDoubleClicked(QTableWidgetItem *item);

    // Dynamic Artist Profile slot
    void on_tableCellClicked(int row, int column, QTableWidget* table);
    void on_artistTrackTable_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::MainWindow *ui;
    User currentUser;

    Playlist likedSongsPlaylist;
    std::vector<Playlist> customPlaylists;
    Playlist* currentViewPlaylist; // Pointer to currently viewed playlist
    Playlist activePlayingPlaylist; // Copy or pointer to what's currently playing

    std::vector<Track> allTracks;

    QMediaPlayer *player;
    QAudioOutput *audioOutput;

    void setupDatabase();
    void applySpotifyStyle();
    void applyFloatingAesthetics(); // New floating UI builder
    void setupArtistPage(); // New dynamic page builder
    void openArtistProfile(const QString& artistName);

    void updateNowPlayingInfo();
    void loadHomeData();
    void populateTrackTable(const Playlist& playlist, QTableWidget* table);
    void refreshPlaylistsList();
    void toggleLikeStatus();
    
    // Dynamic Artist Profile Widgets
    QWidget* pageArtist;
    QLabel* lblArtistHeader;
    QTableWidget* artistTrackTable;
    Playlist artistPlaylist;
    
    // Dynamic Home Grid Widgets
    QScrollArea* homeScrollArea;
    QWidget* homeGridWidget;
    QGridLayout* homeGridLayout;
};
#endif // MAINWINDOW_H
