#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "scanar_gui/ros_worker.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateImage(QImage img);
    void updateOdom(double x, double y, double z);
    void updateLidar(int points);
    void onStartClicked();
    void onStopClicked();

private:
    RosWorker *ros_worker_;
    QLabel *image_label_;
    QLabel *status_label_;
    QLabel *odom_label_;
    QLabel *lidar_label_;
    QPushButton *start_button_;
    QPushButton *stop_button_;
};

#endif
