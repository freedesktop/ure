
#include  <salhelper/dynload.hxx>
#include <rtl/ustring>
#include <stdio.h>
#include "samplelib.hxx"


using namespace salhelper;
using namespace rtl;


class SampleLibLoader 
    : public ::salhelper::ODynamicLoader<SampleLib_Api>
{
public:
    SampleLibLoader():
        ::salhelper::ODynamicLoader<SampleLib_Api>
            (::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( SAL_MODULENAME( "samplelib") ) ), 
             ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(SAMPLELIB_INIT_FUNCTION_NAME) ))  
        {}

};


int main( int argc, char *argv[ ], char *envp[ ] )
{
    SampleLibLoader Loader;
    SampleLibLoader Loader2;
    Loader= Loader2;
    SampleLib_Api *pApi= Loader.getApi();

    sal_Int32 retint= pApi->funcA( 10);
    double retdouble= pApi->funcB( 3.14);


    return 0;
}
