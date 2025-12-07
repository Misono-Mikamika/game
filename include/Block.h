#pragma once  // 防止头文件被重复包含
/*
 * @file Block.h
 * @brief 俄罗斯方块的方块类定义
 * @note 需预先安装EasyX图形库（https://easyx.cn/）
 */
#include <graphics.h>  // EasyX图形库（必须安装）
#include <vector>

using namespace std;

struct Point
{
    int row;
    int col;

};


class Block
{
public:
     Block();
     void drop();
     void moveLeftRight(int offset);
     void retate();//选择方向,旋转
     void draw(int LeftMargin,int topMargin);
     static IMAGE** getimages();

     Block& operator =(const Block& other);//对象符重改
     
     bool blockInMap(const vector<vector<int>>&map);
     //Point* getSmallBlocks();//新接口
     void solidify(vector<vector<int>>& map);


private:
     int blockType;//方块类型
     Point smallBlocks[4];
     IMAGE *img;
     
     static IMAGE* imgs[7];//int* data[7];
     static int size;//静态变量
};