#include "ifish.h"
#include "iscene.h"
#include "maindialog.h"
#include "igamedata.h"
#include "imoney.h"
#include <QDebug>
#include <QPainter>
#include <QtMath>

IFish::IFish(int x,int y,QWidget* parent):ICreature(x,y,parent)
{
    IGameScene* scene=static_cast<IGameScene*>(parent);
    MainDialog* dialog=static_cast<MainDialog*>(scene->dialog);
    resourceDir=dialog->resourceDir;
    //随机初始化xSpeed:-5~5
    do{
    xSpeed=qrand()%11-5;
    }while(xSpeed==0);
}

void IFish::alienComing()
{
    hasAlien=true;
}

void IFish::alienDie()
{
    hasAlien=false;
}

void IFish::eaten()
{
    if (status==Status::Die){
        return;
    }
    //播放血雾
    this->iseaten=true;
    blood=new QMovie(":/background/resource/image/background/blood.gif");
    labelBlood=new QLabel(this);
    labelBlood->setGeometry(0,0,this->width(),this->height());
    labelBlood->setMovie(blood);
    labelBlood->setScaledContents(true);
    blood->start();
    labelBlood->show();
    connect(blood,&QMovie::finished,[=](){
        this->exists=false;
    });
}

void IFish::read(const QJsonObject &json)
{
    ICreature::read(json);
    starvingDuration=json["starvingDuration"].toInt();
    starvingTime=json["starvingTime"].toInt();
    deadTime=json["deadTime"].toInt();
    foodCount=json["foodCount"].toInt();
    hasAlien=json["hasAlien"].toBool();
    prefix=json["prefix"].toString();
    status=Status(json["status"].toInt());
    action=Action(json["action"].toInt());
    iseaten=json["iseaten"].toBool();
}

void IFish::write(QJsonObject &json)
{
    ICreature::write(json);
    json["starvingDuration"]=starvingDuration;
    json["starvingTime"]=starvingTime;
    json["deadTime"]=deadTime;
    json["foodCount"]=foodCount;
    json["hasAlien"]=false;
    json["prefix"]=prefix;
    json["status"]=status;
    json["action"]=action;
    json["iseaten"]=iseaten;
}

bool IFish::checkStatus()
{
    if (this->status==Status::Die && this->getY()>=500){//从死变成触底消失
        this->exists=false;
        return true;
    }
    else if (this->status==Status::Starving && starvingDuration>=50* deadTime){//从饿变成死
        this->status=Status::Die;
        dieLeft=xSpeed>0?false:true;
        this->xSpeed=0;//上下游动
        this->ySpeed=2;//向下飘落
        return true;
    }
    else if (this->status==Status::Normal && starvingDuration>=50*starvingTime){//从正常变成饿
        this->status=Status::Starving;
        return true;
    }
    else if (this->status==Status::Starving&&starvingDuration<50*starvingTime){//吃了食物恢复正常
        this->status=Status::Normal;
        return true;
    }
    return false;
}

void IFish::updatePosition()
{
    timeCounter++;
    if (!hasAlien){
        starvingDuration++;
    }
    if (!(timeCounter%4)){
        x+=xSpeed;
        y+=ySpeed;
        this->setGeometry(x,y,this->width(),this->height());
    }
}

void IFish::updateMovie()
{
    if (action==Action::EAT){
        return;//进食动画不受其他动画打断
    }
    QString strDirection="";
    if (status==Status::Die){
            strDirection= dieLeft?QString("left"):QString("right");
    }
    else{
        strDirection=xSpeed<0?QString("left"):QString("right");
    }
    QString strStatus=(status==Status::Normal)?QString("normal"):(status==Status::Die?QString("die"):QString("hungry"));
    QString strAction="swim";
    //left,right
    QString filename=QString("%1%2%3%4.gif")
            .arg(prefix)
            .arg(strAction)
            .arg(strStatus)
            .arg(strDirection);
    qDebug()<<filename;
    swim->deleteLater();
    swim=new QMovie(filename);
    this->setMovie(swim);
    swim->start();
}

