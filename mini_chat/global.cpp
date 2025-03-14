#include "global.h"
#include "QCryptographicHash"
std::function<void(QWidget*)> repolish = [](QWidget* w){
    w->style()->unpolish(w);
    w->style()->polish(w);
};

std::function<QString(QString)> xorString = [](QString input)
{
    QString result = input;
    int length = result.length();
    length = length % 255;

    for(int i = 0; i < length; i++)
    {
        result[i] = QChar(static_cast<ushort>(input[i].unicode() ^ static_cast<ushort>(length)));
    }

    return result;
};

QString gate_url_prefix = "";

std::function<QString(const QString&)> md5Encrypt = [](const QString& input){
    QByteArray array = input.toUtf8();
    QByteArray hash = QCryptographicHash::hash(array, QCryptographicHash::Md5);
    return QString(hash.toHex());
};

std::vector<QString> strs ={"hello world !",
    "nice to meet u",
    "New year，new life",
    "You have to love yourself",
    "My love is written in the wind ever since the whole world is you"
};

std::vector<QString> heads = {
    ":/icons/icon/博丽灵梦.png",
    ":/icons/icon/魔理沙.png",
    ":/icons/icon/琪露诺.png",
    ":/icons/icon/东风谷早苗.png",
    ":/icons/icon/古明地恋.png",
    ":/icons/icon/古明地觉.png",
    ":/icons/icon/蕾米莉亚.png",
    ":/icons/icon/芙兰朵露.png",
    ":/icons/icon/帕秋莉.png",
    ":/icons/icon/十六夜咲夜.png",
    ":/icons/icon/红美玲.png",
    ":/icons/icon/幽幽子.png",
    ":/icons/icon/魂魄妖梦.png",
    ":/icons/icon/射命丸文.png",
};
std::vector<QString> names = {
    "博丽灵梦",
    "魔理沙",
    "琪露诺",
    "东风谷早苗",
    "古明地恋",
    "古明地觉",
    "蕾米莉亚",
    "芙兰朵露",
    "帕秋莉",
    "十六夜咲夜",
    "红美玲",
    "幽幽子",
    "魂魄妖梦",
    "射命丸文"
};
