#ifndef IFISH_H
#define IFISH_H
#include "icreature.h"
#include <QMovie>
#include <QDebug>
#include <QtMath>
#include <QJsonObject>
#include <QTimer>

class IGameScene;

class IFish:public ICreature{
    Q_OBJECT

public:
    explicit IFish(int x=0,int y=0,QWidget* parent=0);
    virtual ~IFish()=0;
    virtual void shift()=0;
    int getPrice() const{return price;}
    bool isDie(){return status==Status::Die;}
    void alienComing();
    void alienDie();
    void updateMovie();//有改变才执行
    void eaten()override;
    void read(const QJsonObject &json) override;
    void write(QJsonObject& json) override;
protected:
    bool checkStatus();
    virtual bool checkSizeChange()=0;
    template<typename T>
    bool IFish::checkFood(QList<T*>& foodList)
    {
        //不需要吃食物：死了，很饱，没有食物
        if (this->status==Status::Die){
            return false;
        }
        if (starvingDuration<=40*starvingTime){//即将进入饥饿:40starving time
            return false;
        }
        if (foodList.isEmpty()){
            return false;
        }
        //周围有可以吃的食物
        int canEatId=-1;
        for (int i=0;i<foodList.count();i++){
            IObject* tempFood=foodList[i];
            if (tempFood->iseaten){
                continue;
            }
            QRect rectFood=QRect(gamePanel->pos()+tempFood->pos()
                                 +QPoint(this->width()/4,this->height()/4),
                                tempFood->size()*2/3);
            QRect rectFish=QRect(this->pos()+gamePanel->pos()+QPoint(this->width()/4,this->height()/4),
            this->size()*2/3);
            if (rectFood.intersects(rectFish)){
                canEatId=i;
                break;
            }
        }
        if (canEatId>=0)//找到了
        {
            action=Action::EAT;
            updateEat();
            int tempXspeed=xSpeed;
            int tempYspeed=ySpeed;
            xSpeed=0;
            ySpeed=0;//停在原地吃东西
            IObject* food=(foodList)[canEatId];
            if (!(food->iseaten)){
                food->eaten();
            }
            foodCount++;
            this->starvingDuration=0;
            QTimer* timerEat=new QTimer(this);
            timerEat->setSingleShot(true);
            connect(timerEat,&QTimer::timeout,[=](){
                if (tempXspeed<0){
                    do{
                        xSpeed=qrand()%5-5;
                    }while(xSpeed==0);
                }
                else{
                    do{
                        xSpeed=5-qrand()%5;
                    }while(xSpeed==0);
                }
                if (tempYspeed<0){
                    do{
                        ySpeed=qrand()%5-5;
                    }while(ySpeed==0);
                }
                else{
                    do{
                        ySpeed=5-qrand()%5;
                    }while(ySpeed==0);
                }
            });
            timerEat->start(1000);
            return false;
        }
        else{
        //根据自己和最近的食物的距离调整水平和垂直速度
            qreal minDistance,minId;
            minDistance=1000;
            for (int i=0;i<foodList.count();i++){
                IObject* tempFood=foodList[i];
                qreal distance=qSqrt((x-tempFood->getX())*(x-tempFood->getX())+(y-tempFood->getY())*(y-tempFood->getY()));
                if (minDistance>distance){
                    minDistance=distance;
                    minId=i;
                }
            }
            IObject* food=foodList[minId];
            int tempXspeed=xSpeed;
            int tempYspeed=ySpeed;
            int dX=food->getX()-x;
            int dY=food->getY()-y;
            if (dX>0){
                xSpeed=qMin(int(dX*starvingDuration/(minDistance*14)),dX);
            }
            else {
                xSpeed=qMax(int(dX*starvingDuration/(minDistance*14)),dX);
            }
            if (dY>0){
                ySpeed=qMin(int(dY*starvingDuration/(minDistance*14)),dY);//不能一步跨过食物
            }
            else{
                ySpeed=qMax(int(dY*starvingDuration/(minDistance*14)),dY);//不能一步跨过食物
            }

            if (ySpeed==0){
                    ySpeed=(tempYspeed>0)?1:-1;
            }
            if (xSpeed==0){
                xSpeed=(tempXspeed>0)?1:-1;
            }
            return xSpeed*tempXspeed<0;
        }
    }
    void updateEat();//在吃到食物后立刻执行
    void updatePosition();
    virtual void makeMoney()=0;

