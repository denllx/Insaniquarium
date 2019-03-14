#ifndef BUTTON_H
#define BUTTON_H
#include <QPushButton>

class MyButton:public QPushButton{
    Q_OBJECT

public:
    explicit MyButton(const QString& filename,QWidget* parent=0);
    void setHoverPixmap(const QString& );
    void setPressedPixmap(const QString&);
    void setReleasedPixmap(const QString&);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

protected:
    QString hoverFilename,pressedFilename,releasedFilename,normalFilename;

};

//可多选，用于宠物选择环节
class MyButtonMulti:public MyButton{
    Q_OBJECT

public:
    explicit MyButtonMulti(const QString& filename,QWidget* parent=0);
    void mouseReleaseEvent(QMouseEvent* event);
    void leaveEvent(QEvent *event);
    bool isChosen=false;//点击后被选中或者不选中

};

#endif // BUTTON_H
