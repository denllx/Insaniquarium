#ifndef MAINDIALOG_H
#define MAINDIALOG_H
#include <QTranslator>
#include <QDialog>
#include "iscene.h"
#include "igamedata.h"

namespace Ui {
class mainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();
    QString username;
    QString resourceDir;
    IGameData* getData(){return data;}
    int level=1;
    bool firstPlayed=false;//是否第一次玩（没有游戏记录）
    QFont f;
    QString language="en";

private slots:
    void switchToLoading(const QString& uesrname);//切换到加载界面
    void switchSelector();//切换到主界面
    void switchToHelp();//切换到帮助界面
    void swithToWelcome();//切换到欢迎界面
    void nextLevel();//通关
    void thisLevel();
    void restart();//鱼死光后从头开始
    //void resume();//中途退出后重来，但是保留进度
    //void resumeGame();
    void switchToPet();//通关后认识宠物
    void swithToSelectPet();//挑选宠物
    void petsChosenToNextLevel(QList<int>&);//挑选宠物后通关
    void closeEvent(QCloseEvent* event);
    void firstTimeGame();//加载后首次游戏
    void changeTr(const QString&);//修改语言

private:
    void read(const QJsonObject& json);
    void loadGame();
    void write(QJsonObject& json);
    void saveGame();
    void refreshText();
    Ui::mainDialog *ui;
    IScene* scene;
    IScene* newScene;//在中途退出时暂时指向主界面
    IGameData* data;
    const int WELCOME=1;
    const int LOADING=2;
    const int SELECTOR=3;
    const int GAME=4;
    const int PET=5;
    const int SELECTPET=6;
    const int HELP=7;
    int status=WELCOME;
    static QTranslator* translator;
    QString savejsonName;
};

#endif // MAINDIALOG_H
