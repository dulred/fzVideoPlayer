#include <QDebug>
#include <QTime>
#include <QSettings>
#include <QTimer>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
// #include <QtSvgWidgets/QSvgWidget>
 
#include "ctrlbar.h"
#include "globalhelper.h"
#include <QFileDialog>
#include "videoctl.h"
#include "mainwindow.h"

extern bool isRecording;

const int PLAY_SLIDER_MAX = 3600000; // 例如，最大支持1小时的视频（以毫秒为单位）
const int VOLUME_SLIDER_MAX = 1000;  // 体积滑块最大值，代表100%
CtrlBar::CtrlBar(QWidget *parent) :
    QWidget(parent)
{

    if (this->objectName().isEmpty())
        this->setObjectName("CtrlBar");
    this->resize(594, 60);
    this->setMaximumSize(QSize(16777215, 60));
    gridLayout_3 = new QGridLayout(this);
    gridLayout_3->setSpacing(0);
    gridLayout_3->setObjectName("gridLayout_3");
    gridLayout_3->setContentsMargins(0, 0, 0, 0);
    gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    gridLayout->setObjectName("gridLayout");
    ForwardBtn = new QPushButton(this);
    ForwardBtn->setObjectName("ForwardBtn");
    ForwardBtn->setMinimumSize(QSize(30, 30));
    ForwardBtn->setMaximumSize(QSize(30, 30));

    gridLayout->addWidget(ForwardBtn, 0, 2, 1, 1);

    VideoTotalTimeTimeEdit = new QTimeEdit(this);
    VideoTotalTimeTimeEdit->setObjectName("VideoTotalTimeTimeEdit");
    VideoTotalTimeTimeEdit->setMaximumSize(QSize(70, 16777215));
    VideoTotalTimeTimeEdit->setWrapping(false);
    VideoTotalTimeTimeEdit->setFrame(false);
    VideoTotalTimeTimeEdit->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);
    VideoTotalTimeTimeEdit->setReadOnly(true);
    VideoTotalTimeTimeEdit->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
    VideoTotalTimeTimeEdit->setKeyboardTracking(true);
    VideoTotalTimeTimeEdit->setDisplayFormat("HH:mm:ss");

    gridLayout->addWidget(VideoTotalTimeTimeEdit, 0, 8, 1, 1);

    speedBtn = new QPushButton(this);
    speedBtn->setObjectName("speedBtn");
    speedBtn->setMinimumSize(QSize(30, 30));
    speedBtn->setMaximumSize(QSize(100, 30));
    speedBtn->setText("倍速");

    QFont font;
    font.setFamilies({QString::fromUtf8("\346\245\267\344\275\223")});
    font.setPointSize(12);
    font.setBold(true);
    speedBtn->setFont(font);

    gridLayout->addWidget(speedBtn, 0, 10, 1, 1);

    WordBtn = new QPushButton(this);
    WordBtn->setObjectName("WordBtn");
    WordBtn->setMinimumSize(QSize(30, 30));
    WordBtn->setMaximumSize(QSize(30, 30));

    gridLayout->addWidget(WordBtn, 0, 12, 1, 1);

    SettingBtn = new QPushButton(this);
    SettingBtn->setObjectName("SettingBtn");
    SettingBtn->setMinimumSize(QSize(30, 30));
    SettingBtn->setMaximumSize(QSize(30, 30));

    gridLayout->addWidget(SettingBtn, 0, 15, 1, 1);

    PlaylistCtrlBtn = new QPushButton(this);
    PlaylistCtrlBtn->setObjectName("PlaylistCtrlBtn");
    PlaylistCtrlBtn->setMinimumSize(QSize(30, 30));
    PlaylistCtrlBtn->setMaximumSize(QSize(30, 30));
    QFont font1;
    font1.setPointSize(20);
    PlaylistCtrlBtn->setFont(font1);
    PlaylistCtrlBtn->setText("1");

    gridLayout->addWidget(PlaylistCtrlBtn, 0, 14, 1, 1);

    clearBtn = new QPushButton(this);
    clearBtn->setObjectName("clearBtn");
    clearBtn->setMinimumSize(QSize(30, 30));
    clearBtn->setMaximumSize(QSize(100, 30));
    clearBtn->setFont(font);
    clearBtn->setText("自动");

    gridLayout->addWidget(clearBtn, 0, 11, 1, 1);

    VideoPlayTimeTimeEdit = new QTimeEdit(this);
    VideoPlayTimeTimeEdit->setObjectName("VideoPlayTimeTimeEdit");
    VideoPlayTimeTimeEdit->setMaximumSize(QSize(70, 16777215));
    VideoPlayTimeTimeEdit->setFrame(false);
    VideoPlayTimeTimeEdit->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
    VideoPlayTimeTimeEdit->setReadOnly(true);
    VideoPlayTimeTimeEdit->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
    VideoPlayTimeTimeEdit->setDisplayFormat("HH:mm:ss");

    gridLayout->addWidget(VideoPlayTimeTimeEdit, 0, 6, 1, 1);

    TimeSplitLabel = new QLabel(this);
    TimeSplitLabel->setObjectName("TimeSplitLabel");
    TimeSplitLabel->setMaximumSize(QSize(8, 16777215));
    TimeSplitLabel->setText("/");

    gridLayout->addWidget(TimeSplitLabel, 0, 7, 1, 1);

    PlayOrPauseBtn = new QPushButton(this);
    PlayOrPauseBtn->setObjectName("PlayOrPauseBtn");
    PlayOrPauseBtn->setMinimumSize(QSize(30, 30));
    PlayOrPauseBtn->setMaximumSize(QSize(30, 30));

    gridLayout->addWidget(PlayOrPauseBtn, 0, 1, 1, 1);

    BackwardBtn = new QPushButton(this);
    BackwardBtn->setObjectName("BackwardBtn");
    BackwardBtn->setMinimumSize(QSize(30, 30));
    BackwardBtn->setMaximumSize(QSize(30, 30));

    gridLayout->addWidget(BackwardBtn, 0, 0, 1, 1);

    RecordBtn = new QPushButton(this);
    RecordBtn->setObjectName("RecordBtn");
    RecordBtn->setMinimumSize(QSize(30, 30));
    RecordBtn->setMaximumSize(QSize(30, 30));

    gridLayout->addWidget(RecordBtn, 0, 13, 1, 1);


    gridLayout_3->addLayout(gridLayout, 1, 0, 1, 1);

    PlaySliderBgWidget = new QWidget(this);
    PlaySliderBgWidget->setObjectName("PlaySliderBgWidget");
    PlaySliderBgWidget->setMaximumSize(QSize(16777215, 25));
    horizontalLayout = new QHBoxLayout(PlaySliderBgWidget);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setObjectName("horizontalLayout");
    horizontalLayout->setContentsMargins(3, 0, 0, 0);
    PlaySlider = new CustomSlider(PlaySliderBgWidget);
    PlaySlider->setObjectName("PlaySlider");
    PlaySlider->setMaximum(65536);
    PlaySlider->setOrientation(Qt::Orientation::Horizontal);

    horizontalLayout->addWidget(PlaySlider);

    widget_2 = new QWidget(PlaySliderBgWidget);
    widget_2->setObjectName("widget_2");
    widget_2->setMaximumSize(QSize(108, 25));
    gridLayout_2 = new QGridLayout(widget_2);
    gridLayout_2->setSpacing(0);
    gridLayout_2->setObjectName("gridLayout_2");
    gridLayout_2->setContentsMargins(0, 0, 0, 0);
    VolumeBtn = new QPushButton(widget_2);
    VolumeBtn->setObjectName("VolumeBtn");
    VolumeBtn->setMinimumSize(QSize(20, 20));
    VolumeBtn->setMaximumSize(QSize(20, 20));

    gridLayout_2->addWidget(VolumeBtn, 0, 0, 1, 1);

    VolumeSlider = new CustomSlider(widget_2);
    VolumeSlider->setObjectName("VolumeSlider");
    VolumeSlider->setMinimumSize(QSize(80, 25));
    VolumeSlider->setMaximumSize(QSize(80, 25));
    VolumeSlider->setOrientation(Qt::Orientation::Horizontal);

    gridLayout_2->addWidget(VolumeSlider, 0, 1, 1, 1);


    horizontalLayout->addWidget(widget_2);


    gridLayout_3->addWidget(PlaySliderBgWidget, 0, 0, 1, 1);

    this->setWindowTitle("Form");





    LastVolumePercent = 1.0;

    PlaySlider->setMinimum(0);
    PlaySlider->setMaximum(PLAY_SLIDER_MAX);
    PlaySlider->setSingleStep(100);   // 每次步进100毫秒
    PlaySlider->setPageStep(1000);    // 每次页面步进1秒

    VolumeSlider->setMinimum(0);
    VolumeSlider->setMaximum(VOLUME_SLIDER_MAX);
    VolumeSlider->setSingleStep(50);   // 每次步进5%
    VolumeSlider->setPageStep(100);    // 每次页面步进10%

    QTimer* textCheckTimer = new QTimer(this);
    connect(textCheckTimer, &QTimer::timeout, this, &CtrlBar::checkSpeedBtnTextChanged);
    connect(textCheckTimer, &QTimer::timeout, this, &CtrlBar::checkClearBtnTextChanged);
    connect(textCheckTimer, &QTimer::timeout, this, &CtrlBar::checkSettingBtnTextChanged);
    textCheckTimer->start(100);

}
void CtrlBar::checkSpeedBtnTextChanged()
{
    static QString lastText = speedBtn->text();
    QString currentText = speedBtn->text();

    if (currentText != lastText)
    {
        on_speedBtn_changed();
        lastText = currentText;
    }
}
void CtrlBar::checkClearBtnTextChanged()
{
    static QString lastText = clearBtn->text();
    QString currentText = clearBtn->text();

    if (currentText != lastText)
    {
        on_clearBtn_changed();
        lastText = currentText;
    }
}
void CtrlBar::checkSettingBtnTextChanged()
{
    static QString lastText = SettingBtn->text();
    QString currentText = SettingBtn->text();

    if (currentText != lastText)
    {
        on_SettingBtn_clicked();
        lastText = currentText;
    }
}
void CtrlBar::ChangeText()
{
    clearBtn->setText("自动");
    GlobalHelper::SetIcon(WordBtn, 15, QChar(0xe135));
    VideoCtl::GetInstance()->OnSetWordPlay(false);
}
CtrlBar::~CtrlBar()
{
}

