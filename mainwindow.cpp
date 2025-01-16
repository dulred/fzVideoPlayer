#include "mainwindow.h"
#include "globalhelper.h"

#include <QApplication>
#include <QGuiApplication>
#include <QMessageBox>
#include "videoctl.h"
bool isRecording;
const int FULLSCREEN_MOUSE_DETECT_TIME = 500;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ShadowWidth(0)
{

    if (this->objectName().isEmpty())
        this->setObjectName("MainWindow");
    this->resize(1100, 600);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/res/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);
    centralwidget = new QWidget(this);
    centralwidget->setObjectName("centralwidget");
    gridLayout_2 = new QGridLayout(centralwidget);
    gridLayout_2->setObjectName("gridLayout_2");
    ShowCtrlBarPlaylistBgWidget = new QWidget(centralwidget);
    ShowCtrlBarPlaylistBgWidget->setObjectName("ShowCtrlBarPlaylistBgWidget");
    gridLayout = new QGridLayout(ShowCtrlBarPlaylistBgWidget);
    gridLayout->setSpacing(0);
    gridLayout->setObjectName("gridLayout");
    gridLayout->setContentsMargins(0, 0, 0, 0);
    CtrlBarWid = new CtrlBar(ShowCtrlBarPlaylistBgWidget);
    CtrlBarWid->setObjectName("CtrlBarWid");
    CtrlBarWid->setMinimumSize(QSize(0, 60));
    CtrlBarWid->setMaximumSize(QSize(16777215, 60));

    gridLayout->addWidget(CtrlBarWid, 1, 0, 1, 1);

    ShowWid = new Show(ShowCtrlBarPlaylistBgWidget);
    ShowWid->setObjectName("ShowWid");
    ShowWid->setMinimumSize(QSize(100, 100));

    gridLayout->addWidget(ShowWid, 0, 0, 1, 1);

    ShowWid->raise();
    CtrlBarWid->raise();

    gridLayout_2->addWidget(ShowCtrlBarPlaylistBgWidget, 0, 0, 1, 1);

    this->setCentralWidget(centralwidget);
    menubar = new QMenuBar(this);
    menubar->setObjectName("menubar");
    menubar->setGeometry(QRect(0, 0, 1100, 18));
    this->setMenuBar(menubar);
    PlaylistWid = new QDockWidget(this);
    PlaylistWid->setObjectName("PlaylistWid");
    PlaylistWid->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
    PlaylistWid->setAutoFillBackground(false);
    this->addDockWidget(Qt::RightDockWidgetArea, PlaylistWid);
    statusbar = new QStatusBar(this);
    statusbar->setObjectName("statusbar");
    this->setStatusBar(statusbar);
    TitleWid = new QDockWidget(this);
    TitleWid->setObjectName("TitleWid");
    TitleWid->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
    this->addDockWidget(Qt::TopDockWidgetArea, TitleWid);
    this->setWindowTitle("MainWindow");



    isRecording = false;

    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowMinimizeButtonHint);

    ShadowWidth = 10;


    QString qss = GlobalHelper::GetQssStr(":/res/qss/mainwid.css");
    setStyleSheet(qss);

    this->setMouseTracking(true);

    isPlaying = false;

    FullScreenPlay = false;

    MoveDrag = false;

    CtrlBarAnimationTimer.setInterval(2000);
    FullscreenMouseDetectTimer.setInterval(FULLSCREEN_MOUSE_DETECT_TIME);


    loopActionGroup = new QActionGroup(this);
    loopActionGroup->setExclusive(true);

}

MainWindow::~MainWindow() {}

bool MainWindow::Init()
{
    QWidget *em = new QWidget(this);
    PlaylistWid->setTitleBarWidget(em);
    PlaylistWid->setFixedWidth(200);
    PlaylistWid->setWidget(&Playlist);

    QWidget *emTitle = new QWidget(this);
    TitleWid->setTitleBarWidget(emTitle);
    TitleWid->setWidget(&Title);
    if (ConnectSignalSlots() == false)
    {
        return false;
    }

    if (CtrlBarWid->Init() == false || Playlist.Init() == false || ShowWid->Init() == false || Title.Init() == false)
    {
        return false;
    }


    CtrlbarAnimationShow = new QPropertyAnimation(CtrlBarWid, "geometry");
    CtrlbarAnimationHide = new QPropertyAnimation(CtrlBarWid, "geometry");


    InitMenuActions();


    InitMenu();
    return true;
}
void MainWindow::InitMenuActions()
{
    Map_act["OpenFile"] = &MainWindow::OpenFile;
    Map_act["OnCloseBtnClicked"] = &MainWindow::OnCloseBtnClicked;
    Map_act["OnSaveBmp"] = &MainWindow::OnSaveBmp;
    Map_act["OnFullScreenPlay"] = &MainWindow::OnFullScreenPlay;
    Map_act["OnSavePeriod"] = &MainWindow::OnSavePeriod;
    Map_act["OnLoop"] = &MainWindow::OnLoop;
    Map_act["OnList"] = &MainWindow::OnList;
    Map_act["OnRandom"] = &MainWindow::OnRandom;
}
void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
}


