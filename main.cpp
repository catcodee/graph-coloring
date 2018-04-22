#include <iostream>
#include <string>
#include <fstream>
#include <new>
#include <time.h>
#include<vector>
using namespace std;
class TabuSearch;
class Evolution;
int N;       //节点数
int k;       //颜色数
int **g;     //邻接表
int *v_edge; //每个节点的邻接边数

class TabuSearch{
private:

    int *sol;                   //节点对应颜色，即解
    int **TabuTenure;           //禁忌表
    int **Adjacent_Color_Table; //邻接颜色表

    int fbest;                  //最好的冲突边数
    int fs;       //当前冲突边数
    unsigned int seed;     //随机种子
    long iter = 0; //
    int u;
    int delt;
    int v2;
    int sameList[2000][2];
    int tabu_sameList[2000][2];
    clock_t t;

public:

    TabuSearch(){
        int v = 0;
        iter = 0;
        seed = time(0);
        srand(seed);
        try
        {
            sol = new int[::N];
            TabuTenure = new int *[::N];
            Adjacent_Color_Table = new int *[::N];
            for (v = 0; v < ::N; v++)
            {
                TabuTenure[v] = new int[::k];
                Adjacent_Color_Table[v] = new int[::k];
            }
        }
        catch (std::bad_alloc const &e)
        {
            std::cout << " " << e.what() << endl;

        }
        for (v = 0; v < ::N; v++)
        {
            sol[v] = 1 + rand() % (::k);
            //cout << sol[v] << "    "; //结果显示

        }
        cout<<endl;
        update();
    }
    void  initial(int *p){
        int v = 0;
        iter = 0;
        seed = time(0);
        srand(seed);

        for (v = 0; v < ::N; v++)
        {
            sol[v] = p[v];
            //cout << sol[v] << "    "; //结果显示

        }

        update();
    }
    void update()
    {
        int v,j,c;
        for (v = 0; v < ::N; v++)
            for (c = 0; c < ::k; c++)
            {
                TabuTenure[v][c] = 0;           //初始化禁忌表
                Adjacent_Color_Table[v][c] = 0;
            }
        for (v = 0; v < ::N; v++)                 //
        {
            for (c = 0; c < ::k; c++)
            {
                for (j = 0; j < ::v_edge[v]; j++)
                {
                    if ((c + 1) == sol[(::g[v][j] - 1)])
                        Adjacent_Color_Table[v][c]++;
                }
            }
        }

        for (v = 0,j =0; v < ::N; v++)
            j += Adjacent_Color_Table[v][sol[v] - 1];
        fbest = fs = j / 2;
        cout<<fs<<endl;
    }

    int * get_sol(int *p)
    {
        int i;

        for (i = 0; i < ::N; i++)
        {
            p[i] = sol[i];
        }
        return p;
    }

    void showcolortable(){
        int v, c;
        for (v = 0; v < ::N; v++) //查看颜色表
        {
            cout << v + 1 << "    ";
            for (c = 0; c < ::k; c++)
                cout << Adjacent_Color_Table[v][c] << "--";
            cout << endl;
        }
    };

    void showtabutable()
    {
        int v,  j;
        for (v = 0; v < ::N; v++) //查看颜色表
        {
            cout << v + 1 << "    ";
            for (j = 0; j < ::k; j++)
                cout << TabuTenure[v][j] << "--";
            cout << endl;
        }
    };