void IFish::updateEat(){
    QString strDirection=xSpeed<0?"left":"right";
    QString strStatus=status==Status::Normal?"normal":(status==Status::Starving?"hungry":"die");
    QString strAction="eat";
    QString filename=QString("%1%2%3%4.gif")
            .arg(prefix)
            .arg(strAction)
            .arg(strStatus)
            .arg(strDirection);
    swim->deleteLater();
    swim=new QMovie(filename);
    this->setMovie(swim);
    swim->start();
    connect(swim,&QMovie::finished,[=](){
        this->action=Action::SWIM;
        updateMovie();
    });
}

IFish::~IFish()
{

}


GuppyFish::GuppyFish(int foodClass,int x,int y,QWidget* parent):IFish(x,y,parent)
{
    this->setGeometry(this->x,this->y,IGameData::GUPPYFISHWIDTH,IGameData::GUPPYFISHHEIGHT);
    prefix=":/smallguppy/resource/image/creature/fish/smallguppy/";

    QString initialFilename;
    initialFilename=QString("%1%2%3.gif")
            .arg(prefix)
            .arg("swimnormal")
            .arg(xSpeed<0?"left":"right");
    swim=new QMovie(initialFilename);
    this->setMovie(swim);
    swim->start();
    this->setScaledContents(true);
    this->show();

    this->foodClass=foodClass;
    ySpeed=0;
    starvingTime=IFood::classToStarving(this->foodClass);
    deadTime=IFood::classToDead(this->foodClass);
    status=Status::Normal;
    price=100;

}

GuppyFish::~GuppyFish()
{
    swim->deleteLater();
}

void GuppyFish::shift()
{
    if (this->iseaten){
        return;
    }
    bool ret1=checkFood(gamePanel->foods);//检查是否有食物，修改速度,如果方向改变返回true
    bool ret2=checkWall(this->status==Status::Normal||this->status==Status::Starving);//检查是否碰壁，修改速度，如果x方向改变返回true
    bool xspeedChanged=ret1^ret2;
    bool statusChanged=checkStatus();//检查饥饿时间是否达到阈值，修改状态
    bool sizeChanged=false;
    sizeChanged=checkSizeChange();
    if (xspeedChanged||sizeChanged||statusChanged){
        updateMovie();
    }
    updatePosition();
    makeMoney();
}

void GuppyFish::setStarvingTime(int t)
{
    starvingTime=t;
}

void GuppyFish::setDeadTime(int t)
{
    deadTime=t;
}

void GuppyFish::setFoodClass(int c)
{
    foodClass=c;
}

void GuppyFish::read(const QJsonObject &json)
{
    IFish::read(json);
    size=json["size"].toInt();
    foodClass=json["foodClass"].toInt();
    updateMovie();
    updatePosition();
}

void GuppyFish::write(QJsonObject &json)
{
    IFish::write(json);
    json["size"]=size;
    json["foodClass"]=foodClass;
}

QRect &GuppyFish::getRect()
{
    switch (this->size) {
    case 0:
        return QRect(x+IGameData::GUPPYFISHLEFT,
                     y+IGameData::GUPPYFISHUPPER,
                     IGameData::GUPPYFISHWIDTH-2*IGameData::GUPPYFISHLEFT,
                     IGameData::GUPPYFISHHEIGHT-2*IGameData::GUPPYFISHUPPER );
    case 1:
        return QRect(x+IGameData::MEDIUMGUPPYLEFT,
                     y+IGameData::MEDIUMGUPPYUPPER,
                     IGameData::GUPPYFISHWIDTH-2*IGameData::MEDIUMGUPPYLEFT,
                     IGameData::GUPPYFISHHEIGHT-2*IGameData::MEDIUMGUPPYUPPER);
    case 2:
    case 3:
        return QRect(x,
                     y+IGameData::BIGGUPPYUPPER,
                     IGameData::GUPPYFISHWIDTH,
                     IGameData::GUPPYFISHHEIGHT-2*IGameData::BIGGUPPYUPPER);
    default:
        break;
    }
}

bool GuppyFish::checkSizeChange()
{
    if (size==SMALL&&foodCount>=FOODFORMIDDLE){

        this->size=MIDDLE;
        prefix=":/mediumguppy/resource/image/creature/fish/mediumguppy/";
        return true;
    }
    else if (size<LARGE && foodCount>=FOODFORLARGE){
        this->size=LARGE;
        prefix=":/bigguppy/resource/image/creature/fish/bigguppy/";
        return true;
    }
    else if (size==LARGE){
        int ret=qrand()%10000;
        if (ret==0){
            this->size=KING;
            prefix=":/kingguppy/resource/image/creature/fish/kingguppy/";
            return true;
        }
    }
    return false;
}

