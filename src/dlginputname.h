#ifndef DLGINPUTNAME_H
#define DLGINPUTNAME_H
#include <QLabel>
#include <QMovie>
#include <QLineEdit>
#include <QComboBox>
#include "button.h"

class DlgInputName:public QLabel{

    Q_OBJECT

public:
    explicit DlgInputName(int x,int y,QWidget* parent=0);
    ~DlgInputName();

signals:
    void enterWelcome();
    void enterLoading(const QString& filename);

private:
    QMovie* background=new QMovie(":/interface/resource/image/background/username.png");
    QLabel* hint;
    QLineEdit* edit;
    MyButton* btnCancel,*btnContinue;
    bool eventFilter(QObject *watched, QEvent *event);
    QString username;

private slots:
    void getName(const QString& s);
};

class DlgWarning:public QLabel{
    Q_OBJECT

public:
    explicit DlgWarning(int x,int y,QWidget* parent=0);
    ~DlgWarning();
signals:
    void quit();
    void goon();
    void restart();
    void toSelector();
private:
    MyButton* btnQuit,*btnContinue,*btnRestart,*btnMain;
    QMovie* bg=new QMovie(":/interface/resource/image/background/username.png");

};

class DlgOptions:public QLabel{
    Q_OBJECT

public:
    explicit DlgOptions(int x,int y,QWidget* parent=0);
    ~DlgOptions();
signals:
    void reset();
    void changeTr(const QString&);
    void back();

private slots:
    void resetWarning();
    void langChanged(int index);
private:
    QMovie* bg=new QMovie(":/interface/resource/image/background/username.png");
    MyButton* btnReset;
    QComboBox* boxLang;
    MyButton* btnBack;
};

#endif // DLGINPUTNAME_H
