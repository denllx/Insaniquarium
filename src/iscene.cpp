#include "iscene.h"
#include "maindialog.h"
#include "dlginputname.h"
#include "maindialog.h"
#include "ipet.h"
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QMessageBox>
#include <QColor>
#include <QList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtMath>


IScene::IScene(QWidget *parent):QLabel(parent)
{
    this->setMouseTracking(true);
    dialog=static_cast<MainDialog*>(parent);
}

IWelcomeScene::IWelcomeScene(QWidget *parent):IScene(parent)
{
    this->setGeometry(0,0,800,600);
    clickHere=new QLabel(this);
    clickHere->setGeometry(250,520,300,50);
    clickHere->setMovie(movie1);
    movie1->start();
    clickHere->setScaledContents(true);
    clickHere->setStyleSheet("QLabel:hover{width:350px;height:60px;}");
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();
    clickHere->installEventFilter(this);
}

IWelcomeScene::~IWelcomeScene()
{
    delete movie;
    movie=0;
    delete movie1;
    movie1=0;
}

bool IWelcomeScene::eventFilter(QObject *watched, QEvent *event)
{
    if (watched==clickHere){
        if (event->type()==QEvent::MouseButtonPress){
            QMouseEvent* mouseEvent=static_cast<QMouseEvent*>(event);
            if (mouseEvent->button()==Qt::LeftButton){
                if (dialog->username==nullptr){
                    dialog->firstPlayed=true;
                    if (!dlg){
                        dlg=new DlgInputName(200,100,this);
                        connect(dlg,SIGNAL(enterWelcome()),this,SLOT(back()));
                        connect(dlg,SIGNAL(enterLoading(QString)),this,SLOT(getUsername(QString)));
                        return true;
                    }
                    return false;
                }
                else{
                    emit enterLoading(dialog->username);
                    return true;
                }
            }
            return false;
        }
        return false;
    }
    return IScene::eventFilter(watched,event);
}

void IWelcomeScene::refreshText()
{

}

void IWelcomeScene::back()
{
    dlg->deleteLater();
}

void IWelcomeScene::getUsername(const QString &username)
{
    QFile file("save.json");
    if (!file.open(QIODevice::ReadOnly)){
        qWarning("couldn't open file in welcomescene:getusername");
        return;
    }
    QByteArray data=file.readAll();
    QJsonDocument doc=QJsonDocument::fromJson(data);
    QJsonObject json=doc.object();
    if (!(json.contains("gameData"))){
        QJsonObject gamedataObject;
        gamedataObject["level"]=1;
        gamedataObject["score"]=100;
        gamedataObject["eggNum"]=0;
        gamedataObject["hasAlien"]=false;
        gamedataObject["foodLimitation"]=2;
        gamedataObject["foodClass"]=1;
        gamedataObject["timeCounter"]=0;
        json.insert("gameData",gamedataObject);
    }
    QJsonObject userObject=json["user"].toObject();
    if (userObject["name"].toString().length()<=0){
        json.remove("user");
        QJsonObject userObject;
        userObject["name"]=username;
        json.insert("user",userObject);
    }
    file.close();
    if (!file.open(QIODevice::WriteOnly|QIODevice::Truncate)){
        qWarning("unable to open file in welcome scene");
        return;
    }
    QJsonDocument saveDoc(json);
    file.write(saveDoc.toJson());
    file.close();
    emit enterLoading(username);
}

ILoadingScene::ILoadingScene(QWidget *parent):
    IScene(parent)
{
    this->setGeometry(0,0,800,600);
    this->setObjectName("bg");
    this->setStyleSheet("#bg{background:black;}");

    //提示符
    loadingText=new QLabel(QString("LOADING..."),this);
    loadingText->setGeometry(200,450,400,50);
    textMovie=new QMovie(":/background/resource/image/background/loading.png");
    loadingText->setMovie(textMovie);
    textMovie->start();
    loadingText->setScaledContents(true);
    loadingText->show();

    //转圈圈
    loadingPic=new QLabel(this);
    loadingPic->setGeometry(300,200,200,200);
    loadingMovie=new QMovie(":/interface/resource/image/background/loading.gif");
    loadingPic->setMovie(loadingMovie);
    loadingMovie->start();
    loadingPic->show();

    //数字
    loadingNum=new QLabel(loadingPic);
    loadingNum->setGeometry(20,50,100,100);
    loadingNum->setText("0%");
    QFont font("SimHei",20,900);
    loadingNum->setStyleSheet("color:white");
    loadingNum->setFont(font);
    loadingNum->setAlignment(Qt::AlignCenter);

    //定时器
    timer=new QTimer(this);
    timer->setInterval(20);
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimer()));
    timer->start();
    this->show();
}

ILoadingScene::~ILoadingScene()
{
    delete textMovie;
}

void ILoadingScene::onTimer()
{
    timeCounter++;
    if (progress<=95 && !(timeCounter%4)){
        progress+=qrand()%6;
        loadingNum->setText(QString("%1%")
                .arg(progress));
    }
    else if (progress>=96){
        loadingNum->setText("100%");
        emit enterSelector();
    }
}

void ILoadingScene::refreshText()
{

}


void ISelectorScene::refreshText()
{
    if (dialog->firstPlayed){
        labelDialog->setText(tr("TXT_WELCOME_TO_INSANIQUARIUM","welcome"));
    }
    else{
        labelDialog->setText(tr("TXT_WELCOME_BACK","welcome_back")+dialog->username);
    }
    advanture->setText(tr("TXT_ADVENTRUE","adventure")+QString("%1-%2")
                       .arg(int((dialog->level+4)/5))
                       .arg((dialog->level-1)%5+1));
    timeTrial->setText(tr("TXT_TIME_TRAIL","time trial"));
    challenge->setText(tr("TXT_CHALLENGE","challenge"));
    virtualTank->setText(tr("TXT_VIRTUAL_TANK","virtual tank"));
    hallOfGame->setText(tr("TXT_HALLOFGAME","hall of game"));
    options->setText(tr("TXT_OPTIONS","options"));
    help->setText(tr("TXT_HELP","help"));
    quit->setText(tr("TXT_QUIT","quit"));
}