    int starvingTime,deadTime;
    int starvingDuration=0;//已经饿了多久
    int foodCount=0;
    int price;
    bool hasAlien=false;
    QString resourceDir;
    QString prefix;
    enum Status{Normal=0,Starving,Die};
    Status status=Status::Normal;
    enum Action{SWIM=3,EAT};
    Action action=Action::SWIM;
    QMovie* swim=nullptr;
    QMovie* turn=nullptr;
    QMovie* blood;
    QLabel* labelBlood;
};

class GuppyFish:public IFish{
    Q_OBJECT
public:
    explicit GuppyFish(int foodClass,int x,int y,QWidget* parent=0);
    ~GuppyFish();
    void shift() override;
    void setStarvingTime(int t);
    void setDeadTime(int t);
    void setFoodClass(int c);
    int getSize() const{return size;}
    void read(const QJsonObject& json);
    void write(QJsonObject &json);
    QRect& getRect();
private:
    const int SMALL=0;
    const int MIDDLE=1;
    const int LARGE=2;
    const int KING=3;
    const int FOODFORMIDDLE=1;
    const int FOODFORLARGE=2;
    const int MONEYPRODUCTINGTIMEFORMIDDLE=250;//中5秒下一个金币
    const int MONEYPRODUCTINGTIMEFORSMALL=250;//小5秒下一个银币
    const int MONEYPRODUCTINGTIMEFORBIG=150;//大3秒一个金币
    const int MONEYPRODUCTINGTIMEFORKING=250;//王5秒1个钻石

    int size=SMALL;//0小，1中，2王，3大
    int foodClass;//食物的等级
    bool checkSizeChange() override;
    void makeMoney();
};

class GuppyMommy:public IFish{
    Q_OBJECT
public:
    explicit GuppyMommy(int foodClass,int x, int y, QWidget* parent=0);
    ~GuppyMommy();
    void shift() override;
    void setStarvingTime(int t);
    void setDeadTime(int t);
    void setFoodClass(int c);
    void read(const QJsonObject& json);
    void write(QJsonObject& json);
    QRect& getRect() override;

protected:
    void makeMoney();
private:
    const int MEDIUM=0;
    const int LARGE=1;
    const int FOODFORLARGE=2;
    const int BABYPRODUCTINGTIMEFORMIDDLE=750;//中等古比妈咪15秒一个baby
    const int BABYPRODUCTINGTIMEFORLARGE=375;//大型
    int size=MEDIUM;
    int foodClass;//食物的等级
    bool checkSizeChange() override;
    void makeBaby();
};

class Carnivore:public IFish{
    Q_OBJECT
public:
    explicit Carnivore(int x,int y,QWidget* parent=0);
    ~Carnivore();
    void shift() override;
    void write(QJsonObject& json);
    void read(const QJsonObject& json);
    QRect& getRect() override;
    static int const getPrice(){return PRICE;}
protected:
    void makeMoney();
private:
    int MONEYPRODUCTINGTIME=250;//5秒1个钻石
    static int const PRICE=1000;
    bool checkSizeChange(){return false;}
};

class SimCarnivore:public IFish{
    Q_OBJECT
public:
    explicit SimCarnivore(int x,int y,QWidget* parent=0);
    ~SimCarnivore();
    void shift() override;
    static int const getPrice(){return PRICE;}
    void read(const QJsonObject& json);
    void write(QJsonObject& json);
    QRect& getRect() override;
protected:
    void makeMoney();
private:
    int MONEYPRODUCTINGTIME=500;//10秒一个宝箱
    static int const PRICE=10000;
    bool checkSizeChange(){return false;}
};

#endif // IFISH_H
