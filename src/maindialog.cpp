#include "maindialog.h"
#include "ui_maindialog.h"
#include "iscene.h"
#include <QMovie>
#include <QDir>
#include <QDebug>
#include <QTime>
#include <QJsonDocument>
#include <QTranslator>
#include <QApplication>

QTranslator* MainDialog::translator=nullptr;

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mainDialog)
{
    ui->setupUi(this);
    this->setMouseTracking(true);
    this->setFixedSize(800,600);
    f=QFont("'Times New Roman','Microsft Yahei'",9,75);
    qApp->setFont(f);
    data=new IGameData();
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    loadGame();
    refreshText();
    this->setWindowTitle(tr("Insaniquarium"));
    scene=new IWelcomeScene(this);//呈现欢迎界面
    connect(scene,SIGNAL(enterLoading(const QString&)),
            this,SLOT(switchToLoading(const QString&)));//欢迎->加载
}

MainDialog::~MainDialog()
{
    delete ui;
}


//进入加载界面
void MainDialog::switchToLoading(const QString &username)
{
    this->setWindowTitle(tr("Loading"));
    status=LOADING;
    this->username=username;
    scene->deleteLater();
    scene=new ILoadingScene(this);//呈现加载界面
    connect(scene,SIGNAL(enterSelector()),this,SLOT(switchSelector()));
}

void MainDialog::switchSelector()
{
    this->setWindowTitle(tr("Selector"));
    status=SELECTOR;
    scene->deleteLater();
    scene=new ISelectorScene(this);
    connect(scene,SIGNAL(enterHelp()),this,SLOT(switchToHelp()));
    connect(scene,SIGNAL(enterGame()),this,SLOT(firstTimeGame()));
    connect(scene,SIGNAL(enterWelcome()),this,SLOT(swithToWelcome()));
    connect(scene,SIGNAL(changeTr(const QString&)),this,SLOT(changeTr(const QString&)));
}

void MainDialog::switchToHelp()
{
    this->setWindowTitle(tr("Help"));
    status=HELP;
    scene->deleteLater();
    scene=new IHelpScene(this);
    connect(scene,SIGNAL(enterSelector()),this,SLOT(switchSelector()));
}

//重置数据
void MainDialog::swithToWelcome()
{
    this->setWindowTitle(tr("Insaniquarium"));
    username=nullptr;
    firstPlayed=true;
    level=1;
    loadGame();
    refreshText();
    status=WELCOME;
    scene->deleteLater();
    scene=new IWelcomeScene(this);
    connect(scene,SIGNAL(enterLoading(const QString&)),
            this,SLOT(switchToLoading(const QString&)));
}

void MainDialog::nextLevel()
{
    this->setWindowTitle(tr("Game"));
    status=GAME;
    scene->deleteLater();
    scene=new IGameScene(++level,this);
    connect(scene,SIGNAL(restart()),this,SLOT(restart()));
    connect(scene,SIGNAL(enterPet()),this,SLOT(switchToPet()));
    connect(scene,SIGNAL(enterSelector()),this,SLOT(switchSelector()));
}

void MainDialog::thisLevel()
{
    this->setWindowTitle(tr("Game"));
    status=GAME;
    scene->deleteLater();
    scene=new IGameScene(level,this);
    connect(scene,SIGNAL(restart()),this,SLOT(restart()));
    connect(scene,SIGNAL(enterPet()),this,SLOT(switchToPet()));
    connect(scene,SIGNAL(enterSelector()),this,SLOT(switchSelector()));

}

void MainDialog::restart()
{
    this->setWindowTitle(tr("Game"));
    status=GAME;
    scene->deleteLater();
    scene=new IGameScene(level,this);
    connect(scene,SIGNAL(restart()),this,SLOT(restart()));
    connect(scene,SIGNAL(enterPet()),this,SLOT(switchToPet()));
    connect(scene,SIGNAL(enterSelector()),this,SLOT(switchSelector()));

}

void MainDialog::firstTimeGame()
{
    this->setWindowTitle(tr("Game"));
    status=GAME;
    scene->deleteLater();
    //从主界面进入游戏2种方式：第一次玩进入；中途退出后继续。都需要加载json文件
    scene=new IGameScene(level,this,true,firstPlayed);
    firstPlayed=false;//进入过一次游戏场景后，就不能没有游戏记录
    connect(scene,SIGNAL(restart()),this,SLOT(restart()));
    connect(scene,SIGNAL(enterPet()),this,SLOT(switchToPet()));
    connect(scene,SIGNAL(enterSelector()),this,SLOT(switchSelector()));

}