void GuppyFish::makeMoney()
{
    if (this->status==Status::Normal){
        switch (size) {
        case 0:
            if (!(timeCounter%MONEYPRODUCTINGTIMEFORSMALL)){
                gamePanel->moneys.append(new ISilverCoin(this->x+this->width()/2-IGameData::MONEYWIDTH/2,
                                                         y+this->height()-IGameData::GUPPYFISHUPPER-IGameData::COINUPPER,
                                                         gamePanel));
            }
            break;
        case 1:
            if (!(timeCounter%MONEYPRODUCTINGTIMEFORMIDDLE)){
                gamePanel->moneys.append(new IGoldCoin(x+this->width()/2-IGameData::MONEYWIDTH/2,
                                                       y+this->height()-IGameData::MEDIUMGUPPYUPPER-IGameData::COINUPPER ,
                                                       gamePanel));
            }
            break;
        case 2:
            if (!(timeCounter%MONEYPRODUCTINGTIMEFORBIG)){
                gamePanel->moneys.append(new IGoldCoin(x+this->width()/2-IGameData::MONEYWIDTH/2,
                                                       y+this->height()-IGameData::BIGGUPPYUPPER-IGameData::COINUPPER ,
                                                       gamePanel));
            }
            break;
        case 3:
            if (!(timeCounter%MONEYPRODUCTINGTIMEFORKING)){
                gamePanel->moneys.append(new IDiamond(x+this->width()/2-IGameData::MONEYWIDTH/2,
                                                      y+this->height()-IGameData::BIGGUPPYUPPER-IGameData::COINUPPER ,
                                                      gamePanel));
            }
            break;
        default:
            break;
        }
    }
}

GuppyMommy::GuppyMommy(int foodClass, int x, int y, QWidget *parent):
    IFish(x,y,parent)
{
    this->setGeometry(this->x,this->y,IGameData::GUPPYMOMMYWIDTH,IGameData::GUPPYMOMMYHEIGHT);
    prefix=":/guppymommy/resource/image/creature/fish/guppymommy/middle/";
    QString initialFilename;
    initialFilename=QString("%1%2%3.gif")
            .arg(prefix)
            .arg("swimnormal")
            .arg(xSpeed<0?"left":"right");
    swim=new QMovie(initialFilename);
    this->setMovie(swim);
    swim->start();
    this->setScaledContents(true);
    this->show();

    this->foodClass=foodClass;
    starvingTime=IFood::classToStarving(this->foodClass)*2;
    deadTime=IFood::classToDead(this->foodClass)*2;
    status=Status::Normal;
    ySpeed=0;
    price=200;
}

void GuppyMommy::setStarvingTime(int t)
{
    starvingTime=2*t;//妈咪比一般的鱼更能挨饿
}

void GuppyMommy::setDeadTime(int t)
{
    deadTime=2*t;
}

void GuppyMommy::setFoodClass(int c)
{
    foodClass=c;
}

void GuppyMommy::read(const QJsonObject &json)
{
    IFish::read(json);
    size=json["size"].toInt();
    foodClass=json["foodClass"].toInt();
    updateMovie();
    updatePosition();
}

void GuppyMommy::write(QJsonObject &json)
{
    IFish::write(json);
    json["size"]=size;
    json["foodClass"]=foodClass;
}

QRect &GuppyMommy::getRect()
{
    switch (this->size) {
    case 0:
        return QRect(x+IGameData::GUPPYMOMMYLEFT,
                     y+IGameData::GUPPYMOMMYUPPER,
                     IGameData::GUPPYMOMMYWIDTH-2*IGameData::GUPPYMOMMYLEFT,
                     IGameData::GUPPYMOMMYHEIGHT-IGameData::GUPPYMOMMYUPPER-IGameData::GUPPYMOMMYBOTTOM);
    case 1:
        return QRect(x+IGameData::BIGGUPPYMOMMYLEFT,
                     y+IGameData::BIGGUPPYMOMMYUPPER,
                     IGameData::GUPPYMOMMYWIDTH-2*IGameData::BIGGUPPYMOMMYLEFT,
                     IGameData::GUPPYMOMMYHEIGHT-IGameData::BIGGUPPYMOMMYBOTTOM-IGameData::BIGGUPPYMOMMYUPPER);

    default:
        break;
    }
}

