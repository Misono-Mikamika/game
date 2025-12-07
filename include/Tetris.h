#pragma once
#include <vector>
#include <graphics.h> 
#include "Block.h"


using namespace std;
class Tetris
{
public:
    //构造函数
    Tetris(int rows,int cols,int left, int top,int blockSize);
    void init();//初始化
    void play(); //开始游戏
    
private://方法
    void keyEven();
    void updateWindow();
    //第一次调用函数返回0
    int getDelay();//返回距离上一次调用该函数，间隔了多少时间（ms）
    void drop();
    void clearLine();
    void moveLeftRight(int offset);
    void rotate();//旋转
    void drawScore();//绘制当前分数
    void checkOver();//检查是否结束
    void saveScore();//保存最高分
    void displayOver();//更新游戏结束

private://数据成员
    int delay;
    bool update;//是否更新

    //int map[20][10];

    //0:空白，无方块
    //5：是第五种俄罗斯方块
    vector<vector<int>> map;
    int rows;
    int cols;
    int leftMargin;
    int topMargin;
    int blockSize;
    IMAGE imaBg;//背景图片

    Block* curBlock;//当前方块
    Block* nextBlock;//预告方块
    Block bakBlock;//当前方块下落过程中的备用方块

    int score;//当前的分数
    int highestScroe;
    int level;//当前的关数
    int lineCount;//当前已经消了多少行
    bool gameOver;//结束了？

    IMAGE imgOver;
    IMAGE imgWin;
};
