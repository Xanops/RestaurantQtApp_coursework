#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFile>
#include <QMessageBox>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

// Загрузить бд товаров
void MainWindow::on_action_triggered() {
      QString path = "items.txt";

      if (!QFile::exists(path)) {
          QMessageBox::critical(this, "Ошибка", "Файл items.txt не найден!");
          return;
      }
      _items = parseFile(path.toStdString().c_str());

      if (_items.empty()) {
          QMessageBox::warning(this, "Внимание", "Файл пуст.");
          return;
      }

      ui->comboBox->clear();
      for (const auto& item : _items) {
          ui->comboBox->addItem(QString::fromStdString(item.second.name()));
      }

}

// Загрузить заказы
void MainWindow::on_action_2_triggered() {
    if (_items.empty()) {
          QMessageBox::warning(this, "Ошибка", "Сначала загрузите товары!");
          return;
      }

      QString path = "orders.bin";
      if (!QFile::exists(path)) {
          QMessageBox::critical(this, "Ошибка", "Файл orders.bin не найден!");
          return;
      }

      _orders = parseOrders(path.toStdString().c_str());
      if (_orders.empty()) {
          QMessageBox::warning(this, "Внимание", "Файл заказов пуст.");
          return;
      }

      ui->tableWidget->clear();
      ui->tableWidget->setRowCount(_orders.size());
      ui->tableWidget->setColumnCount(2);
      ui->tableWidget->setHorizontalHeaderLabels({"ID", "Дата"});

      int row = 0;
      for (const auto &[id, order] : _orders) {
          QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(id));
          QTableWidgetItem *dateItem = new QTableWidgetItem(order.date().toString("dd.MM.yyyy"));

          ui->tableWidget->setItem(row, 0, idItem);
          ui->tableWidget->setItem(row, 1, dateItem);
          row++;
      }
}

// Сохранить заказы
void MainWindow::on_action_3_triggered() {
    if (_items.empty()) {
        QMessageBox::warning(this, "Ошибка", "Невозможно сохранить заказы: товары не загружены.");
        return;
    }

    writeOrdersToFile(_orders, "orders.bin");
    QMessageBox::information(this, "Успех", "Заказы успешно сохранены.");
}


// Загрузка товаров
std::map<int, Item> MainWindow::parseFile(const char *filename) {
    std::map<int, Item> items;
    FILE *file = fopen(filename, "r");

    if (!file) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return items;
    }

    char line[256];
    int id;
    while (fgets(line, sizeof(line), file)) {
        std::cout<<line;
        // Удаляем символ новой строки, если он есть
        line[strcspn(line, "\n")] = '\0';

        Item item;
        char *token = strtok(line, ";");
        id = atoi(token);

        if (token)
          item.setId(atoi(token));

        token = strtok(nullptr, ";");
        if (token)
          item.setName(token);

        token = strtok(nullptr, ";");
        if (token)
          item.setDescription(token);

        token = strtok(nullptr, ";");
        if (token)
          item.setCost(atof(token));

        items[id] = item;
    }

    fclose(file);
    return items;
}


// Загрузка заказов
std::map<int, Order> MainWindow::parseOrders(const char *filename) {
    std::map<int, Order> orders;
    FILE *file = fopen(filename, "rb");
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return orders;
    }

    while (true) {
        int order_id;
        // Чтение ID заказа
        if (fread(&order_id, sizeof(int), 1, file) != 1) break;

        // Чтение даты
        size_t date_len;
        if (fread(&date_len, sizeof(size_t), 1, file) != 1) break;

        std::string date_str(date_len, '\0');
        if (fread(&date_str[0], sizeof(char), date_len, file) != date_len) break;

        Order order;
        order.setDate(QDate::fromString(QString::fromStdString(date_str), "dd.MM.yyyy"));

        // Чтение товаров
        size_t item_count;
        if (fread(&item_count, sizeof(size_t), 1, file) != 1) break;

        for (size_t i = 0; i < item_count; i++) {
            int item_id;
            if (fread(&item_id, sizeof(int), 1, file) != 1) break;

            // Поиск товара в _items по ID
            auto it = std::find_if(_items.begin(), _items.end(),
                [&](const auto &pair) { return pair.second.id() == item_id; });

            if (it != _items.end()) {
                order.addItem(it->second);
            }
        }
        orders[order_id] = order;
    }

    fclose(file);
    return orders;
}


// Запись заказов в файл
void MainWindow::writeOrdersToFile(const std::map<int, Order> &orders,
                                   const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << " for writing."
                  << std::endl;
        return;
    }

    for (const auto &order_pair : orders) {
        int order_id = order_pair.first;
        const Order &order = order_pair.second;

        // Записываем ID заказа
        fwrite(&order_id, sizeof(int), 1, file);

        // Записываем дату
        std::string date_str = order.date().toString("dd.MM.yyyy").toStdString();
        size_t date_len = date_str.size();
        fwrite(&date_len, sizeof(size_t), 1, file);
        fwrite(date_str.c_str(), sizeof(char), date_len, file);

        // Записываем товары
        size_t item_count = order.items().size();
        fwrite(&item_count, sizeof(size_t), 1, file);

        for (const auto &item_pair : order.items()) {
            int item_id = item_pair.second.id();
            fwrite(&item_id, sizeof(int), 1, file);
        }
    }
    fclose(file);
    std::cout << "Successfully wrote " << orders.size() << " orders to "
              << filename << std::endl;
}


