/*开发日志
*1.创建项目
*2.先导入素材（找美工）
*3.C++开发
*4.设计C++的模板
*  就是设计类: Block(图型方块)和Tetris(俄罗斯方块游戏控制)
*5.设计各个模块的主要接口
*6.laolilaoli启动！！！
*/

#include "../include/Tetris.h"
#include "../include/Block.h" 

int main(void)
{
    Tetris game(20,10,250,100,30);
    game.play();
    return 0;
}