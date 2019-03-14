#include "imoney.h"
#include "iscene.h"
#include "igamedata.h"
#include <QtMath>
#include <QDebug>


IMoney::IMoney(int x, int y, QWidget *parent):
    IObject(x,y,parent)
{
    xSpeed=0;
    ySpeed=1;
}

void IMoney::shift()
{
    updatePosition();
    checkExist();
}

void IMoney::setClicked(bool clicked)
{
    this->clicked=clicked;
}

void IMoney::read(const QJsonObject &json)
{
    IObject::read(json);
    clicked=json["clicked"].toBool();
    updatePosition();
}

void IMoney::write(QJsonObject &json)
{
    IObject::write(json);
    json["clicked"]=clicked;
}

void IMoney::updatePosition()
{
    y+=ySpeed;
    x+=xSpeed;
    this->setGeometry(x,y,IGameData::MONEYWIDTH,IGameData::MONEYHEIGHT);
}

void IMoney::checkExist()
{
    int dX=670-x;
    int dY=50-y;
    //判断是否可以消失
    if (qAbs(dX)<20&&qAbs(dY)<20){
        this->exists=false;
        gamePanel->updateScore(this->getValue());
        return;
    }
    if (y>500){
        this->exists=false;
        return;
    }
    if (x>670&&y<70){//越界
        this->exists=false;
        gamePanel->updateScore(this->getValue());
       return;
    }
    if (clicked){
        qreal distance=qSqrt(dX*dX+dY*dY);
        xSpeed=dX*10/distance;
        ySpeed=dY*10/distance;
    }
}

IMoney::~IMoney()
{

}

IGoldCoin::IGoldCoin(int x, int y, QWidget *parent):
    IMoney(x,y,parent)
{
    value=35;
    this->setGeometry(x,y,IGameData::MONEYWIDTH,IGameData::MONEYHEIGHT);
    this->setMovie(movie);
    movie->start();
    this->show();
}

IGoldCoin::~IGoldCoin()
{
    delete movie;
}

void IGoldCoin::write(QJsonObject &json)
{
    IMoney::write(json);
    json["type"]="goldcoin";
}

QRect &IGoldCoin::getRect()
{
    return QRect(0,0,0,0);
}


ISilverCoin::ISilverCoin(int x, int y, QWidget *parent):
    IMoney(x,y,parent)
{
    value=15;
    this->setGeometry(x,y,IGameData::MONEYWIDTH,IGameData::MONEYHEIGHT);
    this->setMovie(movie);
    movie->start();
    this->show();

}

ISilverCoin::~ISilverCoin()
{
    delete movie;
}

void ISilverCoin::write(QJsonObject &json)
{
    IMoney::write(json);
    json["type"]="silvercoin";
}

QRect &ISilverCoin::getRect()
{
    return QRect(0,0,0,0);
}


IDiamond::IDiamond(int x, int y, QWidget *parent):
    IMoney(x,y,parent)
{
    value=200;

    this->setGeometry(x,y,IGameData::MONEYWIDTH,IGameData::MONEYHEIGHT);
    this->setMovie(movie);
    movie->start();
    this->show();
}

IDiamond::~IDiamond()
{
    delete movie;
}

void IDiamond::write(QJsonObject &json)
{
    IMoney::write(json);
    json["type"]="diamond";
}

QRect &IDiamond::getRect()
{
    return QRect(0,0,0,0);
}


IBox::IBox(int x, int y, QWidget *parent):
    IMoney(x,y,parent)
{
    value=2000;
    this->setGeometry(x,y,IGameData::MONEYWIDTH,IGameData::MONEYHEIGHT);
    this->setMovie(movie);
    movie->start();
    this->show();

}

IBox::~IBox()
{
    delete movie;
}

void IBox::write(QJsonObject &json)
{
    IMoney::write(json);
    json["type"]="box";
}

QRect &IBox::getRect()
{
    return QRect(0,0,0,0);
}

IPearl::IPearl(int x, int y, QWidget *parent):IMoney(x,y,parent)
{
    value=200;
    ySpeed=0;
    this->setGeometry(x,y,IGameData::MONEYWIDTH,IGameData::MONEYHEIGHT);
    this->setMovie(movie);
    movie->start();
    this->show();
    this->setClicked(true);
    this->updatePosition();
}

IPearl::~IPearl()
{
    delete movie;
}

void IPearl::write(QJsonObject &json)
{
    IMoney::write(json);
    json["type"]="pearl";
}

QRect &IPearl::getRect()
{
    return QRect(0,0,0,0);
}
