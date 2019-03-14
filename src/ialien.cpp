#include "ialien.h"
#include "iscene.h"
#include "igamedata.h"
#include <QDebug>
#include <QtMath>

IAlien::IAlien(int x, int y, QWidget *parent):
    ICreature(x,y,parent)
{
    do{
    xSpeed=qrand()%11-5;
    }while(xSpeed==0);
    do{
    ySpeed=qrand()%11-5;
    }while(ySpeed==0);
    this->setGeometry(x,y,IGameData::ALIENWIDTH,IGameData::ALIENHEIGHT);
}

IAlien::~IAlien()
{

}

void IAlien::hit()
{
    qDebug()<<life;
    life-=hitPoint;
}

void IAlien::updatePosition()
{
    timeCounter++;
    if (!(timeCounter%4)){
        x+=xSpeed;
        y+=ySpeed;
        this->setGeometry(x,y,IGameData::ALIENWIDTH,IGameData::ALIENHEIGHT);
    }
}

void IAlien::checkAlive()
{
    if (life<=0){
        qDebug()<<"alien die";
        gamePanel->moneys.append(new IDiamond(
                                     gamePanel->pos().x()+this->x+this->width()/2-IGameData::MONEYWIDTH/2,
                                     gamePanel->pos().y()+this->y+this->height()-IGameData::DIAMONDUPPER,
                                     gamePanel));

        exists=false;
    }
}

void IAlien::killCreature()
{
    IGameScene* scene=static_cast<IGameScene*>(this->parent());
    killCreatureByGroup(scene->guppyFish);
    killCreatureByGroup(scene->guppyMommy);
    killCreatureByGroup(scene->carnivore);
    killCreatureByGroup(scene->simcarnivore);//不杀宠物
}

//更新贴图
void IAlien::updateMovie()
{
    if (action==Action::OTHER){
        return;
    }
    QString direction=xSpeed<0?"left":"right";
    delete movie;
    QString filename=QString("%1%2%3.gif").arg(prefix).arg("swim").arg(direction);
    movie=new QMovie(filename);
    this->setMovie(movie);
    this->setScaledContents(true);
    movie->start();
    this->show();
}

void IAlien::turnRight()
{
    xSpeed=5-qrand()%5;
    updateMovie();
}

void IAlien::turnLeft()
{
    xSpeed=qrand()%5-5;
    updateMovie();
}

void IAlien::turnDown()
{
    ySpeed=5-qrand()%5;
}

void IAlien::turnUp()
{
    ySpeed=qrand()%5-5;
}

void IAlien::read(const QJsonObject &json)
{
    ICreature::read(json);
    life=json["life"].toInt();
    action=Action(json["action"].toInt());
}

void IAlien::write(QJsonObject &json)
{
    ICreature::write(json);
    json["id"]=id;
    json["life"]=life;
    json["action"]=action;
}


ISylvester::ISylvester(int x, int y, QWidget *parent):
    IAlien(x,y,parent)
{
    hitPoint=2;
    id=IGameData::SYL;
    acceptClickHit=true;
    prefix=":/alien/resource/image/creature/alien/syl/";

    QString initialFilename;
    initialFilename=QString("%1%2%3.gif")
            .arg(prefix)
            .arg("swim")
            .arg(xSpeed<0?"left":"right");
    movie=new QMovie(initialFilename);
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();
}

ISylvester::~ISylvester()
{
    delete movie;
}

void ISylvester::shift()
{
    bool ret=checkWall();
    if (ret){
        updateMovie();
    }
    killCreature();
    checkAlive();
    updatePosition();
}

void ISylvester::read(const QJsonObject &json)
{
    IAlien::read(json);
    updateMovie();
    updatePosition();
}

void ISylvester::write(QJsonObject &json)
{
    IAlien::write(json);
}

QRect &ISylvester::getRect()
{
    return QRect(0,0,0,0);
}

IGus::IGus(int x, int y, QWidget *parent):
    IAlien(x,y,parent)
{
    qDebug()<<"alien constructor";
    hitPoint=10;
    id=IGameData::GUS;
    acceptClickHit=false;
    prefix=":/alien/resource/image/creature/alien/gus/";

    QString initialFilename=QString("%1%2%3.gif")
            .arg(prefix)
            .arg("swim")
            .arg(xSpeed<0?"left":"right");
    movie=new QMovie(initialFilename);
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();
}

IGus::~IGus()
{
    delete movie;
}

void IGus::shift()
{
    bool ret1=checkWall();
    if(ret1){
        updateMovie();
    }
    bool ret2=checkFood();
    if (ret2){
        updateMovie();
    }
    checkAlive();
    updatePosition();
}

void IGus::read(const QJsonObject &json)
{
    IAlien::read(json);
    updateMovie();
    updatePosition();
}

void IGus::write(QJsonObject &json)
{
    IAlien::write(json);
}

QRect &IGus::getRect()
{
    return QRect(0,0,0,0);
}

