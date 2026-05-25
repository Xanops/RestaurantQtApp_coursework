#ifndef ITEM_H
#define ITEM_H
#include "string"

class Item {
public:
  Item(int id = -1, const std::string &name = "", const std::string &desc = "", float cost=0);

  int id() const;
  void setId(int newId);

  std::string name() const;
  void setName(const std::string &newName);

  std::string description() const;
  void setDescription(const std::string &newDescription);

  float cost() const;
  void setCost(float newCost);

private:
  int _id;
  std::string _name;
  std::string _description;
  float _cost;
};

#endif // ITEM_H
