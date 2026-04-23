#include "mainwindow.h"
#include "../ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //disable resizing window
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    setFixedSize(width(), height());
    this->setWindowTitle("WAVe");

    p.q.fillQueue("./music");

    updateList();

    updatePlayButton();

    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setValue(100);

    connect(ui->PlayBtn, &QPushButton::clicked, this, &MainWindow::handlePlayButtonPush);
    connect(ui->NextBtn, &QPushButton::clicked, this, &MainWindow::handleNextButtonPush);
    connect(ui->PrevBtn, &QPushButton::clicked, this, &MainWindow::handlePrevButtonPush);
    connect(ui->repeatBtn, &QPushButton::clicked, this, &MainWindow::hanleRepeatleButtonPush);
    connect(ui->shuffleBtn, &QPushButton::clicked, this, &MainWindow::hanleShuffleButtonPush);

    connect(ui->audioSlider, &QSlider::sliderMoved, this, [this](int val){
        p.get_slider_position(val);
    });
    connect(ui->audioSlider, &QSlider::sliderReleased, this, [this](){
        p.play();
    });

    connect(ui->volumeSlider, &QSlider::valueChanged, this, [this](int val){
        p.set_volume(val);
    });

    connect(ui->listWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        int index = ui->listWidget->row(item);
        //p.set_mode(MODE::LIST);
        p.q.setCurrentTrack(index);
        setTrack(0);
    });
    connect(&p, &Player::currentTime, this, &MainWindow::onTimeChanged);
    connect(&p, &Player::currentProgress, this, &MainWindow::onProgressChanged);
    connect(&p, &Player::trackEnded, this, &MainWindow::setNextTrack);
}

MainWindow::~MainWindow()
{
    p.~Player();
    delete ui;
}

void MainWindow::updateList()
{ 
    ui->listWidget->clear();

    for(const auto& track : p.q.track_queue) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(":/default_cover.png"), QString::fromStdString(track.track_name));
        ui->listWidget->addItem(item);
        ui->listWidget->setIconSize(QSize(32,32));
    }
}


void MainWindow::setTrack(const int& position)
{

    try{
        if(p.play_mode == MODE::SHUFFLE){
            p.setTrack(p.q.setRandomTrack());
        }
        else{
            p.setTrack(position);
        }
    }
    catch(const std::runtime_error& e)
    {
        p.set_current_state(PlayerState::STOPPED);
        //showMessage(static_cast<std::string>(e.what()));
        //return;
        p.setTrack(position + 1);
    }

    ui->trackName->setText(QString::fromStdString(p.get_track_name()));
    ui->trackArtist->setText(QString::fromStdString(p.get_track_artist()));
    ui->audioSlider->setRange(0, 100);

    ui->total_time->setText(QString::fromStdString(p.count_total_time()));

    std::string apic = p.get_apic();
    if(!apic.empty())
    {
        QByteArray byteArray(reinterpret_cast<const char*>(apic.data()), apic.length());
        QImage img;
        img.loadFromData(byteArray);
        QPixmap pixmap = QPixmap::fromImage(img);
        ui->label->setPixmap(pixmap);
    }
    else{
        ui->label->setPixmap(QPixmap(":/vynil2.jpeg"));
    }

    p.play();
    updatePlayButton();
}

void MainWindow::showMessage(const std::string& msg)
{
    QMessageBox m;
    m.setText(QString::fromStdString(msg));
    m.exec();
}

void MainWindow::updatePlayButton()
{
    switch (p.get_current_state())
    {
    case PlayerState::PLAYING:
        ui->PlayBtn->setIcon(
            style()->standardIcon(QStyle::SP_MediaPause)
            );
        break;
    case PlayerState::STOPPED:
        ui->PlayBtn->setIcon(
            style()->standardIcon(QStyle::SP_MediaPlay)
            );
        break;
    case PlayerState::PAUSED:
        ui->PlayBtn->setIcon(
            style()->standardIcon(QStyle::SP_MediaPlay)
            );
        break;
    }
}

void MainWindow::onTimeChanged(const std::string& current_time)
{
    ui->played_time->setText(QString::fromStdString(p.count_current_time()));
}

void MainWindow::onProgressChanged(uint32_t currentBytes, uint32_t totalBytes)
{
    // защита от авто-триггера seek
    ui->audioSlider->blockSignals(true);

    ui->audioSlider->setRange(0, totalBytes);
    ui->audioSlider->setValue(currentBytes);

    ui->audioSlider->blockSignals(false);
}

/**
 * @todo ПЕРЕДЕЛАТЬ
 */
void MainWindow::handlePlayButtonPush()//ПЕРЕДЕЛАТЬ
{
    if(p.get_current_state() == PlayerState::STOPPED)
    {
        setTrack(0);
    }
    else if(p.get_current_state() == PlayerState::PAUSED)
    {
        p.play();
    }
    else if(p.get_current_state() == PlayerState::PLAYING)
    {
        p.pause();
    }
    updatePlayButton();
}

void MainWindow::handlePrevButtonPush()
{
    p.pause();
    //p.set_mode(MODE::LIST);
    setTrack(-1);

    QListWidgetItem* item = ui->listWidget->item(p.q.currentIndex());
    ui->listWidget->setCurrentItem(item);
}

void MainWindow::handleNextButtonPush()
{
    p.pause();
    //p.set_mode(MODE::LIST);
    setTrack(1);

    QListWidgetItem* item = ui->listWidget->item(p.q.currentIndex());
    ui->listWidget->setCurrentItem(item);
}
void MainWindow::hanleRepeatleButtonPush()
{
    p.set_mode(MODE::REPEAT);
    if(p.play_mode == MODE::REPEAT)
    {
        ui->repeatBtn->setStyleSheet("background-color: #0b77cf");
        ui->shuffleBtn->setStyleSheet("background-color: #4b4b4b");
    }
    else{
        ui->repeatBtn->setStyleSheet("background-color: #4b4b4b");
    }
}

void MainWindow::hanleShuffleButtonPush()
{
    p.set_mode(MODE::SHUFFLE);
    if(p.play_mode == MODE::SHUFFLE)
    {
        ui->shuffleBtn->setStyleSheet("background-color: #0b77cf");
        ui->repeatBtn->setStyleSheet("background-color: #4b4b4b");
    }
    else{
        ui->shuffleBtn->setStyleSheet("background-color: #4b4b4b");
    }
}

void MainWindow::setNextTrack()
{
    if(p.play_mode == MODE::REPEAT)
    {
        p.repeat();
    }
    else{
        setTrack(1);
    }

    QListWidgetItem* item = ui->listWidget->item(p.q.currentIndex());
    ui->listWidget->setCurrentItem(item);
}