    void showsol(){
        int i;
        cout<<"solution"<<endl;
        for(i = 0; i < ::N; i++)
            cout<<i+1<<"    "<<sol[i]<<endl;
        cout<<"iter = "<<iter<<endl;
        cout<<"fs = "<<fs<<endl;
        cout<<"seed = "<<seed<<endl;
        cout<<"time = "<<t<<endl;
    };
    void free_alloc();
    void get_fs();
    int f();
    void findmove();
    void makemove();
    void run(int mode, int maxiter, int minfs);
    void record(ofstream & f);
    void copy(const TabuSearch &x)
    {
        int v,c;
          fbest = x.fbest;                  //最好的冲突边数
            fs = x.fs;       //当前冲突边数
            seed = x.seed;     //随机种子
            iter = x.iter; //
            u = x.u;
            delt = x.delt;
            v2 = x.v2;
           for (v = 0; v < ::N; v++)
                for (c = 0; c < ::k; c++)
                {
                    TabuTenure[v][c] = x.TabuTenure[v][c];           //初始化禁忌表
                    Adjacent_Color_Table[v][c] = x.Adjacent_Color_Table[v][c];
                }
            for (v = 0; v < ::N; v++)
            {
                sol[v] = x.sol[v];
            }

    }
     ~TabuSearch();
};
TabuSearch::~TabuSearch()
{

    delete(sol);
}
void TabuSearch::record(ofstream & f)
{
    int i;
    f<<seed<<"|"<<iter<<"|"<<t<<"|"<<((double)iter/t)<<"|";
    for (i = 0; i < ::N;i ++)
        f<<sol[i]<<"    ";
    f<<endl;
}
void TabuSearch::free_alloc()
{
    int i = 0;
    for (; i < ::N; i++)
    {
        delete (TabuTenure[i]);
        delete (Adjacent_Color_Table[i]);
    }

    delete (TabuTenure);
    delete (Adjacent_Color_Table);

}

void TabuSearch::get_fs() //得到冲突边数
{
    int v = 0;
    int s = 0;
    for (v = 0; v < ::N; v++)
        s += Adjacent_Color_Table[v][sol[v] - 1];
    fbest = fs = s / 2;
    cout << "fs =" << fs << endl;
}
int TabuSearch::f() //得到冲突边数
{
    return fs;
}

/************************************************
寻找最优动作
int & u: 需要转变颜色的节点
int & c_current 转化前的颜色
int & c_change 要转化的颜色
int & delt 转化后冲突边的改变数
*************************************************/

