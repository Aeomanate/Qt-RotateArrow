#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <thread>
#include <condition_variable>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    friend void loop(MainWindow* main_window);

private slots:
    void buttonStartClicked();
    void lineEditEnterPressed();

    void on_ButtonPlus_pressed();

    void on_ButtonMinus_pressed();

private:
    bool is_program_work = true;
    bool is_animation_run = false;
    std::mutex animation_mutex;
    std::condition_variable waiter;
    Ui::MainWindow *ui;
    std::thread animation_thread;
    QPixmap arrow_image;
    qreal rotate_angle = 0;
    qreal rotate_speed = 1;
};
#endif // MAINWINDOW_H
