#include <QDebug>
#include <QDir>
#include <QFile>
#include <QPainter>
#include <QtMath>
#include <QDebug>
#include <QAbstractItemView>
#include <QMimeData>
#include <QSizeGrip>
#include <QWindow>
#include <QScreen>
#include <QRect>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QProcess>

#include "playlist.h"
#include "globalhelper.h"
#include <QRandomGenerator>

/*
 * Windows：
 * QDir::tempPath()
 * 临时目录通常位于 C:\Users\<用户名>\AppData\Local\Temp，或者在某些情况下可能是 C:\Temp。
 * 你可以通过环境变量 %TEMP% 或 %TMP% 获取该路径。
 * Linux 和 macOS：
 *      临时目录通常是 /tmp。
 *      也可以通过环境变量 TMPDIR 获取该路径（如果设置了的话）。
 */

QString Playlist::GenerateThumbnail(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();

    // 定义音频和视频扩展名
    const QStringList audioExtensions = { "mp3", "wav", "aac", "flac", "ogg", "m4a", "wma" };
    const QStringList videoExtensions = { "mkv", "rmvb", "mp4", "avi", "flv", "wmv", "mov", "yuv", "3gp" };

    // 如果是音频文件，返回默认封面图像
    if (audioExtensions.contains(extension))
    {
        QString defaultImagePath = "://res//c.jpeg"; // 确保这个路径正确
        return defaultImagePath;
    }

    if (videoExtensions.contains(extension))
    {
        QString thumbnailPath = QDir::tempPath() + "/" + fileInfo.baseName() + "_thumbnail.jpg";

        if (QFile::exists(thumbnailPath))
        {
            return thumbnailPath;
        }

        QString ffmpegCmd = QString("ffmpeg -i \"%1\" -vf \"select='eq(pict_type,I)'\" -frames:v 1 \"%2\"")
            .arg(filePath)
            .arg(thumbnailPath);

        QProcess process;
        process.start(ffmpegCmd);
        process.waitForFinished();

        // 检查是否生成成功
        if (!QFile::exists(thumbnailPath))
        {
            qDebug() << "Failed to generate thumbnail for" << filePath;
            return QString();
        }

        return thumbnailPath;
    }
    return QString();
}

Playlist::Playlist(QWidget *parent) :
    QWidget(parent)
{

    if (this->objectName().isEmpty())
        this->setObjectName("Playlist");
    this->resize(115, 254);
    gridLayout = new QGridLayout(this);
    gridLayout->setObjectName("gridLayout");
    List = new MediaList(this);
    new QListWidgetItem(List);
    new QListWidgetItem(List);
    new QListWidgetItem(List);
    List->setObjectName("List");
    List->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    List->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
    List->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

    gridLayout->addWidget(List, 1, 0, 1, 1);

    label = new QLabel(this);
    label->setObjectName("label");
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    label->setFont(font);
    label->setAlignment(Qt::AlignmentFlag::AlignCenter);
    label->setText("播放列表");

    gridLayout->addWidget(label, 0, 0, 1, 1);

    this->setWindowTitle("Form");
    List->item(0)->setText("testlist0");
    List->item(1)->setText("testlist1");
    List->item(2)->setText("testlist2");

    connect(List, &QListWidget::itemDoubleClicked, this, &Playlist::on_List_itemDoubleClicked);

}

Playlist::~Playlist()
{
    QStringList strListPlayList;
    for (int i = 0; i < List->count(); i++)
    {
        strListPlayList.append(List->item(i)->toolTip());
    }
    GlobalHelper::SavePlaylist(strListPlayList);
}

bool Playlist::Init()
{
    if (List->Init() == false)
    {
        return false;
    }

    if (InitUi() == false)
    {
        return false;
    }

    if (ConnectSignalSlots() == false)
    {
        return false;
    }

    setAcceptDrops(true);

    return true;
}