void MainWindow::enterEvent(QEvent *event)
{
    Q_UNUSED(event);

}
void MainWindow::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);

}

bool MainWindow::ConnectSignalSlots()
{
    connect(&Title, &Title::SigCloseBtnClicked, this, &MainWindow::OnCloseBtnClicked);
    connect(&Title, &Title::SigMaxBtnClicked, this, &MainWindow::OnMaxBtnClicked);
    connect(&Title, &Title::SigMinBtnClicked, this, &MainWindow::OnMinBtnClicked);
    connect(&Title, &Title::SigDoubleClicked, this, &MainWindow::OnMaxBtnClicked);
    connect(&Title, &Title::SigFullScreenBtnClicked, this, &MainWindow::OnFullScreenPlay);
    connect(&Title, &Title::SigOpenFile, &Playlist, &Playlist::OnAddFileAndPlay);
    connect(&Title, &Title::SigShowMenu, this, &MainWindow::OnShowMenu);


    connect(&Playlist, &Playlist::SigPlay, ShowWid, &Show::SigPlay);

    connect(ShowWid, &Show::SigOpenFile, &Playlist, &Playlist::OnAddFileAndPlay);
    connect(ShowWid, &Show::SigFullScreen, this, &MainWindow::OnFullScreenPlay);
//     connect(ui->ShowWid, &Show::SigPlayOrPause, VideoCtl::GetInstance(), &VideoCtl::OnPause);
//     connect(ui->ShowWid, &Show::SigStop, VideoCtl::GetInstance(), &VideoCtl::OnStop);
    connect(ShowWid, &Show::SigShowMenu, this, &MainWindow::OnShowMenu);
//     connect(ui->ShowWid, &Show::SigSeekForward, VideoCtl::GetInstance(), &VideoCtl::OnSeekForward);
//     connect(ui->ShowWid, &Show::SigSeekBack, VideoCtl::GetInstance(), &VideoCtl::OnSeekBack);
//     connect(ui->ShowWid, &Show::SigAddVolume, VideoCtl::GetInstance(), &VideoCtl::OnAddVolume);
//     connect(ui->ShowWid, &Show::SigSubVolume, VideoCtl::GetInstance(), &VideoCtl::OnSubVolume);


//     connect(ui->CtrlBarWid, &CtrlBar::SigSpeed, VideoCtl::GetInstance(), &VideoCtl::OnSpeed);
//     connect(ui->CtrlBarWid, &CtrlBar::SigShowOrHidePlaylist, this, &MainWindow::OnShowOrHidePlaylist);
    connect(CtrlBarWid, &CtrlBar::SigPlaySeek, VideoCtl::GetInstance(), &VideoCtl::OnPlaySeek);
    connect(CtrlBarWid, &CtrlBar::SigPlayVolume, VideoCtl::GetInstance(), &VideoCtl::OnPlayVolume);
    connect(CtrlBarWid, &CtrlBar::SigPlayOrPause, VideoCtl::GetInstance(), &VideoCtl::OnPause);
//     connect(ui->CtrlBarWid, &CtrlBar::SigStop, VideoCtl::GetInstance(), &VideoCtl::OnStop);
    connect(CtrlBarWid, &CtrlBar::SigBackwardPlay, &Playlist, &Playlist::OnBackwardPlay);
    connect(CtrlBarWid, &CtrlBar::SigForwardPlay, &Playlist, &Playlist::OnForwardPlay);
    connect(CtrlBarWid, &CtrlBar::SigShowMenu, this, &MainWindow::OnShowMenu);
//     connect(ui->CtrlBarWid, &CtrlBar::SigShowSetting, this, &MainWindow::OnShowSettingWid);

//     connect(this, &MainWindow::SigShowMax, &Title, &Title::OnChangeMaxBtnStyle);
//     connect(this, &MainWindow::SigSeekForward, VideoCtl::GetInstance(), &VideoCtl::OnSeekForward);
//     connect(this, &MainWindow::SigSeekBack, VideoCtl::GetInstance(), &VideoCtl::OnSeekBack);
//     connect(this, &MainWindow::SigAddVolume, VideoCtl::GetInstance(), &VideoCtl::OnAddVolume);
//     connect(this, &MainWindow::SigSubVolume, VideoCtl::GetInstance(), &VideoCtl::OnSubVolume);
    connect(this, &MainWindow::SigOpenFile, &Playlist, &Playlist::OnAddFileAndPlay);
//     connect(this, &MainWindow::SigVideoLoop, ui->CtrlBarWid, &CtrlBar::OnChangeVideo);

    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoTotalSeconds, CtrlBarWid, &CtrlBar::OnVideoTotalSeconds);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoPlaySeconds, CtrlBarWid, &CtrlBar::OnVideoPlaySeconds);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoVolume, CtrlBarWid, &CtrlBar::OnVideopVolume);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigPauseStat, CtrlBarWid, &CtrlBar::OnPauseStat, Qt::QueuedConnection);