ISelectorScene::ISelectorScene(QWidget *parent):
    IScene(parent)
{
    this->setGeometry(0,0,800,600);
    labelDialog=new QLabel(this);
    labelDialog->setGeometry(100,50,300,160);
    labelDialog->setStyleSheet(QString("border-image:url(:/interface/resource/image/background/dialog.png);color:blue;"));
    QString dialogText="";
    QFont font(dialog->f);
    font.setPointSize(20);
    if (dialog->firstPlayed){
        dialogText=tr("WELCOME TO INSANIQUARIUM!");
    }
    else{
        dialogText=tr("WELCOME BACK,")+
                QString("%1").arg(dialog->username);
    }
    labelDialog->setText(dialogText);
    labelDialog->setFont(font);
    labelDialog->setWordWrap(true);

    labelDialog->setFont(font);
    labelDialog->setAlignment(Qt::AlignCenter);
    labelDialog->show();

    advanture=new MyButton(":/interface/resource/image/background/btndarkblue.png",this);
    advanture->setGeometry(453,60,255,80);
    advanture->setHoverPixmap(":/interface/resource/image/background/btnyellow.png");
    advanture->setPressedPixmap(":/interface/resource/image/background/btnyellowdown.png");
    advanture->setReleasedPixmap(":/interface/resource/image/background/btnyellow.png");
    advanture->setText(tr("ADVANTURE")+
                       QString("\nTANK %1-%2")
                       .arg(int((dialog->level+4)/5))
                       .arg((dialog->level-1)%5+1));
    advanture->setFont(font);
    advanture->installEventFilter(this);
    advanture->show();

    timeTrial=new MyButton(":/interface/resource/image/background/btndarkblue.png",this);
    timeTrial->setGeometry(453,175,255,65);
    timeTrial->setHoverPixmap(":/interface/resource/image/background/btnyellow.png");
    timeTrial->setPressedPixmap(":/interface/resource/image/background/btnyellowdown.png");
    timeTrial->setReleasedPixmap(":/interface/resource/image/background/btnyellow.png");
    timeTrial->setText(tr("TIME TRIAL"));
    timeTrial->setFont(font);
    timeTrial->installEventFilter(this);
    timeTrial->show();

    challenge=new MyButton(":/interface/resource/image/background/btndarkblue.png",this);
    challenge->setGeometry(453,260,255,65);
    challenge->setHoverPixmap(":/interface/resource/image/background/btnyellow.png");
    challenge->setPressedPixmap(":/interface/resource/image/background/btnyellowdown.png");
    challenge->setReleasedPixmap(":/interface/resource/image/background/btnyellow.png");
    challenge->setText(tr("CHALLENGE"));
    challenge->setFont(font);
    challenge->installEventFilter(this);
    challenge->show();

    virtualTank=new MyButton(":/interface/resource/image/background/btndarkblue.png",this);
    virtualTank->setGeometry(453,360,255,80);
    virtualTank->setHoverPixmap(":/interface/resource/image/background/btnyellow.png");
    virtualTank->setPressedPixmap(":/interface/resource/image/background/btnyellowdown.png");
    virtualTank->setReleasedPixmap(":/interface/resource/image/background/btnyellow.png");
    virtualTank->setText(tr("VIRTUAL TANK"));
    virtualTank->setFont(font);
    virtualTank->installEventFilter(this);
    virtualTank->show();

    QFont font1(dialog->f);
    font1.setPointSize(12);
    hallOfGame=new MyButton(":/interface/resource/image/background/btndarkblue.png",this);
    hallOfGame->setGeometry(505,475,150,35);
    hallOfGame->setHoverPixmap(":/interface/resource/image/background/btnyellow.png");
    hallOfGame->setPressedPixmap(":/interface/resource/image/background/btnyellowdown.png");
    hallOfGame->setReleasedPixmap(":/interface/resource/image/background/btnyellow.png");
    hallOfGame->setText(tr("HALL OF GAME"));
    hallOfGame->setFont(font1);
    hallOfGame->installEventFilter(this);
    hallOfGame->show();

    options=new MyButton(":/interface/resource/image/background/btndarkblue.png",this);
    options->setGeometry(408,515,110,35);
    options->setHoverPixmap(":/interface/resource/image/background/btnyellow.png");
    options->setPressedPixmap(":/interface/resource/image/background/btnyellowdown.png");
    options->setReleasedPixmap(":/interface/resource/image/background/btnyellow.png");
    options->setText(tr("OPTIONS"));
    options->setFont(font1);
    options->installEventFilter(this);
    options->show();

    help=new MyButton(":/interface/resource/image/background/btndarkblue.png",this);
    help->setGeometry(528,515,110,35);
    help->setHoverPixmap(":/interface/resource/image/background/btnyellow.png");
    help->setPressedPixmap(":/interface/resource/image/background/btnyellowdown.png");
    help->setReleasedPixmap(":/interface/resource/image/background/btnyellow.png");
    help->setText(tr("HELP"));
    help->setFont(font1);
    help->installEventFilter(this);
    help->show();

    quit=new MyButton(":/interface/resource/image/background/btndarkblue.png",this);
    quit->setGeometry(645,515,110,35);
    quit->setHoverPixmap(":/interface/resource/image/background/btnyellow.png");
    quit->setPressedPixmap(":/interface/resource/image/background/btnyellowdown.png");
    quit->setReleasedPixmap(":/interface/resource/image/background/btnyellow.png");
    quit->setText(tr("QUIT"));
    quit->setFont(font1);
    quit->installEventFilter(this);
    quit->show();
    dialog->f.setPointSize(8);

    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();
}

ISelectorScene::~ISelectorScene()
{
    if (movie){
        movie->stop();
        delete movie;
        movie=0;
    }
}

bool ISelectorScene::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type()==QEvent::MouseButtonPress){
        QMouseEvent* mouseEvent=static_cast<QMouseEvent*>(event);
        if (mouseEvent->button()==Qt::LeftButton){
            if (watched==advanture){
                emit enterGame();
                return true;
            }
            if (watched==help){
                emit enterHelp();
                return true;
            }
            if (watched==quit){
                qApp->quit();
                return true;
            }
            if (watched==virtualTank){
                return true;
            }
            if (watched==challenge){
                return true;
            }
            if (watched==timeTrial){
                return true;
            }
            if (watched==options){
                DlgOptions* dlg=new DlgOptions(200,200,this);
                dlg->show();
                connect(dlg,&DlgOptions::reset,[=](){
                    dlg->deleteLater();
                    QFile saveFile("save.json");
                    if (!saveFile.open(QIODevice::WriteOnly|QIODevice::Truncate)){
                        qWarning("couldn't open file in selectorscene:eventfilter");
                        return;
                    }
                    QJsonObject object;
                    QJsonObject gamedataObject;
                    gamedataObject["level"]=1;
                    gamedataObject["score"]=100;
                    gamedataObject["eggNum"]=0;
                    gamedataObject["hasAlien"]=false;
                    gamedataObject["foodLimitation"]=2;
                    gamedataObject["foodClass"]=1;
                    gamedataObject["timeCounter"]=0;
                    object["gameData"]=gamedataObject;
                    QJsonObject userObject;
                    userObject["name"]="";
                    object["user"]=userObject;
                    object["language"]="en";
                    QJsonDocument saveDoc(object);
                    int ret=saveFile.write(saveDoc.toJson());
                    saveFile.close();
                    emit enterWelcome();
                });
                connect(dlg,&DlgOptions::changeTr,[=](const QString& langCode){
                    emit changeTr(langCode);
                    refreshText();
                });
                connect(dlg,&DlgOptions::back,[=](){
                    dlg->deleteLater();
                });
                return true;
            }

            return false;

        }
        return false;
    }
    return IScene::eventFilter(watched,event);
}

void IHelpScene::refreshText()
{
}


IHelpScene::IHelpScene(QWidget *parent):IScene(parent)
{
    this->setGeometry(0,0,800,600);
    movie=new QMovie(":/help/resource/image/help/1.png");
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();

    back=new MyButton(":/interface/resource/image/background/btnmenu.gif",this);
    back->setGeometry(656,5,99,35);
    back->setHoverPixmap(":/interface/resource/image/background/btnmenu.gif");
    back->setReleasedPixmap(":/interface/resource/image/background/btnmenu.gif");
    back->setPressedPixmap(":/interface/resource/image/background/btnmenudown.gif");
    back->show();
    connect(back,SIGNAL(clicked(bool)),this,SIGNAL(enterSelector()));

    previous=new MyButton(":/interface/resource/image/background/btnblue.gif",this);
    previous->setGeometry(240,535,150,38);
    previous->setText(tr("PREVIOUS"));
    QFont font("SimHei",15,900);
    previous->setFont(font);
    previous->setHoverPixmap(":/interface/resource/image/background/btnblue.gif");
    previous->setPressedPixmap(":/interface/resource/image/background/btnbluedown.gif");
    previous->setReleasedPixmap(":/interface/resource/image/background/btnblue.gif");
    //previous->installEventFilter(this);
    previous->show();
    QObject::connect(previous,&QPushButton::clicked,[=](){
        this->setCurrentPage((currentPage+6)%8+1);
    });

    next=new MyButton(":/interface/resource/image/background/btnblue.gif",this);
    next->setGeometry(410,535,150,38);
    next->setHoverPixmap(":/interface/resource/image/background/btnblue.gif");
    next->setPressedPixmap(":/interface/resource/image/background/btnbluedown.gif");
    next->setReleasedPixmap(":/interface/resource/image/background/btnblue.gif");
    next->setText(tr("NEXT"));
    next->setFont(font);
    //next->installEventFilter(this);
    next->show();
    QObject::connect(next,&QPushButton::clicked,[=](){
        this->setCurrentPage(currentPage%8+1);
    });
}

IHelpScene::~IHelpScene()
{
    if (movie){
        movie->stop();
        delete movie;
        movie=0;
    }
}

void IHelpScene::setCurrentPage(int page)
{
    currentPage=page;
    QString filename=QString(":/help/resource/image/help/%1.png").arg(currentPage);
    movie=new QMovie(filename);
    this->setMovie(movie);
    movie->start();
}

bool IHelpScene::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type()==QEvent::MouseButtonPress){
        QMouseEvent* mouseEvent=static_cast<QMouseEvent*>(event);
        if (mouseEvent->button()==Qt::LeftButton){
            //返回selector
            if (watched==back){
                emit enterSelector();
                return true;
            }
            //下一张
            if (watched==next){
                this->setCurrentPage(currentPage%8+1);
                return true;
            }
             //上一张
            if (watched==previous){
                this->setCurrentPage((currentPage+6)%8+1);
                return true;
            }
            return false;

        }
        return false;
    }
    return IScene::eventFilter(watched,event);
}