bool CtrlBar::Init()
{
    setStyleSheet(GlobalHelper::GetQssStr(":/res/qss/ctrlbar.css"));


    GlobalHelper::SetIcon(PlayOrPauseBtn, 15, QChar(0xf04b));
    GlobalHelper::SetIcon(WordBtn, 15, QChar(0xe135));
    GlobalHelper::SetIcon(VolumeBtn, 12, QChar(0xf6a8));
    GlobalHelper::SetIcon(PlaylistCtrlBtn, 15, QChar(0xf0ae));
    GlobalHelper::SetIcon(ForwardBtn, 15, QChar(0xf051));
    GlobalHelper::SetIcon(BackwardBtn, 15, QChar(0xf048));
    GlobalHelper::SetIcon(RecordBtn, 15, QChar(0xf8d9));
    GlobalHelper::SetIcon(SettingBtn, 15, QChar(0xf2f9));
 
    GlobalHelper::CreateSpeedMenu(speedBtn);
    GlobalHelper::CreateClearMenu(clearBtn);
    GlobalHelper::CreateSettingMenu(SettingBtn);

    PlaylistCtrlBtn->setToolTip("播放列表");
    SettingBtn->setToolTip("设置播放方式");
    VolumeBtn->setToolTip("静音");
    ForwardBtn->setToolTip("下一个");
    BackwardBtn->setToolTip("上一个");
    WordBtn->setToolTip("字幕");
    PlayOrPauseBtn->setToolTip("播放");
    speedBtn->setToolTip("倍速");
    clearBtn->setToolTip("清晰度");
    RecordBtn->setToolTip("录屏");
    
    ConnectSignalSlots();

    double dPercent = -1.0;
    GlobalHelper::GetPlayVolume(dPercent);
    if (dPercent != -1.0)
    {
        emit SigPlayVolume(dPercent);
        OnVideopVolume(dPercent);
    }



    return true;

}

