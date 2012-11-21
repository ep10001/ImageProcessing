#include <stdlib.h>
#include <stdio.h>
#include "./Graphics.h"
#include "./Image.h"
#include "./labeling.h"
#include "./BorderFollowing.h"

#define IE 480  // 画像の縦幅
#define JE 640  // 画像の横幅

extern void GWaitLoop();

int DisplayColorImage (int **R, int **G, int **B);
int TranslateGrayScale(int **R, int **G, int **B, int **Y);

int DisplayColorImage(int **R, int **G, int **B){
    int i,j;
    for(i = 0; i < IE; ++i){      // I方向の走査
        for(j = 0; j < JE; ++j){  // J方向の走査
            //画素(i,j)に関する処理
                //赤：R[i][j]
                //緑：G[i][j]
                //青：B[i][j]
            GSetColor(R[i][j], G[i][j], B[i][j]);
            GPSet(j,i);
        }
    }
    return(0);
}

int TranslateGrayScale(int **R, int **G, int **B, int **Y){
    int i,j;
    for(i = 0; i < IE; ++i){      // I方向の走査
        for(j = 0; j < JE; ++j){  // J方向の走査
            Y[i][j] = R[i][j]*0.25 + G[i][j]*0.25 + B[i][j]*0.5;
        }
    }
    return(0);
}

int main(void){
    int **R, **G, **B, **Y;

    R = (int **)IAllocImage(640,480);   // 画像領域（赤）を確保
    G = (int **)IAllocImage(640,480);   // 画像領域（緑）を確保
    B = (int **)IAllocImage(640,480);   // 画像領域（青）を確保
    Y = (int **)IAllocImage(640,480);   // 画像領域（グレースケール）を確保

    GInit(640,480);                     // Windowの生成
    ILoadPpmImage("abstmix.ppm",R,G,B); // ppmファイルの読み込み
    TranslateGrayScale(R,G,B,Y);        // グレースケールへの変換
    DisplayColorImage(Y,Y,Y);           // グレースケール画像の表示
    GWaitLoop();                        // 終了処理

    exit(EXIT_SUCCESS);
}
