#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUrl>
#include <QTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

MainWindow::MainWindow(User user, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentUser(user)
{
    ui->setupUi(this);

    // Initial setup
    currentViewPlaylist = nullptr;
    
    // Apply main style
    applySpotifyStyle();

    // Init Audio Engine
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5);
    ui->volumeSlider->setValue(50);

    // Initial labels
    ui->lblPlayingTitle->setText("No track playing");
    ui->lblPlayingArtist->setText("");
    ui->lblCoverArt->setPixmap(QPixmap());
    ui->btnLike->setText("♡");

    // Hook up sliders
    connect(player, &QMediaPlayer::durationChanged, this, [this](qint64 duration) {
        ui->progressSlider->setMaximum(duration);
        QTime totalTime(0, 0, 0);
        totalTime = totalTime.addMSecs(duration);
        ui->lblTotalTime->setText(totalTime.toString("m:ss"));
    });

    connect(player, &QMediaPlayer::positionChanged, this, [this](qint64 pos) {
        if (!ui->progressSlider->isSliderDown()) {
            ui->progressSlider->setValue(pos);
        }
        QTime currentTime(0, 0, 0);
        currentTime = currentTime.addMSecs(pos);
        ui->lblCurrentTime->setText(currentTime.toString("m:ss"));
    });

    connect(ui->progressSlider, &QSlider::sliderMoved, this, [this](int pos) {
        player->setPosition(pos);
    });

    setupDatabase();
    
    // Page switching connections
    ui->stackedWidget->setCurrentWidget(ui->pageHome);
    loadHomeData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::applySpotifyStyle()
{
    // Keeping same styles with some additions for lists and inputs
    this->setStyleSheet(R"(
        QMainWindow { background-color: #000000; }
        QSplitter::handle { background: transparent; }
        QWidget#sidebarWidget { background-color: #000000; }
        
        QPushButton#btnHome, QPushButton#btnSearch, QPushButton#btnLibrary, QPushButton#btnAddPlaylist {
            color: #b3b3b3; text-align: left; padding: 10px; font-size: 14px; font-weight: bold; border: none; border-radius: 5px;
        }
        QPushButton#btnHome:hover, QPushButton#btnSearch:hover, QPushButton#btnLibrary:hover, QPushButton#btnAddPlaylist:hover {
            color: #ffffff; background-color: #282828;
        }
        
        QWidget#pageHome, QWidget#pageSearch, QWidget#pageLibrary {
            background-color: #121212; border-top-left-radius: 8px; border-top-right-radius: 8px;
        }
        
        QLabel#lblPlaylistHeader {
            color: #ffffff; padding: 30px; background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #1db954, stop:1 #121212);
            border-top-left-radius: 8px; border-top-right-radius: 8px;
        }
        
        QTableWidget {
            background-color: #121212; color: #b3b3b3; gridline-color: transparent; border: none;
            selection-background-color: #2a2a2a; selection-color: #ffffff; outline: 0; font-size: 13px;
        }
        QTableWidget::item { border: none; padding: 5px; }
        QHeaderView::section {
            background-color: #121212; color: #b3b3b3; padding: 4px; border: none; border-bottom: 1px solid #282828;
            font-size: 12px; font-weight: bold; text-align: left;
        }
        QTableWidget::item:selected { background-color: #2a2a2a; color: #1db954; }

        QWidget#playerBarWidget { background-color: #181818; border-top: 1px solid #282828; }
        QLabel#lblPlayingTitle { color: #ffffff; font-size: 14px; font-weight: bold; }
        QLabel#lblPlayingArtist { color: #b3b3b3; font-size: 12px; }
        QLabel#lblCurrentTime, QLabel#lblTotalTime { color: #b3b3b3; font-size: 11px; }

        QPushButton#btnPrev, QPushButton#btnNext, QPushButton#btnShuffle, QPushButton#btnRepeat {
            color: #b3b3b3; font-size: 18px; background: transparent; border: none;
        }
        QPushButton#btnPrev:hover, QPushButton#btnNext:hover, QPushButton#btnShuffle:hover, QPushButton#btnRepeat:hover {
            color: #ffffff;
        }
        QPushButton#btnPlayPause {
            background-color: #ffffff; color: #000000; border-radius: 16px; min-width: 32px; max-width: 32px;
            min-height: 32px; max-height: 32px; font-size: 14px; font-weight: bold; border: none;
        }
        QPushButton#btnPlayPause:hover { background-color: #1ed760; }
        QLabel#lblVolumeIcon { color: #b3b3b3; font-size: 14px; }

        QSlider::groove:horizontal { border-radius: 2px; height: 4px; background: #4d4d4d; }
        QSlider::sub-page:horizontal { background: #ffffff; border-radius: 2px; }
        QSlider::sub-page:horizontal:hover { background: #1db954; }
        QSlider::handle:horizontal { background: #ffffff; width: 12px; height: 12px; margin: -4px 0; border-radius: 6px; }
    )");
}

void MainWindow::setupDatabase()
{
    QSqlDatabase db = QSqlDatabase::database();
    
    if (!db.isOpen()) {
        qDebug() << "Error: Database is not open!";
        return;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS tracks ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "title TEXT, "
               "artist TEXT, "
               "file_path TEXT, "
               "duration INTEGER, "
               "cover_path TEXT, "
               "is_liked INTEGER DEFAULT 0)");

    // Ensure older databases get the new columns
    query.exec("ALTER TABLE tracks ADD COLUMN cover_path TEXT");
    query.exec("ALTER TABLE tracks ADD COLUMN is_liked INTEGER DEFAULT 0");

    query.exec("CREATE TABLE IF NOT EXISTS playlists ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT)");

    query.exec("CREATE TABLE IF NOT EXISTS playlist_tracks ("
               "playlist_id INTEGER, "
               "track_id INTEGER)");

    query.exec("SELECT COUNT(*) FROM tracks");
    if (query.next() && query.value(0).toInt() == 0) {
        // Insert dummy data
        query.exec("INSERT INTO tracks (title, artist, file_path, duration, cover_path, is_liked) VALUES "
                   "('First Song', 'Artist A', 'my_queen.mp3', 200, '', 0)");
        query.exec("INSERT INTO tracks (title, artist, file_path, duration, cover_path, is_liked) VALUES "
                   "('Second Song', 'Artist B', 'shararat.mp3', 180, '', 1)");
        query.exec("INSERT INTO tracks (title, artist, file_path, duration, cover_path, is_liked) VALUES "
                   "('Third Song', 'Artist C', 'baby_doll.mp3', 210, '', 0)");
    }

    // Load Data
    allTracks.clear();
    likedSongsPlaylist.clear();
    likedSongsPlaylist.setName("Liked Songs");
    
    query.exec("SELECT id, title, artist, file_path, duration, cover_path, is_liked FROM tracks");
    while (query.next()) {
        int id = query.value(0).toInt();
        std::string title = query.value(1).toString().toStdString();
        std::string artist = query.value(2).toString().toStdString();
        std::string path = query.value(3).toString().toStdString();
        int duration = query.value(4).toInt();
        std::string cover = query.value(5).toString().toStdString();
        bool liked = query.value(6).toBool();

        Track newTrack(id, title, artist, path, duration, cover, liked);
        allTracks.push_back(newTrack);
        
        if (liked) {
            likedSongsPlaylist.addTrack(newTrack);
        }
    }

    // Load custom playlists
    customPlaylists.clear();
    query.exec("SELECT id, name FROM playlists");
    while (query.next()) {
        int pid = query.value(0).toInt();
        std::string pname = query.value(1).toString().toStdString();
        Playlist pl(pid, pname);
        
        // Load tracks for this playlist
        QSqlQuery tq;
        tq.prepare("SELECT t.id, t.title, t.artist, t.file_path, t.duration, t.cover_path, t.is_liked "
                   "FROM tracks t INNER JOIN playlist_tracks pt ON t.id = pt.track_id WHERE pt.playlist_id = ?");
        tq.addBindValue(pid);
        tq.exec();
        while (tq.next()) {
            Track t(tq.value(0).toInt(), tq.value(1).toString().toStdString(), tq.value(2).toString().toStdString(),
                    tq.value(3).toString().toStdString(), tq.value(4).toInt(), tq.value(5).toString().toStdString(),
                    tq.value(6).toBool());
            pl.addTrack(t);
        }
        customPlaylists.push_back(pl);
    }
    
    refreshPlaylistsList();
}

void MainWindow::loadHomeData()
{
    ui->listHomeItems->clear();
    for (const auto& track : allTracks) {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(QString::fromStdString(track.getTitle() + "\n" + track.getArtist()));
        
        if (!track.getCoverPath().empty()) {
            QPixmap pix(QString::fromStdString(track.getCoverPath()));
            item->setIcon(QIcon(pix));
        } else {
            // Dummy icon
            QPixmap dummy(150, 150);
            dummy.fill(QColor("#282828"));
            item->setIcon(QIcon(dummy));
        }
        item->setData(Qt::UserRole, track.getId());
        ui->listHomeItems->addItem(item);
    }
}

void MainWindow::refreshPlaylistsList()
{
    ui->listPlaylists->clear();
    for (const auto& pl : customPlaylists) {
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(pl.getName()));
        item->setData(Qt::UserRole, pl.getId());
        ui->listPlaylists->addItem(item);
    }
}

void MainWindow::populateTrackTable(const Playlist& playlist, QTableWidget* table)
{
    table->clearContents();
    table->setRowCount(0);
    
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"#", "Title", "Artist", "Duration", "♡"});
    table->horizontalHeader()->setStretchLastSection(false);
    table->setColumnWidth(0, 30);
    table->setColumnWidth(1, 250);
    table->setColumnWidth(2, 200);
    table->setColumnWidth(3, 80);
    table->setColumnWidth(4, 50);

    const auto& tracks = playlist.getTracks();
    int row = 0;
    for (const auto& t : tracks) {
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(t.getTitle())));
        table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(t.getArtist())));
        
        QString timeStr = QString("%1:%2").arg(t.getDuration() / 60).arg(t.getDuration() % 60, 2, 10, QChar('0'));
        table->setItem(row, 3, new QTableWidgetItem(timeStr));
        
        QTableWidgetItem* likeItem = new QTableWidgetItem(t.getIsLiked() ? "♥" : "♡");
        likeItem->setForeground(QBrush(QColor(t.getIsLiked() ? "#1db954" : "#b3b3b3")));
        table->setItem(row, 4, likeItem);
        
        // Hide track ID inside row
        table->item(row, 0)->setData(Qt::UserRole, t.getId());
        row++;
    }
}

