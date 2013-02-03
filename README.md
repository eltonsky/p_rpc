p_rpc
=====

A efficient non-blocking rpc framework. It's written in c++, mostly using boost::asio. p_rpc is 
a general purpose framework. It's currently used as the rpc module of pfs. 

As c++ doesn't support reflection, the map of <class,method> for a rpc call part is handled with 
a configuration file. This is suppoosed to be fixed in future version.