void IPetScene::refreshText()
{
}

IPetScene::IPetScene(QWidget *parent):IScene(parent)
{
    this->setGeometry(0,0,800,600);
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    this->show();

    petLabel=new QLabel(this);
    petLabel->setGeometry(330,80,150,150);
    petMovie=new QMovie(IGameData::petNameList[
                        IGameData::totalDeservedPets[dialog->level]]);
    petLabel->setMovie(petMovie);
    petMovie->start();
    petLabel->setScaledContents(true);
    petLabel->show();

    petDescription=new QLabel(this);
    petDescription->setGeometry(270,250,300,250);
    QFont font(dialog->f);
    font.setPointSize(14);
    petDescription->setFont(font);
    switch (IGameData::totalDeservedPets[dialog->level]) {
    case IGameData::SNAIL:
        petDescription->setText(tr("THIS IS STIKY.IT WILL CATCH THE MONEY YOU DROP."));
        break;
    case IGameData::SAILFISH:
        petDescription->setText(tr("THIS IS ITCHY. IT WILL HELP YOU ATTACK THE ENEMY."));
        break;
    case IGameData::ZORF:
        petDescription->setText(tr("THIS IS ZORF. IT WILL GENERATE FOOD TO SURVIVE YOUR FISH."));
        break;
    case IGameData::VERT:
        petDescription->setText(tr("THIS IS VERT. IT WILL GENERATE GOLD COIN FOR YOU."));
        break;
    case IGameData::CLAM:
        petDescription->setText(tr("THIS IS NIKO. IT WILL GENERATE A PEARL FOR YOU."));
        break;
    default:
        break;
    }
    petDescription->setStyleSheet("font-size:35px;color:white;");
    petDescription->setWordWrap(true);
    petDescription->show();

    continueLabel=new MyButton(":/interface/resource/image/background/btnblue.gif",this);
    continueLabel->setHoverPixmap(":/interface/resource/image/background/btnblue.gif");
    continueLabel->setPressedPixmap(":/interface/resource/image/background/btnbluedown.gif");
    continueLabel->setReleasedPixmap(":/interface/resource/image/background/btnblue.gif");
    continueLabel->setGeometry(230,550,350,45);
    continueLabel->setText(tr("CONTINUE"));
    QObject::connect(continueLabel,&QPushButton::clicked,[=](){
        if (dialog->level<=3){
            emit enterGame();
        }
        else{
            emit enterSelectPet();
        }
    });
    continueLabel->show();
}

IPetScene::~IPetScene()
{
    delete movie;
}

IGameScene::IGameScene(int l, QWidget *parent,bool firstTime,bool firstPlayed):
    IScene(parent)
{
    this->firstTime=firstTime;//false
    this->firstPlayed=firstPlayed;//false
    if (firstPlayed){
        level=1;
    }
    else{
        qDebug()<<"level="<<l;
        level=l;
    }
    qDebug()<<firstTime<<firstPlayed;
    if(firstTime && !firstPlayed){//有游戏记录且是第一次加载
        loadGame();
    }
    else{
        initAnimal();
    }
    initLayout();
    initialWindows();
    timer=new QTimer(this);
    timer->setInterval(20);
    timer->start();
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimer()));
}


void IGameScene::initAnimal()
{
    for (int i=0;i<2;i++){
        int x=qrand()%this->rect().width();
        int y=qrand()%(this->rect().height()-100) +100;
    guppyFish.append(new GuppyFish(1,x,y,this));
    }
    for (int i=0;i<2;i++){
        int x=qrand()%800;
        int y=qrand()%(this->rect().height()-100) +100;
        guppyMommy.append(new GuppyMommy(1,x,y,this));
    }
    IGameData* data=dialog->getData();
    if (this->level<=4){//总共拥有的的宠物不超过3只
        for (int i=0;i<this->level;i++){
            switch (data->totalDeservedPets[i]) {
            case IGameData::SNAIL:
                pets.append(new ISnail(400,440,this));
                break;
            case IGameData::SAILFISH:
                pets.append(new ISailFish(qrand()%600+100,qrand()%400+100,this));
                break;
            case IGameData::ZORF:
                pets.append(new IZorf(qrand()%600+100,qrand()%400+100,this));
                break;
                //TO BE DELETED!
            case IGameData::VERT:
                pets.append(new IFishbone(qrand()%600+100,qrand()%400+100,this));
                break;
            case IGameData::CLAM:
                pets.append(new IClam(220,220,this));
                break;
            default:
                break;
            }
        }
    }
    else{
        QList<int>& chosenPets=data->chosenPets;
        for (int i=0;i<chosenPets.count();i++){
            switch (chosenPets[i]) {
            case IGameData::SNAIL:
                pets.append(new ISnail(400,440,this));
                break;
            case IGameData::SAILFISH:
                pets.append(new ISailFish(qrand()%600+100,qrand()%400+100,this));
                break;
            case IGameData::ZORF:
                pets.append(new IZorf(qrand()%600+100,qrand()%400+100,this));
                break;
            case IGameData::VERT:
                pets.append(new IFishbone(qrand()%600+100,qrand()%400+100,this));
                break;
            case IGameData::CLAM:
                pets.append(new IClam(220,220,this));
                break;
            default:
                break;
            }
        }
    }
}

void IGameScene::initLayout()
{
    this->setGeometry(0,0,800,600);
    QString bgFilename=QString(
                ":/background/resource/image/background/aquarium%1.jpg")
            .arg(static_cast<int>((level+4)/5));
    movie=new QMovie(bgFilename);
    this->setMovie(movie);
    movie->start();
    this->setScaledContents(true);
    movie->installEventFilter(this);
    this->show();

    boxbar=new QLabel(this);
    boxbar->setGeometry(0,0,800,100);
    boxbar->setMovie(barMovie);
    barMovie->start();
    boxbar->setScaledContents(true);
    boxbar->show();

    labelWarning=new QLabel(this);
    labelWarning->setGeometry(200,460,400,100);
    QFont font(dialog->f);
    font.setPointSize(20);
    labelWarning->setFont(font);
    labelWarning->setText(tr("TXT_ALIEN_COMING","alien coming"));
    labelWarning->setStyleSheet("color:#c0070e;");
    labelWarning->setVisible(false);
    dialog->f.setPointSize(8);

    menu=new MyButton(":/interface/resource/image/background/btnmenu.gif",this);
    menu->setGeometry(670,10,100,28);
    menu->setHoverPixmap(":/interface/resource/image/background/btnmenu.gif");
    menu->setPressedPixmap(":/interface/resource/image/background/btnmenudown.gif");
    menu->setReleasedPixmap(":/interface/resource/image/background/btnmenu.gif");
    menu->show();
    QObject::connect(menu,&QPushButton::clicked,[=](){
        this->pause();
        menuClicked();
    });

    scoreBar=new QLabel(this);
    scoreBar->setText(QString("$%1").arg(score));
    scoreBar->setStyleSheet("background:#006400;color:yellow;"
                            "font-weight:bold;font-size:25px;"
                            "font-family:sans-serif;"
                            "border-radius:10px;");
    scoreBar->setAlignment(Qt::AlignCenter);
    scoreBar->setGeometry(680,50,100,28);
    scoreBar->show();
}

IGameScene::~IGameScene()
{
    while(!carnivore.isEmpty()){
        delete carnivore[0];
        carnivore.removeAt(0);
    }
    while(!(pets.isEmpty())){
        delete pets[0];
        pets.removeAt(0);
    }
    while(!aliens.isEmpty()){
        delete aliens[0];
        aliens.removeAt(0);
    }
    while(!simcarnivore.isEmpty()){
        delete simcarnivore[0];
        simcarnivore.removeAt(0);
    }
    while(!guppyFish.isEmpty()){
        delete guppyFish[0];
        guppyFish.removeAt(0);
    }

    while(!guppyMommy.isEmpty()){
        delete guppyMommy[0];
        guppyMommy.removeAt(0);
    }
    while(!foods.isEmpty()){
        delete foods[0];
        foods.removeAt(0);
    }

    while(!moneys.isEmpty()){
        delete moneys[0];
        moneys.removeAt(0);
    }
     while(!windows.isEmpty()){
         windows.removeAt(0);
     }

     while(!onSell.isEmpty()){
         if (onSell[0]){
             onSell[0]->stop();
             delete onSell[0];
             onSell[0]=0;
         }
         onSell.removeAt(0);
     }

     while(!windowopen.isEmpty()){
         delete windowopen[0];
         windowopen.removeAt(0);
     }

    if (movie) {
        movie->stop();
        delete movie;
        movie=0;
    }
}

