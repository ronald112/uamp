#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QThread>
#include <QTime>
#include <QMediaMetaData>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void on_actionAdd_track_triggered();

    void on_pushButtonPlayPause_clicked();

private:
    Ui::MainWindow *ui;

    bool playPauseBtnStatus = false;

    QStandardItemModel  *m_playListModel = nullptr;   // Data Model for Playlist
    QMediaPlayer        *m_player = nullptr;
    QMediaPlaylist      *m_playlist = nullptr;

private:
    int m_duration;

private:
    void metaDataChanged();
    void setTrackInfo(const QString &info);
    void updateDurationInfo(qint64 currentInfo);
};
#endif // MAINWINDOW_H