//     connect(VideoCtl::GetInstance(), &VideoCtl::SigStopFinished, ui->ShowWid, &Show::OnStopFinished, Qt::QueuedConnection);
//     connect(VideoCtl::GetInstance(), &VideoCtl::SigFrameDimensionsChanged, ui->ShowWid, &Show::OnFrameDimensionsChanged, Qt::QueuedConnection);
//     connect(VideoCtl::GetInstance(), &VideoCtl::SigStopFinished, &Title, &Title::OnStopFinished, Qt::DirectConnection);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigStartPlay, &Title, &Title::OnPlay, Qt::DirectConnection);
//     connect(VideoCtl::GetInstance(), &VideoCtl::SigList, &Playlist, &Playlist::OnForwardPlay);
//     connect(VideoCtl::GetInstance(), &VideoCtl::SigRandom, &Playlist, &Playlist::OnRandomPlay);

//     connect(ui->CtrlBarWid, &CtrlBar::SigLoadSubtitle, VideoCtl::GetInstance(), &VideoCtl::LoadSubtitle);

    connect(&CtrlBarAnimationTimer, &QTimer::timeout, this, &MainWindow::OnCtrlBarAnimationTimeOut);

    connect(&FullscreenMouseDetectTimer, &QTimer::timeout, this, &MainWindow::OnFullscreenMouseDetectTimeOut);


    connect(&ActFullscreen, &QAction::triggered, this, &MainWindow::OnFullScreenPlay);



    return true;
}


void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    qDebug() << "MainWindow::keyPressEvent:" << event->key();
    switch (event->key())
    {
    case Qt::Key_F:
        OpenFile();
        break;
    case Qt::Key_Return:
        OnFullScreenPlay();
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
    case Qt::Key_S:
        OnSaveBmp();
        break;
    case Qt::Key_Escape:
        OnCloseBtnClicked();
        break;
    case Qt::Key_A:
        OnSavePeriod();
        break;
    case Qt::Key_M:
        OnShowMenu();
        break;
    default:
        break;
    }
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (TitleWid->geometry().contains(event->pos()))
        {
            MoveDrag = true;
            DragPosition = event->globalPos() - this->pos();
        }
    }

    QWidget::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    MoveDrag = false;

    QWidget::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (MoveDrag)
    {
        move(event->globalPos() - DragPosition);
    }

    QWidget::mouseMoveEvent(event);
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
    Menu.popup(event->globalPos());
}