void MainWindow::on_tableWidget_cellClicked(int row, int column) {
    if (row < 0 || row >= static_cast<int>(_orders.size()))
            return;

    auto it = _orders.begin();
    std::advance(it, row);

    ui->spinBox->setValue(it->first);
    ui->dateEdit->setDate(it->second.date());
    ui->listWidget->clear();

    float summ = 0;
    for (auto item : it->second.items())
    {
        QListWidgetItem* li = new QListWidgetItem(QString::fromStdString(item.second.name()));
        li->setToolTip(QString::fromStdString(item.second.description()) +
                       ", " + QString::number(item.second.cost(), 'f', 2));
        ui->listWidget->addItem(li);
        summ += item.second.cost();
    }
    ui->label_5->setText(QString::number(summ, 'f', 2));
}


// Добавить товар в заказ
void MainWindow::on_pushButton_clicked()
{
    if (!isNewOrderMode) {
            QMessageBox::warning(this, "Ошибка", "Необходимбо создать новый заказ, чтобы начать добавление товаров.");
            return;
        }

    int index = ui->comboBox->currentIndex();
    if (index < 0 || index >= static_cast<int>(_items.size()))
        return;

    auto it = _items.begin();
    std::advance(it, index);
    int currentItems_size = currentItems.size();
    currentItems[currentItems_size] = it->second;

    QListWidgetItem* li = new QListWidgetItem(QString::fromStdString(it->second.name()));
    li->setToolTip(QString::fromStdString(it->second.description()) + ", " + QString::number(it->second.cost(), 'f', 2));
    ui->listWidget->addItem(li);

    float total = 0;
    for (const auto& item : currentItems)
        total += item.second.cost();
    ui->label_5->setText(QString::number(total, 'f', 2));
}


void MainWindow::on_pushButton_2_clicked()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0) return;

    QTableWidgetItem *idItem = ui->tableWidget->item(row, 0);
    if (!idItem) return;

    int id = idItem->text().toInt();
    for (auto it = _orders.begin(); it != _orders.end(); ++it) {
        if (it->first == id) {
            _orders.erase(it);
            break;
        }
    }
    ui->tableWidget->removeRow(row);

    ui->spinBox->setValue(0);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->listWidget->clear();
    ui->label_5->setText("0");
}


// Добавить заказ
void MainWindow::on_pushButton_4_clicked()
{
    int id = ui->spinBox->value();
    QDate date = ui->dateEdit->date();

    for (const auto& order : _orders) {
        if (order.first == id) {
            QMessageBox::warning(this, "Ошибка", "ID заказа уже существует!");
            return;
        }
    }

    if (currentItems.empty()) {
        QMessageBox::warning(this, "Ошибка", "Список товаров пуст!");
        return;
    }

    Order newOrder(id, date);
    newOrder.setItems(currentItems);
    for (auto i: newOrder.items())
        std::cout << i.first << ' ' << i.second.id() << std::endl;
    _orders[id] = newOrder;

    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(date.toString("dd.MM.yyyy")));

    currentItems.clear();
    ui->listWidget->clear();
    ui->label_5->setText("0");
}



// Новый заказ
void MainWindow::on_pushButton_3_clicked()
{
    isNewOrderMode = true;
    if (ui->tableWidget->columnCount() == 0) {
        ui->tableWidget->setColumnCount(2);
        ui->tableWidget->setHorizontalHeaderLabels({"ID", "Дата"});
    }

    int maxId = 0;
    for (const auto& order : _orders) {
        if (order.first > maxId)
            maxId = order.first;
    }
    ui->spinBox->setValue(maxId + 1);

    ui->dateEdit->setDate(QDate::currentDate());
    ui->listWidget->clear();
    ui->label_5->setText("0");
    currentItems.clear();
}



// Удалить товар из заказа
void MainWindow::on_pushButton_5_clicked()
{
    if (!isNewOrderMode) {
            QMessageBox::warning(this, "Ошибка", "Нельзя удалить товар существующего заказа");
            return;
    }

    int row = ui->listWidget->currentRow();
    if (row < 0 || row >= static_cast<int>(currentItems.size())) {
        QMessageBox::warning(this, "Ошибка", "Выберите товар для удаления");
        return;
    }

    auto it = currentItems.begin();
    std::advance(it, row);
    currentItems.erase(it);

    delete ui->listWidget->takeItem(row);

    float total = 0;
    for (const auto& item : currentItems)
        total += item.second.cost();
    ui->label_5->setText(QString::number(total, 'f', 2));
}

