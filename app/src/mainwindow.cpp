#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_playListModel = new QStandardItemModel(this);
    ui->playlistView->setModel(m_playListModel);
    m_playListModel->setHorizontalHeaderLabels(QStringList()  << tr("Audio Track")
                                                            << tr("File Path"));
    ui->playlistView->hideColumn(1);
    ui->playlistView->verticalHeader()->setVisible(false);
    ui->playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlistView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->playlistView->horizontalHeader()->setStretchLastSection(true);

    m_player = new QMediaPlayer(this);          // Init player
    m_player->setAudioRole(QAudio::Role::MusicRole);
    qInfo() << "Supported audio roles:";
    for (QAudio::Role role : m_player->supportedAudioRoles())
        qInfo() << "    " << role;
    m_playlist = new QMediaPlaylist(m_player);  // Init playlist
    m_player->setPlaylist(m_playlist);
    m_player->setVolume(0);
    m_playlist->setPlaybackMode(QMediaPlaylist::Loop);  // Set circular play mode playlist

    // подключаем кнопки управления к слотам управления
    connect(ui->verticalSliderVolume, &QSlider::valueChanged, m_playlist, [this](int value) {
        m_player->setVolume(value);
        qDebug() << "volume changed: " + QString::number(value);
    });
    connect(ui->pushButtonPrevious, &QPushButton::clicked, m_playlist, &QMediaPlaylist::previous);
    connect(ui->pushButtonNext, &QPushButton::clicked, m_playlist, &QMediaPlaylist::next);
    connect(ui->pushButtonStop, &QPushButton::clicked, m_player, &QMediaPlayer::stop);

    // change icon (doesn't work)
    connect(m_player, QOverload<>::of(&QMediaPlayer::metaDataChanged), this, &MainWindow::metaDataChanged);

    // When you doubleclick on the track in the table set the track in the playlist
    // TODO take tags prom file here
    connect(ui->playlistView, &QTableView::doubleClicked, [this](const QModelIndex &index){
        m_playlist->setCurrentIndex(index.row());

    });

    // if the current track of the index change in the playlist, set the file name in a special label
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, [this](int index){
        ui->currentTrack->setText(m_playListModel->data(m_playListModel->index(index, 0)).toString());
    });

    // Song duration changed
    connect(m_player, &QMediaPlayer::durationChanged, this, [this](qint64 duration) {
        ui->horizontalSliderSongProgress->setMaximum(duration / 1000);
    });

    // sSong progress changed
    connect(m_player, &QMediaPlayer::positionChanged, this, [this](qint64 progress){
        if (!ui->horizontalSliderSongProgress->isSliderDown())
                ui->horizontalSliderSongProgress->setValue(progress / 1000);

            updateDurationInfo(progress / 1000);
    });



    // Drag slider
    ui->horizontalSliderSongProgress->setRange(0, m_player->duration() / 1000);
    connect(ui->horizontalSliderSongProgress, &QSlider::sliderMoved, this, [this](int seconds) {
        m_player->setPosition(seconds * 1000);
        qDebug() << "volume progress changed: " + QString::number(seconds);
    });

   metaDataChanged();
}

void MainWindow::metaDataChanged()
{
    if (m_player->isMetaDataAvailable()) {
//        setTrackInfo(QString("%1 - %2")
//                .arg(m_player->metaData(QMediaMetaData::AlbumArtist).toString())
//                .arg(m_player->metaData(QMediaMetaData::Title).toString()));

        if (ui->coverLabelImage) {
            QUrl url = m_player->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();

            ui->coverLabelImage->setPixmap(!url.isEmpty()
                    ? QPixmap(url.toString())
                    : QPixmap());
        }
    }
}

//void MainWindow::setTrackInfo(const QString &info)
//{
//    m_trackInfo = info;

//    if (m_statusBar) {
//        m_statusBar->showMessage(m_trackInfo);
//        m_statusLabel->setText(m_statusInfo);
//    } else {
//        if (!m_statusInfo.isEmpty())
//            setWindowTitle(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
//        else
//            setWindowTitle(m_trackInfo);
//    }
//}

void MainWindow::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
            currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
            m_duration % 60, (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) /*+ " / " + totalTime.toString(format)*/;
    }
    ui->recordPositionLabel->setText(tStr);
}

MainWindow::~MainWindow()
{
}


void MainWindow::on_actionAdd_track_triggered()
{
    // Using the file selection dialog to make multiple selections of mp3 files
    QStringList files = QFileDialog::getOpenFileNames(this,
                                                      tr("Open files"),
                                                      QString(),
                                                      tr("Audio Files (*.mp3; *.flac)"));



    // Next, set the data names and file paths
    // into the playlist and table displaying the playlist
    foreach (QString filePath, files) {
        QList<QStandardItem *> items;
        items.append(new QStandardItem(QDir(filePath).dirName()));
        items.append(new QStandardItem(filePath));
        m_playListModel->appendRow(items);
        m_playlist->addMedia(QUrl(filePath));
    }
}

// connect(ui->pushButtonPlayPause, &QPushButton::clicked, m_player, &QMediaPlayer::play);
void MainWindow::on_pushButtonPlayPause_clicked()
{
    if (this->playPauseBtnStatus == false) {
        this->playPauseBtnStatus = true;
        ui->pushButtonPlayPause->setIcon(QIcon(":/pause.png"));
        m_player->play();
    }
    else {
        this->playPauseBtnStatus = false;
        ui->pushButtonPlayPause->setIcon(QIcon(":/play.png"));
        m_player->pause();
    }

}
