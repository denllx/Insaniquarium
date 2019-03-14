#include "ibubble.h"

IBubble::IBubble(int x, int y, QWidget *parent):
    IObject(x,y,parent)
{
    this->setGeometry(x,y,20,20);
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();
}

void IBubble::shift()
{
    timeCounter++;
    this->y-=2;
    this->setGeometry(x,y,this->width(),this->height());
    if (!(timeCounter%20)){//0.2秒上升一次
        if (page<5){
            delete movie;
            movie=new QMovie(QString(":/background/resource/image/background/0%1.gif")
                             .arg(++page));
            this->setMovie(movie);
            movie->start();

        }
        else{
            this->exists=false;
        }

     }

}

IBubble::~IBubble()
{
    delete movie;
}

void IBubble::read(const QJsonObject &json)
{
    page=json["page"].toInt();
    x=json["x"].toInt();
    y=json["y"].toInt();
    xSpeed=json["xSpeed"].toInt();
    ySpeed=json["ySpeed"].toInt();
    timeCounter=json["timeCounter"].toInt();
}

void IBubble::write(QJsonObject &json)
{
    json["page"]=page;
    json["x"]=x;
    json["y"]=y;
    json["xSpeed"]=xSpeed;
    json["ySpeed"]=ySpeed;
    json["timeCounter"]=timeCounter;
}