void TabuSearch::findmove()
{
    int v = 0; //节点

    int c_current, c_change;
    int delt_min = 100, Tabudelt_min = 100, delt_current; //设为较大值，确保第一次更新获得正确值

    int same = 0, tabu_same = 0;
    int elect;
    int deltbest = fbest - fs;
    for (v = 0; v < N; v++)
    {
        c_current = sol[v] - 1;                   //获取当前节点颜色
        if(Adjacent_Color_Table[v][c_current] > 0)//绑定是否有冲突边
        {
            for (c_change = 0; c_change < ::k; c_change++)
            {
                if ((c_change + 1) != sol[v])            //更改为其他颜色，计算delt
                {

                    delt_current = -Adjacent_Color_Table[v][c_current] + Adjacent_Color_Table[v][c_change];
                                    //减去当前颜色仇人数，加上要变成的颜色的仇人数

                    if (TabuTenure[v][c_change] < iter)   //若该动作非禁忌
                    {
                        if (delt_current < delt_min)      //若为为非禁忌最好则更新非禁忌最好
                        {
                            sameList[0][0] = v;
                            //nTabu_v1 = c_
                            sameList[0][1]  = c_change;
                            delt_min = delt_current;

                            same = 0;
                        }
                        else if(delt_current == delt_min)
                        {
                            same++;
                            sameList[same][0] = v;
                            sameList[same][1] = c_change;
                            //delt_min = delt_current;
                        }
                    }
                    else                                  //若该动作为禁忌
                    {
                        if (delt_current < Tabudelt_min)   //若为禁忌最好，更新禁忌最好
                        {
                            tabu_sameList[0][0] = v;
                            //Tabu_v1 = c_curren
                            tabu_sameList[0][1] = c_change;
                            Tabudelt_min = delt_current;
                            tabu_same = 0;
                        }
                        else if(delt_current == Tabudelt_min)
                        {
                            tabu_same++;
                            tabu_sameList[tabu_same][0] = v;
                            tabu_sameList[tabu_same][1] = c_change;
                            //Tabudelt_min = delt_current;
                        }
                    }
                }
            }
        }
    }

    if((delt_min > Tabudelt_min)&&(Tabudelt_min<deltbest))
    {
        if(tabu_same > 0)
        {
            elect = rand()%tabu_same;
            u = tabu_sameList[elect][0];
            //v1 = sol[u] - 1;
            v2 = tabu_sameList[elect][1];
            //cout<<"????????"<<elect<<endl;
        }
        else{
            u = tabu_sameList[0][0];
            //v1 = sol[u] - 1;
            v2 = tabu_sameList[0][1];

        }
        delt = Tabudelt_min;
        //cout<<"解禁成功"<<endl;
    }
    else{
            if(same > 0)
            {
                elect = rand()%same;
                u = sameList[elect][0];
                //v1 = sol[u]-1;
                v2 = sameList[elect][1];
            //cout<<"????????"<<elect<<" "<<same<<endl;
            }
            else{
                u = sameList[0][0];
                //v1 = sol[u]-1;
                v2 = sameList[0][1];
            }
            delt = delt_min;
    }

    //检验

    /*cout<<"u = "<<u+1<<endl;
    cout<<"u_color"<<sol[u]<<endl;

    cout<<"v2 = "<<v2+1<<endl;
    cout<<"delt = "<<delt<<endl;*/
}
void TabuSearch::makemove()
{
    int i, j;
    int v1 = sol[u] - 1;
    sol[u] = v2 + 1;
    fs += delt;

    if (fs < fbest)
        fbest = fs;
    int *h = ::g[u];
    int m = ::v_edge[u];

    for (i = 0; i < m; i++)
    {
        j = h[i] - 1;
        Adjacent_Color_Table[j][v1]--;
        Adjacent_Color_Table[j][v2]++;
    }

    TabuTenure[u][v1] = fs + iter + rand() % 10 + 1;

    /*for (u=0; u<::N; u++) //查看颜色表
    {
        cout<<u+1<<"    ";
        for(j=0 ;j<::k; j++)
            cout<<Adjacent_Color_Table[u][j]<<"--";
        cout<<endl;
    }*/
    /*for (u=0; u<::N; u++) //查看禁忌表
    {
        cout<<u+1<<"    ";
        for(j=0 ;j<::k; j++)
            cout<<TabuTenure[u][j]<<"--";
        cout<<endl;
    }*/
}

void TabuSearch::run(int mode, int maxiter,int minfs)
{
    t = clock();
    switch(mode){
        case 0:
            while (fs > 0)
            {
                iter++;
                findmove();
                makemove();
                //cout<<fs<<" "<<delt<<endl;
                if((iter%1000000) == 0) cout<<fbest<<endl;
            }  break;
        case 1:
            while ((iter < maxiter)&&(fs > 0))
            {
                iter++;
                findmove();
                makemove();
                if ((iter % 1000000) == 0)
                    cout << fbest << endl;
            }
            break;
        case 2:
            while ((fs >= minfs)&&(fs > 0))
            {
                iter++;
                findmove();
                makemove();
                /*if ((iter % 1000000) == 0)
                    cout << fbest << endl;*/
            }
            break;
        default: cout<<"input correct mode 0,1,2"<<endl;
    }
    t = clock() - t;
    //showsol();
}
class Evolution{
private:
    vector<vector<int> > p[2];
    vector<vector<int> > child;

    int * c_sol;

public:
    Evolution()
    {
        try{

            c_sol = new int [::N];

        }
        catch (std::bad_alloc const &e)
        {
            std::cout << " " << e.what() << endl;
        }
        p[0].resize(::k);
        p[1].resize(::k);
        child.resize(::k);
    }

    void initialization (int s1[],int s2[]);
    void start();
    void convert();
    void get_sol(int * p);
    void clear()
    {

        vector<vector<int> > & p1 = p[0];
        vector<vector<int> > & p2 = p[1];
        p1.clear();
        p2.clear();
        child.clear();
        p[0].resize(::k);
        p[1].resize(::k);
        child.resize(::k);
        /*for (c = 0;c < ::k; c++)
        {
            if(p1[c].size()>0)   p1[c].clear();
            if(p2[c].size()>0)   p2[c].clear();
            if(child[c].size()>0) child[c].clear();
        }*/
    }
    ~Evolution(){
        delete(c_sol);
    }


};

