#ifndef IMONEY_H
#define IMONEY_H
#include "iobject.h"
#include <QJsonObject>
#include <QMovie>
#include <QTimer>

class IMoney:public IObject{
    Q_OBJECT

public:
    explicit IMoney(int x,int y,QWidget* parent=0);
    virtual ~IMoney()=0;
    void shift();
    void setClicked(bool clicked);
    void read(const QJsonObject& json);
    virtual void write(QJsonObject& json);

protected:
    int value;
    bool clicked=false;
    virtual int getValue()=0;
    void updatePosition();//更新坐标，判断是否到终点，加钱
    void checkExist();

};

class IGoldCoin:public IMoney{
    Q_OBJECT

public:
    explicit IGoldCoin(int x,int y,QWidget* parent=0);
    ~IGoldCoin();
    void write(QJsonObject &json) override;
    QRect& getRect() override;

protected:
    int getValue(){return this->value;}
private:
    QMovie* movie=new QMovie(":/money/resource/image/money/gold.gif");
};

class ISilverCoin:public IMoney{
    Q_OBJECT

public:
    explicit ISilverCoin(int x,int y,QWidget* parent=0);
    ~ISilverCoin();
    void write(QJsonObject &json) override;
    QRect& getRect() override;

protected:
    int getValue(){return this->value;}
private:
    QMovie* movie=new QMovie(":/money/resource/image/money/silver.gif");
};

class IDiamond:public IMoney{
    Q_OBJECT

public:
    explicit IDiamond(int x,int y,QWidget* parent=0);
    ~IDiamond();
    void write(QJsonObject &json) override;
    QRect& getRect() override;

protected:
    int getValue(){return this->value;}
private:
    QMovie* movie=new QMovie(":/money/resource/image/money/diamond.gif");
};

class IBox:public IMoney{
    Q_OBJECT

public:
    explicit IBox(int x,int y,QWidget* parent=0);
    ~IBox();
    void write(QJsonObject &json) override;
    QRect& getRect();

protected:
    int getValue(){return this->value;}
private:
    QMovie* movie=new QMovie(":/money/resource/image/money/box.gif");

};

class IPearl:public IMoney{
    Q_OBJECT

public:
    explicit IPearl(int x,int y,QWidget* parent=0);
    ~IPearl();
    void write(QJsonObject &json) override;
    QRect& getRect() override;

protected:
    int getValue(){return this->value;}
private:
    QMovie* movie=new QMovie(":/money/resource/image/money/pearl.gif");

};

#endif // IMONEY_H
