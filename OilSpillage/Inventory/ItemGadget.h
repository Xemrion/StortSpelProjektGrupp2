#ifndef ITEMGADGET_H
#define ITEMGADGET_H


#pragma once
#include "Item.h"
#include "Gadget.h"
class ItemGadget : public Item
{
private:
	static std::string generateDescription(Gadget gadget);
	Gadget gadget;
public:
	ItemGadget(std::string name, Gadget gadget, GameObject* object);
	virtual ~ItemGadget();
	ItemGadget(const ItemGadget& other);
	Item* clone()const;
	//bool operator ==(const ItemGadget& other)const;
	virtual void randomize();
	Gadget& getGadget();
};

#endif // !ITEMGADGET

 