void Evolution::initialization(int s1[],int s2[])
{
    int v,c1,c2;
    vector<vector<int> > & p1 = p[0];
    vector<vector<int> > & p2 = p[1];

    for(v = 0; v < ::N; v++ )
    {
        c1 = s1[v] - 1;
        c2 = s2[v] - 1;

        p1[c1].push_back(v+1);
        p2[c2].push_back(v+1);
    }
    /*for(c = 0; c < ::k; c++)
    {
        cout<<c<<"  ";

        for(auto iter = p1[c].begin();iter != p1[c].end(); iter++)
        {
            cout<<(*iter)<<"--";
        }
        cout<<endl;
    }
    for(c = 0; c < ::k; c++)
    {
        cout<<c<<"  ";
        for(auto iter = p2[c].begin();iter != p2[c].end(); iter++)
        {
            cout<<(*iter)<<"--";
        }
        cout<<endl;
    }*/
}

void Evolution::start()
{
    int i,choice,c;
    int max = 0;

    int c_size;
    int same[::k];
    int del;
    int samenum = 0;
    int tmp;

    vector<vector<int>> *p1;
    vector<vector<int>> *p2;

    for(i = 0; i < ::k; i++)    //k种颜色取k次
    {
        choice = i%2;
        p1 = &(p[choice]);
        p2 = &(p[!choice]);
        samenum = 0;               //每次交换p1,p2
        max = 0;c = 0;
        for (auto iter = (*p1).begin(); iter != (*p1).end(); iter++,c++) //迭代，找出最大颜色集
        {
            c_size = (*iter).size();
            if (c_size >= max)
            {
                if (c_size > max)
                {
                    max = c_size;
                    samenum = 0;
                    same[0] = c;
                }
                else{
                    samenum++;
                    same[samenum] = c;
                }

            }
        }

        if(samenum > 0){
            tmp = rand()%samenum;
            del = same[tmp];
        }
        else del = same[0];

        for(auto iter1 = (*p2).begin(); iter1 != (*p2).end(); iter1++)    //p2第一层迭代
        {
            for(auto iter3 = (*p1)[del].begin(); iter3 != (*p1)[del].end(); iter3++) //要删除的颜色集迭代
            {
                for(auto iter2 = (*iter1).begin(); iter2 != (*iter1).end(); iter2++)//p2某一颜色集的迭代
                {
                    if(*iter2 == *iter3)
                    {
                        iter2 = (*iter1).erase(iter2);
                        --iter2;
                    }
                }

            }
            //if((*iter1).size() == 0) {(*p2).erase(iter1); --iter1;}
        }
        if(p2->size() > 0)
        {
            for(auto iter1 = (*p2).begin(); iter1 != (*p2).end(); iter1++)
            {
                if((*iter1).size() == 0)
                {
                    iter1 = (*p2).erase(iter1);
                    --iter1;
                }
            }

        }

        child[i] = (*p1)[del];
        if(p1->size() > 0)(*p1).erase((*p1).begin()+del);
        /*cout<<"child"<<endl;
        for(c = 0; c < ::k; c++)
        {
            cout<<c<<"  ";
            for(auto iter4 = child[c].begin();iter4 != child[c].end(); iter4++)
            {
                cout<<(*iter4)<<"--";
            }
            cout<<endl;
        }*/

    }
    if(p[0].size() > 0)
    {
        for(auto iter1 = p[0].begin(); iter1 != p[0].end(); iter1++)
        {
            for(auto iter2 = (*iter1).begin(); iter2 != (*iter1).end(); iter2++)
            {
                tmp = rand()%(::k);
                child[tmp].push_back(*iter2);
            }
        }
    }

    convert();
    /*cout<<"child"<<endl;
    for(c = 0; c < ::k; c++)
    {
        cout<<c<<"  ";
        for(auto iter4 = child[c].begin();iter4 != child[c].end(); iter4++)
        {
            cout<<(*iter4)<<"--";
        }
        cout<<endl;
    }*/
}
void Evolution::convert()
{
    int i;
    int yan[::N] = {0};
    for (i = 0; i < ::k; i++)
    {
        for(auto iter = child[i].begin(); iter != child[i].end(); iter++)
        {
            c_sol[*iter - 1] = i + 1;
            yan[*iter - 1]++;
        }

    }
    /*for(i = 0; i < ::N; i++)
    {
        cout<<c_sol[i] <<"     "<<yan[i]<<endl;
    }*/

}

