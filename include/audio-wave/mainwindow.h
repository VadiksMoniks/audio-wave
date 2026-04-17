#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Player.hpp"
#include <QMainWindow>
/**
*@todo СДЕЛАТЬ ВЫБОР ДИРЕКТОРИИ С ПЕСНЯМИ И КАК-ТО ПОФИКСИТЬ МОМЕНТ, ЕСЛИ ПАПКА НЕ СОДЕРЖИТ ТРЕКОВ, ТОЖЕ СООБЩЕНИЕ КИДАТЬ
*@todo REPEAT ?SHUFFLE?
*/
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateList();
    //void audioLoop();
    void setTrack(const int& position);
    void initSlider();
    void pause();
    void showMessage(const std::string& msg);
    void updatePlayButton();

private:
    Ui::MainWindow *ui;

    Player p;

private slots:
    void handlePlayButtonPush();
    void handlePrevButtonPush();
    void handleNextButtonPush();
    void hanleRepeatleButtonPush();
    void hanleShuffleButtonPush();
    void onTimeChanged(const std::string& current_time);
    void onProgressChanged(uint32_t currentBytes, uint32_t totalBytes);
    void setNextTrack();
};
#endif // MAINWINDOW_H
