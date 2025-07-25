#include "game.h"

inline void block_cpy(Block_info &dblock, Block_info &sblock) { dblock = sblock; }

//旋转
void GameWidget::BlockRotate(Block_info &head_block)
{
    head_block.dir = (head_block.dir + 1) % 4;
}

//平移
void GameWidget::BlockTranslate(Direction key_dir)
{
    // 如果游戏已结束，不处理任何方块移动
    if (isGameOver) {
        return;
    }
    
    //整体逻辑是以判头块为主！头块为实体而腿块为虚体仅作碰撞判断用，即确定好头块位置前就预留有放腿块的空间
    block_point h_bck = cur_block.bp, l_bck = h_bck;
    l_bck.pos_x += dj[cur_block.dir]; l_bck.pos_y += di[cur_block.dir]; //pos —— 下落层格数

    int match_count = 0;

    switch (key_dir) {  //只有下落运动中(DOWN & SPACE)才设置block_check
    case UP:
        if(IsCollide(cur_block, UP))
            break;
        //顺时针旋转90度
        BlockRotate(cur_block);
        break;
    case DOWN:
        //人物块(包括头或腿)到达边界则不再移动（**触底判定要更改为根据实时坐标判定，否则会被提前吸附）
        //new：根据实时y坐标增加，并确定当前pos_y以用于碰撞检测
        //碰撞检测，只计算上下左右边界，如果碰撞则稳定方块后跳出
        if(IsCollide(cur_block, DOWN))
        {
            // 播放落地音效
            landEffect->play();

            // 只有最终不能下落才转成稳定方块(腿块和头块要统一！)
            ConvertStable(h_bck.pos_x, h_bck.pos_y, cur_block);

            if (!isGameOver) // 腿未出限第一重判定
            {
                match_count = BlockCheck();
                // 无极限消除第二重判定
                if (!isGameOver) { // 只有游戏真正未结束状态下才reset
                    ResetBlock();
                } else {
                    GameOver();
                }
            } else {
                GameOver();
            }
            break;
        }
        cur_block.y += fallingHeight;
        cur_block.bp.pos_y = qFloor(cur_block.y / BLOCK_SIZE * 1.0); //粗赋值*当前块转活动块*所要占据的下落层格数（下标零起点故下取整），精判过程在collision中
        break;
    case LEFT:
        //到左边界或者碰撞不再往左
        if(l_bck.pos_x == 0 || IsCollide(cur_block, LEFT) )
            break;

        cur_block.bp.pos_x -= 1;
        break;
    case RIGHT:
        //到左边界或者碰撞不再往左
        if(l_bck.pos_x == AREA_COL - 1 || IsCollide(cur_block, RIGHT) )
            break;

        cur_block.bp.pos_x += 1;
        break;
    case SPACE: //一次到底
        //一格一格试错下移，直到不能下移
        while(!IsCollide(cur_block, DOWN))
        {
            //new：根据实时y坐标增加，并确定当前pos_y以用于碰撞检测
            cur_block.y += fallingHeight;
            cur_block.bp.pos_y = qFloor(cur_block.y / BLOCK_SIZE * 1.0);
        }

        landEffect->play();
        ConvertStable(cur_block.bp.pos_x, cur_block.bp.pos_y, cur_block);

        if (!isGameOver) //腿未出限第一重判定
        {
            match_count = BlockCheck();
            // 无极限消除第二重判定
            if (!isGameOver) { // 只有游戏真正未结束状态下才reset
                ResetBlock();
            } else {
                GameOver();
            }
        } else {
            GameOver();
        }
        break;
    default:
        break;
    }
    // 分数计算（策划的一环）
    if(match_count){
        MaxCombo = max(MaxCombo, match_count);
        score += (int)pow(10, match_count); //得分（连击奖励设置）
    }
}