//绑定定时器
void IGameScene::shift()
{
    for (auto fish:guppyFish){
        fish->shift();
    }
    for (auto mommy:guppyMommy){
        mommy->shift();
    }
    for (auto food:foods){
        food->shift();
    }
    for (auto money:moneys){
        money->shift();
    }
    for (auto fish:carnivore){
        fish->shift();
    }
    for (auto fish:simcarnivore){
        fish->shift();
    }
    for (auto alien:aliens){
        alien->shift();
    }
    for (auto pet:pets){
        pet->shift();
    }
    for (auto b:bubble){
        b->shift();
    }

}

void IGameScene::onTimer()
{
    timeCounter++;    
    checkAlien();
    shift();
    updateWindows();
    updateBubble();
    removeDeath();
    judge();
}

void IGameScene::alienCome()
{
    labelWarning->setVisible(false);
    notifyAlienComing();//通知所有生物alien来了
    IGameData* data=dialog->getData();
    switch (data->totalAliens[level-1]) {
    case IGameData::SYL:
        aliens.append(new ISylvester(qrand()%400+200,qrand()%100+350,this));
        break;
    case IGameData::GUS:
        aliens.append(new IGus(600,qrand()%100+400,this));
        break;
    case IGameData::BALROG:
        aliens.append(new IBalrog(qrand()%400+200,qrand()%100+350,this));
        break;
    default:
        break;
    }
}

void IGameScene::mousePressEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton){
        //生成古比鱼
        QRect firstWindowRect=QRect(windows[0]->pos()+this->pos(),windows[0]->size());
        if (firstWindowRect.contains(event->pos()) && score>=100){
            GuppyFish* newFish=new GuppyFish(foodClass,firstWindowRect.x(),firstWindowRect.y()+firstWindowRect.height()/2,this);
            if (hasAlien){
                newFish->alienComing();
            }
            newFish->setYspeed(2);
            guppyFish.append(newFish);
            updateScore(-newFish->getPrice());
            return;
        }
        //生成食肉鱼
        QRect fourthWindowRect=QRect(windows[3]->pos()+this->pos(),windows[3]->size());
        if (fourthWindowRect.contains(event->pos()) && score>=Carnivore::getPrice()){
            Carnivore* newFish=new Carnivore(fourthWindowRect.x(),fourthWindowRect.y()+fourthWindowRect.height()/2,this);
            if (hasAlien){
                newFish->alienComing();
            }
            newFish->setYspeed(2);
            carnivore.append(newFish);
            updateScore(-Carnivore::getPrice());
            return;
        }
        //生成巨型食肉鱼
        QRect fifthWindowRect=QRect(windows[4]->pos()+this->pos(),windows[4]->size());
        if (fifthWindowRect.contains(event->pos())&& score>=SimCarnivore::getPrice()){
            SimCarnivore* newFish=new SimCarnivore(fifthWindowRect.x(),fifthWindowRect.y()+fifthWindowRect.height()/2,this);
            if (hasAlien){
                newFish->alienComing();
            }
            newFish->setYspeed(2);
            simcarnivore.append(newFish);
            updateScore(-SimCarnivore::getPrice());
            return;
        }
        //升级饲料
        QRect secondWindowRect=QRect(windows[1]->pos()+this->pos(),windows[1]->size());
        if (foodClass<=2
                && secondWindowRect.contains(event->pos())
                && score>=IFood::classToPrice(foodClass+1)){
            updateScore(-IFood::classToPrice(++foodClass));
            int starving=IFood::classToStarving(foodClass);
            int dead=IFood::classToDead(foodClass);
            for (auto fish:guppyFish){
                fish->setStarvingTime(starving);
                fish->setDeadTime(dead);
                fish->setFoodClass(foodClass);
            }
            for (auto fish:guppyMommy){
                fish->setStarvingTime(starving);
                fish->setDeadTime(dead);
                fish->setFoodClass(foodClass);
            }

            if (foodClass<=2){
                updateWindow2();
            }
            return;
        }
        //升级数量
        QRect thirdWindowRect=QRect(windows[2]->pos()+this->pos(),windows[2]->size());
        if (thirdWindowRect.contains(event->pos())
                &&score>=IFood::addingPrice()){
            updateScore(-IFood::addingPrice());
            foodLimitation+=1;
            updateWindow3();

            return;
        }
        //买蛋
        QRect seventhWindowRect=QRect(windows[6]->pos()+this->pos(),windows[6]->size());
        if (seventhWindowRect.contains(event->pos())
                &&score>=eggPrice){
            updateScore(-eggPrice);
            updateWindow7();
            eggNum++;
            return;
        }
        //点击钱
        bool clickMoney=false;
        IMoney* currentMoney=nullptr;
        for (auto money:moneys){
            QRect rect=QRect(this->pos()+money->pos(),money->size());
            if (rect.contains(event->pos())){
                clickMoney=true;
                currentMoney=money;
            }
        }
       if (clickMoney==true){
            currentMoney->setClicked(true);
            return;

        }
       //点击殴打外星人
       if (!(aliens.isEmpty())){
           for (auto alien:aliens){
               if (!(alien->acceptClickHit)){
                   continue;
               }
               QRect rect=QRect(alien->pos()+this->pos(),alien->size());
               if (rect.contains(event->pos())){
                   alien->hit();
                   if (event->pos().x()<=rect.center().x() && alien->getXspeed()<=0){
                       alien->turnRight();
                   }
                   else if (event->pos().x()>=rect.center().x()&& alien->getXspeed()>=0){
                       alien->turnLeft();
                   }
                   if (event->pos().y()<=rect.center().y() && alien->getYspeed()<=0){
                       alien->turnDown();
                   }
                   else if (event->pos().y()>=rect.center().y() && alien->getYspeed() >=0){
                       alien->turnUp();
                   }
                   return;
               }
           }

       }
        //点击获得珍珠
       //TODO判断当前是否有珍珠
       for (auto pet:pets){
           if (pet->id==IGameData::CLAM){
                IClam* clam=static_cast<IClam*>(pet);
               if (!(clam->isOpen())){
                   continue;
                }
               QRect rectPearl=QRect(220,220,72,72);
               if (rectPearl.contains(event->pos())){
                   moneys.append(new IPearl(280,220,this));
                   IClam* clam=static_cast<IClam*>(pet);
                   clam->getClosed();
               }
           }
       }

        //点击处生成食物
       QRect boxbarRect=QRect(boxbar->pos()+this->pos(),boxbar->size());
        if (this->rect().contains(event->pos())
                &&!(boxbarRect.contains(event->pos()))){
            if (foods.count()<foodLimitation
                    && score>=IFood::getPrice()){
            foods.append(new IFood(foodClass,event->pos().x(),event->pos().y(),this));
            updateScore(-IFood::getPrice());
            }
        }
    }
}

void IGameScene::refreshText()
{

}

//退出或者重置
void IGameScene::menuClicked()
{
    DlgWarning* warning=new DlgWarning(200,200,this);
    connect(warning,&DlgWarning::goon,[=](){
      warning->setVisible(false);
       timer->start();
    });
    connect(warning,&DlgWarning::restart,[=](){
        this->resetGame();
        emit restart();
    });
    connect(warning,&DlgWarning::quit,[=](){
        saveGame();
        qApp->quit();
    });
    connect(warning,&DlgWarning::toSelector,[=](){
        qDebug()<<"savegame";
       saveGame();
       emit enterSelector();
    });
}

void IGameScene::failWarning()
{
    timer->stop();
    QString msg=tr("Oops! All of your fish has been dead!Do you want to restart?","fail warning");
    QMessageBox quitBox(this);
    quitBox.setText(msg);
    QAbstractButton *pYesBtn = (QAbstractButton *)quitBox.addButton(tr("RESTART"), QMessageBox::YesRole);
    QAbstractButton *pNoBtn = (QAbstractButton *)quitBox.addButton(tr("QUIT"), QMessageBox::NoRole);
    quitBox.exec();
    if (quitBox.clickedButton()==pYesBtn){
        //清空文件
        this->resetGame();
        emit restart();//退出从头开始
    }
    else if (quitBox.clickedButton()==pNoBtn){
        saveGame();
        qApp->quit();
    }
}