bool CtrlBar::ConnectSignalSlots()
{
    QList<bool> listRet;
    bool bRet;

//     connect(ui->PlaylistCtrlBtn, &QPushButton::clicked, this, &CtrlBar::SigShowOrHidePlaylist);
    connect(PlaySlider, &CustomSlider::SigCustomSliderValueChanged, this, &CtrlBar::OnPlaySliderValueChanged);
    connect(VolumeSlider, &CustomSlider::SigCustomSliderValueChanged, this, &CtrlBar::OnVolumeSliderValueChanged);
    connect(BackwardBtn, &QPushButton::clicked, this, &CtrlBar::SigBackwardPlay);
    connect(ForwardBtn, &QPushButton::clicked, this, &CtrlBar::SigForwardPlay);
//     connect(ui->WordBtn, &QPushButton::clicked, this, &CtrlBar::On_WordBtn_clicked);

//     //connect(ui->speedBtn, &QPushButton::, this, &CtrlBar::on_speedBtn_changed);
//     connect(VideoCtl::GetInstance(), &VideoCtl::SigClear, this, &CtrlBar::ChangeText);
//     connect(ui->RecordBtn, &QPushButton::clicked, this, &CtrlBar::OnRecordBtn);
    connect(PlayOrPauseBtn,&QPushButton::clicked,this,&CtrlBar::on_PlayOrPauseBtn_clicked);
    return true;
}

