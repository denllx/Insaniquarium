#ifndef IFOOD_H
#define IFOOD_H
#include "iobject.h"
#include <QJsonObject>

class IFood:public IObject{
    Q_OBJECT

public:
    explicit IFood(int level,int x,int y,QWidget* parent=0);
    ~IFood();
    virtual void shift() override;
    static const int getPrice();
    static const int classToPrice(int c);
    static const int classToStarving(int c);
    static const int classToDead(int c);
    static const int addingPrice(){return ADDINGPRICE;}
    int getLevel() const{return level;}
    void eaten(){iseaten=true;exists=false;}
    void updateMovie();
    void read(const QJsonObject& json);
    void write(QJsonObject& json);
    QRect& getRect() override;

private:
    QMovie* movie;
    void judgeStatus();
    static const int PRICE=5;
    static const int PRICE1=0;
    static const int PRICE2=200;
    static const int PRICE3=300;
    static const int STARVING1=8;
    static const int DEAD1=10;//每提高一个等级，时间延长1.5
    static const int ADDINGPRICE=300;
    int level;
};


#endif // IFOOD_H
