#include "iobject.h"
#include "iscene.h"

IObject::IObject(int x,int y,QWidget *parent):QLabel(parent)
{
    this->x=x;
    this->y=y;
    gamePanel=static_cast<IGameScene*>(parent);
}

void IObject::read(const QJsonObject &json)
{
    x=json["x"].toInt();
    y=json["y"].toInt();
    xSpeed=json["xSpeed"].toInt();
    ySpeed=json["ySpeed"].toInt();
    timeCounter=json["timeCounter"].toInt();
}

void IObject::write(QJsonObject &json)
{
    json["x"]=x;
    json["y"]=y;
    json["xSpeed"]=xSpeed;
    json["ySpeed"]=ySpeed;
    json["timeCounter"]=timeCounter;
}

IObject::~IObject()
{

}
