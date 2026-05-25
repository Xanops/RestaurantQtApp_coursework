#ifndef ORDER_H
#define ORDER_H

#include "Item.h"
#include "map"
#include <QDate>

class Order {
public:
    Order(int id, const QDate &date, const std::map<int, Item> &items);
    Order(int id = -1, const QDate &date = QDate());

    int id() const;
    void setId(int newId);

    QDate date() const;
    void setDate(const QDate &newDate);

    std::map<int, Item> items() const;
    void setItems(const std::map<int, Item> &newItems);

    void addItem(const Item &newItem);

    void deleteItem(int index);

private:
    int _id;
    QDate _date;
    std::map<int , Item> _items;

    int _map_iter = 0;
    int map_iter() {
      _map_iter++;
      return _map_iter;
    }
};

#endif // ORDER_H
