#include <stdlib.h>
#include <stdio.h>
#include "./Graphics.h"
#include "./Image.h"
#include "./labeling.h" // 課題5-1でinclude
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
int getLabeledImgLand (int **L, int label);
int printLabelingData (int **L, int label_num);
int makeLabeled       (int **N, int **L, int label);
int DisplayLabeled    (int **N);

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

int getLabeledImgLand(int **L, int label){
    int result=0;
    for(int i=0; i<IE; ++i){
        for(int j=0; j<JE; ++j){
            if(L[i][j]==label){
                ++result;
            }
        }
    }
    return(result);
}

int printLabelingData(int **L, int label_num){
    for(int i=0; i<=label_num; ++i){
        printf("label=%d,"  ,i);
        printf("land =%d\n",getLabeledImgLand(L,i));
    }
    return(0);
}

int makeLabeled(int **N, int **L, int label){
    int lp=0;
    int li[1000],lj[1000];
    for(int i=0; i<IE; ++i){
        for(int j=0; j<JE; ++j){
            if(L[i][j]!=label){
                N[i][j]=0x00;
            }else{
                N[i][j]=0x01;
            }
        }
    }
    DisplayColorImage(N,N,N);
    BorderFollowing(N, li, lj, 1000, &lp);
    printf("lp=%d\n",lp);
    for(int i=0; i<lp; ++i){
        if(li[i]>=0&&lj[i]>=0){
            //printf("%5d %5d\n",li[i],lj[i]);
            N[li[i]][lj[i]]=0xff;
        }
    }
    DisplayLabeled(N);
    return(lp);
}

int DisplayLabeled(int **N){
    int i,j;
    for(i = 0; i < IE; ++i){      // I方向の走査
        for(j = 0; j < JE; ++j){  // J方向の走査
            GSetColor(N[i][j], 0x00, 0x00);
            GPSet(j,i);
        }
    }
    return(0);
}

int main(void){
    int    **R, **G, **B, **Y, **F, **L, **N;
    int    hist[256];
    int    boader;
    int    label_num;

    R = (int **)IAllocImage(640,480);   // 画像領域（赤）を確保
    G = (int **)IAllocImage(640,480);   // 画像領域（緑）を確保
    B = (int **)IAllocImage(640,480);   // 画像領域（青）を確保
    Y = (int **)IAllocImage(640,480);   // 画像領域（グレースケール）を確保
    F = (int **)IAllocImage(640,480);   // 画像領域（二値）を確保
    L = (int **)IAllocImage(640,480);   // 画像領域（ラベル付）を確保
    N = (int **)IAllocImage(640,480);   // 画像領域（輪郭線）を確保

    GInit(640,480);                     // Windowの生成
    ILoadPpmImage("abstmix.ppm",R,G,B); // ppmファイルの読み込み
    TranslateGrayScale(R,G,B,Y);        // グレースケールに変換
    MakeHistogram(Y,hist);              // ヒストグラム作成
    DisplayColorImage(Y,Y,Y);

    boader = Percentile(Y,hist,0.92);
    TranslateBoolScale(Y,F,boader);     // パーセンタイル法で2値化
    remove_noise(F,13);
    label_num=labeling(F,L);
    DisplayColorImage(F,F,F);
    printf("label_num=%d\n",label_num);
    printLabelingData(L,label_num);
    for(int i=0; i<=label_num; ++i){
        makeLabeled(N,L,i);
        DisplayLabeled(N);
    }

    GWaitLoop();                        // 終了処理

    exit(EXIT_SUCCESS);
}
