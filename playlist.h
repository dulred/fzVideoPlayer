#pragma once

#include <QWidget>
#include <QListWidgetItem>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QGridLayout>
#include <QLabel>
#include "medialist.h"

class Playlist : public QWidget
{
    Q_OBJECT

public:
    QString GenerateThumbnail(const QString& videoPath);
    explicit Playlist(QWidget *parent = 0);
    ~Playlist();

    bool Init();
    bool GetPlaylistStatus(); //列表状态

private:
    QGridLayout *gridLayout;
    MediaList *List;
    QLabel *label;

public:

    void OnAddFile(QString strFileName);
    void OnAddFileAndPlay(QString strFileName);

    void OnBackwardPlay();
    void OnForwardPlay();
    void OnRandomPlay();
    QSize sizeHint() const
    {
        return QSize(150, 900);
    }
protected:

    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);

signals:
    void SigUpdateUi();
    void SigPlay(QString strFile);

private:
    bool InitUi();
    bool ConnectSignalSlots();
    
private slots:

    void on_List_itemDoubleClicked(QListWidgetItem *item);

private:

    int CurrentPlayListIndex;
};
