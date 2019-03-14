#include "dlginputname.h"
#include "iscene.h"
#include "maindialog.h"
#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>
#include <QRegExpValidator>

DlgInputName::DlgInputName(int x, int y, QWidget *parent):
    QLabel(parent)
{
    this->setGeometry(x,y,400,400);
    this->setMovie(background);
    this->setScaledContents(true);
    background->start();
    this->show();

    hint=new QLabel(this);
    hint->setText(tr("PLEASE INPUT YOUR NAME HERE"));
    hint->setStyleSheet("font-weight:900;color:yellow;font-size:30px;font-family:SimHei;");
    hint->setWordWrap(true);
    hint->setGeometry(50,100,300,100);
    hint->show();

    QRegExp rx("^[a-zA-Z0-9_]+$");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    edit=new QLineEdit(this);
    edit->setValidator(pReg);
    edit->setGeometry(50,200,300,50);
    edit->setStyleSheet("border-radius:20px");
    edit->show();
    connect(edit,SIGNAL(textChanged(QString)),this,SLOT(getName(const QString&)));

    btnContinue=new MyButton(":/interface/resource/image/background/btnblue.gif",this);
    btnContinue->setGeometry(50,260,150,50);
    btnContinue->setHoverPixmap(":/interface/resource/image/background/btnblue.gif");
    btnContinue->setPressedPixmap(":/interface/resource/image/background/btnbluedown.gif");
    btnContinue->setReleasedPixmap(":/interface/resource/image/background/btnblue.gif");
    btnContinue->setText(tr("CONTINUE"));
    QFont font("SimHei",15,900);
    btnContinue->setFont(font);
    btnContinue->show();
    btnContinue->setEnabled(false);
    btnContinue->installEventFilter(this);

    btnCancel=new MyButton(":/interface/resource/image/background/btnblue.gif",this);
    btnCancel->setGeometry(200,260,150,50);
    btnCancel->setText(tr("CANCEL"));
    btnCancel->setHoverPixmap(":/interface/resource/image/background/btnblue.gif");
    btnCancel->setPressedPixmap(":/interface/resource/image/background/btnbluedown.gif");
    btnCancel->setReleasedPixmap(":/interface/resource/image/background/btnblue.gif");
    btnCancel->setFont(font);
    btnCancel->show();
    btnCancel->installEventFilter(this);

}

DlgInputName::~DlgInputName()
{
    delete background;
    background=0;

}

bool DlgInputName::eventFilter(QObject *watched, QEvent *event)
{
    //取消游戏
    if (event->type()==QEvent::MouseButtonPress){
        QMouseEvent* mouseEvent=static_cast<QMouseEvent*>(event);
        if (mouseEvent->button()==Qt::LeftButton){
            if (watched==btnCancel){
                btnCancel->setStyleSheet("font-size:20px;"
                                         "font-family:SimHei;"
                                         "color:white;"
                                         "font-weight:900;"
                                         "border-image:url(:/interface/resource/image/background/btnbluedown.gif)");
                return true;
            }
            else if (watched==btnContinue){
                btnContinue->setStyleSheet("font-size:20px;"
                                         "font-family:SimHei;"
                                         "color:white;"
                                         "font-weight:900;"
                                         "border-image:url(:/interface/resource/image/background/btnbluedown.gif)");
                return true;
            }
            return false;
        }
        return false;
    }
    else if (event->type()==QEvent::MouseButtonRelease){
        QMouseEvent* mouseEvent=static_cast<QMouseEvent*>(event);
        if (mouseEvent->button()==Qt::LeftButton){
            if (watched==btnCancel){
                emit enterWelcome();
                return true;
            }
            else if (watched==btnContinue){
                emit enterLoading(username);
                return true;
            }
            return false;
        }
        return false;
    }

    return false;
}

void DlgInputName::getName(const QString &s)
{
    username=s;
    if (username.length()>=1){
        btnContinue->setEnabled(true);
    }
}


