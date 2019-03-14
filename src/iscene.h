#ifndef ISCENE_H
#define ISCENE_H
#include <QLabel>
#include <QMovie>
#include <QTimer>
#include "ifish.h"
#include "ifood.h"
#include "imoney.h"
#include "ialien.h"
#include "ipet.h"
#include "dlginputname.h"
#include "ibubble.h"
#include "button.h"

class MainDialog;

//游戏场景类
class IScene : public QLabel
{
    Q_OBJECT
public:
    explicit IScene(QWidget* parent=0);
    MainDialog* dialog;

protected:
    virtual void refreshText()=0;

private:
};

//欢迎界面
class IWelcomeScene:public IScene{
    Q_OBJECT

signals:
    void enterLoading(const QString&);

public:
    explicit IWelcomeScene(QWidget* parent=0);
    ~IWelcomeScene();
    QMovie* movie1=new QMovie(":/background/resource/image/background/click.png");
    QMovie* movie=new QMovie(":/background/resource/image/background/titlescreen.jpg");
    QLabel* clickHere;
    bool eventFilter(QObject *watched, QEvent *event);//监听开始按钮
protected:
    void refreshText() override;
private slots:
    void back();
    void getUsername(const QString& username);
private:
    DlgInputName* dlg=nullptr;


};

//加载界面
class ILoadingScene:public IScene{
    Q_OBJECT

signals:
    void enterSelector();//进入主界面

private slots:
    void onTimer();

protected:
    void refreshText() override;
public:
    explicit ILoadingScene(QWidget* parent=0);
    ~ILoadingScene();
    QTimer* timer;
    QLabel* loadingText,*loadingPic,*loadingNum;
    QMovie* textMovie;
    QMovie* loadingMovie;
    int progress=0;
    int timeCounter=0;
};

//主界面
class ISelectorScene:public IScene{
    Q_OBJECT

signals:
    void enterGame();//进入游戏
    void enterHelp();//进入帮助
    void enterWelcome();
    void changeTr(const QString&);

protected:
    void refreshText() override;
public:
    explicit ISelectorScene(QWidget* parent=0);
    ~ISelectorScene();
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QMovie* movie=new QMovie(":/background/resource/image/background/selectorback.jpg");
    QLabel* labelDialog;
    MyButton* advanture,*timeTrial,*challenge,*virtualTank;
    MyButton* hallOfGame,*options,*help,*quit;
};

//帮助
class IHelpScene:public IScene{
    Q_OBJECT

public:
    explicit IHelpScene(QWidget* parent=0);
    ~IHelpScene();
    void setCurrentPage(int page);
    bool eventFilter(QObject *watched, QEvent *event);
protected:
    void refreshText() override;
signals:
    void enterSelector();

private:
    int currentPage=1;
    QMovie* movie;
    MyButton *back,*previous,*next;
};

//认识宠物
class IPetScene:public IScene{
    Q_OBJECT
public:
    explicit IPetScene(QWidget* parent=0);
    ~IPetScene();
signals:
    void enterGame();
    void enterSelectPet();
protected:
    void refreshText() override;
private:
    QMovie* movie=new QMovie(":/background/resource/image/background/hatchscreen.gif");
    QMovie* petMovie;
    QMovie* menuMovie=new QMovie(":/interface/resource/image/background/button.gif");
    MyButton* continueLabel;
    QLabel* petLabel;
    QLabel* petDescription;
    QString prefix=":/pet/resource/image/creature/pet/";
};

//游戏开始
class IGameScene:public IScene{
    Q_OBJECT
public:
    explicit IGameScene(int level, QWidget* parent=0,bool firstTime=false,bool firstPlayed=false);
    //firsttime表示是否为加载后的第一场,之前可能有玩过
    //firstPlayed表示是否第一次玩,即是否有游戏记录
    ~IGameScene();
    void shift();
    void saveGame();
    void loadGame();
    void resetGame();
    void updateScore(int shoot);
    void initialWindows();
    void updateWindows();//更新上方的窗口
    void updateWindow1();
    void updateWindow2();
    void updateWindow3();
    void updateWindow4();
    void updateWindow5();
    void updateWindow6();
    void updateWindow7();
    void initialWindow1();
    void initialWindow2();
    void initialWindow3();
    void initialWindow4();
    void initialWindow5();
    void initialWindow6();
    void initialWindow7();
    void pause();
    void goon();

    QList<GuppyFish*> guppyFish;
    QList<IFood*> foods;
    QList<IMoney*> moneys;
    QList<GuppyMommy*> guppyMommy;
    QList<Carnivore*> carnivore;
    QList<SimCarnivore*> simcarnivore;
    QList<IAlien*> aliens;
    QList<IPet*> pets;
    int score=100;

signals:
    void restart();
    //void resume();
    void enterPet();
    void enterSelector();

private slots:
    void onTimer();
    void alienCome();

protected:
    void mousePressEvent(QMouseEvent* event);
    void refreshText() override;

private:
    QMovie* movie;
    QMovie* barMovie=new QMovie(":/interface/resource/image/background/menubar.gif");
    QTimer* timer=nullptr;
    QLabel* boxbar,*labelWarning;
    MyButton* menu;
    QLabel* scoreBar;
    QList<QLabel*> windows;
    QList<QMovie*> windowopen;
    QList<QMovie*> onSell;
    QList<bool> alreadyopen;
    QList<IBubble*> bubble;
    int foodLimitation=2;
    int foodClass=1;
    int eggPrice=1000;
    int eggNum=0;
    int timeAlienComing=1500;//每60秒一个alien
    int timeCounter=0;
    int level;
    bool firstTime;//是否为第一次加载
    bool firstPlayed;
    bool hasAlien=false;

    void menuClicked();//退出游戏时警告
    void failWarning();//鱼全部死光
    void removeDeath();
    void checkAlien();
    void notifyAlienComing();
    void notifyAlienDie();
    void updateBubble();
    void judge();
    void write(QJsonObject& json);
    void read(const QJsonObject &json);
    void reset(QJsonObject &json);
    void initAnimal();
    void initLayout();

    template<typename T>
    void IGameScene::removeDeathByGroup(QList<T *> &group)
    {
        int p = 0;
        while (p < group.count())
        {
            if (!(group[p]->exists))
            {
                delete (group[p]);
                group.removeAt(p);
            }
            else
            {
                p++;
            }
        }
    }
};


//选择宠物
class ISelectPetScene:public IScene{
    Q_OBJECT

public:
    explicit ISelectPetScene(QWidget* parent=0);
    ~ISelectPetScene();
signals:
    void petsChosen(QList<int>&);

private slots:
    void clickPet();

protected:
    void refreshText() override;

private:
    QLabel* petBox,*petPic,*petDescription,*hint;
    QMovie* petPicMovie=nullptr;
    QList<MyButtonMulti*> buttons;
    MyButton* btnContinue;
    QList<QMovie*> petMovies;
    QList<QLabel*> petLables;
    int chosenNum=0;
    void less3Warning(QList<int>& chosen);
};


#endif // ISCENE_H
