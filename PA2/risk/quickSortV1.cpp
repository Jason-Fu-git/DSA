#include<iostream>
using namespace std;

//快速排序（升序）推荐使用此算法
//@author Jason FU
int n;

void quicksort(int a[],int min,int max){//每一次排序都把比目标小的放到左边
    if (min<max)//need sort
    {
        int x,y,k;
        x=min;
        y=max;
        k=a[x];//选择最小点作为目标

   //start sort
        do{
            //从右方开始搜索
            while(x<y&&a[y]>k)//bigger than a[x]
               y--;//tail--
            //find a[y]<=k
            if (x<y)//防止越界
            {
                a[x]=a[y];//swap
                x++;
            }
            
           //从左方开始搜索
            while(x<y&&a[x]<k)//smaller than a[x]
               x++;//head
            //find a[x]>=k
            if (x<y)//防止越界
            {
                a[y]=a[x];//swap
                y--;
            }


        }while(x!=y);//while head!=tail


        a[x]=k;//put k

        // for(int i=0;i<n;i++){
        //     cout<<a[i];
        //     (i==n-1)? cout<<endl : cout<<' ';
        // }

        quicksort(a,min,x-1);//排序子列
        quicksort(a,x+1,max);
    }
   
}

int main(){
    cin>>n;
    int *a=new int[n];
    for(int i=0;i<n;i++){
        cin>>a[i];
    }
    quicksort(a,0,n-1);
    for (int i = 0; i < n; i++)
    {
        cout << a[i] <<' ';
    }
    return 0;
}