void IGameScene::removeDeath()
{
    removeDeathByGroup(guppyFish);
    removeDeathByGroup(foods);
    removeDeathByGroup(moneys);
    removeDeathByGroup(guppyMommy);
    removeDeathByGroup(carnivore);
    removeDeathByGroup(simcarnivore);
    removeDeathByGroup(aliens);
    removeDeathByGroup(pets);
    removeDeathByGroup(bubble);
    if (hasAlien && aliens.isEmpty()){
        notifyAlienDie();
    }
}

void IGameScene::checkAlien()
{
    if (!(timeCounter%timeAlienComing)){
        //预警
        labelWarning->setVisible(true);
       QTimer* timerAlien=new QTimer(this);
       timerAlien->setSingleShot(true);
       timerAlien->start(3000);
       connect(timerAlien,SIGNAL(timeout()),this,SLOT(alienCome()));
    }
}

void IGameScene::notifyAlienComing()
{
    hasAlien=true;
    for (auto fish:guppyFish){
       fish->alienComing();
    }
    for (auto fish:guppyMommy){
        fish->alienComing();
    }
    for (auto fish:carnivore){
        fish->alienComing();
    }
    for (auto fish:simcarnivore){
        fish->alienComing();
    }
    for (auto pet:pets){
        pet->alienComing();
    }
}

void IGameScene::notifyAlienDie()
{
    hasAlien=false;
    for (auto fish:guppyFish){
       fish->alienDie();
    }
    for (auto fish:guppyMommy){
        fish->alienDie();
    }
    for (auto fish:carnivore){
        fish->alienDie();
    }
    for (auto fish:simcarnivore){
        fish->alienDie();
    }
    for (auto pet:pets){
        pet->alienDie();
    }
}

void IGameScene::updateBubble()
{
    if (!(timeCounter%100))//10秒一串水泡
    {
        bubble.append(new IBubble(qrand()%800,500,this));
    }
}

void IGameScene::judge()
{
    if (eggNum>=3){
        timer->stop();
        emit enterPet();
        return;
    }
    if (guppyFish.isEmpty()
            &&guppyMommy.isEmpty()
            &&carnivore.isEmpty()
            &&simcarnivore.isEmpty()){
        failWarning();
        return;
    }
}

void IGameScene::saveGame()
{
    QFile saveFile("save.json");
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open file in gamescene:savegame");
        return;
    }
    QJsonObject gameObject;
    this->write(gameObject);
    QJsonDocument saveDoc(gameObject);
    saveFile.write(saveDoc.toJson());
    saveFile.close();
}

void IGameScene::loadGame()
{
    QFile loadFile("save.json");
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("couldn't open file in gamescene:loadgame");
    }
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    read(loadDoc.object());
    loadFile.close();
}

void IGameScene::resetGame()
{
    QFile saveFile("save.json");
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("couldn't open file in gamescene:resetgame");
        return;
    }
    QJsonObject gameObject;
    this->reset(gameObject);
    QJsonDocument saveDoc(gameObject);
    saveFile.write(saveDoc.toJson());
    saveFile.close();
}

void IGameScene::write(QJsonObject &json)
{
    QJsonObject gamedataObject;
    gamedataObject["level"]=this->level;
    gamedataObject["score"]=this->score;
    gamedataObject["eggNum"]=this->eggNum;
    gamedataObject["hasAlien"]=this->hasAlien;
    gamedataObject["foodLimitation"]=this->foodLimitation;
    gamedataObject["foodClass"]=this->foodClass;
    gamedataObject["timeCounter"]=this->timeCounter;
    QJsonArray arrayOpen;
    for (auto open:alreadyopen){
        QJsonObject objectOpen;
        objectOpen["isopen"]=open?true:false;
        arrayOpen.append(objectOpen);
    }
    gamedataObject["alreadyopen"]=arrayOpen;
    json["gameData"]=gamedataObject;
    QJsonObject userObject;
    userObject["name"]=dialog->username;
    json["user"]=userObject;

    json["language"]=this->dialog->language;

    QJsonArray arrayGuppyFish;
    for (auto fish:guppyFish){
        QJsonObject objectGuppyFish;
        (*fish).write(objectGuppyFish);
        arrayGuppyFish.append(objectGuppyFish);
    }
    json["guppyFish"]=arrayGuppyFish;

    QJsonArray arrayGuppyMommy;
    for (auto mommy:guppyMommy){
        QJsonObject objectGuppyMommy;
        (*mommy).write(objectGuppyMommy);
        arrayGuppyMommy.append(objectGuppyMommy);
    }
    json["guppyMommy"]=arrayGuppyMommy;

    QJsonArray arrayCarnivore;
    for (auto fish:carnivore){
        QJsonObject objectCarnivore;
        (*fish).write(objectCarnivore);
        arrayCarnivore.append(objectCarnivore);
    }
    json["carnivore"]=arrayCarnivore;

    QJsonArray arraySimCarnivore;
    for (auto fish:simcarnivore){
        QJsonObject objectSimCarnivore;
        (*fish).write(objectSimCarnivore);
        arraySimCarnivore.append(objectSimCarnivore);
    }
    json["simcarnivore"]=arraySimCarnivore;

    QJsonArray arrayPet;
    for (auto pet:pets){
        QJsonObject objectPet;
        (*pet).write(objectPet);
        arrayPet.append(objectPet);
    }
    json["pets"]=arrayPet;

    QJsonArray arrayAliens;
    for (auto alien:aliens){
        QJsonObject objectAlien;
        (*alien).write(objectAlien);
        arrayAliens.append(objectAlien);
    }
    json["aliens"]=arrayAliens;

    QJsonArray arrayBubble;
    for (auto b:bubble){
        QJsonObject objectBubble;
        (*b).write(objectBubble);
        arrayBubble.append(objectBubble);
    }
    json["bubble"]=arrayBubble;

    QJsonArray arrayFood;
    for (auto food:foods){
        QJsonObject objectFood;
        (*food).write(objectFood);
        arrayFood.append(objectFood);
    }
    json["foods"]=arrayFood;

    QJsonArray arrayMoney;
    for (auto money:moneys){
        QJsonObject objectMoney;
        (*money).write(objectMoney);
        arrayMoney.append(objectMoney);
    }
    json["moneys"]=arrayMoney;
}

