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
int MakeHistogram     (int **Y, int hist[256]);
int OutputHistogram   (int hist[256]);
int Percentile        (int **Y, int hist[256], double per);
int TranslateBoolScale(int **Y, int **F, int boader);
int expand            (int **F);
int cut_down          (int **F);
int remove_noise      (int **F, int loop);

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

int MakeHistogram(int **Y, int hist[256]){
    int i,j;
    for(i = 0; i < 256; ++i){     // 初期化
        hist[i]=0;
    }
    for(i = 0; i < IE; ++i){      // I方向の走査
        for(j = 0; j < JE; ++j){  // J方向の走査
            hist[Y[i][j]]++;
        }
    }
    return(0);
}

int OutputHistogram(int hist[256]){
    printf("#\t濃度値\t度数\n");
    for(int i=0; i< 256; ++i){
        printf("\t%03d\t%d\n",i,hist[i]);
    }
    return(0);
}

int Percentile(int **Y, int hist[256], double per){
    int  border;
    long sum;
    double black;
    black  = (IE*JE)*per; // black dot＝総dot数*p
    border = 0;
    sum    = 0;

    for(int i=0; i<256; ++i){
        border  = i;
        sum    += hist[i];
        if(sum>=black){
            break;
        }
    }
    return(border);
}

int TranslateBoolScale(int **Y, int **F, int boader){
    int i,j;
    for(i = 0; i < IE; ++i){      // I方向の走査
        for(j = 0; j < JE; ++j){  // J方向の走査
            if(Y[i][j]<=boader){
                F[i][j]=0;
            }else{
                F[i][j]=0xff;
            }
        }
    }
    return(0);
}


int expand(int **F){
    int ex[IE][JE];
    for(int i=0; i<IE; ++i){
        for(int j=0; j<JE; ++j){
            ex[i][j]=0;
        }
    }
    for(int i=1; i<IE-1; ++i){
        for(int j=1; j<JE-1; ++j){
            if(F[i][j]>0){
                ex[i-1][j-1]=0xff; ex[i-1][j]=0xff; ex[i-1][j+1]=0xff;
                ex[i  ][j-1]=0xff; ex[i  ][j]=0xff; ex[i  ][j+1]=0xff;
                ex[i+1][j-1]=0xff; ex[i+1][j]=0xff; ex[i+2][j+1]=0xff;
            }
        }
    }
    for(int i=0; i<IE; ++i){
        for(int j=0; j<JE; ++j){
            F[i][j]=ex[i][j];
        }
    }
    return(0);
}

int cut_down(int **F){
    int cut[IE][JE];
    for(int i=0; i<IE; ++i){ // ゼロで初期化
        for(int j=0; j<JE; ++j){
            cut[i][j]=0;
        }
    }
    for(int i=1; i<IE-1; ++i){ // 縮小画像の作成
        for(int j=1; j<JE-1; ++j){
            if(  F[i-1][j-1]>0&&F[i-1][j]>0&&F[i-1][j+1]>0
               &&F[i  ][j-1]>0&&F[i  ][j]>0&&F[i  ][j+1]>0
               &&F[i+1][j-1]>0&&F[i+1][j]>0&&F[i+1][j+1]>0
              ){
                cut[i][j]=0xff;
            }
        }
    }
    for(int i=0; i<IE; ++i){ // 縮小画像を引数に転写
        for(int j=0; j<JE; ++j){
            F[i][j]=cut[i][j];
        }
    }
    return(0);
}

int remove_noise(int **F, int loop){
       for(int i=0; i<loop; ++i){
         cut_down(F);
         expand(F);
       }
       return(0); 
}

int main(void){
    int    **R, **G, **B, **Y, **F;
    int    hist[256];
    double per;  // パーセンタイル法におけるpの値
    int    boader;

    R = (int **)IAllocImage(640,480);   // 画像領域（赤）を確保
    G = (int **)IAllocImage(640,480);   // 画像領域（緑）を確保
    B = (int **)IAllocImage(640,480);   // 画像領域（青）を確保
    Y = (int **)IAllocImage(640,480);   // 画像領域（グレースケール）を確保
    F = (int **)IAllocImage(640,480);   // 画像領域（二値）を確保

    GInit(640,480);                     // Windowの生成
    ILoadPpmImage("abstmix.ppm",R,G,B); // ppmファイルの読み込み
    TranslateGrayScale(R,G,B,Y);        // グレースケールに変換
    MakeHistogram(Y,hist);              // ヒストグラム作成
    DisplayColorImage(Y,Y,Y);

    printf("input per\n");
    while(EOF!=scanf("%lf",&per)){                // パーセンタイル法で二値化
       printf("per=%g,black=%g\n",per,IE*JE*per);
       boader = Percentile(Y,hist,per);
       printf("boader=%d\n",boader);
       TranslateBoolScale(Y,F,boader);
       remove_noise(F,0);
       DisplayColorImage(F,F,F);
    }

    GWaitLoop();                        // 終了処理

    exit(EXIT_SUCCESS);
}