void MainWindow::updateNowPlayingInfo()
{
    Track* current = activePlayingPlaylist.getCurrentTrack();
    if (current != nullptr) {
        QString basePath = QFileInfo(__FILE__).absolutePath() + "/Songs/";
        QString path = basePath + QString::fromStdString(current->getFilePath());
        player->setSource(QUrl::fromLocalFile(path));
        
        ui->lblPlayingTitle->setText(QString::fromStdString(current->getTitle()));
        ui->lblPlayingArtist->setText(QString::fromStdString(current->getArtist()));
        
        if (!current->getCoverPath().empty()) {
            ui->lblCoverArt->setPixmap(QPixmap(QString::fromStdString(current->getCoverPath())));
        } else {
            ui->lblCoverArt->setPixmap(QPixmap());
        }
        
        ui->btnLike->setText(current->getIsLiked() ? "♥" : "♡");
        ui->btnLike->setStyleSheet(current->getIsLiked() ? "color: #1db954; font-size: 20px;" : "color: #b3b3b3; font-size: 20px;");
        
        player->play();
        ui->btnPlayPause->setText("⏸");
    }
}

// ==== UI BUTTON SLOTS ====
void MainWindow::on_btnHome_clicked() { ui->stackedWidget->setCurrentWidget(ui->pageHome); }

