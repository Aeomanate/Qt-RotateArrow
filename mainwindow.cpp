#include <chrono>
#include <QPainter>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
using namespace std::chrono_literals;

void loop(MainWindow* main_window) {
    auto now = std::chrono::steady_clock::now;
    auto last = now();
    auto pause = 33'333us;

    while(main_window->is_program_work) {
        if(!main_window->is_animation_run) {
            std::unique_lock lock(main_window->animation_mutex);
            main_window->waiter.wait(lock, [&] () { return main_window->is_animation_run || !main_window->is_program_work; } );
        }
        if(now() - last < pause) {
            continue;
        } else {
            last = now();
        }
        main_window->rotate_angle += main_window->rotate_speed;
        QPixmap to_rotate(main_window->arrow_image);
        auto [width, height] = std::pair{to_rotate.size().width(), to_rotate.size().height()};
        auto [cx, cy] = std::pair{width/2, height/2};

        QPainter p(&to_rotate);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.translate(cx, cy);
        p.rotate(main_window->rotate_angle);
        p.translate(-cx, -cy);
        p.drawPixmap(0, 0, main_window->arrow_image);
        p.end();

        main_window->ui->Arrow->setPixmap(to_rotate);
    }
}

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
, animation_thread(loop, this)
, arrow_image(":/im/a.PNG")
{

    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    is_program_work = false;
    {
        std::unique_lock lock(animation_mutex);
        is_animation_run = false;
    }
    waiter.notify_all();
    animation_thread.join();
    delete ui;
}


void MainWindow::buttonStartClicked() {
    {
        std::unique_lock lock(animation_mutex);
        is_animation_run = !is_animation_run;
    }
    waiter.notify_all();
    this->ui->MainButton->setText(is_animation_run ? QString("Stop") : QString("Start"));
}

void MainWindow::lineEditEnterPressed() {
    bool is_accepted;
    auto input = this->ui->LineEditVelocity->text();
    int new_speed = input.toInt(&is_accepted);
    if(!is_accepted) {
        this->ui->LineEditVelocity->setText(input + "?");
        return;
    }
    rotate_speed = new_speed;
}

void MainWindow::on_ButtonPlus_pressed() {
    this->ui->LineEditVelocity->setText(QString::number(++rotate_speed, 'g', 18));
}

void MainWindow::on_ButtonMinus_pressed() {
    this->ui->LineEditVelocity->setText(QString::number(--rotate_speed, 'g', 18));
}
