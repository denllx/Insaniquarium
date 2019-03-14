#ifndef IGAMEDATA_H
#define IGAMEDATA_H
#include <QObject>
#include <QList>

class IGameData:public QObject{
    Q_OBJECT

public:
    explicit IGameData();
    //宠物编号
    static const int SNAIL=0;
    static const int SAILFISH=1;
    static const int ZORF=2;
    static const int VERT=3;
    static const int CLAM=4;
    //宠物路径名
    static  QString prefix;
    static  QStringList petNameList;
    //宠物介绍
    static  QStringList petDescription;
    //怪物编号
    static const int SYL=10;
    static const int GUS=11;
    static const int BALROG=12;
    //每关获得宠物列表
    static const QList<int> totalDeservedPets;
    //每关怪兽列表
    static const QList<int> totalAliens;
    //当前拥有宠物列表
    QList<int> totalPets;
    //当前选择宠物列表
    QList<int> chosenPets;
    //所有贴图的尺寸
    static const int FOODWIDTH=40;
    static const int FOODHEIGHT=40;
    static const int MONEYWIDTH=72;
    static const int MONEYHEIGHT=72;
    static const int ALIENWIDTH=160;
    static const int ALIENHEIGHT=160;
    static const int GUPPYFISHWIDTH=80;
    static const int GUPPYFISHHEIGHT=80;
    static const int GUPPYMOMMYWIDTH=80;
    static const int GUPPYMOMMYHEIGHT=80;
    static const int CARNIVOREWIDTH=80;
    static const int CARNIVOREHEIGHT=80;
    static const int SIMCARNIVOREWIDTH=160;
    static const int SIMCARNIVOREHEIGHT=160;
    static const int SNAILWIDTH=80;
    static const int SNAILHEIGHT=80;
    static const int SNAILFISHWIDTH=60;
    static const int SNAILFISHHEIGHT=60;
    static const int ZORFWIDTH=80;
    static const int ZORTHEIGHT=80;
    static const int FISHBONEWIDTH=80;
    static const int FISHBONEHEIGHT=80;
    static const int CLAMWIDTH=80;
    static const int CLAMHEIGHT=80;
    //边缘距离
    static const int COINUPPER=20;
    static const int DIAMONDUPPER=25;
    static const int BOXUPPER=16;
    static const int GUPPYFISHUPPER=26;
    static const int GUPPYFISHLEFT=19;
    static const int MEDIUMGUPPYUPPER=18;
    static const int MEDIUMGUPPYLEFT=13;
    static const int BIGGUPPYUPPER=11;
    static const int GUPPYMOMMYLEFT=19;
    static const int GUPPYMOMMYUPPER=7;
    static const int GUPPYMOMMYBOTTOM=28;
    static const int BIGGUPPYMOMMYUPPER=0;
    static const int BIGGUPPYMOMMYLEFT=13;
    static const int BIGGUPPYMOMMYBOTTOM=21;
    static const int CARNIVOREUPPER=9;
    static const int CARNIVOREBOTTOM=19;
    static const int BIGCARNIVOREUPPER=28;
    static const int BIGCARNIVOREBOTTOM=28;
    static const int FOODUPPER=15;
    static const int FOODLEFT=9;

};


#endif // IGAMEDATA_H
