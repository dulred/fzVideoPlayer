#pragma once
#include "CustomSlider.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimeEdit>
#include <QWidget>

class CtrlBar : public QWidget
{
    Q_OBJECT

public:
    explicit CtrlBar(QWidget *parent = 0);
    void checkSpeedBtnTextChanged();
    void checkClearBtnTextChanged();
    void checkSettingBtnTextChanged();
    void ChangeText();
    ~CtrlBar();
    bool Init();
private:
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayout;
    QPushButton *ForwardBtn;
    QTimeEdit *VideoTotalTimeTimeEdit;
    QPushButton *speedBtn;
    QPushButton *WordBtn;
    QPushButton *SettingBtn;
    QPushButton *PlaylistCtrlBtn;
    QPushButton *clearBtn;
    QTimeEdit *VideoPlayTimeTimeEdit;
    QLabel *TimeSplitLabel;
    QPushButton *PlayOrPauseBtn;
    QPushButton *BackwardBtn;
    QPushButton *RecordBtn;
    QWidget *PlaySliderBgWidget;
    QHBoxLayout *horizontalLayout;
    CustomSlider *PlaySlider;
    QWidget *widget_2;
    QGridLayout *gridLayout_2;
    QPushButton *VolumeBtn;
    CustomSlider *VolumeSlider;

public:
    void OnVideoTotalSeconds(double nSeconds);
    void OnVideoPlaySeconds(double nSeconds);
    void OnVideopVolume(double dPercent);
    void OnPauseStat(bool bPaused);
//     void OnStopFinished();
//     void OnSpeed(float speed);

//     void OnChangeVideo(QString s);
private:
    void OnPlaySliderValueChanged();
    void OnVolumeSliderValueChanged();
//     void OnRecordBtn();

private slots:
    void on_PlayOrPauseBtn_clicked();
    void on_VolumeBtn_clicked();
    void on_SettingBtn_clicked();
    void on_speedBtn_changed();
    void on_clearBtn_changed();
    bool ConnectSignalSlots();
    void On_WordBtn_clicked();

signals:
    void SigShowOrHidePlaylist();
    void SigPlaySeek(double dPercent);
    void SigPlayVolume(double dPercent);
    void SigPlayOrPause();
    void SigStop();
    void SigForwardPlay();
    void SigBackwardPlay();
    void SigShowMenu();
    void SigShowSetting();
    void SigSpeed(double speed);

    void SigLoadSubtitle(QString word);
    void SigHideSubtitle();

private:
    double TotalPlaySeconds;
    double LastVolumePercent;
};