void MainWindow::OnFullScreenPlay()
{
    if (FullScreenPlay == false)
    {
        FullScreenPlay = true;
        ActFullscreen.setChecked(true);

        ShowWid->setWindowFlags(Qt::Window);
        QScreen *pStCurScreen = screen();
        ShowWid->windowHandle()->setScreen(pStCurScreen);

        ShowWid->showFullScreen();

        QRect stScreenRect = pStCurScreen->geometry();
        int nCtrlBarHeight = CtrlBarWid->height();
        int nX = ShowWid->x();
        CtrlBarAnimationShowRect = QRect(nX, stScreenRect.height() - nCtrlBarHeight, stScreenRect.width(), nCtrlBarHeight);
        CtrlBarAnimationHideRect = QRect(nX, stScreenRect.height(), stScreenRect.width(), nCtrlBarHeight);

        CtrlbarAnimationShow->setStartValue(CtrlBarAnimationHideRect);
        CtrlbarAnimationShow->setEndValue(CtrlBarAnimationShowRect);
        CtrlbarAnimationShow->setDuration(1000);

        CtrlbarAnimationHide->setStartValue(CtrlBarAnimationShowRect);
        CtrlbarAnimationHide->setEndValue(CtrlBarAnimationHideRect);
        CtrlbarAnimationHide->setDuration(1000);

        CtrlBarWid->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
        CtrlBarWid->windowHandle()->setScreen(pStCurScreen);
        CtrlBarWid->raise();
        CtrlBarWid->setWindowOpacity(0.5);
        CtrlBarWid->showNormal();
        CtrlBarWid->windowHandle()->setScreen(pStCurScreen);

        CtrlbarAnimationShow->start();
        FullscreenCtrlBarShow = true;
        FullscreenMouseDetectTimer.start();

        this->setFocus();
    }
    else
    {
        FullScreenPlay = false;
        ActFullscreen.setChecked(false);

        CtrlbarAnimationShow->stop();
        CtrlbarAnimationHide->stop();
        CtrlBarWid->setWindowOpacity(1);
        CtrlBarWid->setWindowFlags(Qt::SubWindow);

        ShowWid->setWindowFlags(Qt::SubWindow);

        CtrlBarWid->showNormal();
        ShowWid->showNormal();

        FullscreenMouseDetectTimer.stop();
        this->setFocus();
    }
}

void MainWindow::OnCtrlBarAnimationTimeOut()
{
    QApplication::setOverrideCursor(Qt::BlankCursor);
}

void MainWindow::OnFullscreenMouseDetectTimeOut()
{
    if (FullScreenPlay)
    {
        if (CtrlBarAnimationShowRect.contains(cursor().pos()))
        {

            if (CtrlBarWid->geometry().contains(cursor().pos()))
            {
                FullscreenCtrlBarShow = true;
            }
            else
            {
                CtrlBarWid->raise();

                CtrlbarAnimationShow->start();
                CtrlbarAnimationHide->stop();
                CtrlBarHideTimer.stop();
            }
        }
        else
        {
            if (FullscreenCtrlBarShow)
            {
                FullscreenCtrlBarShow = false;
                CtrlBarHideTimer.singleShot(2000, this, &MainWindow::OnCtrlBarHideTimeOut);
            }

        }

    }
}

void MainWindow::OnCtrlBarHideTimeOut()
{
    if (FullScreenPlay)
    {
        CtrlbarAnimationHide->start();
    }
}

void MainWindow::OnShowMenu()
{
    qDebug() << "MainWindow::OnShowMenu triggered at position:" << QCursor::pos();
    QPoint globalPos = QCursor::pos();
    QScreen* currentScreen = QGuiApplication::screenAt(globalPos);
    if (currentScreen)
    {
        QRect screenGeometry = currentScreen->geometry();
        QPoint adjustedPos = globalPos;
        if (globalPos.x() + Menu.sizeHint().width() > screenGeometry.right())
            adjustedPos.setX(screenGeometry.right() - Menu.sizeHint().width());
        if (globalPos.y() + Menu.sizeHint().height() > screenGeometry.bottom())
            adjustedPos.setY(screenGeometry.bottom() - Menu.sizeHint().height());
        Menu.popup(adjustedPos);
    }
    else
    {
        Menu.popup(globalPos);
    }
}

void MainWindow::OnShowAbout()
{

}