void Evolution::get_sol(int * p)
{
    int i;
    for(i = 0; i < N; i++)
    {
        p[i] = c_sol[i];
    }
}

class Population{
private:
    int n;
    int initialiter;
    int maxiter;
    int min1,min2,max1;
    int min1pos,min2pos,max1pos;
    int _count = 0;
    int sol_pos;
    int sol1[1000];
    int sol2[1000];
    clock_t t;
    TabuSearch * s;
    Evolution evol;
    TabuSearch child;
public:
    Population(int a,int b,int c):
        n(a),initialiter(b),maxiter(c)
    {
        int i;

        s = new TabuSearch[n];
        t = clock();
        for ( i = 0; i < n; i++)
        {
            s[i].run(1,initialiter,0);
            cout<<i+1<<"号完成！"<<endl;

        }
        _count = 0;
        sol_pos = 0;
        min1 = 10000000;
        min2 = 10000000;
        max1 = -1;

    }
    void run()
    {
        int i;

        while(1)
        {
            min1 = 10000000;
            min2 = 10000000;
            max1 = -1;

            for(i = 0; i < n; i++)
            {
                int c = s[i].f();
                if(c <= min1)
                {
                    min1 = c;
                    min1pos = i;
                }
                else{
                    if(c <= min2 )
                    {
                        min2 = c;
                        min2pos = i;
                    }
                }
                if(c > max1)
                {
                    max1 = c;
                    max1pos = i;
                }

            }
            if(min1 > 0)
            {
                s[min1pos].get_sol(sol1);
                s[min2pos].get_sol(sol2);
                evol.clear();
                evol.initialization(sol1,sol2);
                evol.start();
                evol.get_sol(sol1);
                child.initial(sol1);
                child.run(1,maxiter,0);
                cout<<child.f()<<endl;
                if(child.f() < max1)
                {
                    s[max1pos].copy(child);
                    _count++;
                    cout<<"进化代数 = "<<_count<<endl;
                    cout<<"最小变数 = "<<min1<<endl;
                }

            }
            else{
                cout<<"找到解"<<endl;
                s[min1pos].showsol();
                cout<<"进化代数 = "<<_count<<endl;
                t = clock() - t;
                break;
            }
        }
    }
    void record(ofstream & f)
    {
        int i;
        f<<n<<"|"<<initialiter<<"|"<<maxiter<<"|"<<t<<"|";
        s[min1pos].get_sol(sol1);
        for(i = 0; i < ::N;i++)
        {
            f<<sol1[i]<<"   ";
        }
        f<<endl;
    }
};



