#include "../include/Tetris.h"
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include "../include/Block.h"
#include <windows.h>
#include <Tetris.h>
#include <fstream>
#include <iostream>
#include <shlwapi.h> // 用于 PathFileExists
#include <tchar.h>

#include <mmsystem.h>
#include <sec_api/stdio_s.h>
#pragma comment (lib,"winmm.lib")
#pragma comment(lib, "shlwapi.lib")

#define MAX_LEVEL 5
#define RECORDER_FILE "recoeder.txt"
//const int SPEED_NORMAL = 500;//ms
const int SPEED_NORMAL[MAX_LEVEL]={500,400,300,150,80};
const int SPEED_QUICK = 45;

Tetris::Tetris(int rows, int cols, int left, int top, int blockSize)
{
   
    this->rows = rows;
    this->cols = cols;
    this->leftMargin = left;
    this->topMargin = top;
    this->blockSize = blockSize;

    for(int i=0;i< rows;i++){
        vector<int> mapRow;
        for(int j=0;j<cols;j++){
            mapRow.push_back(0);
        }
        map.push_back(mapRow);
    }
}

void Tetris::init()
{

    // 播放背景音乐
    
    mciSendString("play res/tetoris.mp3",0,0,0);

    delay = SPEED_NORMAL[0];
    
    //配置随机种子
    srand(time(NULL));

    //创建游戏窗口
    initgraph(800,800);
    
    //加载背景图片
    loadimage(&imaBg,"res/bg.png");

    loadimage(&imgWin,"res/over.png");
    loadimage(&imgOver,"res/over.png");

    //初始化游戏区中的数据
    char data[20][10];
    for(int i=0;i<rows;i++)
    {
        for(int j=0;j<cols;j++)
        {
            map[i][j]=0;
        }
    }

    score = 0;
    level = 1;
    lineCount = 0;

    //初始化最高分
    ifstream file(RECORDER_FILE);//记录文件
    if(!file.is_open()){
        cout << RECORDER_FILE << "打开失败" <<endl;
        highestScroe = 0;
    }
    else{
        file >> highestScroe;//我了个老天，这么方便，我宣布c++是最nb的语言
    }
    file.close();//关闭文件
    
    gameOver = false;
    
    
}

void Tetris::play()
{
    init();

    nextBlock = new Block;
    curBlock = nextBlock;
    nextBlock = new Block;

    int timer=0;
    

    while(1){
        //接受用户的输入
        keyEven();

        timer+=getDelay();
        if(timer > delay){
            timer=0;
            drop();
            //是否渲染游戏画面
            update =true;
        }

        if(update){
            update =false;
            //更新游戏的画面
            updateWindow();//渲染
           

            //更新游戏的数据
            clearLine();//清行
        }

        if(gameOver){
            //保存分数
            saveScore();

            //更新游戏
            displayOver();

            system("pause");
            
        }

    }
    
}

void Tetris::drop()
{
    bakBlock = *curBlock;
    curBlock->drop();//调用

    //下降过程中，
    if(!curBlock->blockInMap(map)){
        //把这个方块固化
        bakBlock.solidify(map);
        delete curBlock;
        curBlock = nextBlock;
        nextBlock = new Block;


        //检查游戏是否结束
        checkOver();
    
    }

}


void Tetris::keyEven()//因为输入值判定出现我也搞不明白的错误，所以用window.api判定了
{
    static unsigned long lastMoveTime = 0; // 上次移动时间
    static bool rotatePressed = false;     // 旋转键按下状态
    static bool downPressed = false;       // 下键按下状态
    
    unsigned long currentTime = GetTickCount();
    
    // 左键检测 - 带移动延迟控制
    if(GetAsyncKeyState(VK_LEFT) & 0x8000) {
        if(currentTime - lastMoveTime > 150) { // 150ms移动间隔
            moveLeftRight(-1);
            update = true;
            lastMoveTime = currentTime;
        }
    }
    // 右键检测 - 带移动延迟控制
    else if(GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        if(currentTime - lastMoveTime > 150) { // 150ms移动间隔
            moveLeftRight(1);
            update = true;
            lastMoveTime = currentTime;
        }
    }
    
    // 下键检测
    if(GetAsyncKeyState(VK_DOWN) & 0x8000) {
        delay = SPEED_QUICK;
        downPressed = true;
    } else if(downPressed) {
        delay = SPEED_NORMAL[level-1];
        downPressed = false;
    }
    
    // 上键检测（旋转）- 确保每次按键只触发一次
    if(GetAsyncKeyState(VK_UP) & 0x8000) {
        if(!rotatePressed) {
            bakBlock = *curBlock;
            curBlock->retate();
            if(!curBlock->blockInMap(map)) {
                *curBlock = bakBlock;
            }
            update = true;
            rotatePressed = true;
        }
    } else {
        rotatePressed = false;
    }

}

