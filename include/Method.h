#ifndef METHOD_H
#define METHOD_H

#include "Writable.h"
#include "IntWritable.h"
#include "Call.h"

using namespace std;

class Method
{
    public:
        Method();
        virtual ~Method();

        static void call(Server::Call*);
    protected:
    private:
};

#endif // METHOD_H
