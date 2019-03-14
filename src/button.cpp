#include "button.h"
#include <QDebug>

MyButton::MyButton(const QString &filename, QWidget *parent):QPushButton(parent)
{
    this->setMouseTracking(true);
    normalFilename=filename;
    this->setStyleSheet("border-image:url("+normalFilename+");");
}

void MyButton::setHoverPixmap(const QString & name)
{
    hoverFilename=name;
}

void MyButton::setPressedPixmap(const QString & name)
{
    pressedFilename=name;
}

void MyButton::setReleasedPixmap(const QString & name)
{
    releasedFilename=name;
}

void MyButton::mousePressEvent(QMouseEvent *event)
{
    this->setStyleSheet("border-image:url("+pressedFilename+");color:white;");
    QPushButton::mousePressEvent(event);
}

void MyButton::mouseReleaseEvent(QMouseEvent *event)
{
    this->setStyleSheet("border-image:url("+releasedFilename+");color:white;");
    QPushButton::mouseReleaseEvent(event);
}

void MyButton::enterEvent(QEvent* event)
{
    this->setStyleSheet("border-image:url("+hoverFilename+");color:white;");
    QPushButton::enterEvent(event);
}

void MyButton::leaveEvent(QEvent* event)
{
    this->setStyleSheet("border-image:url("+normalFilename+");");
    QPushButton::leaveEvent(event);
}

MyButtonMulti::MyButtonMulti(const QString &filename, QWidget *parent)
    :MyButton(filename,parent)
{

}

void MyButtonMulti::mouseReleaseEvent(QMouseEvent *event)
{
    isChosen=!isChosen;
    MyButton::mouseReleaseEvent(event);
}

void MyButtonMulti::leaveEvent(QEvent *event)
{
    if (!isChosen){
        MyButton::leaveEvent(event);
    }
}