bool IGus::checkFood()
{
    for (auto food:gamePanel->foods){
        if (food->iseaten){
            continue;
        }
        QPoint foodLeft=food->pos()+gamePanel->pos();
        QRect rectFood=QRect(foodLeft.x()+food->width(),
                             foodLeft.y()+food->height(),
                             food->width(),
                             food->height());
        QRect rectGus;
        //怪物朝左：右上半部分
        if (xSpeed>0){
        rectGus=QRect(x,
                      y+this->height()/3,
                      this->width()/2,
                      this->height()/3);
        }
        //怪物朝右：右上半部分
        else{
            rectGus=QRect(x+this->width()/2,
                          y+this->height()/3,
                          this->width()/2,
                          this->height()/3);
        }
        if (rectFood.intersects(rectGus)){
            //吃东西的动作
            this->xSpeed=0;
            action=Action::OTHER;
            updateEat();
            IObject* f=static_cast<IObject*>(food);
            if (!(f->iseaten)){
                f->eaten();
            }
            this->hit();
            qDebug()<<"food hit";
            return false;
        }

    }
    qreal minDistance=1000;
    int minId=-1,mindX,mindY;
    for (int i=0;i<gamePanel->guppyFish.count();i++){
        auto fish=gamePanel->guppyFish[i];
        if (fish->iseaten || fish->isDie()){
            continue;
        }
        int fishX,fishY,fishWidth,fishHeight;
        //不同的鱼贴图空白大小相差很多，应分类讨论
        switch (fish->getSize()) {
        case 0:
            fishX=fish->getX()+IGameData::GUPPYFISHUPPER;
            fishY=fish->getY()+IGameData::GUPPYFISHUPPER;
            fishWidth=IGameData::GUPPYFISHWIDTH-IGameData::GUPPYFISHUPPER*2;
            fishHeight=IGameData::GUPPYFISHHEIGHT-IGameData::GUPPYFISHUPPER*2;
            break;
        case 1:
            fishX=fish->getX()+IGameData::MEDIUMGUPPYUPPER;
            fishY=fish->getY()+IGameData::MEDIUMGUPPYUPPER;
            fishWidth=IGameData::GUPPYFISHWIDTH-IGameData::MEDIUMGUPPYUPPER*2;
            fishHeight=IGameData::GUPPYFISHHEIGHT-IGameData::MEDIUMGUPPYUPPER*2;
            break;
        case 2:
            fishX=fish->getX()+IGameData::BIGGUPPYUPPER;
            fishY=fish->getY()+IGameData::BIGGUPPYUPPER;
            fishWidth=IGameData::GUPPYFISHWIDTH-IGameData::BIGGUPPYUPPER*2;
            fishHeight=IGameData::GUPPYFISHHEIGHT-IGameData::BIGGUPPYUPPER*2;
            break;
        case 3:
            fishX=fish->getX()+IGameData::BIGGUPPYUPPER;
            fishY=fish->getY()+IGameData::BIGGUPPYUPPER;
            fishWidth=IGameData::GUPPYFISHWIDTH-IGameData::BIGGUPPYUPPER*2;
            fishHeight=IGameData::GUPPYFISHHEIGHT-IGameData::BIGGUPPYUPPER*2;
            break;
        default:
            break;
        }
        QRect rectFish=QRect(fishX,fishY,fishWidth,fishHeight);
        QRect rectGus;
        //怪物朝左：右上半部分
        if (xSpeed>0){
        rectGus=QRect(x,
                      y+this->height()/3,
                      this->width()/3,
                      this->height()/3);
        }
        //怪物朝右：右上半部分
        else{
            rectGus=QRect(x+this->width()/2,
                          y+this->height()/3,
                          this->width()/3,
                          this->height()/3);
        }
        if (rectFish.intersects(rectGus)){
            //吃东西的动作
            this->xSpeed=0;
            action=Action::OTHER;
            updateEat();
            IObject* f=static_cast<IObject*>(fish);
            if (!(f->iseaten || fish->isDie())){
                f->eaten();
            }
            this->hit();
            qDebug()<<rectGus<<" "<<rectFish;
            qDebug()<<"fish hit";
            return false;
        }
        //奔向最近的鱼
        qreal distance=qSqrt((x-fish->getX())*(x-fish->getX())+(y-fish->getX())*(y-fish->getX()));
        if (distance<minDistance){
            minId=i;
            mindX=fish->getX()-x;
            mindY=fish->getY()-y;
        }
    }
    if (minId>=0){
        int tempxSpeed=xSpeed;
        xSpeed=mindX/20;
        ySpeed=mindY/20;
        if (xSpeed*tempxSpeed<0){
           updateMovie();
            return true;
        }
        return false;
    }
    return false;
}

//播放吃东西的动画
void IGus::updateEat()
{
    delete movie;
    QString filename=QString("%1%2%3.gif")
            .arg(prefix)
            .arg("eat")
            .arg(xSpeed<0?"left":"right");
    movie=new QMovie(filename);
    this->setMovie(movie);
    movie->start();
    connect(movie,&QMovie::finished,[=](){
        updateMovie();
        action=Action::SWIM;
    });
}

IBalrog::IBalrog(int x, int y, QWidget *parent):IAlien(x,y,parent)
{
    hitPoint=2;
    id=IGameData::BALROG;
    acceptClickHit=true;
    prefix=":/alien/resource/image/creature/alien/balrog/";
    QString initialFilename;
    initialFilename=QString("%1%2%3.gif")
            .arg(prefix)
            .arg("swim")
            .arg(xSpeed<0?"left":"right");
    movie=new QMovie(initialFilename);
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();
}

IBalrog::~IBalrog()
{
    delete movie;
}

void IBalrog::shift()
{
    bool ret=checkWall();
    if (ret){
        updateMovie();
    }
    killCreature();
    checkAlive();
    updatePosition();
}

void IBalrog::read(const QJsonObject &json)
{
    IAlien::read(json);
    updateMovie();
    updatePosition();
}

void IBalrog::write(QJsonObject &json)
{
    IAlien::write(json);
}

QRect &IBalrog::getRect()
{
    return QRect(0,0,0,0);
}

