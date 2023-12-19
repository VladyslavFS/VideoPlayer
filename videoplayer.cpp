#include "videoplayer.h"
#include "ui_videoplayer.h"

#include <QTime>
#include <QFileInfo>
#include <QFileDialog>

VideoPlayer::VideoPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VideoPlayer)
{
    // Ініціалізація користувацького інтерфейсу
    ui->setupUi(this);

    // Налаштування слайдера гучності
    ui->sliderVolume->setMinimum(0);
    ui->sliderVolume->setMaximum(10);
    ui->sliderVolume->setValue(80);

    // Ініціалізація стану вимкненої звуку
    isMute = false;

    // Створення екземплярів QMediaPlayer, QAudioOutput та QVideoWidget
    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    videoWidget = new QVideoWidget();

    // Встановлення вихідних пристроїв для аудіо та відео для медіапрогравача
    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setVideoOutput(videoWidget);

    // Додавання відео-віджета до користувацького інтерфейсу
    ui->videoFrame->addWidget(videoWidget);

    // Підключення сигналів та слотів для управління медіапрогравачем
    connect(mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &VideoPlayer::stateChanged);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoPlayer::positionChanged);

    connect(ui->pushPlay, SIGNAL(clicked(bool)), this, SLOT(pushPlay()));
    connect(ui->pushPause, SIGNAL(clicked(bool)), this, SLOT(pushPause()));
    connect(ui->pushStop, SIGNAL(clicked(bool)), this, SLOT(pushStop()));
    connect(ui->pushMute, SIGNAL(clicked(bool)), this, SLOT(pushMute()));

    // Підключення сигналів та слотів для дій з файлами
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(fileOpen()));
    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(fileExit()));

    // Підключення сигналів та слотів для руху слайдерів
    connect(ui->sliderTiming, SIGNAL(sliderMoved(int)), this, SLOT(sliderTiming_Moved(int)));
    connect(ui->sliderVolume, SIGNAL(sliderMoved(int)), this, SLOT(sliderVolume_Moved(int)));
}

VideoPlayer::~VideoPlayer()
{
    // Деструктор для звільнення динамічно виділених ресурсів
    delete ui;
}

// Слот для обробки змін стану відтворення
void VideoPlayer::stateChanged(QMediaPlayer::PlaybackState state)
{
    if (state == QMediaPlayer::PlayingState)
    {
        // Увімкнення/вимкнення кнопок в залежності від стану відтворення
        ui->pushPause->setEnabled(true);
        ui->pushPlay->setEnabled(false);
        ui->pushStop->setEnabled(true);
    }
    else if (state == QMediaPlayer::PausedState)
    {
        ui->pushPlay->setEnabled(true);
        ui->pushPause->setEnabled(false);
        ui->pushStop->setEnabled(true);
    }
    else if (state == QMediaPlayer::StoppedState)
    {
        ui->pushPlay->setEnabled(true);
        ui->pushPause->setEnabled(false);
        ui->pushStop->setEnabled(false);
    }
}

// Слот для обробки змін позиції відтворення
void VideoPlayer::positionChanged(qint64 position)
{
    // Оновлення слайдера позиції та напису з часом
    if (ui->sliderTiming->maximum() != mediaPlayer->duration())
        ui->sliderTiming->setMaximum(mediaPlayer->duration());
    ui->sliderTiming->setValue(position);

    int seconds = position / 1000 % 60;
    int minutes = seconds / 60 % 60;
    int hours = minutes / 60 % 24;

    // Форматування часу та оновлення напису
    QTime time(hours, minutes, seconds);
    ui->labelTiming->setText(time.toString());
}

// Слоти для кнопок управління медіапрогравачем
void VideoPlayer::pushPlay() { mediaPlayer->play(); }
void VideoPlayer::pushPause() { mediaPlayer->pause(); }
void VideoPlayer::pushStop() { mediaPlayer->stop(); }

// Слот для обробки кліків на кнопці вимкнення звуку
void VideoPlayer::pushMute()
{
    if (isMute)
    {
        audioOutput->setMuted(true);
        ui->pushMute->setText("Вимкнено");
        isMute = false;
    }
    else
    {
        audioOutput->setMuted(false);
        ui->pushMute->setText("Вимкнути");
        isMute = true;
    }
}

// Слот для відкриття файлу за допомогою діалогу вибору файлу
void VideoPlayer::fileOpen()
{
    QString filename = QFileDialog::getOpenFileName();
    QFileInfo fileInfo(filename);

    // Встановлення джерела для медіапрогравача та оновлення елементів інтерфейсу
    mediaPlayer->setMedia(QUrl::fromLocalFile(filename));
    ui->pushPlay->setEnabled(true);
    ui->fileName->setText(fileInfo.fileName());
}

// Слот для обробки дії виходу
void VideoPlayer::fileExit() { this->close(); }

// Слот для обробки переміщення слайдера гучності
void VideoPlayer::sliderVolume_Moved(int volume) { audioOutput->setVolume(volume); }

// Слот для обробки переміщення слайдера позиції відтворення
void VideoPlayer::sliderTiming_Moved(int timing) { mediaPlayer->setPosition(timing); }