void CtrlBar::OnVideoTotalSeconds(double nSeconds)
{
    TotalPlaySeconds = nSeconds;

    // 计算小时、分钟、秒和毫秒
    int thh = static_cast<int>(nSeconds) / 3600;
    int tmm = (static_cast<int>(nSeconds) % 3600) / 60;
    int tss = static_cast<int>(nSeconds) % 60;
    int ms = static_cast<int>((nSeconds - static_cast<int>(nSeconds)) * 1000);

    // 创建包含毫秒的 QTime 对象
    QTime TotalTime(thh, tmm, tss, ms);
    VideoTotalTimeTimeEdit->setTime(TotalTime);

    // 设置滑块的最大值为总毫秒数，确保不会超过预设的最大值
    int totalMillis = static_cast<int>(nSeconds * 1000);
    totalMillis = qMin(totalMillis, PLAY_SLIDER_MAX);
    PlaySlider->setMaximum(totalMillis);
}



void CtrlBar::OnVideoPlaySeconds(double currentSeconds)
{
    currentSeconds = std::min(currentSeconds, TotalPlaySeconds);

    // 计算小时、分钟、秒和毫秒
    int thh = static_cast<int>(currentSeconds) / 3600;
    int tmm = (static_cast<int>(currentSeconds) % 3600) / 60;
    int tss = static_cast<int>(currentSeconds) % 60;
    int ms = static_cast<int>((currentSeconds - static_cast<int>(currentSeconds)) * 1000);

    // 创建包含毫秒的 QTime 对象
    QTime CurrentTime(thh, tmm, tss, ms);
    VideoPlayTimeTimeEdit->setTime(CurrentTime);

    // 将当前播放时间转换为毫秒，并限制在滑块的范围内
    int currentMillis = static_cast<int>(currentSeconds * 1000);
    currentMillis = qMin(currentMillis, PlaySlider->maximum());

    // 设置滑块值
    PlaySlider->setValue(currentMillis);
}

