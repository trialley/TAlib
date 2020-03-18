#include <iostream>
#include <memory>

using namespace std;

//一般的单例模式是懒汉单例模式
//建议不要用饿汉单例模式
//Meyers单例模式能够保证在多线程的情况下
//有且只有一个实例，但是不能保证初始化的
//时候操作原子性，初始化数据安全性
//建议使用Meyers单例模式
//如果需要更高要求的单例模式，可以在单例模式中
//使用锁机制

class Sington {
 public:
  Sington& getInstance() {
    static Sington sington;
    return sington;
  }
  Sington() {
    cout << "bengin init" << endl;
    __m_nData = 10;
    cout << "end init" << endl;
  }
  ~Sington() {
    cout << "destrory Sington" << endl;
  }

 private:
  int __m_nData;
  Sington(const Sington& sington) {}
  Sington& operator=(const Sington& sington) {}
};