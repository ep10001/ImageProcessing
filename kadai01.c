#include <stdlib.h>
#include <stdio.h>
#include "./Graphics.h"
#include "./Image.h"
#include "./labeling.h"
#include "./BorderFollowing.h"

#define IE 480  // 画像の縦幅
#define JE 640  // 画像の横幅

extern void GWaitLoop();

int main(void){
    int **R, **G, **B;

    R = (int **)IAllocImage(JE,IE);   // 画像領域（赤）を確保
    G = (int **)IAllocImage(JE,IE);   // 画像領域（緑）を確保
    B = (int **)IAllocImage(JE,IE);   // 画像領域（青）を確保

    GInit(JE,IE);                     // Windowの生成
    ILoadPpmImage("abstmix.ppm",R,G,B); // ppmファイルの読み込み
    GWaitLoop();                        // 終了処理

    exit(EXIT_SUCCESS);
}