DlgWarning::DlgWarning(int x, int y, QWidget *parent):QLabel(parent)
{
    this->setGeometry(x,y,300,300);
    this->setMovie(bg);
    bg->start();
    this->setScaledContents(true);
    this->show();
    btnContinue=new MyButton(":/interface/resource/image/background/btnblue.gif",this);
    btnContinue->setGeometry(30,30,240,40);
    btnContinue->setHoverPixmap(":/interface/resource/image/background/btnblue.gif");
    btnContinue->setPressedPixmap(":/interface/resource/image/background/btnbluedown.gif");
    btnContinue->setReleasedPixmap(":/interface/resource/image/background/btnblue.gif");
    btnContinue->setText(tr("CONTINUE"));
    btnContinue->setFont(QFont("SimHei"));
    btnContinue->show();
    connect(btnContinue,SIGNAL(clicked(bool)),this,SIGNAL(goon()));

    btnQuit=new MyButton(":/interface/resource/image/background/btnblue.gif",this);
    btnQuit->setGeometry(30,100,240,40);
    btnQuit->setHoverPixmap(":/interface/resource/image/background/btnblue.gif");
    btnQuit->setPressedPixmap(":/interface/resource/image/background/btnbluedown.gif");
    btnQuit->setReleasedPixmap(":/interface/resource/image/background/btnblue.gif");
    btnQuit->setText(tr("QUIT"));
    btnQuit->setFont(QFont("SimHei"));
    btnQuit->show();
    connect(btnQuit,SIGNAL(clicked(bool)),this,SIGNAL(quit()));

    btnRestart=new MyButton(":/interface/resource/image/background/btnblue.gif",this);
    btnRestart->setGeometry(30,170,240,40);
    btnRestart->setHoverPixmap(":/interface/resource/image/background/btnblue.gif");
    btnRestart->setPressedPixmap(":/interface/resource/image/background/btnbluedown.gif");
    btnRestart->setReleasedPixmap(":/interface/resource/image/background/btnblue.gif");
    btnRestart->setText(tr("RESTART"));
    btnRestart->setFont(QFont("SimHei"));
    btnRestart->show();
    connect(btnRestart,SIGNAL(clicked(bool)),this,SIGNAL(restart()));

    btnMain=new MyButton(":/interface/resource/image/background/btnblue.gif",this);
    btnMain->setGeometry(30,240,240,40);
    btnMain->setHoverPixmap(":/interface/resource/image/background/btnblue.gif");
    btnMain->setPressedPixmap(":/interface/resource/image/background/btnbluedown.gif");
    btnMain->setReleasedPixmap(":/interface/resource/image/background/btnblue.gif");
    btnMain->setText(tr("BACK TO MAIN MENU"));
    btnMain->setFont(QFont("SimHei"));
    btnMain->show();
    connect(btnMain,SIGNAL(clicked(bool)),this,SIGNAL(toSelector()));
}

DlgWarning::~DlgWarning()
{
    delete bg;
}

DlgOptions::DlgOptions(int x, int y, QWidget *parent):QLabel(parent)
{
    this->setGeometry(x,y,300,300);
    this->setMovie(bg);
    bg->start();
    this->setScaledContents(true);
    this->show();

    btnReset=new MyButton(":/interface/resource/image/background/btnblue.gif",this);
    btnReset->setGeometry(50,50,200,30);
    btnReset->setPressedPixmap(":/interface/resource/image/background/btnbluedown.gif");
    btnReset->setReleasedPixmap(":/interface/resource/image/background/btnblue.gif");
    btnReset->setHoverPixmap(":/interface/resource/image/background/btnblue.gif");
    btnReset->setText(tr("RESET"));
    btnReset->show();
    connect(btnReset,SIGNAL(clicked(bool)),this,SLOT(resetWarning()));

    btnBack=new MyButton(":/interface/resource/image/background/btnblue.gif",this);
    btnBack->setGeometry(50,150,200,30);
    btnBack->setPressedPixmap(":/interface/resource/image/background/btnbluedown.gif");
    btnBack->setReleasedPixmap(":/interface/resource/image/background/btnblue.gif");
    btnBack->setHoverPixmap(":/interface/resource/image/background/btnblue.gif");
    btnBack->setText(tr("BACK TO MAIN MENU"));
    btnBack->show();
    connect(btnBack,SIGNAL(clicked(bool)),this,SIGNAL(back()));

   boxLang=new QComboBox(this);
   boxLang->setGeometry(50,100,200,30);
   boxLang->addItem("English","en");
   boxLang->addItem("Chinese","ch");
   ISelectorScene* scene=static_cast<ISelectorScene*>(parent);
   MainDialog* dialog=scene->dialog;
   int index=dialog->language=="en"?0:1;
   boxLang->setCurrentIndex(index);
   boxLang->show();
   connect(boxLang,SIGNAL(currentIndexChanged(int)),this,SLOT(langChanged(int)));
}

DlgOptions::~DlgOptions()
{
    bg->deleteLater();
}

void DlgOptions::resetWarning()
{
    QString msg=tr("All of you data will be deleted, including you name."
                "Still reset?");
    QMessageBox quitBox(this);
    quitBox.setText(msg);
    QAbstractButton *pYesBtn = (QAbstractButton *)quitBox.addButton(tr("RESET"), QMessageBox::YesRole);
    QAbstractButton *pNoBtn = (QAbstractButton *)quitBox.addButton(tr("CANCEL"), QMessageBox::NoRole);
    quitBox.exec();
    if (quitBox.clickedButton()==pYesBtn){
        //清空文件
        emit reset();
    }
}

void DlgOptions::langChanged(int index)
{
    QString langCode=boxLang->itemData(index).toString();
    emit changeTr(langCode);
}