void IGameScene::read(const QJsonObject &json)
{
    QJsonObject objectGameData=json["gameData"].toObject();
    this->level=objectGameData["level"].toInt();
    this->score=objectGameData["score"].toInt();
    this->eggNum=objectGameData["eggNum"].toInt();
    this->hasAlien=objectGameData["hasAlien"].toBool();
    this->foodLimitation=objectGameData["foodLimitation"].toInt();
    this->foodClass=objectGameData["foodClass"].toInt();
    this->timeCounter=objectGameData["timeCounter"].toInt();

    alreadyopen.clear();
    QJsonArray arrayOpen=objectGameData["alreadyopen"].toArray();
    for (int i=0;i<arrayOpen.size();++i){
        QJsonObject objectOpen=arrayOpen[i].toObject();
        bool isopen=objectOpen["isopen"].toBool();
        alreadyopen.append(isopen);
    }
    //updateWindows();

    qDeleteAll(guppyFish.begin(),guppyFish.end());
    guppyFish.clear();
    QJsonArray arrayGuppyFish=json["guppyFish"].toArray();
    for (int i=0;i<arrayGuppyFish.size();++i){
        QJsonObject objectGuppyFish=arrayGuppyFish[i].toObject();
        GuppyFish* fish=new GuppyFish(0,0,0,this);//不能用对象初始化，否则函数结束会变成野指针
        (*fish).read(objectGuppyFish);
        guppyFish.append(fish);
    }

    qDeleteAll(guppyMommy.begin(),guppyMommy.end());
    guppyMommy.clear();
    QJsonArray arrayGuppyMommy=json["guppyMommy"].toArray();
    for (int i=0;i<arrayGuppyMommy.size();++i){
        QJsonObject objectGuppyMommy=arrayGuppyMommy[i].toObject();
        GuppyMommy* mommy=new GuppyMommy(0,0,0,this);
        (*mommy).read(objectGuppyMommy);
        guppyMommy.append(mommy);
    }

    qDeleteAll(carnivore.begin(),carnivore.end());
    carnivore.clear();
    QJsonArray arrayCarnivore=json["carnivore"].toArray();
    for (int i=0;i<arrayCarnivore.size();++i){
        QJsonObject objectCarnivore=arrayCarnivore[i].toObject();
        Carnivore* fish=new Carnivore(0,0,this);
        (*fish).read(objectCarnivore);
        carnivore.append(fish);
    }

    qDeleteAll(simcarnivore.begin(),simcarnivore.end());
    simcarnivore.clear();
    QJsonArray arraySimCarnivore=json["simcarnivore"].toArray();
    for (int i=0;i<arraySimCarnivore.size();++i){
        QJsonObject objectSimCarnivore=arraySimCarnivore[i].toObject();
        SimCarnivore* fish=new SimCarnivore(0,0,this);
        (*fish).read(objectSimCarnivore);
        simcarnivore.append(fish);
    }

    qDeleteAll(pets.begin(),pets.end());
    pets.clear();
    QJsonArray arrayPets=json["pets"].toArray();
    for (int i=0;i<arrayPets.size();++i){
        QJsonObject objectPet=arrayPets[i].toObject();
        IPet* pet=nullptr;
        switch (objectPet["id"].toInt()) {
        case IGameData::SNAIL:
            pet=new ISnail(0,0,this);
            break;
        case IGameData::SAILFISH:
            pet=new ISailFish(0,0,this);
            break;
        case IGameData::ZORF:
            pet=new IZorf(0,0,this);
            break;
        case IGameData::VERT:
            pet=new IFishbone(0,0,this);
            break;
        case IGameData::CLAM:
            pet=new IClam(220,220,this);
            break;
        default:
            break;
        }
        (*pet).read(objectPet);
        pets.append(pet);
    }

    qDeleteAll(aliens.begin(),aliens.end());
    aliens.clear();
    QJsonArray arrayAliens=json["aliens"].toArray();
    for (int i=0;i<arrayAliens.size();++i){
        QJsonObject objectAlien=arrayAliens[i].toObject();
        IAlien* alien=nullptr;
        switch (objectAlien["id"].toInt()) {
        case IGameData::SYL:
            alien=new ISylvester(0,0,this);
            break;
        case IGameData::GUS:
            alien=new IGus(0,0,this);
            break;
        case IGameData::BALROG:
            alien=new  IBalrog(0,0,this);
            break;
        default:
            break;
        }
        (*alien).read(objectAlien);
        aliens.append(alien);
    }

    qDeleteAll(bubble.begin(),bubble.end());
    bubble.clear();
    QJsonArray arrayBubble=json["bubble"].toArray();
    for (int i=0;i<arrayBubble.size();++i){
        QJsonObject objectBubble=arrayBubble[i].toObject();
        IBubble* b=new IBubble(0,0,this);
        (*b).read(objectBubble);
        bubble.append(b);
    }

    qDeleteAll(foods.begin(),foods.end());
    foods.clear();
    QJsonArray arrayFood=json["foods"].toArray();
    for (int i=0;i<arrayFood.size();++i){
        QJsonObject objectFood=arrayFood[i].toObject();
        IFood* food=new IFood(0,0,0,this);
        (*food).read(objectFood);
        foods.append(food);
    }

    qDeleteAll(moneys.begin(),moneys.end());
    moneys.clear();
    QJsonArray arrayMoney=json["moneys"].toArray();
    for (int i=0;i<arrayMoney.size();++i){
        QJsonObject objectMoney=arrayMoney[i].toObject();
        IMoney* money=nullptr;
        if (objectMoney["type"].toString()=="goldcoin"){
            money=new IGoldCoin(0,0,this);
        }
        else if (objectMoney["type"].toString()=="silvercoin"){
            money=new ISilverCoin(0,0,this);
        }
        else if (objectMoney["type"].toString()=="diamond"){
            money=new IDiamond(0,0,this);
        }
        else if (objectMoney["type"].toString()=="box"){
            money=new IBox(0,0,this);
        }
        else if (objectMoney["type"].toString()=="pearl"){
            money=new IPearl(0,0,this);
        }
        (*money).read(objectMoney);
        moneys.append(money);
    }
}

//回到本关开始处
void IGameScene::reset(QJsonObject& json)
{
    QJsonObject gamedataObject;
    gamedataObject["level"]=this->level;
    gamedataObject["score"]=100;
    gamedataObject["eggNum"]=0;
    gamedataObject["hasAlien"]=false;
    gamedataObject["foodLimitation"]=2;
    gamedataObject["foodClass"]=1;
    gamedataObject["timeCounter"]=0;
    QJsonArray arrayOpen;
    for (auto open:alreadyopen){
        QJsonObject objectOpen;
        objectOpen["isopen"]=false;
        arrayOpen.append(objectOpen);
    }
    gamedataObject["alreadyopen"]=arrayOpen;
    json["gameData"]=gamedataObject;
    json["language"]="en";
}


void IGameScene::updateScore(int shoot)
{
    score+=shoot;
    QString strScore=QString("$%1").arg(score);
    this->scoreBar->setText(strScore);
}

void IGameScene::initialWindows()
{
    QLabel* window1=new QLabel(this);
    window1->setGeometry(25,0,73,85);
    window1->show();
    windows.append(window1);
    QLabel* window2=new QLabel(this);
    window2->setGeometry(110,0,73,85);
    window2->show();
    windows.append(window2);
    QLabel* window3=new QLabel(this);
    window3->setGeometry(182,0,73,85);
    window3->show();
    windows.append(window3);
    QLabel* window4=new QLabel(this);
    window4->setGeometry(272,0,73,85);
    window4->show();
    windows.append(window4);
    QLabel* window5=new QLabel(this);
    window5->setGeometry(365,0,73,85);
    window5->show();
    windows.append(window5);
    QLabel* window6=new QLabel(this);
    window6->setGeometry(455,0,73,85);
    window6->show();
    windows.append(window6);
    QLabel* window7=new QLabel(this);
    window7->setGeometry(545,0,73,85);
    windows.append(window7);
    for (int i=0;i<windows.count();i++){
        QLabel* label1=new QLabel(windows[i]);
        QString filename1=QString("label1_%1").arg(i);
        label1->setObjectName(filename1);
        label1->setGeometry(10,0,53,63);

        QLabel* label2=new QLabel(windows[i]);
        QString filename2=QString("label2_%1").arg(i);
        label2->setObjectName(filename2);
        label2->setGeometry(10,63,47,17);
        windows[i]->setVisible(false);
        windows[i]->setScaledContents(true);
        windowopen.append(nullptr);
    }

    for (int i=0;i<windows.count();i++){
        QMovie* movie=nullptr;
        onSell.append(movie);
    }
    if (alreadyopen.isEmpty()){
        for (int i=0;i<windows.count();i++){
            bool open=false;
            alreadyopen.append(open);
        }
    }
    if (alreadyopen[0]){
        initialWindow1();
    }
    if (alreadyopen[1]){
        initialWindow2();
    }
    if (alreadyopen[2]){
        initialWindow3();
    }
    if (alreadyopen[3]){
        initialWindow4();
    }
    if (alreadyopen[4]){
        initialWindow5();
    }
    if (alreadyopen[5]){
        initialWindow6();
    }
    if (alreadyopen[6]){
        initialWindow7();
    }
}

void IGameScene::updateWindows()
{
    //有钱买鱼
    if (score>=100&&(!alreadyopen[0])){
        updateWindow1();
        alreadyopen[0]=true;
    }
    //有钱更新饲料
    if (foodClass<=2 && score>=IFood::classToPrice(foodClass+1)){
        updateWindow2();
    }
    //有钱增加饲料
    if (score>=IFood::addingPrice()&&(!alreadyopen[2])){
        updateWindow3();
    }
    //有钱买食肉鱼
    if (score>=Carnivore::getPrice() && (!alreadyopen[3])){
        updateWindow4();
        alreadyopen[3]=true;
    }
    //有钱买巨型食肉鱼
    if (score>=SimCarnivore::getPrice() && (!alreadyopen[4])){
        updateWindow5();
        alreadyopen[4]=true;
    }

    updateWindow6();
    if (score>=eggPrice && (!alreadyopen[6])){
        updateWindow7();
        alreadyopen[6]=true;
    }
}

