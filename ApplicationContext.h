#ifndef TEST_THREADS_APPLICATIONCONTEXT_H
#define TEST_THREADS_APPLICATIONCONTEXT_H
#include <mutex>
#include "StreamContext.h"
#include <vector>

using namespace std;

class ApplicationContext {
public:
    vector<StreamContext*> streams;
};


#endif //TEST_THREADS_APPLICATIONCONTEXT_H