bool Playlist::InitUi()
{
    setStyleSheet(GlobalHelper::GetQssStr(":/res/qss/playlist.css"));
    List->clear();

    QStringList strListPlaylist;
    GlobalHelper::GetPlaylist(strListPlaylist);

    for (QString strVideoFile : strListPlaylist)
    {
        QFileInfo fileInfo(strVideoFile);
        if (fileInfo.exists())
        {
            QListWidgetItem *pItem = new QListWidgetItem(List);
            pItem->setData(Qt::UserRole, QVariant(fileInfo.filePath()));  // 用户数据
            pItem->setText(QString("%1").arg(fileInfo.fileName()));  // 显示文本
            pItem->setToolTip(fileInfo.filePath());
            QString thumbnailPath = GenerateThumbnail(fileInfo.filePath());
            if (!thumbnailPath.isEmpty())
            {
                QPixmap pixmap;
                if (thumbnailPath.startsWith("://")) // 资源文件路径
                {
                    pixmap.load(thumbnailPath);
                }
                else
                {
                    pixmap.load(thumbnailPath);
                }
                if (!pixmap.isNull())
                {
                    pItem->setIcon(QIcon(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation))); // 设置缩略图
                }
                else
                {
                    qDebug() << "Failed to load pixmap from" << thumbnailPath;
                }
            }
            List->addItem(pItem);
        }
    }
    if (strListPlaylist.length() > 0)
    {
        List->setCurrentRow(0);
    }

    List->setIconSize(QSize(75, 75));
    List->setResizeMode(QListWidget::Adjust);
    List->setStyleSheet("QListWidget { background: #222222; border: none; }"
        "QListWidget::item { border: none; }");
    return true;
}

bool Playlist::ConnectSignalSlots()
{
    QList<bool> listRet;
    bool bRet;

    bRet = connect(List, &MediaList::SigAddFile, this, &Playlist::OnAddFile);
    listRet.append(bRet);

    for (bool bReturn : listRet)
    {
        if (bReturn == false)
        {
            return false;
        }
    }

    return true;
}

void Playlist::on_List_itemDoubleClicked(QListWidgetItem *item)
{
    emit SigPlay(item->data(Qt::UserRole).toString());
    CurrentPlayListIndex = List->row(item);
    List->setCurrentRow(CurrentPlayListIndex);
}

bool Playlist::GetPlaylistStatus()
{
    if (this->isHidden())
    {
        return false;
    }

    return true;
}