void MainDialog::changeTr(const QString & langCode)
{
    this->language=langCode;
    if(translator != nullptr)
    {
        qApp->removeTranslator(translator);
        delete translator;
        translator = nullptr;
    }
    translator = new QTranslator;
    QString lang=QString(":/translation/resource/translations/%1.qm")
            .arg(langCode);
    if ( translator->load(lang)){
        qApp->installTranslator(translator);
    }
}

void MainDialog::refreshText(){
    translator = new QTranslator;
    QString lang=QString(":/translation/resource/translations/%1.qm")
            .arg(this->language);
    if ( translator->load(lang)){
        qApp->installTranslator(translator);
    }
}


void MainDialog::switchToPet()
{
    //前5关进pet
    if (level<=IGameData::totalDeservedPets.length()-1){
        scene->deleteLater();
        this->setWindowTitle(tr("Pet"));
        status=PET;
        scene=new IPetScene(this);
        connect(scene,SIGNAL(enterGame()),this,SLOT(nextLevel()));
        connect(scene,SIGNAL(enterSelectPet()),this,SLOT(swithToSelectPet()));
    }
    //否则没有新宠物，直接进入宠物挑选环节
    else{
        swithToSelectPet();
    }
}

void MainDialog::swithToSelectPet(){
    scene->deleteLater();
    this->setWindowTitle(tr("Select pets"));
    status=SELECTPET;
    scene=new ISelectPetScene(this);
    connect(scene,SIGNAL(petsChosen(QList<int>&)),
            this,SLOT(petsChosenToNextLevel(QList<int>&)));
}

void MainDialog::petsChosenToNextLevel(QList<int> & pets)
{
    data->chosenPets.clear();
    for (int pet:pets){
        data->chosenPets.append(pet);
    }
    this->nextLevel();
}

void MainDialog::closeEvent(QCloseEvent *event)
{
    if (status==GAME){
        IGameScene* gameScene=static_cast<IGameScene*>(scene);
        gameScene->saveGame();
    }
    else if (status==PET || status==SELECTPET){
        event->ignore();
    }
    else{
        saveGame();
    }
}


void MainDialog::read(const QJsonObject &json)
{
    if (json.contains("gameData")){
        QJsonObject objectGameData=json["gameData"].toObject();
        this->level=objectGameData["level"].toInt();
    }
    else{//真正第一次启动时没有任何数据
        this->level=1;
        firstPlayed=true;
    }
    QJsonObject userObject=json["user"].toObject();
    this->username=userObject["name"].toString();
    if (!(json.contains("guppyFish"))){//重置后第一次启动
        firstPlayed=true;
    }
    if (json["language"].toString()!=nullptr){
        this->language=json["language"].toString();
    }
    qDebug()<<"after read, firstplayed="<<firstPlayed;
}

void MainDialog::loadGame()
{
    QFile loadFile("save.json");
    if (!loadFile.open(QIODevice::ReadOnly|QIODevice::WriteOnly)) {
        qWarning("Couldn't open file in maindialog:loadgame");
    }
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    read(loadDoc.object());
    loadFile.close();
}
void MainDialog::saveGame(){

    QFile saveFile("save.json");
    if (!saveFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open file in maindialog:savegame");
        return;
    }
    QByteArray saveData = saveFile.readAll();//将文件中的数据转换为字节流
    QJsonDocument loadDoc(QJsonDocument::fromJson((saveData)));//根据文件的格式将字节流解析为json或者binarydata
    QJsonObject object=loadDoc.object();
    if (object.contains("language")){
        object.remove("language");
    }
    object.insert("language",this->language);
    saveFile.close();
    if (!saveFile.open(QIODevice::WriteOnly|QIODevice::Truncate)){
        qWarning("Couldn't open save file");
        return;
    }
    QJsonDocument saveDoc(object);
    saveFile.write(saveDoc.toJson());
    saveFile.close();
}

void MainDialog::write(QJsonObject &json)
{


}
