#include "game.h"

inline void block_cpy(Block_info &dblock, Block_info &sblock) { dblock = sblock; }

//物块创建
void Widget::CreateBlock(Block_info &head_block)    //对next_block的引用传递！
{
    head_block = Block_info();   //使用默认构造函数 创建实例

    QString ImgPath = ":/imgs/img/", rbandSoundPath = "qrc:/sounds/sound/";
    int sum = 0;
    for(int i = 0; i < SET_NUM/*BAND_NUM*/; i++)
        sum += charRest[i].size();
    int is_item = -1;
    if(sum > 0)       //避免模零异常，下同
        is_item = rand() % sum;
    const double prob = 1/5.0; //基于剩余成员越少，万能块越容易刷出的动态概率(整型数判断)

    if(is_item > sum *(1-prob) || is_item < 0){ //无剩余乐队则必是物块，同时避免rof模零异常
        int numItems = 3;
        switch (rand() % numItems) {
        case 0:
            ImgPath += "items/coffee.png";
            head_block.char_name = "coffee";
            break;
        case 1:
            ImgPath += "items/coronet.png";
            head_block.char_name = "coronet";
            break;
        case 2:
            ImgPath += "items/croquette.png";
            head_block.char_name = "croquette";
            break;
        default:
            break;
        }
        head_block.belong = Item;
        head_block.bandSoundPath = rbandSoundPath;
    }
    else {
        //人物乐队所属及成员名随机逻辑(主要是通过基于集合内元素多少而随机迭代器偏移量(random_offset)实现)   tip.前缀r表示"random"
        QSet<int>::const_iterator rB_it(bandRest.begin());
        int rof1 = rand()%bandRest.size();      //bandRest.size(); 减少余数以增加同乐队成员概率
        while(rof1--) rB_it ++;
        int rBand_id = *rB_it;

        QSet<int>::const_iterator rC_it(charRest[rBand_id].begin());
        int rof2 = rand()%charRest[rBand_id].size();
        while(rof2--) rC_it ++;
        int rChar_id = *rC_it;

        //成员选出即移除场外集合中
        charRest[rBand_id].remove(rChar_id);    //维护逻辑（erase参数为迭代器，remove参数为对应元素值）
        if(charRest[rBand_id].empty())          //若移除后所属乐队剩余人员已空，则移除对应编号
            bandRest.remove(rBand_id);

        ImgPath += bandList[rBand_id] + "/" + nameList[rBand_id][rChar_id] + ".png";
        rbandSoundPath += "bandSound/" + bandList[rBand_id] + ".wav";
        head_block.belong = Band_name(rBand_id);
        head_block.char_name = nameList[rBand_id][rChar_id];
        head_block.bandSoundPath = rbandSoundPath;
    }

    //初始位置
    int rPos_x = 4, rDir = 2;
//    //人物块位置信息随机逻辑（主要是横轴）, 以方块左上角为锚点
//    int rPos_x = rand()%AREA_COL, rDir = rand()%4; //rand()%4; 0 —— 腿部朝下
//    while(rPos_x == 0 || rPos_x == AREA_COL - 1 ) //注意需要一直随机到避免是生成在边缘且刚好腿部延申出界的情况
//        rPos_x = rand()%AREA_COL;

    //设置初始人物块基本信息（先实现正常下落）
    head_block.img = QPixmap(ImgPath);
    head_block.y = fallingHeight;   //改为页边栏就开始下坠
    head_block.bp.pos_x = rPos_x;   //AREA_COL/2
    head_block.bp.pos_y = qFloor(head_block.y / BLOCK_SIZE * 1.0);
    head_block.is_head = 1; //头部标识
    head_block.dir = rDir;
}

//物块重置
void Widget::ResetBlock()
{
    //应该是先判断游戏是否结束才进行替换(初始位置是否已经被占据)
    if(game_area[next_block.bp.pos_y][next_block.bp.pos_x].is_stable) {
        GameOver();
        return;
    }

    //产生当前方块
    block_cpy(cur_block, next_block);
    // game_area[cur_block.bp.pos_y][cur_block.bp.pos_x] = cur_block; //保证在第1行开始输出掉落

    //提前加载资源，保证能获取到总时长
    cur_bandMP->setSource(QUrl(cur_block.bandSoundPath));

    //播放麻里奈动画
    emit this->MarinaAnimation(); //实例信号！！
//    playStartMA();
    //产生下一个方块
    CreateBlock(next_block);
}

