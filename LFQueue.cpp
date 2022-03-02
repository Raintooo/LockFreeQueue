#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include "LFQueue.h"

using namespace std;

 MaxLib::LockFreeQueue<int> q(300);
// std::queue<char*> q;
std::atomic<int> g_i;

void test(int j)
{
    for(int i = 0; i < 30; i++)
    {
        q.push(new int(g_i++));
        usleep(j % 2 ? 5000 : 200);
        // cout<< "test1 "<< i<< endl;
    }
}

void rcv()
{
    int i = 0;
    while(1)
    {
        if(q.size() > 0)
        {
            int* v = q.pop();
            if(v)
            {
                cout<< *v<< " "<< endl;
            }
        }
        else
        {
            usleep(2000);
        }
    }
    cout<< endl;
}

int main()
{
    std::vector<std::thread> v;
    std::vector<std::thread> v2;
    
    for(int i = 0; i < 10; i++)
    {
        v.push_back(std::thread(test, i));
    }

    for(int i = 0; i < 10; i++)
    {
        v2.push_back(std::thread(rcv));
    } 

    for(int i = 0; i < 10; i++)
    {
        v[i].join();
    }   

    for(int i = 0; i < 10; i++)
    {
        v2[i].join();
    }   

    cout<< "element : "<< g_i<< endl;
}