void IGameScene::updateWindow1()
{

    QString css="font-weight:bold;font-family:sans_serif;font-size:20px;color:yellow;";
    windowopen[0]=new QMovie(":/background/resource/image/background/window.gif");
    windows[0]->setMovie(windowopen[0]);
    windowopen[0]->start();
    windows[0]->setVisible(true);

    QLabel* label1=windows[0]->findChild<QLabel*>("label1_0",Qt::FindDirectChildrenOnly);
    onSell[0]=new QMovie(":/smallguppy/resource/image/creature/fish/smallguppy/swimnormalleft.gif");//小古比鱼
    label1->setMovie(onSell[0]);
    onSell[0]->start();
    label1->setScaledContents(true);
    label1->show();

    QLabel* label2=windows[0]->findChild<QLabel*>("label2_0",Qt::FindDirectChildrenOnly);
    label2->setText("$100");
    label2->setStyleSheet(css);
    label2->setAlignment(Qt::AlignCenter);
    label2->show();
}

//饲料质量
void IGameScene::updateWindow2()
{
    QString css="font-weight:bold;font-family:sans_serif;font-size:20px;color:yellow;";
    if (!alreadyopen[1]){
        windowopen[1]=new QMovie(":/background/resource/image/background/window.gif");
        windows[1]->setMovie(windowopen[1]);
        windowopen[1]->start();
        windows[1]->setVisible(true);
        alreadyopen[1]=true;
    }

    QLabel* label1=windows[1]->findChild<QLabel*>("label1_1",Qt::FindDirectChildrenOnly);
    QString filename=QString(":/feed/resource/image/food/%1.gif").arg(foodClass+1);
    if (onSell[1]==nullptr){
        onSell[1]=new QMovie(filename);
    }
    else{
        onSell[1]->deleteLater();
        onSell[1]=new QMovie(filename);//鱼食
    }
    label1->setMovie(onSell[1]);
    onSell[1]->start();
    label1->setScaledContents(true);
    label1->show();

    QLabel* label2=windows[1]->findChild<QLabel*>("label2_1",Qt::FindDirectChildrenOnly);
    QString strPrice=QString("%1").arg(IFood::classToPrice(foodClass+1));
    label2->setText(strPrice);
    label2->setStyleSheet(css);
    label2->setAlignment(Qt::AlignCenter);
    label2->show();
}

//饲料数量
void IGameScene::updateWindow3()
{
    QString css="font-weight:bold;font-family:sans_serif;font-size:20px;color:yellow;";
    QString css1="font-weight:bold;font-family:sans_serif;font-size:40px;color:yellow;";
    if (!alreadyopen[2]){
        windowopen[2]=new QMovie(":/background/resource/image/background/window.gif");
        windows[2]->setMovie(windowopen[2]);
        windowopen[2]->start();
        windows[2]->setVisible(true);
        alreadyopen[2]=true;
    }

    QLabel* label1=windows[2]->findChild<QLabel*>("label1_2",Qt::FindDirectChildrenOnly);
    label1->setStyleSheet(css1);
    QString limitation=QString("%1").arg(foodLimitation);
    label1->setText(limitation);
    label1->setAlignment(Qt::AlignCenter);
    label1->setScaledContents(true);
    label1->show();

    QLabel* label2=windows[2]->findChild<QLabel*>("label2_2",Qt::FindDirectChildrenOnly);
    QString strPrice=QString("$%1").arg(IFood::addingPrice());
    label2->setText(strPrice);
    label2->setStyleSheet(css);
    label2->setAlignment(Qt::AlignCenter);
    label2->show();

}

void IGameScene::updateWindow4()
{
    QString css="font-weight:bold;font-family:sans_serif;font-size:20px;color:yellow;";
    windowopen[3]=new QMovie(":/background/resource/image/background/window.gif");
    windows[3]->setMovie(windowopen[3]);
    windowopen[3]->start();
    windows[3]->setVisible(true);

    QLabel* label1=windows[3]->findChild<QLabel*>("label1_3",Qt::FindDirectChildrenOnly);
    onSell[3]=new QMovie(":/carnivore/resource/image/creature/fish/carnivore/swimnormalleft.gif");
    label1->setMovie(onSell[3]);
    onSell[3]->start();
    label1->setScaledContents(true);
    label1->show();

    QLabel* label2=windows[3]->findChild<QLabel*>("label2_3",Qt::FindDirectChildrenOnly);
    label2->setText(QString("$%1").arg(Carnivore::getPrice()));
    label2->setStyleSheet(css);
    label2->setAlignment(Qt::AlignCenter);
    label2->show();
}

void IGameScene::updateWindow5()
{
    QString css="font-weight:bold;font-family:sans_serif;font-size:20px;color:yellow;";
    windowopen[4]=new QMovie(":/background/resource/image/background/window.gif");
    windows[4]->setMovie(windowopen[4]);
    windowopen[4]->start();
    windows[4]->setVisible(true);

    QLabel* label1=windows[4]->findChild<QLabel*>("label1_4",Qt::FindDirectChildrenOnly);
    onSell[4]=new QMovie(":/simcarnivore/resource/image/creature/fish/bigcarnivore/swimnormalleft.gif");
    label1->setMovie(onSell[4]);
    onSell[4]->start();
    label1->setScaledContents(true);
    label1->show();

    QLabel* label2=windows[4]->findChild<QLabel*>("label2_4",Qt::FindDirectChildrenOnly);
    label2->setText(QString("$%1").arg(SimCarnivore::getPrice()));
    label2->setStyleSheet(css);
    label2->setAlignment(Qt::AlignCenter);
    label2->show();
}

void IGameScene::updateWindow6()
{

}

void IGameScene::updateWindow7()
{
    QString css="font-weight:bold;font-family:sans_serif;font-size:20px;color:yellow;";
    windows[6]->setVisible(true);
    if (windowopen[6]==nullptr){
        windowopen[6]=new QMovie(":/background/resource/image/background/window.gif");
        windows[6]->setMovie(windowopen[6]);
    }
    windowopen[6]->start();

    QLabel* label1=windows[6]->findChild<QLabel*>("label1_6",Qt::FindDirectChildrenOnly);
    if (onSell[6]==nullptr){
        onSell[6]=new QMovie(":/egg/resource/image/egg/eggbought.gif");
        label1->setMovie(onSell[6]);
        label1->setScaledContents(true);
        label1->show();
    }
    onSell[6]->start();

    QLabel* label2=windows[6]->findChild<QLabel*>("label2_6",Qt::FindDirectChildrenOnly);
    label2->setText(QString("$%1").arg(eggPrice));
    label2->setStyleSheet(css);
    label2->setAlignment(Qt::AlignCenter);
    label2->show();
}

void IGameScene::initialWindow1()
{
    updateWindow1();
}

void IGameScene::initialWindow2()
{
    QString css="font-weight:bold;font-family:sans_serif;font-size:20px;color:yellow;";
    windowopen[1]=new QMovie(":/background/resource/image/background/window.gif");
    windows[1]->setMovie(windowopen[1]);
    windowopen[1]->start();
    windows[1]->setVisible(true);
    alreadyopen[1]=true;

    QLabel* label1=windows[1]->findChild<QLabel*>("label1_1",Qt::FindDirectChildrenOnly);
    QString filename=QString(":/feed/resource/image/food/%1.gif").arg(foodClass);
    onSell[1]=new QMovie(filename);
    label1->setMovie(onSell[1]);
    onSell[1]->start();
    label1->setScaledContents(true);
    label1->show();

    QLabel* label2=windows[1]->findChild<QLabel*>("label2_1",Qt::FindDirectChildrenOnly);
    QString strPrice=QString("%1").arg(IFood::classToPrice(foodClass));
    label2->setText(strPrice);
    label2->setStyleSheet(css);
    label2->setAlignment(Qt::AlignCenter);
    label2->show();
}

void IGameScene::initialWindow3()
{
    QString css="font-weight:bold;font-family:sans_serif;font-size:20px;color:yellow;";
    QString css1="font-weight:bold;font-family:sans_serif;font-size:40px;color:yellow;";
    windowopen[2]=new QMovie(":/background/resource/image/background/window.gif");
    windows[2]->setMovie(windowopen[2]);
    windowopen[2]->start();
    windows[2]->setVisible(true);
    alreadyopen[2]=true;

    QLabel* label1=windows[2]->findChild<QLabel*>("label1_2",Qt::FindDirectChildrenOnly);
    label1->setStyleSheet(css1);
    QString limitation=QString("%1").arg(foodLimitation);
    label1->setText(limitation);
    label1->setAlignment(Qt::AlignCenter);
    label1->setScaledContents(true);
    label1->show();

    QLabel* label2=windows[2]->findChild<QLabel*>("label2_2",Qt::FindDirectChildrenOnly);
    QString strPrice=QString("$%1").arg(IFood::addingPrice());
    label2->setText(strPrice);
    label2->setStyleSheet(css);
    label2->setAlignment(Qt::AlignCenter);
    label2->show();
}

