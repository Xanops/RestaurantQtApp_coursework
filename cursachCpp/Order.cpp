#include "Order.h"

Order::Order(int id, const QDate &date) : _id(id), _date(date) {}

int Order::id() const { return _id; }

void Order::setId(int newId) { _id = newId; }

QDate Order::date() const { return _date; }

void Order::setDate(const QDate &newDate) { _date = newDate; }

std::map<int, Item> Order::items() const { return _items; }

void Order::setItems(const std::map<int, Item> &newItems) { _items = newItems; }

void Order::addItem(const Item &newItem) { _items[map_iter()] = newItem; }

void Order::deleteItem(int index) {
    if (index < 0 || index >= static_cast<int>(_items.size()))
        return;

    auto it = _items.begin();
    std::advance(it, index);
    _items.erase(it);
}

