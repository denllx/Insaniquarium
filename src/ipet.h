#ifndef IPET_H
#define IPET_H
#include "icreature.h"
#include <QMovie>
#include <QJsonObject>

class IPet:public ICreature{
    Q_OBJECT

public:
    explicit IPet(int x,int y,QWidget* parent=0);
    virtual ~IPet()=0;
    void updatePosition();
    virtual void updateMovie();
    int id;
    void read(const QJsonObject &json) override;
    void write(QJsonObject &json) override;
    QRect& getRect(){return QRect(0,0,0,0);}
protected:
    QMovie* movie;
    QString prefix;

};
//蜗牛NIKO
class ISnail:public IPet{
    Q_OBJECT

public:
    explicit ISnail(int x,int y,QWidget* parent=0);
    ~ISnail();
    void shift() override;
    void alienComing();
    void alienDie();
    void read(const QJsonObject& json);
    void write(QJsonObject& json);

private:
    const int MOVE=0;
    const int HIDE=1;
    int status=MOVE;
    int previousSpeed;//缩起来之前的速度
    bool checkStatus();
    bool checkMoney();
    void updateStatus();
};

//旗鱼itchy攻击敌人
class ISailFish:public IPet{
    Q_OBJECT

public:
    explicit ISailFish(int x,int y,QWidget* parent=0);
    ~ISailFish();
    void shift() override;
    void alienComing();
    void alienDie();
    void read(const QJsonObject& json);
    void write(QJsonObject& json);

private:
    const int NORMAL=0;
    const int SEARCHALIEN=1;//发现alien并追杀
    int status=NORMAL;
    bool checkStatus();
    void hitAlien();
    void updateStatus(){}
    bool checkAlien();

};

//海马能吐出初级鱼食
class IZorf:public IPet{
    Q_OBJECT

public:
    explicit IZorf(int x,int y,QWidget* parent=0);
    ~IZorf();
    void shift() override;
    void alienComing(){}
    void alienDie(){}
    void read(const QJsonObject& json);
    void write(QJsonObject& json);

private:
    bool checkStatus(){return false;}
    void updateStatus(){}
    void makeFood();
};

//鱼骨头能产生金币
class IFishbone:public IPet{
    Q_OBJECT

public:
    explicit IFishbone(int x,int y,QWidget* parent=0);
    ~IFishbone();
    void shift() override;
    void alienComing(){}
    void alienDie(){}
    void read(const QJsonObject &json) override;
    void write(QJsonObject &json) override;

private:
    bool checkStatus(){return false;}
    void updateStatus(){}
    void makeMoney();
};

//河蚌产生珍珠，一段时间不取合上
class IClam:public IPet{
    Q_OBJECT

public:
    explicit IClam(int x,int y,QWidget* parent=0);
    ~IClam();
    void shift() override;
    void alienComing(){}
    void alienDie(){}
    void getClosed();//被用户点击强行关闭
    bool isOpen(){return status==OPEN;}
    void read(const QJsonObject &json) override;
    void write(QJsonObject &json) override;

private:
    //三种状态：闭合不动，打开，关上
    const int STATIC=0;
    const int OPEN=1;
    const int CLOSE=2;
    int status=STATIC;
    bool checkStatus();
    void updateStatus();
};


#endif // IPET_H