void GuppyMommy::makeMoney()
{

}

GuppyMommy::~GuppyMommy()
{
    swim->deleteLater();
}

void GuppyMommy::shift()
{
    if (this->iseaten){
        return;
    }
    bool xspeedChanged=false;
    bool ret1=checkFood(gamePanel->foods);//检查是否有食物，修改速度,如果方向改变返回true
    bool ret2=checkWall(this->status==Status::Normal||this->status==Status::Starving);//检查是否碰壁，修改速度，如果x方向改变返回true
    xspeedChanged=ret1^ret2;
    bool statusChanged=checkStatus();//检查饥饿时间是否达到阈值，修改状态
    bool sizeChanged=false;
    sizeChanged=checkSizeChange();
    if (xspeedChanged||statusChanged||sizeChanged){
        updateMovie();
    }
    updatePosition();
    makeBaby();
}

bool GuppyMommy::checkSizeChange()
{
    if (size==MEDIUM&&foodCount>=FOODFORLARGE){
        this->size=LARGE;
        prefix=":/guppymommy/resource/image/creature/fish/guppymommy/big/";
        return true;
    }
    return false;
}

void GuppyMommy::makeBaby()
{
    if (this->status==Status::Normal){
        int intervel=0;
        switch (size) {
        case 0:
            intervel=BABYPRODUCTINGTIMEFORMIDDLE;
            break;
        case 1:
            intervel=BABYPRODUCTINGTIMEFORLARGE;
            break;
        default:
            break;
        }
    if (!(timeCounter%intervel)){
        int tempXspeed=xSpeed;
        int tempYspeed=ySpeed;
        xSpeed=0;
        ySpeed=0;//停在原地生孩子
        QTimer* timerProduct=new QTimer(this);
        timerProduct->setSingleShot(true);
        connect(timerProduct,&QTimer::timeout,[=](){
            int fishX,fishY;
            int marginBottom=size==MEDIUM?IGameData::GUPPYMOMMYBOTTOM:IGameData::BIGGUPPYMOMMYBOTTOM;
            int marginLeft=size==MEDIUM?IGameData::GUPPYMOMMYLEFT:IGameData::BIGGUPPYMOMMYLEFT;
            fishY=y+IGameData::GUPPYMOMMYHEIGHT-IGameData::GUPPYFISHUPPER-marginBottom;
            if (tempXspeed<0){
                fishX=x+IGameData::GUPPYMOMMYWIDTH-marginLeft-IGameData::GUPPYFISHUPPER-10;
            }
            else{
                fishX=x+marginLeft+IGameData::GUPPYFISHLEFT-IGameData::GUPPYFISHWIDTH+10;
            }
            GuppyFish* newFish=new GuppyFish(1,fishX,fishY,gamePanel);
            newFish->setXspeed(tempXspeed<0?1:-1);
            newFish->updateMovie();
            if (hasAlien){
                newFish->alienComing();
            }
            gamePanel->guppyFish.append(newFish);
            xSpeed=tempXspeed/2;
            if (xSpeed==0){
                xSpeed=tempXspeed>0?1:-1;
            }
            ySpeed=tempYspeed/2;
            if (ySpeed==0){
                ySpeed=tempYspeed>0?1:-1;
            }
        });
        timerProduct->start(1000);
    }
    }
}

Carnivore::Carnivore(int x, int y, QWidget *parent):
    IFish(x,y,parent)
{
    this->setGeometry(this->x,this->y,IGameData::CARNIVOREWIDTH,IGameData::CARNIVOREHEIGHT);
    prefix=":/carnivore/resource/image/creature/fish/carnivore/";
    QString initialFilename;
    initialFilename=QString("%1%2%3.gif")
            .arg(prefix)
            .arg("swimnormal")
            .arg(xSpeed<0?"left":"right");
    swim=new QMovie(initialFilename);
    this->setMovie(swim);
    swim->start();
    this->setScaledContents(true);
    this->show();

    starvingTime=16;//16
    deadTime=20;//20
    status=Status::Normal;
    ySpeed=0;
}

Carnivore::~Carnivore()
{
    swim->deleteLater();
}

