#ifndef CALL_H
#define CALL_H

#include "Writable.h"
#include <string>

using namespace std;

namespace Server{

    class Call
    {
        public:
            Call();
            virtual ~Call();
        protected:
        private:

            string _class;
            string _method;
            vector<Writable> params;
            Writable value;
    };

}

#endif // CALL_H
