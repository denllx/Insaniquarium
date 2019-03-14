#include "icreature.h"

ICreature::ICreature(int x,int y,QWidget* parent):IObject(x,y,parent)
{
    exists=true;
}

bool ICreature::checkWall(bool enabled)
{
    if (!enabled){
        return false;
    }
    if (x+xSpeed+this->width()>800){
        xSpeed=-(qrand()%5+1);
        return true;
    }
    else if (x+xSpeed<0){
        xSpeed=qrand()%5+1;
        return true;
    }
    if (y+ySpeed+this->height()*0.8>520){
        ySpeed=-(qrand()%5+1);
        return false;
    }
    else if (y+ySpeed+this->height()/5<100){
        ySpeed=qrand()%5+1;
        return false;
    }
    return false;
}

void ICreature::read(const QJsonObject &json)
{
    IObject::read(json);
    dieLeft=json["dieLeft"].toBool();
}

void ICreature::write(QJsonObject &json)
{
    IObject::write(json);
    json["dieLeft"]=dieLeft;
}

ICreature::~ICreature()
{
}
