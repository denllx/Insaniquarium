#ifndef IBUBBLE_H
#define IBUBBLE_H
#include "iobject.h"
#include <QMovie>
#include <QJsonObject>

class IBubble:public IObject{
    Q_OBJECT

public:
    explicit IBubble(int x,int y,QWidget* parent=0);
    void shift() override;
    ~IBubble();
    void read(const QJsonObject& json);
    void write(QJsonObject& json);
    QRect& getRect(){return QRect(0,0,0,0);}
private:
    QMovie* movie=new QMovie(":/background/resource/image/background/01.gif");
    int page=1;
};

#endif // IBUBBLE_H