void MainWindow::OpenFile()
{
    QString strFileName = QFileDialog::getOpenFileName(this, "打开文件", QDir::homePath(),"音视频文件(*.wav *.ogg *.mp3 *.mkv *.rmvb *.mp4 *.avi *.flv *.wmv *.3gp *.mov *.yuv)");
    emit SigOpenFile(strFileName);
}
void MainWindow::OnSaveBmp()
{
    QString savePath;
    bool success = VideoCtl::GetInstance()->TakeScreenshot(savePath);
    if (success)
    {
        QMessageBox::information(this, "截图成功", QString("截图已保存到：%1").arg(savePath));
    }
    else
    {
        QMessageBox::warning(this, "截图失败", "无法保存截图。");
    }
}
void MainWindow::OnShowSettingWid()
{

}
void MainWindow::InitMenu()
{
    QString menu_json_file_name = ":/res/menu.json";
    QByteArray ba_json;
    QFile json_file(menu_json_file_name);
    if (json_file.open(QIODevice::ReadOnly))
    {
        ba_json = json_file.readAll();
        json_file.close();
    }

    QJsonDocument json_doc = QJsonDocument::fromJson(ba_json);

    if (json_doc.isObject())
    {
        QJsonObject json_obj = json_doc.object();
        MenuJsonParser(json_obj, &Menu);
    }
    Menu.setStyleSheet("QMenu { background-color: #2C2C2C; color: white; border: 1px solid #5C5C5C; }"
                       "QMenu::item { padding: 5px 20px; }"
                       "QMenu::item:selected { background-color: #4C9AFF; }");
}
void MainWindow::MenuJsonParser(QJsonObject& json_obj, QMenu* menu)
{
    QJsonObject::iterator it = json_obj.begin();
    QJsonObject::iterator end = json_obj.end();
    while (it != end)
    {
        QString key = it.key();
        auto value = it.value();
        if (value.isObject())
        {
            QMenu* sub_menu = menu->addMenu(key);
            QJsonObject obj = value.toObject();
            MenuJsonParser(obj, sub_menu);
        }
        else
        {
            QString value_str = value.toString();
            qDebug() << value_str << "\n";
            QStringList value_info = value_str.split("/");

            if (value_info.size() == 2)
            {
                QString fun_str = value_info[0];
                QString hot_key = value_info[1];
                if (!hot_key.isEmpty())
                {
                    key += "\t" + hot_key;
                }
                QAction* action = menu->addAction(key);

                if (!hot_key.isEmpty())
                {
                    action->setShortcut(QKeySequence(hot_key));
                    action->setShortcutContext(Qt::ApplicationShortcut);
                }

                if (Map_act.contains(fun_str))
                {
                    qDebug() << "设置了菜单" << fun_str << '\n';
                    MenuAction pFunc = Map_act.value(fun_str);
                    connect(action, &QAction::triggered, this, pFunc);
                    if (fun_str == "OnLoop" || fun_str == "OnList")
                    {
                        action->setCheckable(true);
                        loopActionGroup->addAction(action);
                        if (fun_str == "OnLoop")
                            action->setChecked(VideoCtl::GetInstance()->IsLooping());
                        else if (fun_str == "OnList")
                            action->setChecked(!VideoCtl::GetInstance()->IsLooping());
                    }
                }
                else
                {
                    qWarning() << "MenuJsonParser: 未找到对应函数名:" << fun_str;
                }
            }
            else
            {
                QString fun_str = value_info[0];
                QAction* action = menu->addAction(key);
                if (Map_act.contains(fun_str))
                {
                    qDebug() << "设置了菜单" << fun_str << '\n';
                    MenuAction pFunc = Map_act.value(fun_str);
                    connect(action, &QAction::triggered, this, pFunc);
                }
                else
                {
                    qWarning() << "MenuJsonParser: 未找到对应函数名:" << fun_str;
                }
            }
        }

        it++;
    }
}

QMenu* MainWindow::AddMenuFun(QString menu_title, QMenu* menu)
{
    QMenu* menu_t = new QMenu(this);
    menu_t->setTitle(menu_title);
    menu->addMenu(menu_t);
    return menu_t;
}

void MainWindow::AddActionFun(QString action_title, QMenu* menu, void(MainWindow::* slot_addr)())
{
    QAction* action = new QAction(this);;
    action->setText(action_title);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, slot_addr);
}

void MainWindow::OnCloseBtnClicked()
{
    this->close();
}

void MainWindow::OnMinBtnClicked()
{
    this->showMinimized();
}

void MainWindow::OnMaxBtnClicked()
{
    if (isMaximized())
    {
        showNormal();
        emit SigShowMax(false);
    }
    else
    {
        showMaximized();
        emit SigShowMax(true);
    }
}

void MainWindow::OnShowOrHidePlaylist()
{
    if (PlaylistWid->isHidden())
    {
        PlaylistWid->show();
    }
    else
    {
        PlaylistWid->hide();
    }
    this->repaint();
}
void MainWindow::OnSavePeriod()
{
    if (!isRecording)
    {
        VideoCtl::GetInstance()->LoadingSc();
        qDebug() << "正在录屏中" << '\n';
        isRecording = true;
    }
    else
    {
        VideoCtl::GetInstance()->LoadingSc();
        qDebug() << "录屏完成" << '\n';
        isRecording = false;
    }
}

void MainWindow::OnLoop()
{
    VideoCtl::GetInstance()->OnSetLoop(1);
    emit SigVideoLoop("循环播放");
}
void MainWindow::OnList()
{
    VideoCtl::GetInstance()->OnSetLoop(2);
    emit SigVideoLoop("列表播放");
}

void MainWindow::OnRandom()
{
    VideoCtl::GetInstance()->OnSetLoop(3);
    emit SigVideoLoop("随机播放");
}
