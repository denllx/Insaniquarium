#ifndef IOBJECT_H
#define IOBJECT_H
#include <QLabel>
#include <QJsonObject>

class IGameScene;

class IObject : public QLabel
{
    Q_OBJECT
public:
    explicit IObject(int x=0,int y=0,QWidget* parent = 0);
    virtual ~IObject()=0;
    int getX() const{return x;}
    int getY() const{return y;}
    void setX(int x){this->x=x;}
    void setY(int y){this->y=y;}
    int getXspeed() const{return xSpeed;}
    int getYspeed() const{return ySpeed;}
    void setXspeed(int s){this->xSpeed=s;}
    void setYspeed(int s){this->ySpeed=s;}
    virtual void shift()=0;
    virtual void eaten(){}
    bool exists=true;//是否仍然存在屏幕上
    bool iseaten=false;//是否被吃
    IGameScene* gamePanel;
    virtual void read(const QJsonObject& json);
    virtual void write(QJsonObject& json);
    virtual QRect& getRect()=0;

protected:
    int x,y;
    int xSpeed,ySpeed;
    int timeCounter=0;//计时器timeout的次数
};

#endif // IOBJECT_H
