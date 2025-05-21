#include "game.h"

inline void block_cpy(Block_info &dblock, Block_info &sblock) { dblock = sblock; }

inline void Sleep(unsigned int msec){
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed() < msec)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
  //在延迟过程中屏键入事件处理滞后 QEventLoop::ExcludeUserInputEvents
}

/*场景消除逻辑*/
int vis[AREA_ROW][AREA_COL] = {0};
QVector<Block_info> v; //用来存相消块
//递归检查消除
int GameWidget::BlockCheck()
{
    //处理消块，整个场景悬空人物头块下移（并发出相应乐队信号！反馈播出相应bgm）, 考虑使用槽和信号？
        //连通块BFS搜索，相消后还得整体下移
    int match_count=0; //记连击数
    memset(vis, 0, sizeof(vis));
    for( int i = AREA_ROW - 1; i >= 0; i-- ) { //从后往前，从左往右
        for( int j = 0; j < AREA_COL; j++ ) {
            if( !vis[i][j] && game_area[i][j].is_stable && game_area[i][j].belong != Item) { //必须非物块进入BFS
                QVector<QPair<int, int>> Items;  //临时存储万能块位置信息，用于还原单次BFS后的所有万能块的vis情况（不同于乐队块的独特性, 万能块需维护其通用性
                QQueue<Block_info> q;  //BFS搜索队列
                int cur_charNum = 0;   //用来记录目前匹对人数

                v.clear();

                Block_info tBlock = game_area[i][j];
                Band_name cur_blng = tBlock.belong;

                cur_bandMP->setSource(tBlock.bandSoundPath); //提前加载

                vis[i][j] = 1;  //byd BFS真太久妹写了！先记录再push啊！这样多元素同时刻push进队列才不会重复啊！！
                q.push_back(tBlock);
                //BFS开始
                while(q.size()) {
                    block_point tp = q.front().bp;
                    v.push_back(q.front());
                    if(q.front().belong != Item)
                        cur_charNum++;
                    q.pop_front();

                    for(int a = 0; a < 4; a++) { //四方搜索
                        int ti = tp.pos_y, tj = tp.pos_x;
                        ti += di[a]; tj += dj[a];
                        if(ti < 0 || ti > AREA_ROW - 1 || tj < 0 || tj > AREA_COL - 1 )
                            continue;

                        if( !vis[ti][tj] && game_area[ti][tj].is_stable && (game_area[ti][tj].belong == cur_blng || game_area[ti][tj].belong == Item) ){                            
                            vis[ti][tj] = 1;
                            q.push_back(game_area[ti][tj]);
                            if(game_area[ti][tj].belong == Item)
                                Items.push_back({ti, tj});
                        }
                    }
                }
                //保证所有连通万能块在同一次BFS进入待消列中

                if (cur_charNum >= 2 * CHAR_NUM) { //存在匹配连通块
                    match_count ++ ;
                    //剩余乐队及成员集合维护操作
                    charRest[(int)cur_blng] = ini_set;   //乐队成员全部回入
                    bandRest.insert((int)cur_blng);      //相应匹配乐队编号回入剩余集合中
                    cur_bandMP->play();
                    gameTimer->stop();

                    this->installEventFilter(this);     //窗口对自己安装一个事件过滤器，屏蔽对界面的所有输入 实现暂停
                    Sleep(m_MPdur + 100);   //只有QMediaPlayer能获取时长
                    onPlayingChanged();     //必须保证onPlayingChanged调用后清除匹配块，避免二次进入
                } else { //还原万能块vis信息
                    for(auto &p : Items)
                        vis[p.first][p.second] = 0;
                }
            }
        }
    }
    if(match_count)
        return match_count + BlockCheck();  //递归调用（1 + 1 +...+ 0）
    //之前逻辑有误，应该是直到最后没有匹配块时才继续下落和操作！！
    else {
        // 未做到极限消除，补回GameOver状态
        if(game_area[iniPos_y][iniPos_x].is_stable) {
            isGameOver = true;
        }
        // 出口未堵，继续游戏（启用键盘和鼠标事件）
        else {
            this->removeEventFilter(this);
            gameTimer->start(); //继续下落计时器
        }
        return 0;
    }
}

//收尾
void GameWidget::onPlayingChanged(){
    //场景预刷新操作
    memset(vis, 0, sizeof(vis));                         //vis重置
    for(Block_info &tb : v)
        game_area[tb.bp.pos_y][tb.bp.pos_x] = ini_block; //清空匹配连通块
    cur_block = ini_block; //包括cur_block

    //！！时序上必须要先清空腿部虚块！！
    for(int ii = AREA_ROW-1; ii >= 0; ii-- )
        for(int jj = 0; jj < AREA_COL; jj++ )
            if(game_area[ii][jj].is_stable && !game_area[ii][jj].is_head)
                game_area[ii][jj] = ini_block;

    //再作整体下移（即重力机制，差不多就是活动块的一键落地逻辑）
    for(int ii = AREA_ROW-1; ii >= 0; ii-- ) { //自下而上刷新
        for(int jj = 0; jj < AREA_COL; jj++ ) {
            if(game_area[ii][jj].is_stable && game_area[ii][jj].is_head) { //对头部人物快直接作空格下移操作
                Block_info rec_block = Block_info();        //先创建并拷贝实例record&cur_block
                rec_block = game_area[ii][jj];
                block_point h_bck = rec_block.bp;
                game_area[ii][jj] = ini_block;      //后消块
                while(!IsCollide(rec_block, DOWN) )
                {
                    rec_block.y += fallingHeight;
                    rec_block.bp.pos_y = qFloor(rec_block.y / BLOCK_SIZE * 1.0);
                    h_bck = rec_block.bp;
                }                
                ConvertStable(h_bck.pos_x, h_bck.pos_y, rec_block);
            }
        }
    }
}