void Playlist::OnAddFile(QString strFileName)
{
    bool bSupportMovie = strFileName.endsWith(".mkv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".rmvb", Qt::CaseInsensitive) ||
        strFileName.endsWith(".mp4", Qt::CaseInsensitive) ||
        strFileName.endsWith(".avi", Qt::CaseInsensitive) ||
        strFileName.endsWith(".flv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".wmv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".mov", Qt::CaseInsensitive) ||
        strFileName.endsWith(".yuv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".mp3", Qt::CaseInsensitive) ||
        strFileName.endsWith(".wav", Qt::CaseInsensitive) ||
        strFileName.endsWith(".ogg", Qt::CaseInsensitive) ||
        strFileName.endsWith(".3gp", Qt::CaseInsensitive);
    if (!bSupportMovie)
    {
        return;
    }

    QFileInfo fileInfo(strFileName);
    QList<QListWidgetItem *> listItem = List->findItems(fileInfo.fileName(), Qt::MatchExactly);
    QListWidgetItem *pItem = nullptr;
    if (listItem.isEmpty())
    {
        pItem = new QListWidgetItem(List);
        pItem->setData(Qt::UserRole, QVariant(fileInfo.filePath()));  // 用户数据
        pItem->setText(fileInfo.fileName());  // 显示文本
        pItem->setToolTip(fileInfo.filePath());
        QString thumbnailPath = GenerateThumbnail(fileInfo.filePath());
        if (!thumbnailPath.isEmpty())
        {
            QPixmap pixmap(thumbnailPath);
            pItem->setIcon(QIcon(pixmap.scaled(100, 100, Qt::KeepAspectRatio))); // 设置缩略图
        }
        List->addItem(pItem);
    }
    else
    {
        pItem = listItem.at(0);
    }
}

void Playlist::OnAddFileAndPlay(QString strFileName)
{
    bool bSupportMovie = strFileName.endsWith(".mkv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".rmvb", Qt::CaseInsensitive) ||
        strFileName.endsWith(".mp4", Qt::CaseInsensitive) ||
        strFileName.endsWith(".avi", Qt::CaseInsensitive) ||
        strFileName.endsWith(".flv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".wmv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".mov", Qt::CaseInsensitive) ||
        strFileName.endsWith(".yuv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".mp3", Qt::CaseInsensitive) ||
        strFileName.endsWith(".wav", Qt::CaseInsensitive) ||
        strFileName.endsWith(".ogg", Qt::CaseInsensitive) ||
        strFileName.endsWith(".3gp", Qt::CaseInsensitive);
    if (!bSupportMovie)
    {
        return;
    }

    QFileInfo fileInfo(strFileName);
    QList<QListWidgetItem *> listItem = List->findItems(fileInfo.fileName(), Qt::MatchExactly);
    QListWidgetItem *pItem = nullptr;
    if (listItem.isEmpty())
    {
        pItem = new QListWidgetItem(List);
        pItem->setData(Qt::UserRole, QVariant(fileInfo.filePath()));  // 用户数据
        pItem->setText(fileInfo.fileName());  // 显示文本
        pItem->setToolTip(fileInfo.filePath());
        QString thumbnailPath = GenerateThumbnail(fileInfo.filePath());
        if (!thumbnailPath.isEmpty())
        {
            QPixmap pixmap(thumbnailPath);
            pItem->setIcon(QIcon(pixmap.scaled(100, 100, Qt::KeepAspectRatio))); // 设置缩略图
        }
        List->addItem(pItem);
    }
    else
    {
        pItem = listItem.at(0);
    }
    on_List_itemDoubleClicked(pItem);
}

void Playlist::OnBackwardPlay()
{
    if (CurrentPlayListIndex == 0)
    {
        CurrentPlayListIndex = List->count() - 1;
        on_List_itemDoubleClicked(List->item(CurrentPlayListIndex));
        List->setCurrentRow(CurrentPlayListIndex);
    }
    else
    {
        CurrentPlayListIndex--;
        on_List_itemDoubleClicked(List->item(CurrentPlayListIndex));
        List->setCurrentRow(CurrentPlayListIndex);
    }
}

void Playlist::OnForwardPlay()
{
    if (CurrentPlayListIndex == List->count() - 1)
    {
        CurrentPlayListIndex = 0;
        on_List_itemDoubleClicked(List->item(CurrentPlayListIndex));
        List->setCurrentRow(CurrentPlayListIndex);
    }
    else
    {
        CurrentPlayListIndex++;
        on_List_itemDoubleClicked(List->item(CurrentPlayListIndex));
        List->setCurrentRow(CurrentPlayListIndex);
    }
}
void Playlist::OnRandomPlay()
{
    int listCount = List->count();
    if (listCount == 0)
    {
        return;
    }
    int newIndex;
    if (listCount == 1)
    {
        newIndex = 0;
    }
    else
    {
        do
        {
            newIndex = QRandomGenerator::global()->bounded(listCount);
        } while (newIndex == CurrentPlayListIndex);
    }
    CurrentPlayListIndex = newIndex;
    on_List_itemDoubleClicked(List->item(CurrentPlayListIndex));
    List->setCurrentRow(CurrentPlayListIndex);
}

void Playlist::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
    {
        return;
    }

    for (QUrl url : urls)
    {
        QString strFileName = url.toLocalFile();

        OnAddFile(strFileName);
    }
}

void Playlist::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}
