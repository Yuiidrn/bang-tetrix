#ifndef BLOCK_H
#define BLOCK_H

#include <QImage>
using namespace std;

/*----乐队添加----*/
const int BAND_NUM = 9; //乐队个数
const int CHAR_NUM = 5; //成员个数
const QString bandList[BAND_NUM] = {
    "Poppin_Party",   //0 万物起源
    "Afterglow",
    "Pastel_Palettes",
    "Roselia",
    "Hello_Happy_World",
    "RAISE_A_SUILEN",
    "Morfonica",
    "MyGO",
    "Ave_Mujica"
 };
const QString nameList[BAND_NUM][CHAR_NUM] = {
    {"ksm", "ars", "saaya", "otae", "rimi"},
    {"ran", "moka", "hmr", "tomoe", "tsugu"},
    {"aya", "hina", "cst", "maya", "eve"},
    {"ykn", "lisa", "sayo", "rinko", "ako"},
    {"kkr", "kaoru", "hgm", "kanon", "msk"},
    {"chu2", "layer", "lock", "mask", "pareo"},
    {"msr", "nnm", "rui", "toko", "tsk"},
    {"tmr", "soyorin", "anon", "rikki", "rana"},
    {"saki", "mutsumi", "umiri", "nyamu", "uika"},
};
enum Band_name
{
    Poppin_Party,   //0 万物起源
    Afterglow,
    Pastel_Palettes,
    Roselia,
    Hello_Happy_World,
    RAISE_A_SUILEN,
    Morfonica,
    MyGO,
    Ave_Mujica,
    Item,
};
/*----乐队添加----*/

struct block_point{ int pos_x; int pos_y; };
struct block_size{int w; int h; };

class Block_info
{
public:
    bool is_head;   //是否是人物头部：1 是 0 否
    int dir;        //先有头部判断，再有腿部朝向：0~3:分别对应下左上右！（即rotate(i * 90°)）
    // 也方便设置di[4], dj[4]来获取腿部游戏区域块方位
    // if (UP): dir = (dir + 1) % 4
    bool is_stable = 0;  //是否是稳定人物快
    //坐标
    block_point bp;
    block_size bs;
    Band_name belong;     //乐队归属，并是连通块判断相消条件
    QString char_name;    //人物名
    QImage img;           //人物图
    QString bandSoundPath; //音效路径
};

#endif // BLOCK_H