//转化为稳定块(同时判断游戏是否结束)
void Widget::ConvertStable(int x, int pos_y, Block_info &cpy_Block)
{
    //头部
    block_cpy(game_area[pos_y][x], cpy_Block);
    game_area[pos_y][x].is_stable = 1;

    if(game_area[pos_y][x].belong != Item) {
        //腿部
        int leg_i = pos_y + di[game_area[pos_y][x].dir], leg_j = x + dj[game_area[pos_y][x].dir];
        // 优先判断游戏是否结束，避免腿块下标i为-1而造成访问越界，同时改逻辑为窗口外第“0”行出现腿块才结束
        //（无非两种情况：头出或腿出，而开始生成的话头块以固定为第一行，结合现实不太可能会有头出的情况，因此需格外排除腿出的情况）

        //出口顶端也有稳定方块且存在腿部出界
        if(game_area[iniPos_y][iniPos_x].is_stable ) // || (game_area[iniPos_y][x].is_stable && game_area[iniPos_y][x].dir == 2)
        {
            GameOver();
        }
        else
        {
            block_cpy(game_area[leg_i][leg_j], game_area[pos_y][x]); //拷贝头部块信息
            //！！注意个别值的更新！！
            game_area[leg_i][leg_j].bp = {leg_j, leg_i}; //坐标位置更新！
            game_area[leg_i][leg_j].is_head = 0;         //撤销腿部头部块识别！
        }
    }
}
//碰撞逻辑
bool Widget::IsCollide(Block_info check_block, Direction key_dir)  //给定为头块坐标
{
    /*试错法！！！碰撞检测很值得学习的思路！*/
    //用临时方向做判断
    int x = check_block.bp.pos_x, pos_y = check_block.bp.pos_y, ty = check_block.y;
    bool is_item = check_block.belong == Item ? true : false;
    int t_dir = check_block.dir;
    int tpos_y; //pos下取整意味着1.1, 1.2这些刚露头的情况仍然视作已下落至第二行格末端（行下标1），实际上嵌入格的左右是存在碰撞的，需要精判完善
    //先尝试按照某方向走一格
    switch(key_dir)
    {
    case UP:
        t_dir = (t_dir + 1) % 4;
        break;
    case DOWN:
        ty += fallingHeight;  //下落像素距离
        pos_y = qFloor(ty / BLOCK_SIZE * 1.0);
        break;
    case LEFT:
        x -= 1;
        break;
    case RIGHT:
        x += 1;
        break;

    default:
        break;
    }
    //pos_y + 1后去乘块高 才代表当前行格的底端下落高，也即零行下标起点行格像素高特性
    if(abs((pos_y + 1) * BLOCK_SIZE - ty) <= fallingHeight * 2) //嵌入精判(上下放宽n个下落帧
        tpos_y = pos_y;  //差距仅n个下落帧
    else
        tpos_y = (pos_y - 1) < 0 ? 0 : (pos_y - 1); //否则tpos_y仍保持上一行的碰撞体积

    // int BOTTOM = BLOCK_SIZE * AREA_ROW + 2 * fallingHeight;
    if( !is_item ){
        //额外获取腿部块方位 直接再补加一层di[dir]和dj[dir]推出（腿部延申块）
        int leg_x = x + dj[t_dir];
        //下标非负性检查
        int leg_pos_y = pos_y + di[t_dir] < 0 ? 0 : pos_y + di[t_dir];
        int tLeg_pos_y = tpos_y + di[t_dir] < 0 ? 0 : tpos_y + di[t_dir];; // leg_y = y + 2 * di[t_dir] * BLOCK_SIZE; //注意×2！头到脚底板是两个块宽！

        //存在碰撞
        if( (game_area[tpos_y][x].is_stable || game_area[tLeg_pos_y][leg_x].is_stable) ||
            (game_area[pos_y][x].is_stable || game_area[leg_pos_y][leg_x].is_stable) ||
            (x < 0 || leg_x < 0) || (x > AREA_COL - 1 || leg_x > AREA_COL - 1) ||  //
            (pos_y > AREA_ROW - 1 || leg_pos_y > AREA_ROW - 1) ) //注意加下边界！
            return true;

        return false;
    }
    else {
        if( (game_area[tpos_y][x].is_stable || game_area[pos_y][x].is_stable) || (x < 0 || x > AREA_COL - 1 || pos_y > AREA_ROW - 1) )
            return true; //物体块1*1碰撞
        return false;
    }
}
