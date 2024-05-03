#include <stdio.h>
#include <stdlib.h>

int main ()
{
    struct Status
    {
    int number;//盤面に書いてある数字0,1,2,3のどれか、空白だと-1として記述。
    int group;//内外の状態が同じものを同じgroupにする。盤面の外側(確定)のgroupは-1、内側(確定)は1、未確定の場合は正負のみ気にする。最終的に二つのグループに分かれることを目指す。
    }status[100][100];//status[縦][横]

}