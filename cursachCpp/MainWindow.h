#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Item.h"
#include "Order.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void on_pushButton_clicked();

  void on_action_triggered();

  void on_action_2_triggered();

  void on_action_3_triggered();

  void on_tableWidget_cellClicked(int row, int column);

  void on_pushButton_2_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_5_clicked();

private:
  Ui::MainWindow *ui;

  bool isNewOrderMode = false;
  std::map<int, Item> _items;
  std::map<int, Order> _orders;
  std::map<int, Item> currentItems;

  std::map<int, Item> parseFile(const char *filename);
  std::map<int, Order> parseOrders(const char *filename);
  // Функция для записи вектора заказов в файл
  void writeOrdersToFile(const std::map<int, Order> &orders, const char *filename);
};
#endif // MAINWINDOW_H
