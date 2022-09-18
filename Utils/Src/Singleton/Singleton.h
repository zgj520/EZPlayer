#ifndef SINGLETON_H_
#define SINGLETON_H_

template<class T>
class Singleton
{
public:
    static T* getInstance() {
        static T instance;
        return &instance;
    }
};

#endif