#ifndef IALIEN_H
#define IALIEN_H
#include "icreature.h"
#include <QMovie>

class IAlien:public ICreature{
    Q_OBJECT

public:
    explicit IAlien(int x,int y,QWidget* parent=0);
    virtual ~IAlien()=0;
    virtual void shift()=0;
    bool checkStatus(){return false;}
    void updateMovie();
    void hit();
    void turnRight();
    void turnLeft();
    void turnDown();
    void turnUp();
    void alienComing(){}
    void alienDie(){}
    void read(const QJsonObject &json) override;
    void write(QJsonObject &json) override;
    int id;
    bool acceptClickHit;//是否能被鼠标点击攻击


protected:
    void updatePosition();
    void checkAlive();
    int life=100;//声明值
    int hitPoint;//打击一次掉血值，由武器种类和宠物数量决定
    QString prefix;
    enum Action{SWIM=0,OTHER=1};
    Action action=Action::SWIM;
    QMovie* movie;

    template<typename T>
    void killCreatureByGroup(QList<T*> creatureList){
        for (auto c:creatureList){
            ICreature* creature=static_cast<ICreature*>(c);
            QRect rectCreature=QRect(creature->getX()+creature->width()/4,
                                     creature->getY()+creature->height()/4,
                                     creature->width()/2,
                                     creature->height()/2);
            QRect rect=QRect(x+this->width()/4,
                             y+this->height()/4,
                             this->width()/2,
                             this->height()/2);
            if (rectCreature.intersects(rect)
                    &&(rectCreature.center().x()-rect.center().x())*this->xSpeed>=0){
                qDebug()<<rectCreature<<rect;
                IObject* c=static_cast<IObject*>(creature);
                if (!(c->iseaten)){
                    c->eaten();
                }
            }
        }
    }
    void killCreature();
};

class ISylvester:public IAlien{
    Q_OBJECT

public:
    explicit ISylvester(int x,int y,QWidget* parent=0);
    ~ISylvester();
    void shift() override;
    void read(const QJsonObject &json) override;
    void write(QJsonObject &json) override;
    QRect& getRect() override;
};

class IBalrog:public IAlien{
    Q_OBJECT

public:
    explicit IBalrog(int x,int y,QWidget* parent=0);
    ~IBalrog();
    void shift() override;
    void read(const QJsonObject &json) override;
    void write(QJsonObject &json) override;
    QRect& getRect() override;
};

class IGus:public IAlien{
    Q_OBJECT

public:
    explicit IGus(int x,int y,QWidget* parent=0);
    ~IGus();
    void shift() override;
    void read(const QJsonObject &json) override;
    void write(QJsonObject &json) override;
    QRect& getRect() override;

private:
    bool checkFood();
    void updateEat();
};

#endif // IALIEN_H
