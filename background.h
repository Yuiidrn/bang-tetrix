#ifndef BACKGROUND_H
#define BACKGROUND_H

const int defaultWidth=1920;
//命名有误，将错就错了
const int AREA_ROW=10; //场景行数 (顶上预留判定行)
const int AREA_COL=8;  //场景列数
const int di[4] = {1, 0, -1, 0}, dj[4] = {0, -1, 0, 1}; //x <-> j ！！y <-> i ) =》 (若已知头部ij坐标，则腿部ij坐标：i += di[dir], j += dj[dir];

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SPACE,
    Gravity //稳定块的重力碰撞
};

#endif // BACKGROUND_H