void free_alloc()
{
    int i = 0;
    for (; i < N; i++)
        delete (g[i]);

    delete (g);
    delete (v_edge);
}
void createGraph(int color,const string &s)
{
    ifstream in(s,ios::in);
    char buffer[100];
    char tmp[5];
    int v1, v2,v;
    int i, j = 0;

    if (!in.is_open())
    {
        cout << "Error opening file";
        exit(1);
    }
    if (in.good())
    {
        while (!in.fail())
        {
            in.getline(buffer, 100);
            if (buffer[0] == 'p')
            {
                for (i = 7, j = 0; buffer[i] != ' '; i++) //边的标准格式为“e vi vj"因此从pos=2开始
                    tmp[j++] = buffer[i];                 //到第二个' '结束
                tmp[j] = '\0';                            //结束字符串

                N = atoi(tmp);
                k = color;

                try
                {
                    g = new int *[N];
                    v_edge = new int[N];

                    for (v = 0; v < N; v++)
                        g[v] = new int[N];
                }
                catch (std::bad_alloc const &e)
                {
                    std::cout << "" << e.what() << endl;

                }
                for(v=0;v<N;v++)
                    v_edge[v] = 0;
            }
            if (buffer[0] == 'e') //记边的标志为e
            {
                for (i = 2, j = 0; buffer[i] != ' '; i++) //边的标准格式为“e vi vj"因此从pos=2开始
                    tmp[j++] = buffer[i];                 //到第二个' '结束
                tmp[j] = '\0';                            //结束字符串

                v1 = atoi(tmp);

                for (i++, j = 0; buffer[i] != 0; i++) //判断条件为buffer结束，getline()后字符串末尾为'\0'
                    tmp[j++] = buffer[i];             //i从提取v1后开始
                tmp[j] = '\0';                        //结束字符串

                v2 = atoi(tmp);

                v_edge[v1 - 1]++;
                v_edge[v2 - 1]++; //注意pos为节点值-1
                g[v1 - 1][v_edge[v1 - 1] - 1] = v2;
                g[v2 - 1][v_edge[v2 - 1] - 1] = v1; //pos为
            }
        }
    }
    in.close();

    /*for (i=0,j=0; i<N; i++)                     //生成图结果显示
    {
        cout<<endl<<i+1<<"  ";
        for(j=0; j<v_edge[i]; j++)
                cout<<g[i][j]<<"--";
        cout<<endl;
    }*/
}
int main()
{

    int color;
    int choice;

    ofstream ofile("./output.txt", ios::app);
    string s[10] = {
        "./DSJC125.1.col",
        "./DSJC250.1.col",
        "./DSJC250.5.col",
        "./DSJC250.9.col",
        "./DSJC500.1.col",
        "./DSJC500.5.col",
        "./DSJC500.9.col",
        "./DSJC1000.1.col",
        "./DSJC1000.5.col",
        "./DSJC1000.9.1.col",
    };
    cout<<"选择算例"<<endl;
    cout<<"0-----"<<"./DSJC125.1.col"<<endl;
    cout<<"1-----"<<"./DSJC250.1.col"<<endl;
    cout<<"2-----"<<"./DSJC250.5.col"<<endl;
    cout<<"3-----"<<"./DSJC250.9.col"<<endl;
    cout<<"4-----"<<"./DSJC500.1.col"<<endl;
    cout<<"5-----"<<"./DSJC500.5.col"<<endl;
    cout<<"6-----"<<"./DSJC500.9.col"<<endl;
    cout<<"7-----"<<"./DSJC1000.1.col"<<endl;
    cout<<"8-----"<<"./DSJC1000.5.col"<<endl;
    cout<<"9-----"<<"./DSJC1000.9.1.col"<<endl;


    while(cin>>choice)
    {
        if(choice>8||choice<0)
    {
        cout<<"输入正确值"<<endl;continue;
    }
    else break;
    }
    cout<<"输入颜色数"<<endl;
    cin>>color;
    createGraph(color,s[choice]);
    ofile<<s[choice]<<"|";
    cout<<"选择算法"<<endl;
    cout<<"0-----"<<"禁忌"<<endl;
    cout<<"1-----"<<"混合进化"<<endl;
    while(cin>>choice)
    {
        if(choice<0||choice>1)
        {
            cout<<"输入正确值"<<endl;continue;
        }
        else break;
    }

    ofile<<color<<"|";
    if(choice == 0)
    {
        TabuSearch obj;
        //obj.showcolortable();
        obj.run(0,0,0);
        obj.get_fs();
        obj.record(ofile);
        obj.showsol();
        obj.free_alloc();
    }

    else{

        Population p(10,100000,100000);
        p.run();
        p.record(ofile);
    }

    free_alloc();

}