void CtrlBar::OnVideopVolume(double dPercent)
{
    dPercent = qBound(0.0, dPercent, 1.0);

    int volumeValue = static_cast<int>(dPercent * VOLUME_SLIDER_MAX);
    VolumeSlider->setValue(volumeValue);
    LastVolumePercent = dPercent;
    qDebug() << "LastVolumePercent：" << dPercent << '\n';

    if (LastVolumePercent == 0)
    {
        GlobalHelper::SetIcon(VolumeBtn, 12, QChar(0xf2e2)); // 静音图标
        VolumeBtn->setToolTip("取消静音");
    }
    else if (LastVolumePercent == 1)
    {
        GlobalHelper::SetIcon(VolumeBtn, 12, QChar(0xf028)); // 最大音量图标
        VolumeBtn->setToolTip("静音");
    }
    else
    {
        GlobalHelper::SetIcon(VolumeBtn, 12, QChar(0xf6a8)); // 中等音量图标
        VolumeBtn->setToolTip("静音");
    }

    GlobalHelper::SavePlayVolume(dPercent);
}

void CtrlBar::OnPauseStat(bool bPaused)
{
    qDebug() << "CtrlBar::OnPauseStat" << bPaused;
    if (bPaused)
    {
        GlobalHelper::SetIcon(PlayOrPauseBtn, 15, QChar(0xf04b));
        PlayOrPauseBtn->setToolTip("播放");
    }
    else
    {
        GlobalHelper::SetIcon(PlayOrPauseBtn, 15, QChar(0xf04c));
        PlayOrPauseBtn->setToolTip("暂停");
    }
}

// void CtrlBar::OnStopFinished()
// {
//     ui->PlaySlider->setValue(0);
//     QTime StopTime(0, 0, 0, 0);
//     ui->VideoTotalTimeTimeEdit->setTime(StopTime);
//     ui->VideoPlayTimeTimeEdit->setTime(StopTime);
//     GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 15, QChar(0xf04b)); // 播放图标
//     ui->PlayOrPauseBtn->setToolTip("播放");
// }

// void CtrlBar::OnSpeed(float speed)
// {
//     ui->speedBtn->setText(QString("倍速:").arg(speed));
// }
// void CtrlBar::OnChangeVideo(QString s)
// {
//     if (s == "循环播放")
//     {
//         GlobalHelper::SetIcon(ui->SettingBtn, 15, QChar(0xf2f9));
//     }
//     else if (s == "列表播放")
//     {
//         GlobalHelper::SetIcon(ui->SettingBtn, 15, QChar(0xf883));
//     }
//     else
//     {
//         GlobalHelper::SetIcon(ui->SettingBtn, 15, QChar(0xf074));
//     }
// }
void CtrlBar::OnPlaySliderValueChanged()
{

    int currentMillis = PlaySlider->value();

    double targetSeconds = static_cast<double>(currentMillis) / 1000.0;

    emit SigPlaySeek(targetSeconds/TotalPlaySeconds);
    qDebug() << "Seeking to:" << targetSeconds << "seconds";
}

void CtrlBar::OnVolumeSliderValueChanged()
{
    int volumeValue = VolumeSlider->value();

    double dPercent = static_cast<double>(volumeValue) / VOLUME_SLIDER_MAX;

    emit SigPlayVolume(dPercent);

    OnVideopVolume(dPercent);
}

void CtrlBar::on_PlayOrPauseBtn_clicked()
{
    emit SigPlayOrPause();
}

