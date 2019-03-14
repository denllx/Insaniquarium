#include "ipet.h"
#include "ifood.h"
#include "imoney.h"
#include "maindialog.h"
#include "igamedata.h"
#include <QMovie>
#include <QtMath>
#include <QDebug>

IPet::IPet(int x, int y, QWidget *parent):
    ICreature(x,y,parent)
{

}

void IPet::updatePosition(){
    timeCounter++;
    if (!(timeCounter%4)){
        x+=xSpeed;
        y+=ySpeed;
        this->setGeometry(x,y,this->width(),this->height());
    }
}


//普通的宠物只有方向更改
void IPet::updateMovie()
{
    QString direction=xSpeed<0?"left":"right";
    QString filename=QString("%1%2%3.gif")
            .arg(prefix)
            .arg("move")
            .arg(direction);
    delete movie;
    movie=new QMovie(filename);
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();
}

void IPet::read(const QJsonObject &json)
{
    ICreature::read(json);
    id=json["id"].toInt();
}

void IPet::write(QJsonObject &json)
{
    ICreature::write(json);
    json["id"]=id;
}


IPet::~IPet()
{
}

ISnail::ISnail(int x, int y, QWidget *parent):
    IPet(x,y,parent)
{
    this->setGeometry(x,y,IGameData::SNAILFISHWIDTH,IGameData::SNAILFISHHEIGHT);
    id=IGameData::SNAIL;
    ySpeed=0;
    xSpeed=1;
    previousSpeed=xSpeed;
    prefix=":/pet/resource/image/creature/pet/snail";
    movie=new QMovie(prefix+"moveright.gif");
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();
}

ISnail::~ISnail()
{
    delete movie;
}

void ISnail::shift()
{
    bool ret1=checkWall();
    bool ret2=checkMoney();
    bool directionChanged=ret1^ret2;
    if (directionChanged){
        updateMovie();
    }
    updatePosition();
}

void ISnail::alienComing()
{
    status=HIDE;
    previousSpeed=xSpeed;
    xSpeed=0;
    updateStatus();
}

void ISnail::alienDie()
{
    status=MOVE;
    xSpeed=previousSpeed;
    updateStatus();

}

void ISnail::read(const QJsonObject &json)
{
    IPet::read(json);
    previousSpeed=json["previousSpeed"].toInt();
    status=json["status"].toInt();
    updateMovie();
    updatePosition();
}

void ISnail::write(QJsonObject &json)
{
    IPet::write(json);
    json["previousSpeed"]=previousSpeed;
    json["status"]=status;
}

bool ISnail::checkStatus()
{
    //TODO
   return false;
}

bool ISnail::checkMoney()
{
    if (status==HIDE){
        return false;
    }
    //寻找水平距离最近的钱
    IGameScene* gamePanel=static_cast<IGameScene*>(this->parent());
    int mindistance=800,minId=-1,mindX;
    for (int i=0;i<gamePanel->moneys.count();i++){
        auto money=gamePanel->moneys[i];
        //足够近，可以收集
        QRect rectMoney=QRect(money->pos()+gamePanel->pos(),
                              money->size());
        QRect rectSnail=QRect(this->x+this->width()/5,
                              this->y+this->height()/2,
                              this->width()/3,
                              this->height()/6);
        if (rectMoney.intersects(rectSnail)){
            money->setClicked(true);

        }
        //在不能收集的里面找到最近的
        else{
            int dX=money->getX()-this->x;
            if (qAbs(dX)<mindistance && money->getXspeed()==0){
                minId=i;
                mindistance=qAbs(dX);
                mindX=dX;
            }
        }
    }
    if (minId>=0){//找到了
        int tempXspeed=xSpeed;
        xSpeed=mindX/10;
        if (tempXspeed*xSpeed<=0){
            return true;
        }
        return false;
    }
    return false;
}

