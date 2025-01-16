﻿
#include <QDebug>
#include <QMutex>
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


#include "show.h"
#include "globalhelper.h"
#include <QMessageBox>
#include "videoctl.h"

#pragma execution_character_set("utf-8")

QMutex show_rect;

double start = 0;

Show::Show(QWidget *parent) :
    QWidget(parent),
    ActionGroup(this)
{


    if (this->objectName().isEmpty())
        this->setObjectName("Show");
    this->resize(5000, 5000);
    label = new QLabel(this);
    label->setObjectName("label");
    label->setGeometry(QRect(0, 0, 5000, 5000));
    label_2 = new QLabel(this);
    label_2->setObjectName("label_2");
    label_2->setGeometry(QRect(5000, 5000, 40, 12));
    label->setText(QString());
    label_2->setText("0");



    setStyleSheet(GlobalHelper::GetQssStr(":/res/qss/show.css"));
    setAcceptDrops(true);

    this->setAttribute(Qt::WA_OpaquePaintEvent);
    this->setMouseTracking(true);
    


    LastFrameWidth = 0;
    LastFrameHeight = 0;

    connect(VideoCtl::GetInstance(), &VideoCtl::ResolutionChanged,this,&Show::OnResolutionChanged);
}

Show::~Show()
{
}

bool Show::Init()
{
    if (ConnectSignalSlots() == false)
    {
        return false;
    }

    label->installEventFilter(this);
    label_2->installEventFilter(this);
    return true;
}
bool Show::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == label)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::RightButton)
            {
                qDebug() << "Show::eventFilter1: Right button clicked on label";
                emit SigShowMenu();
                return true;
            }
        }
    }
    else if (obj == label_2)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::RightButton)
            {
                qDebug() << "Show::eventFilter2: Right button clicked on label";
                emit SigShowMenu();
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}
void Show::contextMenuEvent(QContextMenuEvent* event)
{
    qDebug() << "Show::contextMenuEvent triggered";
    emit SigShowMenu();
}
void Show::OnFrameDimensionsChanged(int nFrameWidth, int nFrameHeight)
{
    qDebug() << "Show::OnFrameDimensionsChanged" << nFrameWidth << nFrameHeight;
    LastFrameWidth = nFrameWidth;
    LastFrameHeight = nFrameHeight;

    ChangeShow();
}

void Show::ChangeShow()
{
    QMutexLocker locker(&show_rect);

    if (LastFrameWidth == 0 && LastFrameHeight == 0)
    {
        label->setGeometry(0, 0, width(), height());
        qDebug() << "0 0 " << width() << " " << height() << '\n';
        return;
    }
    float videoAspectRatio = static_cast<float>(LastFrameWidth) / static_cast<float>(LastFrameHeight);
    int containerWidth = this->width();
    int containerHeight = this->height();
    float containerAspectRatio = static_cast<float>(containerWidth) / static_cast<float>(containerHeight);

    int displayWidth, displayHeight, offsetX, offsetY;

    if (videoAspectRatio > containerAspectRatio)
    {
        displayWidth = containerWidth;
        displayHeight = static_cast<int>(containerWidth / videoAspectRatio);
        offsetX = 0;
        offsetY = (containerHeight - displayHeight) / 2;
    }
    else
    {
        displayHeight = containerHeight;
        displayWidth = static_cast<int>(containerHeight * videoAspectRatio);
        offsetX = (containerWidth - displayWidth) / 2;
        offsetY = 0;
    }
    label->setGeometry(offsetX, offsetY, displayWidth, displayHeight);
}

void Show::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void Show::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    ChangeShow();
}

void Show::keyReleaseEvent(QKeyEvent *event)
{
    qDebug() << "Show::keyPressEvent:" << event->key();
    switch (event->key())
    {
    case Qt::Key_Return:
        SigFullScreen();
        break;
    case Qt::Key_Left:
        emit SigSeekBack();
        break;
    case Qt::Key_Right:
        qDebug() << "前进5s";
        emit SigSeekForward();
        break;
    case Qt::Key_Up:
        emit SigAddVolume();
        break;
    case Qt::Key_Down:
        emit SigSubVolume();
        break;
    case Qt::Key_Space:
        emit SigPlayOrPause();
        break;

    default:
        QWidget::keyPressEvent(event);
        break;
    }
}
void Show::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        qDebug() << "我点击了右键" << '\n';
        emit SigShowMenu();
        event->accept();
    }
    else
    {
        QWidget::mousePressEvent(event);
    }
}
void Show::OnDisplayMsg(QString strMsg)
{
    qDebug() << "Show::OnDisplayMsg " << strMsg;
}

void Show::OnPlay(QString strFile)
{
    qDebug() << "调用了OnPlay函数" << '\n';
    LastFrameWidth = 0;
    LastFrameHeight = 0;
    label->update();
    ChangeShow();

   // VideoCtl::GetInstance()->ReInit();
    VideoCtl::GetInstance()->StartPlay(strFile, label, start);
    start = 0.0;
}
void Show::OnStopFinished()
{
    update();
}
void Show::OnTimerShowCursorUpdate()
{
}

void Show::OnActionsTriggered(QAction *action)
{
    QString strAction = action->text();
    if (strAction == "全屏")
    {
        emit SigFullScreen();
    }
    else if (strAction == "停止")
    {
        emit SigStop();
    }
    else if (strAction == "暂停" || strAction == "播放")
    {
        emit SigPlayOrPause();
    }
}

bool Show::ConnectSignalSlots()
{
    QList<bool> listRet;
    bool bRet;

    bRet = connect(this, &Show::SigPlay, this, &Show::OnPlay);
    listRet.append(bRet);

    timerShowCursor.setInterval(2000);
    bRet = connect(&timerShowCursor, &QTimer::timeout, this, &Show::OnTimerShowCursorUpdate);
    listRet.append(bRet);

    connect(&ActionGroup, &QActionGroup::triggered, this, &Show::OnActionsTriggered);

    for (bool bReturn : listRet)
    {
        if (bReturn == false)
        {
            return false;
        }
    }

    return true;
}

void Show::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
    {
        return;
    }

    for(QUrl url: urls)
    {
        QString strFileName = url.toLocalFile();
        qDebug() << strFileName;
        emit SigOpenFile(strFileName);
        break;
    }
}

void Show::OnResolutionChanged(bool success,QString outputFilePath,double pos)
{
    if (success)
    {
        qDebug() << "转换后的视频文件：" << outputFilePath;
        start = pos;
        OnPlay(outputFilePath);
    }
    else {
        QMessageBox::warning(this, "转换失败", "");
    }
}
