//
// Created by ghost on 16-4-30.
//

#ifndef SIMPLEFTPSERVER_CTHREAD_H
#define SIMPLEFTPSERVER_CTHREAD_H

#include <pthread.h>



class CThread {
private:
    static void* Wrapper(void *arg);
    void *m_ReturnValue;
    pthread_t m_ThreadT;

    virtual void Execute() = 0;
public:
    CThread();
    void Run();
    void WaitFor();
    void* GetReturnValue();
};


#endif //SIMPLEFTPSERVER_CTHREAD_H
