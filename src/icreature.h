#ifndef ICREATURE_H
#define ICREATURE_H
#include "iobject.h"

class ICreature:public IObject{
    Q_OBJECT

public:
    explicit ICreature(int x,int y,QWidget* parent=0);
    virtual ~ICreature()=0;
    virtual void shift()=0;
    virtual bool checkStatus()=0;
    bool checkWall(bool enabled=true);//检查碰壁(只对活物)
    virtual void alienComing()=0;
    virtual void alienDie()=0;
    void read(const QJsonObject &json) override;
    void write(QJsonObject &json) override;
protected:
    int minEatingDistanceX=20;//能吃到东西的最小距离
    int minEatingDistanceY=20;
    bool dieLeft;//死的时候朝左边还是右边
};

#endif // ICREATURE_H