void MainWindow::on_btnSearch_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->pageSearch);
    ui->lineSearch->clear();
    Playlist all;
    for(auto t : allTracks) all.addTrack(t);
    populateTrackTable(all, ui->searchTable);
    currentViewPlaylist = nullptr; // special for search
}

void MainWindow::on_btnLibrary_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->pageLibrary);
    ui->lblPlaylistHeader->setText("<html><head/><body><p><span style=\" font-size:36pt; font-weight:700;\">Liked Songs</span></p></body></html>");
    currentViewPlaylist = &likedSongsPlaylist;
    populateTrackTable(*currentViewPlaylist, ui->trackTable);
}

void MainWindow::on_listPlaylists_itemClicked(QListWidgetItem *item) {
    int pid = item->data(Qt::UserRole).toInt();
    for (auto& pl : customPlaylists) {
        if (pl.getId() == pid) {
            currentViewPlaylist = &pl;
            ui->lblPlaylistHeader->setText(QString("<html><head/><body><p><span style=\" font-size:36pt; font-weight:700;\">%1</span></p></body></html>").arg(QString::fromStdString(pl.getName())));
            populateTrackTable(pl, ui->trackTable);
            ui->stackedWidget->setCurrentWidget(ui->pageLibrary);
            break;
        }
    }
}

