#include <QApplication>
#include "scanar_gui/mainwindow.h"
#include <rclcpp/rclcpp.hpp>

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    int result = a.exec();
    rclcpp::shutdown();
    return result;
}
