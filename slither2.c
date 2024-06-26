#include <stdio.h>
#include <stdlib.h>

struct Status
{
int number;//盤面に書いてある数字0,1,2,3のどれか、空白だと-1として記述。
int group;//内外の状態が同じものを同じgroupにする。盤面の外側(確定)のgroupは-1、内側(確定)は1、未確定の場合は正負のみ気にする。最終的に二つのグループに分かれることを目指す。
}status[100][100];//status[縦][横]

int grow, gcolumn;//このコード内での盤面の広さに関する変数。要するに入力されたものの一回り大きい大きさ。

int setup()
{
        /*ファイルの読み取りと盤面の初期化ここから空白には-1を記述*/
    FILE *file;

    file = fopen("slither.txt","r");

    if (file == NULL)
    {   
        printf("ファイルを開けませんでした．\n");
        return 1;
    }
    
    const int row, column;
    //行数と列数の読み取り
    if (fscanf(file, "%d, %d",&row, &column) == 2)
    {
        #ifdef debug
        printf("row:%d, column:%d\n",row, column);
        #endif
    }
    else
    {
        printf("有効な二つの整数がありません");
        return 1;
   }
    grow = row + 2;
    gcolumn = column +2;

    //実際の中身の読み取り
    int Setgroup = 2;//左上から順に違うgroupとしてひとまず設定する
   for (int i = 1; i < row + 1; i++)
   {
        for (int j = 1; j < column + 1; j++)
        {
            int temp;
            if (fscanf(file, "%d,",&temp) != 1)
            {
                status[i][j].number = -1;
                fscanf(file,",");
            }   
            else
            {
                status[i][j].number = temp;
            }
            status[i][j].group = Setgroup;
            Setgroup++;
        }
   }
   //盤面の外側にgroup-1とnumber-1を設定
   for (int i = 0; i < column + 2; i++)
   {
    status[0][i].group = -1;
    status[row + 1][i].group = -1;
    status[0][i].number = -1;
    status[row + 1][i].number = -1;
   }

   for (int i = 1; i < row  + 1; i++)
   {
    status[i][0].group = -1;
    status[i][column + 1].group = -1;
    status[i][0].number = -1;
    status[i][column + 1].number = -1;
   }
    fclose(file);
    return 0;
}

void process0()
{
        for (int i = 1; i < grow - 1; i++)
    {
        for (int j = 1; j < gcolumn - 1; j++)
        {
            //0のあるマスの周りで一番小さいgroupを探索
            if (status[i][j].number == 0)
            {
                int smallest = status[i - 1][j].group;
                if (status[i][j - 1].group < smallest)
                {
                    smallest = status[i][j - 1].group;
                }else if (status[i][j].group < smallest)
                {
                    smallest = status[i][j].group;
                }else if (status[i][j + 1].group < smallest)
                {
                    smallest = status[i][j + 1].group;
                }
                //代入
                status[i - 1][j].group = smallest;
                status[i][j-1].group = smallest;
                status[i][j].group = smallest;
                status[i][j + 1].group = smallest;
                status[i + 1][j].group = smallest;
            }
            
        }
    }
}

void changegroup(int a, int b, int sa)//二つのグループの関係が確定した時に用いるchangegroup(group1, group2, 同じか-1倍か)
{
    int small, large;
    if(abs(a) == abs(b)) return;//これここじゃなくchangegroupを呼び出す前で判断したほうがいい気がする
    if (abs(a) < abs(b))
    {
        small = a;
        large = b;
    }else
    {
        small = b;
        large = a;
    }
    
    for (int i = 1; i < grow  - 1; i++)
    {
        for (int j = 1; j < gcolumn - 1; j++)
        {
            if (status[i][j].group == large)
            {
                status[i][j].group = small*sa;

            }else if(status[i][j].group == -large)
            {
                status[i][j].group == -small*sa;
            }
        }   
    }
}