void MainWindow::on_btnAddPlaylist_clicked() {
    bool ok;
    QString text = QInputDialog::getText(this, "Create Playlist", "Playlist Name:", QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty()) {
        QSqlQuery q;
        q.prepare("INSERT INTO playlists (name) VALUES (?)");
        q.addBindValue(text);
        if(q.exec()) {
            setupDatabase(); // Refresh everything trivially
        }
    }
}

void MainWindow::on_btnPlayPause_clicked() {
    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->pause();
        ui->btnPlayPause->setText("▶");
    } else if (player->playbackState() == QMediaPlayer::PausedState) {
        player->play();
        ui->btnPlayPause->setText("⏸");
    } else {
        updateNowPlayingInfo();
    }
}

void MainWindow::on_btnNext_clicked() {
    activePlayingPlaylist.nextTrack();
    updateNowPlayingInfo();
}

void MainWindow::on_btnPrev_clicked() {
    activePlayingPlaylist.previousTrack();
    updateNowPlayingInfo();
}

void MainWindow::on_trackTable_itemDoubleClicked(QTableWidgetItem *item) {
    if(!currentViewPlaylist) return;
    int clickedRow = item->row();
    activePlayingPlaylist = *currentViewPlaylist; // Copy playlist
    activePlayingPlaylist.setCurrentIndex(clickedRow);
    updateNowPlayingInfo();
}

void MainWindow::on_searchTable_itemDoubleClicked(QTableWidgetItem *item) {
    int id = ui->searchTable->item(item->row(), 0)->data(Qt::UserRole).toInt();
    Playlist pl;
    for(auto t : allTracks) {
        if (t.getId() == id) {
            pl.addTrack(t);
            break;
        }
    }
    activePlayingPlaylist = pl;
    activePlayingPlaylist.setCurrentIndex(0);
    updateNowPlayingInfo();
}

void MainWindow::on_listHomeItems_itemDoubleClicked(QListWidgetItem *item) {
    int id = item->data(Qt::UserRole).toInt();
    Playlist pl;
    for(auto t : allTracks) {
        if (t.getId() == id) {
            pl.addTrack(t);
            break;
        }
    }
    activePlayingPlaylist = pl;
    activePlayingPlaylist.setCurrentIndex(0);
    updateNowPlayingInfo();
}

void MainWindow::on_volumeSlider_valueChanged(int value) {
    audioOutput->setVolume(value / 100.0f);
}

void MainWindow::on_lineSearch_textChanged(const QString &text) {
    Playlist filtered;
    for (const auto& t : allTracks) {
        if (QString::fromStdString(t.getTitle()).contains(text, Qt::CaseInsensitive) ||
            QString::fromStdString(t.getArtist()).contains(text, Qt::CaseInsensitive)) {
            filtered.addTrack(t);
        }
    }
    populateTrackTable(filtered, ui->searchTable);
}

void MainWindow::on_btnLike_clicked() {
    Track* t = activePlayingPlaylist.getCurrentTrack();
    if (t) {
        bool newStatus = !t->getIsLiked();
        t->setIsLiked(newStatus);
        
        QSqlQuery q;
        q.prepare("UPDATE tracks SET is_liked = ? WHERE id = ?");
        q.addBindValue(newStatus ? 1 : 0);
        q.addBindValue(t->getId());
        q.exec();
        
        ui->btnLike->setText(newStatus ? "♥" : "♡");
        ui->btnLike->setStyleSheet(newStatus ? "color: #1db954; font-size: 20px;" : "color: #b3b3b3; font-size: 20px;");
        
        setupDatabase(); // Refresh local structures to keep synched
        if (currentViewPlaylist == &likedSongsPlaylist) {
            populateTrackTable(*currentViewPlaylist, ui->trackTable);
        }
    }
}