void ISnail::updateStatus()
{
    QString strStatus=status==MOVE?"move":"retrieve";
    QString direction=previousSpeed<0?"left":"right";
    QString filename=QString("%1%2%3.gif")
            .arg(prefix)
            .arg(strStatus)
            .arg(direction);
    delete movie;
    movie=new QMovie(filename);
    this->setMovie(movie);
    movie->start();
}


ISailFish::ISailFish(int x, int y, QWidget *parent):IPet(x,y,parent)
{
    this->setGeometry(x,y,IGameData::SNAILFISHWIDTH,IGameData::SNAILFISHHEIGHT);
    id=IGameData::SAILFISH;
    ySpeed=0;
    xSpeed=-2;
    prefix=":/pet/resource/image/creature/pet/sailfish";
    movie=new QMovie(prefix+"moveleft.gif");
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();
}

ISailFish::~ISailFish()
{
    delete movie;
}

void ISailFish::shift()
{
    bool ret1=checkWall();
    hitAlien();//击打在攻击范围之内的alien
    bool ret2=checkAlien();//寻找alien并攻击
    bool directionChanged=ret1^ret2;
    if (directionChanged){
        updateMovie();
    }
    updatePosition();
}

void ISailFish::alienComing()
{
    status=SEARCHALIEN;
}

void ISailFish::alienDie()
{
    status=NORMAL;
}

void ISailFish::read(const QJsonObject &json)
{
    IPet::read(json);
    status=json["status"].toInt();
    updateMovie();
    updatePosition();
}

void ISailFish::write(QJsonObject &json)
{
    IPet::write(json);
    json["status"]=status;
}

bool ISailFish::checkStatus()
{
    return false;
}

//身边的alien都会以1秒2分的速度掉血
void ISailFish::hitAlien()
{
    if (timeCounter%50){
        return;
    }
    QRect rectFish=QRect(this->pos()+gamePanel->pos(),this->size());
    for (auto alien:gamePanel->aliens){
        QRect rectAlien=QRect(alien->pos()+gamePanel->pos(),alien->size());
        if (rectAlien.intersects(rectFish)){
            alien->hit();
            qDebug()<<"sailfish hit alien";
        }
    }
}


//找到场上最近的alien，向其奔去
bool ISailFish::checkAlien()
{
    if (status==SEARCHALIEN){
        qreal minDistance=1000,distance;
        int dX,dY;
        for (auto alien:gamePanel->aliens){
            int dx=this->x+this->width()/2-(alien->getX()+alien->width()/2);
            int dy=this->y+this->height()/2-(alien->getY()+alien->height()/2);
            distance=qSqrt(dx*dx+dy*dy);
            if (distance<minDistance){
                minDistance=distance;
                dX=-dx;
                dY=-dy;
            }
        }
        int tempxSpeed=xSpeed;
        xSpeed=dX/30;
        ySpeed=dY/30;
        if (tempxSpeed*xSpeed<=0){
            return true;
        }
        return false;
    }
    return false;
}

IZorf::IZorf(int x, int y, QWidget *parent):
    IPet(x,y,parent)
{
    this->setGeometry(x,y,IGameData::ZORFWIDTH,IGameData::ZORTHEIGHT);
    id=IGameData::ZORF;
    ySpeed=0;
    do{
        xSpeed=qrand()%11-5;
    }while(xSpeed==0);
    prefix=":/pet/resource/image/creature/pet/zorf";
    QString initialFilename=QString("%1%2%3.gif")
            .arg(prefix)
            .arg("move")
            .arg(xSpeed<0?"left":"right");
    movie=new QMovie(initialFilename);
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();
}

IZorf::~IZorf()
{
    delete movie;
}

void IZorf::shift()
{
    bool directionChanged=checkWall();
    if (directionChanged){
        updateMovie();
    }
    if (!(timeCounter%250)){
        ySpeed=qrand()%4-2;
    }
    updatePosition();
    makeFood();
}

void IZorf::read(const QJsonObject &json)
{
    IPet::read(json);
    updateMovie();
    updatePosition();
}