void Tetris::updateWindow()//渲染
{
    putimage(0,0,&imaBg);//绘制背景图片
    //测试方块
    //Block block;
    //block.draw(250,100);
    IMAGE** imgs =Block::getimages();
    BeginBatchDraw();//绘制完整体再输出

    for(int i=0;i< rows;i++)
    {
        for(int j=0;j<cols;j++)
        {
            if(map[i][j]==0)continue;

            int x= j*blockSize +leftMargin;
            int y= i*blockSize +topMargin;
            putimage(x,y,imgs[map[i][j]-1]);
        }
    }

    curBlock->draw(leftMargin,topMargin);
    nextBlock->draw(100,100);

    drawScore();//绘制分数

    EndBatchDraw();
    
}
//第一次调用是直接返回0
//返回距离上一次调用，将来多少ms
int Tetris::getDelay()
{
    static unsigned long long lastTime =0;
    unsigned long long currentTime = GetTickCount64();
    if (lastTime==0){
        lastTime=currentTime;
        return 0;
    }else{
        int ret=currentTime-lastTime;
        lastTime =currentTime;
        return ret;
    }

}

void Tetris::clearLine()
{
    int lines=0;
    int k = rows-1;//存储数据的行数
    for(int i=rows-1;i>=0;i--){
        //检查第i行是否满行
        int count =0;
        for(int j=0;j<cols;j++){
            if(map[i][j]){
                count++;
            }
            map[k][j] = map[i][j];//一边扫描一边存储
        }

        if(count < cols){//不是满行
           k--;
        }else{//满行
            lines++;
            lineCount++;
        }
    }
    if(lines>0){
        //得分
        int t=lines;
        int addScore[4]={10,30,60,80};
        score += addScore[t-1];

        //音效
        int u=lines;
        if(u==1)
        {
        PlaySound(_T("res/ciallo.mp3"), NULL, SND_FILENAME | SND_ASYNC);
        }else if(u==2){
            PlaySound(_T("res/ciallo2.mp3"), NULL, SND_FILENAME | SND_ASYNC);
        }else if(u>2){
            PlaySound(_T("res/boyaya.mp3"), NULL, SND_FILENAME | SND_ASYNC);
        }
        update =true;

        ////每一百分一个等级
        level = (score+99) / 100 ;

    }
}



void Tetris::moveLeftRight(int offset)
{
    bakBlock =*curBlock;
    curBlock->moveLeftRight(offset);

    if(!curBlock->blockInMap(map)){
        *curBlock = bakBlock;
    }
}

void Tetris::drawScore()
{
    char scoreText[32];
    sprintf_s(scoreText,sizeof(scoreText),"%d",score);
    
    //字体颜色
    setcolor(RGB(194,55,82));//三原色配比

    //字体大小
    LOGFONT f;
    gettextstyle(&f);
    f.lfHeight = 60;
    f.lfWidth = 30;
    f.lfQuality = ANTIALIASED_QUALITY;//设置字体“抗锯齿”效果

    strcpy_s(f.lfFaceName,sizeof(f.lfFaceName), _T("Segoe UI Black"));
    settextstyle(&f);
    
    setbkmode(TRANSPARENT);//字体背景设置为透明
    
    outtextxy(595,100,scoreText);
    //绘制消除了多少行
    sprintf_s(scoreText,sizeof(scoreText),"ciallo:%d",lineCount);
    outtextxy(595,200,scoreText);

    //绘制当前是第几关
    sprintf_s(scoreText,sizeof(scoreText),"level:%d",level);
    outtextxy(595,300,scoreText);

    gettextstyle(&f);
    f.lfHeight = 20;
    f.lfWidth = 10;
    settextstyle(&f);

    //绘制最高分
    sprintf_s(scoreText,sizeof(scoreText),"highestScore:%d",highestScroe);
    outtextxy(595,420,scoreText);
}

void Tetris::checkOver()
{
    gameOver = curBlock->blockInMap(map) == false;
    
}

void Tetris::saveScore()
{
    if(score > highestScroe){
        highestScroe =score;

        ofstream file(RECORDER_FILE);
        file << highestScroe;
        file.close();

    }
}

void Tetris::displayOver()
{
    mciSendString("stop res/tetoris.mp3",0,0,0);

    if(score < highestScroe/2){
        putimage(0,0,&imgOver);
        mciSendString("play res/over.mp3",0,0,0);
        PlaySound(_T("res/badover.mp3"), NULL, SND_FILENAME | SND_ASYNC);
    }else{
        putimage(0,0,&imgWin);
        mciSendString("play res/over.mp3",0,0,0);
        PlaySound(_T("res/goodover.mp3"), NULL, SND_FILENAME | SND_ASYNC);

    }

}
