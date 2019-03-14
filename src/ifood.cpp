#include "iscene.h"
#include "ifood.h"
#include "igamedata.h"
#include <QMovie>
#include <QPainter>
#include <QtMath>

IFood::IFood(int level,int x, int y, QWidget *parent):
    IObject(x,y,parent)
{
    this->level=level;
    this->setGeometry(x,y,IGameData::FOODWIDTH,IGameData::FOODHEIGHT);
    updateMovie();
    xSpeed=0;
    ySpeed=2;
}

IFood::~IFood()
{
    movie->deleteLater();
}

void IFood::shift()
{
    //食物向下掉落
    timeCounter++;
    if (!(timeCounter%3)){
        this->setY(this->getY()+ySpeed);
        this->setGeometry(x,y,IGameData::FOODWIDTH,IGameData::FOODHEIGHT);
    }
    judgeStatus();
}

const int IFood::getPrice()
{
    return PRICE;
}

const int IFood::classToPrice(int c)
{
    switch (c) {
    case 1:
        return PRICE1;
    case 2:
        return PRICE2;
    case 3:
        return PRICE3;
    default:
        return -1;
    }
}

const int IFood::classToStarving(int c)
{
    return STARVING1*qPow(1.5,c-1);
}

const int IFood::classToDead(int c)
{
    return DEAD1*qPow(1.5,c-1);
}

void IFood::updateMovie()
{
    QString filename=QString(":/food/resource/image/food/%1.gif").arg(this->level);
    movie=new QMovie(filename);
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();
}

void IFood::read(const QJsonObject &json)
{
    level=json["level"].toInt();
    x=json["x"].toInt();
    y=json["y"].toInt();
    xSpeed=json["xSpeed"].toInt();
    ySpeed=json["ySpeed"].toInt();
    iseaten=json["iseaten"].toBool();
    timeCounter=json["timeCounter"].toInt();
    updateMovie();
}

void IFood::write(QJsonObject &json)
{
    json["level"]=level;
    json["x"]=x;
    json["y"]=y;
    json["xSpeed"]=xSpeed;
    json["ySpeed"]=ySpeed;
    json["iseaten"]=iseaten;
    json["timeCounter"]=timeCounter;
}

QRect &IFood::getRect()
{
    return QRect(x,
                 y,
                 IGameData::FOODWIDTH,
                 IGameData::FOODHEIGHT);
}


void IFood::judgeStatus()
{
    if (this->getY()>=500){
        exists=false;
    }
}