void CtrlBar::on_VolumeBtn_clicked()
{
    if (VolumeBtn->text() == QChar(0xf6a8)|| VolumeBtn->text() == QChar(0xf028))
    {
        GlobalHelper::SetIcon(VolumeBtn, 12, QChar(0xf2e2));
        VolumeSlider->setValue(0);
        emit SigPlayVolume(0);
    }
    else
    {
        GlobalHelper::SetIcon(VolumeBtn, 12, QChar(0xf028));
        VolumeSlider->setValue(LastVolumePercent * VOLUME_SLIDER_MAX);
        emit SigPlayVolume(LastVolumePercent);
    }

}
void CtrlBar::On_WordBtn_clicked()
{
    if (WordBtn->text() == QChar(0xe135))
    {
        VideoCtl::GetInstance()->OnSetWordPlay(true);
        int flag = VideoCtl::GetInstance()->GetHasWord();
        if (flag)
        {
            GlobalHelper::SetIcon(WordBtn, 15, QChar(0xf20a));
        }
        else
        {
            QString subtitleFilePath = QFileDialog::getOpenFileName(this, "选择字幕文件", "", "字幕文件 (*.srt *.ass *.sub *.ssa)");
            if (!subtitleFilePath.isEmpty())
            {
                emit SigLoadSubtitle(subtitleFilePath);
                //GlobalHelper::SetIcon(ui->WordBtn, 15, QChar(0xf20a));
            }
        }
    }
    else
    {
        VideoCtl::GetInstance()->OnSetWordPlay(false);
        emit SigHideSubtitle();
        GlobalHelper::SetIcon(WordBtn, 15, QChar(0xe135));
    }
}
// void CtrlBar::OnRecordBtn()
// {
//     if (!isRecording)
//     {
//         VideoCtl::GetInstance()->LoadingSc();
//         qDebug() << "正在录屏中" << '\n';
//         isRecording = true;
//     }
//     else
//     {
//         VideoCtl::GetInstance()->LoadingSc();
//         qDebug() << "录屏完成" << '\n';
//         isRecording = false;
//     }
// }
void CtrlBar::on_SettingBtn_clicked()
{
    if (SettingBtn->text() == QChar(0xf2f9))
    {
        GlobalHelper::SetIcon(SettingBtn, 15, QChar(0xf2f9));
        VideoCtl::GetInstance()->OnSetLoop(1);
    }
    else if (SettingBtn->text() == QChar(0xf883))
    {
        GlobalHelper::SetIcon(SettingBtn, 15, QChar(0xf883));
        VideoCtl::GetInstance()->OnSetLoop(2);
    }
    else
    {
        VideoCtl::GetInstance()->OnSetLoop(3);
        GlobalHelper::SetIcon(SettingBtn, 15, QChar(0xf074));
    }
}
void CtrlBar::on_speedBtn_changed()
{
    if (speedBtn->text() == "倍速")
    {
        emit SigSpeed(1);
    }
    else if (speedBtn->text() == "0.75x")
    {
        emit SigSpeed(0.75);
    }
    else if (speedBtn->text() == "1.25x")
    {
        emit SigSpeed(1.25);
    }
    else if (speedBtn->text() == "1.5x")
    {
        emit SigSpeed(1.5);
    }
    else if(speedBtn->text()=="0.5x")
    {
        emit SigSpeed(0.5);
    }
    else  emit SigSpeed(2);

}
void CtrlBar::on_clearBtn_changed()
{
    if (clearBtn->text() == "自动")
    {
        qDebug() << "选择了自动" << '\n';
        VideoCtl::GetInstance()->ChangeResolution("自动");
    }
    else if (clearBtn->text() == "360P 流畅")
    {
        qDebug() << "选择了360P" << '\n';
        VideoCtl::GetInstance()->ChangeResolution("360p");
    }
    else if (clearBtn->text() == "480P 清晰")
    {
        qDebug() << "选择了480P" << '\n';
        VideoCtl::GetInstance()->ChangeResolution("480p");
    }
    else if (clearBtn->text() == "720P 高清")
    {
        qDebug() << "选择了720P" << '\n';
        VideoCtl::GetInstance()->ChangeResolution("720p");
    }
    else if (clearBtn->text() == "1080P 高清")
    {
        qDebug() << "选择了1080P" << '\n';
        VideoCtl::GetInstance()->ChangeResolution("1080p");
    }
    else
    {
        qDebug() << "选择了1080P 60帧" << '\n';
        VideoCtl::GetInstance()->ChangeResolution("1080p_60");
    }
}
