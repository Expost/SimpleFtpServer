//
// Created by ghost on 16-4-30.
//

#include "CThread.h"


CThread::CThread():m_ThreadT(0),m_ReturnValue(0) {

}

void CThread::Run(){
    pthread_create(&m_ThreadT,NULL,Wrapper,this);
}

void CThread::WaitFor() {
    if(m_ThreadT != 0){
        pthread_join(m_ThreadT,&m_ReturnValue);
    }
}

void* CThread::GetReturnValue() {
    return m_ReturnValue;
}


void* CThread::Wrapper(void *arg) {
    CThread *thread_this = (CThread*)arg;
    thread_this->Execute();
    pthread_exit(thread_this->m_ReturnValue);
}
