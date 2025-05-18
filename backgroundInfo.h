#ifndef BACKGROUNDINFO_H
#define BACKGROUNDINFO_H

const int defaultWidth=1920; //屏幕宽 用于计算比例
const int upOffset = 30; //上页边栏边长偏差——影响场景 & 物块起始生成位置
//行列命名反了，将错就错了
const int AREA_ROW=10; //场景行数 (顶上预留判定行)
const int AREA_COL=8;  //场景列数
const int di[4] = {1, 0, -1, 0}, dj[4] = {0, -1, 0, 1}; //x <-> j ！！y <-> i ) =》 (若已知头部ij坐标，则腿部ij坐标：i += di[dir], j += dj[dir];
//键盘输入枚举类
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SPACE,
};

#endif // BACKGROUNDINFO_H