void Carnivore::shift()
{
    if (this->iseaten){
        return;
    }
    bool xspeedChanged=false;
    //只能吃小鱼
    QList<GuppyFish*> eatable;
    for (auto fish:gamePanel->guppyFish){
         if (fish->getSize()==0){
             eatable.append(fish);
         }
    }
    bool ret1=checkFood(eatable);//检查是否有食物，修改速度,如果方向改变返回true
    bool ret2=checkWall(this->status==Status::Normal||this->status==Status::Starving);//检查是否碰壁，修改速度，如果x方向改变返回true
    xspeedChanged=ret1^ret2;
    bool statusChanged=checkStatus();//检查饥饿时间是否达到阈值，修改状态
    bool sizeChanged=false;
    if (xspeedChanged||statusChanged||sizeChanged){

        updateMovie();
    }
    updatePosition();
    makeMoney();
}

void Carnivore::write(QJsonObject &json)
{
    IFish::write(json);
}

void Carnivore::read(const QJsonObject &json)
{
    IFish::read(json);
    updateMovie();
    updatePosition();
}

QRect &Carnivore::getRect()
{
    return QRect(x,
                 y+IGameData::CARNIVOREUPPER,
                 IGameData::CARNIVOREWIDTH,
                 IGameData::CARNIVOREHEIGHT-IGameData::CARNIVOREBOTTOM-IGameData::CARNIVOREUPPER);
}

void Carnivore::makeMoney()
{
    if (this->status==Status::Normal){
        if (!(timeCounter%MONEYPRODUCTINGTIME)){
            gamePanel->moneys.append(new IDiamond(x+this->width()/2-IGameData::MONEYWIDTH/2,
                                                  y+this->height()/2-IGameData::DIAMONDUPPER ,
                                                  gamePanel));
        }
    }
}


SimCarnivore::SimCarnivore(int x, int y, QWidget *parent):
    IFish(x,y,parent)
{
    this->setGeometry(this->x,this->y,IGameData::SIMCARNIVOREWIDTH,IGameData::SIMCARNIVOREHEIGHT);
    prefix=":/simcarnivore/resource/image/creature/fish/bigcarnivore/";
    QString initialFilename;
    initialFilename=QString("%1%2%3.gif")
            .arg(prefix)
            .arg("swimnormal")
            .arg(xSpeed<0?"left":"right");
    swim=new QMovie(initialFilename);
    this->setMovie(swim);
    swim->start();
    this->setScaledContents(true);
    this->show();

    starvingTime=16;
    deadTime=20;
    status=Status::Normal;
    ySpeed=0;

}

SimCarnivore::~SimCarnivore()
{
    swim->deleteLater();
}

void SimCarnivore::shift()
{
    if (this->iseaten){
        return;
    }
    bool xspeedChanged=false;
    bool ret1=checkFood(gamePanel->carnivore);//检查是否有食物，修改速度,如果方向改变返回true
    bool ret2=checkWall(this->status==Status::Normal||this->status==Status::Starving);//检查是否碰壁，修改速度，如果x方向改变返回true
    xspeedChanged=ret1^ret2;
    bool statusChanged=checkStatus();//检查饥饿时间是否达到阈值，修改状态
    bool sizeChanged=false;
    if (xspeedChanged||statusChanged||sizeChanged){
        updateMovie();
    }
    updatePosition();
    makeMoney();
}

void SimCarnivore::read(const QJsonObject &json)
{
    IFish::read(json);
    updateMovie();
    updatePosition();
}

void SimCarnivore::write(QJsonObject &json)
{
    IFish::write(json);
}

QRect &SimCarnivore::getRect()
{
    return QRect(x,
                 y+IGameData::BIGCARNIVOREUPPER+20,
                 IGameData::CARNIVOREWIDTH,
                 IGameData::CARNIVOREHEIGHT-IGameData::BIGCARNIVOREBOTTOM-IGameData::BIGCARNIVOREUPPER-20);
}

void SimCarnivore::makeMoney()
{
    if (this->status==Status::Normal){
        if (!(timeCounter%MONEYPRODUCTINGTIME)){
            gamePanel->moneys.append(new IBox(x+this->width()/2-IGameData::MONEYWIDTH/2,
                                              y+this->height()-IGameData::BOXUPPER,
                                              gamePanel));
        }
    }
}
