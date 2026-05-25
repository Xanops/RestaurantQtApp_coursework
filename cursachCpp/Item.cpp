#include "Item.h"

Item::Item(int id, const std::string &name, const std::string &desc, float cost)
    : _id(id), _name(name), _description(desc), _cost(cost)
 {}


int Item::id() const
{
    return _id;
}

void Item::setId(int newId)
{
    _id = newId;
}

std::string Item::name() const
{
    return _name;
}

void Item::setName(const std::string &newName)
{
    _name = newName;
}

std::string Item::description() const
{
    return _description;
}

void Item::setDescription(const std::string &newDescription)
{
    _description = newDescription;
}

float Item::cost() const
{
    return _cost;
}

void Item::setCost(float newCost)
{
    _cost = newCost;
}