void IGameScene::initialWindow4()
{
    updateWindow4();
}

void IGameScene::initialWindow5()
{
    updateWindow5();
}

void IGameScene::initialWindow6()
{

}

void IGameScene::initialWindow7()
{
    QString css="font-weight:bold;font-family:sans_serif;font-size:20px;color:yellow;";
    windows[6]->setVisible(true);
    windowopen[6]=new QMovie(":/background/resource/image/background/window.gif");
    windows[6]->setMovie(windowopen[6]);
    windowopen[6]->start();

    QLabel* label1=windows[6]->findChild<QLabel*>("label1_6",Qt::FindDirectChildrenOnly);
    onSell[6]=new QMovie(":/egg/resource/image/egg/eggbought.gif");
    label1->setMovie(onSell[6]);
    label1->setScaledContents(true);
    label1->show();
    onSell[6]->start();

    QLabel* label2=windows[6]->findChild<QLabel*>("label2_6",Qt::FindDirectChildrenOnly);
    label2->setText(QString("$%1").arg(eggPrice));
    label2->setStyleSheet(css);
    label2->setAlignment(Qt::AlignCenter);
    label2->show();
}

void IGameScene::pause()
{
    timer->stop();
}

void IGameScene::goon()
{
    timer->start();
}

void ISelectPetScene::refreshText()
{

}

ISelectPetScene::ISelectPetScene(QWidget *parent):IScene(parent)
{
    this->setGeometry(0,0,800,600);
    this->setObjectName("bg");
    this->setStyleSheet("#bg{border-image:"
                        "url(:/background/resource/image/background/fishselector.gif);}");
    this->show();

    hint=new QLabel(this);
    hint->setGeometry(260,20,260,70);
    QFont font(dialog->f);
    font.setPointSize(12);
    hint->setFont(font);
    hint->setText(tr("You can carry not more than 3 pets to next level"));
    hint->setWordWrap(true);
    hint->setStyleSheet("color:white");
    hint->show();

    petBox=new QLabel(this);
    petBox->setObjectName("petBox");
    petBox->setGeometry(280,100,220,440);
    petBox->setStyleSheet("#petBox{border-image:"
                           "url(:/interface/resource/image/background/fishbox.gif);}");
    petBox->show();

    petPic=new QLabel(petBox);
    petPic->setGeometry(20,20,180,180);
    petPic->setScaledContents(true);
    petPic->show();

    btnContinue=new MyButton(":/interface/resource/image/background/btnblue.gif",petBox);
    btnContinue->setGeometry(20,200,180,50);
    btnContinue->setText(tr("CONTINUE"));
    btnContinue->setHoverPixmap(":/interface/resource/image/background/btnblue.gif");
    btnContinue->setPressedPixmap(":/interface/resource/image/background/btnbluedown.gif");
    btnContinue->setReleasedPixmap(":/interface/resource/image/background/btnblue.gif");
    btnContinue->show();

    for (int i=0;i<qMin(dialog->level,
                        IGameData::totalDeservedPets.length()-1);i++){
        buttons.append(new MyButtonMulti(":/interface/resource/image/background/btnselectorleave.png",this));
        int col=i/5;
        int row=i%5;
        int starty=row*100+100;
        int startx=col<=1?col*100+100:col*100+300;
        buttons[i]->setGeometry(startx,starty,80,85);
        buttons[i]->setHoverPixmap(":/interface/resource/image/background/btnselectorhover.png");
        buttons[i]->setPressedPixmap(":/interface/resource/image/background/btnselectordown.png");
        buttons[i]->setReleasedPixmap(":/interface/resource/image/background/btnselectorhover.png");
        connect(buttons[i],SIGNAL(clicked(bool)),this,SLOT(clickPet()));
        buttons[i]->show();
    }
    for (int i=qMin(dialog->level,
                    IGameData::totalDeservedPets.length()-1);i<20;i++){
        buttons.append(new MyButtonMulti(":/interface/resource/image/background/btnselectorleave.png",this));
        int col=i/5;
        int row=i%5;
        int starty=row*100+100;
        int startx=col<=1?col*100+100:col*100+300;
        buttons[i]->setGeometry(startx,starty,80,85);
        buttons[i]->setHoverPixmap(":/interface/resource/image/background/btnselectorleave.png");
        buttons[i]->setPressedPixmap(":/interface/resource/image/background/btnselectorleave.png");
        buttons[i]->setReleasedPixmap(":/interface/resource/image/background/btnselectorleave.png");
        buttons[i]->show();
    }
    connect(btnContinue,&QPushButton::clicked,[=]{
        QList<int> chosen;
       for (int i=0;i<qMin(dialog->level,
                           IGameData::totalDeservedPets.length()-1);i++){
           if (buttons[i]->isChosen){
               chosen.append(i);
           }
       }
       if (chosen.count()<3){
           less3Warning(chosen);
       }
       else{
           emit petsChosen(chosen);
       }
    });

    petDescription=new QLabel(petBox);
    petDescription->setGeometry(20,250,180,140);
    petDescription->setWordWrap(true);
    petDescription->setStyleSheet("color:white");
    QFont font1(dialog->f);
    font1.setPointSize(14);
    petDescription->setFont(font1);
    petDescription->show();
    //获取当前的所有宠物
    for (int i=1;i<qMin(dialog->level+1,
                        IGameData::totalDeservedPets.length());i++){
        QLabel* newLabel=new QLabel(buttons[i-1]);
        newLabel->setGeometry(20,20,40,40);
        QMovie* newMovie=nullptr;
        newMovie=new QMovie(IGameData::petNameList[
                            IGameData::totalDeservedPets[i]]);
        newLabel->setMovie(newMovie);
        petLables.append(newLabel);
        petMovies.append(newMovie);
        newMovie->start();
        newLabel->setScaledContents(true);
        newLabel->show();
    }
}

ISelectPetScene::~ISelectPetScene()
{
    while(!(petMovies.isEmpty())){
        delete petMovies[0];
        petMovies.removeAt(0);
    }
    delete petPicMovie;
}


void ISelectPetScene::clickPet()
{
    //已经选中3只则不能再选
    MyButtonMulti* button=static_cast<MyButtonMulti*>(sender());
    if (button->isChosen){
        chosenNum++;
    }
    else{
        chosenNum--;
    }
    if (chosenNum>=3){
        for (int i=0;i<20;i++){
            if (!(buttons[i]->isChosen)){
                buttons[i]->setEnabled(false);
            }
        }
    }
    else{
        for (int i=0;i<20;i++){
            buttons[i]->setEnabled(true);
        }
    }
    if (button->isChosen){
        //显示相应宠物的信息
        int index=buttons.indexOf(button);
        delete petPicMovie;
        petPicMovie=new QMovie(IGameData::petNameList[index]);
        petPic->setMovie(petPicMovie);
        petPicMovie->start();
        switch (index) {
        case IGameData::SNAIL:
            petDescription->setText(tr("THIS IS STIKY.IT WILL CATCH THE MONEY YOU DROP."));
            break;
        case IGameData::SAILFISH:
            petDescription->setText(tr("THIS IS ITCHY. IT WILL HELP YOU ATTACK THE ENEMY."));
            break;
        case IGameData::ZORF:
            petDescription->setText(tr("THIS IS ZORF. IT WILL GENERATE FOOD TO SURVIVE YOUR FISH."));
            break;
        case IGameData::VERT:
            petDescription->setText(tr("THIS IS VERT. IT WILL GENERATE GOLD COIN FOR YOU."));
            break;
        case IGameData::CLAM:
            petDescription->setText(tr("THIS IS NIKO. IT WILL GENERATE A PEARL FOR YOU."));
            break;
        default:
            break;
        }
    }
}

//选择的宠物少于3只警告
void ISelectPetScene::less3Warning(QList<int>& chosen)
{
    QString msg=tr("Are your sure you want to choose less than ")+
            QString("%1").arg(chosen.count())+tr(" pets?");
    QMessageBox warning(this);
    warning.setText(msg);
    QAbstractButton *pYesBtn = (QAbstractButton *)warning.addButton(tr("Yes"), QMessageBox::YesRole);
    QAbstractButton *pNoBtn = (QAbstractButton *)warning.addButton(tr("No"), QMessageBox::NoRole);
    warning.exec();
    if (warning.clickedButton()==pYesBtn){
        emit petsChosen(chosen);
    }
}
