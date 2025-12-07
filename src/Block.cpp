#include "../include/Block.h"
#include <stdlib.h>
#include "Block.h"
/*    0 1 2 3
*0    0 1
*1    2 3
*2    4 5
*3    6 7
*/
IMAGE* Block::imgs[7] = {NULL,};
int Block::size = 30;

Block::Block()
{

    if(imgs[0] == NULL){
        IMAGE imgTmp;
        loadimage(&imgTmp,"res/tetoris.png");
        
        SetWorkingImage(&imgTmp);
        for(int i=0;i<7;i++)
        {
            imgs[i] = new IMAGE;
            getimage(imgs[i],i*size,0,size,size );
        }
        SetWorkingImage();//恢复工作区
    }

    int blocks[7][4]={
    1,3,5,7,//I
    2,4,5,7,//z1
    3,5,4,7,//z2
    2,3,5,7,//拐1
    3,5,7,6,//拐2
    2,3,4,5,//方
    3,5,4,7,//T
};
    //随机生成一种
    blockType = 1 + rand()%7;//0..6 =>1..7
    
    //初始化 smallBlocks
    for(int i=0;i<4;i++){
        int value = blocks[blockType-1][i];
        smallBlocks[i].row=value /2;
        smallBlocks[i].col=value %2;
    }//位置
    img = imgs[blockType-1];//类型

}

void Block::drop()
{
    for(int i=0;i<4;i++)
    {
        smallBlocks[i].row++;
    }
    //for(auto &block : smallBlocks){
    //    block.row++;
    //}//迭代
}


void Block::moveLeftRight(int offset)
{
    for(int i=0;i<4;i++){
        smallBlocks[i].col += offset;
    }

}
void Block::retate()
{
    Point p=smallBlocks[1];

    for(int i=0;i<4;i++){
        Point tmp =smallBlocks[i];
        smallBlocks[i].col = p.col - tmp.row + p.row;
        smallBlocks[i].row = p.row + tmp.col - p.col;
    }
}

void Block::draw(int LeftMargin, int topMargin)
{
    for (int i=0;i<4;i++)
    {
        int x =LeftMargin +smallBlocks[i].col * size;
        int y =topMargin +smallBlocks[i].row * size;
        putimage(x,y,img);
    }
}

IMAGE **Block::getimages()
{
    return imgs;
}

Block& Block::operator=(const Block &other)
{
    if(this == &other)//地址,防止自己引用自己
    return *this;

    this->blockType = other.blockType;
    
    for(int i=0;i<4;i++){
        this->smallBlocks[i] = other.smallBlocks[i];
    }
    return *this;
}

bool Block::blockInMap(const vector<vector<int>> &map)
{
    int rows = map.size();
    int cols = map[0].size();
    for(int i=0;i<4;i++){
        if(smallBlocks[i].col<0||smallBlocks[i].col>=cols||
           smallBlocks[i].row<0||smallBlocks[i].row>=rows||
           map[smallBlocks[i].row][smallBlocks[i].col]!=0) {
            return false;

        }
    }
    return true;
    
}

void Block::solidify(vector<vector<int>>& map)
{
    for(int i=0;i<4;i++){
        //设置标记，“固化”对应的方块
        map[smallBlocks[i].row][smallBlocks[i].col] = blockType;
    
    }
}

