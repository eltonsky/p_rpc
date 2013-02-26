#ifndef METHODWRITABLE_H
#define METHODWRITABLE_H

#include "Writable.h"
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class MethodWritable : public Writable
{
    public:
        MethodWritable();
        MethodWritable(string,string,vector<Writable>&);
        virtual ~MethodWritable();

        virtual int readFields(tcp::socket * sock);
        virtual int write(tcp::socket * sock);

    protected:
    private:
        string _class_name;
        string _method_name;
        vector<Writable> _params;
};

#endif // METHODWRITABLE_H
