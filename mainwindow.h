#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QActionGroup>
#include <QDockWidget>
#include <QGridLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QPropertyAnimation>
#include <QStatusBar>
#include <QTimer>
#include "playlist.h"
#include "ctrlbar.h"
#include "show.h"
#include "title.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool Init();

private:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QWidget *ShowCtrlBarPlaylistBgWidget;
    QGridLayout *gridLayout;
    CtrlBar *CtrlBarWid;
    Show *ShowWid;
    QMenuBar *menubar;
    QDockWidget *PlaylistWid;
    QWidget *PlaylistContents;
    QStatusBar *statusbar;
    QDockWidget *TitleWid;
    QWidget *dockWidgetContents;
protected:
    void paintEvent(QPaintEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent* event);
private:
    bool ConnectSignalSlots();
    void OnCloseBtnClicked();
    void OnMinBtnClicked();
    void OnMaxBtnClicked();
    void OnShowOrHidePlaylist();

    void OnFullScreenPlay();

    void OnCtrlBarAnimationTimeOut();
    void OnFullscreenMouseDetectTimeOut();

    void OnCtrlBarHideTimeOut();
    void OnShowMenu();
    void OnShowAbout();
    void OpenFile();

    void OnSaveBmp();

    void OnShowSettingWid();

    void OnSavePeriod();

    void OnLoop();
    void OnList();
    void OnRandom();

    void InitMenu();
    void InitMenuActions();

    void MenuJsonParser(QJsonObject& json_obj, QMenu* menu);
    QMenu* AddMenuFun(QString menu_title, QMenu* menu);
    void AddActionFun(QString action_title, QMenu* menu, void(MainWindow::* slot_addr)());

signals:

    void SigShowMax(bool bIfMax);
    void SigSeekForward();
    void SigSeekBack();
    void SigAddVolume();
    void SigSubVolume();
    void SigPlayOrPause();
    void SigOpenFile(QString strFilename);

    void SigClear(QString file);

    void SigVideoLoop(QString file);
private:

    bool isPlaying; // 正在播放


    int ShadowWidth; // 阴影宽度

    bool FullScreenPlay; ///< 全屏播放标志

    QPropertyAnimation *CtrlbarAnimationShow; //全屏时控制面板浮动显示
    QPropertyAnimation *CtrlbarAnimationHide; //全屏时控制面板浮动显示
    QRect CtrlBarAnimationShowRect;//控制面板显示区域
    QRect CtrlBarAnimationHideRect;//控制面板隐藏区域

    QTimer CtrlBarAnimationTimer;
    QTimer FullscreenMouseDetectTimer;//全屏时鼠标位置监测时钟

    bool FullscreenCtrlBarShow;
    QTimer CtrlBarHideTimer;

    Playlist Playlist;
    Title Title;

    bool MoveDrag;//移动窗口标志
    QPoint DragPosition;

    typedef void (MainWindow::* MenuAction)();

    QMenu Menu;
    QAction ActFullscreen;

    QActionGroup* loopActionGroup;

    QMap<QString, MenuAction> Map_act;
};
#endif // MAINWINDOW_H