void changefixedgroup(int thisgroup, int i, int j, int sa)
{
    changegroup(thisgroup, status[i - 1][j].group, sa);
    changegroup(thisgroup, status[i][j - 1].group, sa);
    changegroup(thisgroup, status[i][j + 1].group, sa);
    changegroup(thisgroup, status[i + 1][j].group, sa);
}

int countaroundgroup( int thisgroup,int i, int j, int sa)
{
    int numbers = 0;
    if(thisgroup == sa*status[i - 1][j].group) numbers++;
    if(thisgroup == sa*status[i][j - 1].group) numbers++;
    if(thisgroup == sa*status[i][j + 1].group) numbers++;
    if(thisgroup == sa*status[i + 1][j].group) numbers++;

    return numbers;
}

void processbyaroundgroup()
{
    for (int i = 1; i < grow - 1; i++)
    {
       for (int j = 1; j < gcolumn - 1; j++)
       {
            if(status[i][j].number == 1 || status[i][j].number == 2 || status[i][j].number == 3)
            {
                int thisgroup = status[i][j].group;
                if(countaroundgroup(thisgroup, i, j, 1) == 4 - status[i][j].number) 
                {
                    changefixedgroup(thisgroup, i, j, -1);
                } else if (countaroundgroup(thisgroup, i, j, -1) == status[i][j].number)
                {
                    changefixedgroup(thisgroup, i, j, 1);
                }
            }
            
        }
    }
}

void processbycornorgroup()
{
    int plusminus[4][2] = {{-1,-1},{1,-1},{-1,1},{1,1}};
    for (int i = 1; i < grow - 1; i++)
    {
       for (int j = 1; j < gcolumn - 1; j++)
       {
            if(status[i][j].number == 1)
            {
                int thisgroup = status[i][j].group;
                for (int k = 0; k < 4; k++)
                {
                    int type = judgeconortype(i, j, k);
                    if( type != 0)
                    {
                        
                    }
                }
                
            }
       }
    }
}

int judgeconortype(int i, int j, int k)//kが4方向に対応。戻り値0の場合はどれでもない
{
    int plusminus[4][2] = {{-1,-1},{1,-1},{-1,1},{1,1}};
    if(status[i + plusminus[k][0]][j].group == status[i + plusminus[k][0]][j + plusminus[k][1]].group && status[i + plusminus[k][0]][j + plusminus[k][1]].group == status[i][j + plusminus[k][1]].group)
    {
        return 1;
    }else if (status[i + plusminus[k][0]][j].group == status[i + plusminus[k][0]][j + plusminus[k][1]].group && status[i + plusminus[k][0]][j + plusminus[k][1]].group == -status[i][j + plusminus[k][1]].group)
    {
        return 2;
    }else if(status[i + plusminus[k][0]][j].group == -status[i + plusminus[k][0]][j + plusminus[k][1]].group && status[i + plusminus[k][0]][j + plusminus[k][1]].group == status[i][j + plusminus[k][1]].group)
    {
        return 3;
    }else if(status[i + plusminus[k][0]][j].group == -status[i + plusminus[k][0]][j + plusminus[k][1]].group && status[i + plusminus[k][0]][j + plusminus[k][1]].group == -status[i][j + plusminus[k][1]].group)
    {
        return 4;
    }else return 0;
    
}

void preresult()
{
        printf("盤面の状態:\n");
    for (int i = 0; i < grow; i++) {
        for (int j = 0; j < gcolumn; j++) {
            printf("%d ", status[i][j].group);
        }
        printf("\n");
    }
}

void printboard()
{
        printf("盤面の数字:\n");
    for (int i = 0; i < grow; i++) {
        for (int j = 0; j < gcolumn; j++) {
            printf("%d ", status[i][j].number);
        }
        printf("\n");
    }
}

void debughere()
{
    preresult();
    printboard();
}

int main ()
{
    if(setup() == 1) return 0;//構造体に情報を入れる
    //基本戦略として、あるマスの周りにある数字のうち一番絶対値が小さいものに関して同じグループかどうかを判定する。
    process0();
    for (int i = 0; i < 10; i++)
    {
        processbyaroundgroup();
    }
    
    debughere();
    return 0;
}