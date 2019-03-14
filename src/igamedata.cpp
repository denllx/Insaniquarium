#include "igamedata.h"

QString IGameData::prefix=":/pet/resource/image/creature/pet/";
QStringList IGameData::petNameList={IGameData::prefix+"snailmoveleft.gif",
                                     IGameData::prefix+"sailfishmoveleft.gif",
                                     IGameData::prefix+"zorfmoveleft.gif",
                                     IGameData::prefix+"vertmoveleft.gif",
                                    IGameData::prefix+"nikostatic.gif"};
QStringList IGameData::petDescription={"THIS IS STIKY.IT WILL CATCH THE MONEY YOU DROP.",
                                        "THIS IS ITCHY. IT WILL HELP YOU ATTACK THE ENEMY.",
                                        "THIS IS ZORF. IT WILL GENERATE FOOD TO SURVIVE YOUR FISH.",
                                        "THIS IS VERT. IT WILL GENERATE GOLD COIN FOR YOU.",
                                        "THIS IS NIKO. IT WILL GENERATE A PEARL FOR YOU."};

const QList<int> IGameData::totalDeservedPets={-1,
                                    IGameData::SNAIL,
                                    IGameData::SAILFISH,
                                    IGameData::ZORF,
                                    IGameData::VERT,
                                    IGameData::CLAM,
                                              };
const QList<int> IGameData::totalAliens={IGameData::SYL,
                                          IGameData::GUS,
                                          IGameData::BALROG,
                                          IGameData::SYL,
                                          IGameData::GUS,
                                          IGameData::BALROG,
                                         IGameData::SYL,
                                         IGameData::GUS,
                                         IGameData::BALROG,
                                         IGameData::SYL,
                                         IGameData::GUS,
                                         IGameData::BALROG,
                                         IGameData::SYL,
                                         IGameData::GUS,
                                         IGameData::BALROG,
                                         IGameData::SYL,
                                         IGameData::GUS,
                                         IGameData::BALROG,
                                         IGameData::SYL,
                                         IGameData::GUS,
                                         };
IGameData::IGameData()
{

}