void IZorf::write(QJsonObject &json)
{
    IPet::write(json);
}

//吐出初级鱼食
void IZorf::makeFood()
{
    if (!(timeCounter%100)){//2秒产生一个鱼食
        int foodX=xSpeed<0?this->x+this->width()/6:this->x+this->width() *5/6;
        int tempXspeed=xSpeed;
        int tempYspeed=ySpeed;
        xSpeed=0;
        ySpeed=0;//停在原地生产食物
        QTimer* timerFood=new QTimer(this);
        timerFood->setSingleShot(true);
        connect(timerFood,&QTimer::timeout,[=](){
            gamePanel->foods.append(new IFood(1,
                                              foodX,
                                              this->y+this->height()*0.25,
                                              gamePanel));
            this->xSpeed=tempXspeed;
            this->ySpeed=tempYspeed;
        });
        timerFood->start(500);

    }
}


IFishbone::IFishbone(int x, int y, QWidget *parent)
    :IPet(x,y,parent)
{
    this->setGeometry(x,y,IGameData::FISHBONEWIDTH,IGameData::FISHBONEHEIGHT);
    id=IGameData::VERT;
    ySpeed=0;
    do{
        xSpeed=qrand()%11-5;
    }while(xSpeed==0);
    prefix=":/pet/resource/image/creature/pet/vert";
    QString initialFilename=QString("%1%2%3.gif")
            .arg(prefix)
            .arg("move")
            .arg(xSpeed<0?"left":"right");
    movie=new QMovie(initialFilename);
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();
}

IFishbone::~IFishbone()
{
    delete movie;
}

void IFishbone::shift()
{
    bool directionChanged=checkWall();
    if (directionChanged){
        updateMovie();
    }
    updatePosition();
    makeMoney();
}

void IFishbone::read(const QJsonObject &json)
{
    IPet::read(json);
    updateMovie();
    updatePosition();
}

void IFishbone::write(QJsonObject &json)
{
    IPet::write(json);
}

void IFishbone::makeMoney()
{
    if (!(timeCounter%100)){//2秒产生一个金币
        gamePanel->moneys.append(new IGoldCoin(this->x+this->width()/2-IGameData::MONEYWIDTH/2,
                                              this->y+this->height()*7/8-IGameData::COINUPPER,
                                              gamePanel));
    }
}

IClam::IClam(int x, int y, QWidget *parent)
    :IPet(x,y,parent)
{
    this->setGeometry(x,y,IGameData::CLAMWIDTH,IGameData::CLAMHEIGHT);
    id=IGameData::CLAM;
    ySpeed=0;
    xSpeed=0;
    prefix=":/pet/resource/image/creature/pet/niko";
    QString initialFilename=prefix+"static.gif";
    movie=new QMovie(initialFilename);
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();
}

IClam::~IClam()
{
    delete movie;
}

void IClam::shift()
{
    timeCounter++;
    bool statusChanged=checkStatus();
    if (statusChanged){
        updateStatus();
    }
}

//每过10秒打开一次
//打开后10秒关闭
bool IClam::checkStatus()
{
    if (!((timeCounter-100)%200)
            && (status==STATIC || status==CLOSE)){
        status=OPEN;
        return true;
    }
    if (!(timeCounter%200) && status==OPEN){
        status=CLOSE;
        return true;
    }
    return false;
}

void IClam::updateStatus()
{
    QString strStatus=status==OPEN?"open":"close";
    QString filename=QString("%1%2.gif")
            .arg(prefix)
            .arg(strStatus);
    delete movie;
    movie=new QMovie(filename);
    this->setMovie(movie);
    movie->start();
    this->show();
}

void IClam::getClosed()
{
    status=CLOSE;
    updateStatus();
}

void IClam::read(const QJsonObject &json)
{
    IPet::read(json);
    status=json["status"].toInt();
    updateStatus();
}

void IClam::write(QJsonObject &json)
{
    IPet::write(json);
    json["status"]=status;
}
