#include "sockethelper.hxx"

//------------------------------------------------------------------------
// Ip version definition
//------------------------------------------------------------------------
#define IP_VER     4                   /// currently only IPv4 is considered.

//------------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------------

/** compare two OUString.
*/
sal_Bool compareUString( const ::rtl::OUString & ustr1, const ::rtl::OUString & ustr2 )
{
    sal_Bool bOk = ustr1.equalsIgnoreAsciiCase( ustr2 );
    
    return bOk;
}

/** compare a OUString and an ASCII string.
*/
sal_Bool compareUString( const ::rtl::OUString & ustr, const sal_Char *astr )
{
    ::rtl::OUString ustr1 = rtl::OUString::createFromAscii( astr );
    sal_Bool bOk = ustr1.equalsIgnoreAsciiCase( ustr1 );
    
    return bOk;
}

/** compare two socket address.
*/
sal_Bool compareSocketAddr( const ::osl::SocketAddr & addr1 , const ::osl::SocketAddr & addr2  )
{
    return ( ( sal_True == compareUString( addr1.getHostname( 0 ), addr2.getHostname( 0 ) ) ) && ( addr2.getPort( ) == addr2.getPort( ) ) );
}

/*char * oustring2char( const ::rtl::OUString & str )
{
    rtl::OString aString;
    aString = ::rtl::OUStringToOString( str, RTL_TEXTENCODING_ASCII_US );
    printf( "oustring2char %s\n", aString.getStr( ) );
    sal_Char * sStr = aString.getStr( );
    return (char *)sStr;
}*/

/** print a UNI_CODE String. And also print some comments of the string. 
*/
void printUString( const ::rtl::OUString & str, const char* msg)
{
    printf( "#%s #printUString_u# ", msg );
    rtl::OString aString;
    aString = ::rtl::OUStringToOString( str, RTL_TEXTENCODING_ASCII_US );
    //char * sStr = aString.getStr( );
    printf( "%s\n", aString.getStr( ) );
}

/** get the local host name. 
    mindy: gethostbyname( "localhost" ), on Linux, it returns the hostname in /etc/hosts + domain name,
    if no entry in /etc/hosts, it returns "localhost" + domain name
*/
::rtl::OUString getHost( void )
{
    struct hostent *hptr;

    hptr = gethostbyname( "localhost" ); 
    OSL_ENSURE( hptr != NULL, "#In getHostname function, error on gethostbyname()" );
    ::rtl::OUString aUString = ::rtl::OUString::createFromAscii( (const sal_Char *) hptr->h_name );

    return aUString;
}

/** get the full host name of the current processor, such as "aegean.prc.sun.com" --mindyliu
*/
::rtl::OUString getThisHostname( void )
{
    ::rtl::OUString aUString;
#ifdef WNT
    struct hostent *hptr;
    hptr = gethostbyname( "localhost" ); 
    OSL_ENSURE( hptr != NULL, "#In getHostname function, error on gethostbyname()" );
    aUString = ::rtl::OUString::createFromAscii( (const sal_Char *) hptr->h_name );
#else
    char hostname[255];
    int nRes = gethostname(hostname, 255);
    OSL_ENSURE( nRes == 0, "#Error: gethostname failed."  );
    
    struct hostent *hptr;
    //first search /ets/hosts, then search from dns
    hptr = gethostbyname( hostname); 
    if ( hptr != NULL )
    {
        strcpy( hostname, hptr->h_name );
    }
    
    printf("# hostname is %s \n", hostname );		
    aUString = ::rtl::OUString::createFromAscii( (const sal_Char *) hostname );
#endif
    return aUString;	
}

/** get IP by name, search /etc/hosts first, then search from dns, fail return OUString("")
*/
::rtl::OUString getIPbyName( rtl::OString const& str_name )
{
    ::rtl::OUString aUString;
    struct hostent *hptr;
    //first search /ets/hosts, then search from dns
    hptr = gethostbyname( str_name.getStr()); 
    if ( hptr != NULL )
    {
        struct in_addr ** addrptr;
        addrptr = (struct in_addr **) hptr->h_addr_list ;
        //if there are more than one IPs on the same machine, we select one 
        for (; *addrptr; addrptr++)
        {
            printf("#Local IP Address: %s\n", inet_ntoa(**addrptr));	
            aUString = ::rtl::OUString::createFromAscii( (sal_Char *) (inet_ntoa(**addrptr)) );
        }
    }
    return aUString;
}

/** get local ethernet IP
*/
::rtl::OUString getLocalIP( )
{
    char hostname[255];
    gethostname(hostname, 255);
    
        return getIPbyName( hostname );	
}

/** construct error message
*/
::rtl::OUString outputError( const ::rtl::OUString & returnVal, const ::rtl::OUString & rightVal, const sal_Char * msg )
{	
    ::rtl::OUString aUString;
    if ( returnVal.equals( rightVal ) )
        return aUString;
    aUString += ::rtl::OUString::createFromAscii(msg);
    aUString += ::rtl::OUString::createFromAscii(": the returned value is '");
    aUString += returnVal;
    aUString += ::rtl::OUString::createFromAscii("', but the value should be '");
    aUString += rightVal;
    aUString += ::rtl::OUString::createFromAscii("'.");
    return aUString;
}

/** wait _nSec seconds.
*/
void thread_sleep( sal_Int32 _nSec )
{
    /// print statement in thread process must use fflush() to force display.
    printf( "# wait %d seconds. ", _nSec );
    fflush(stdout);
        
#ifdef WNT                               //Windows
    Sleep( _nSec * 100 );
#endif
#if ( defined UNX ) || ( defined OS2 )   //Unix
    usleep(_nSec * 100000);
#endif
    printf( "# done\n" ); 
}

/** print Boolean value.
*/
void printBool( sal_Bool bOk )
{
    printf( "#printBool# " );
    ( sal_True == bOk ) ? printf( "YES!\n" ): printf( "NO!\n" );		
}

/** print content of a ByteSequence.
*/
void printByteSequence_IP( const ::rtl::ByteSequence & bsByteSeq, sal_Int32 nLen )
{
     printf( "#ByteSequence is: " );
    for ( int i = 0; i < nLen; i++ ){
        if ( bsByteSeq[i] < 0 )
            printf("%d ",  256 + bsByteSeq[i] );
        else 
            printf("%d ",  bsByteSeq[i] );
    }
    printf( " .\n" );
}

/** convert an IP which is stored as a UString format to a ByteSequence array for later use.
*/
::rtl::ByteSequence UStringIPToByteSequence( ::rtl::OUString aUStr )
{

    rtl::OString aString = ::rtl::OUStringToOString( aUStr, RTL_TEXTENCODING_ASCII_US );
    const sal_Char *pChar = aString.getStr( ) ;
    sal_Char tmpBuffer[4];
    sal_Int32 nCharCounter = 0;
    ::rtl::ByteSequence bsByteSequence( IP_VER );
    sal_Int32 nByteSeqCounter = 0;

    for ( int i = 0; i < aString.getLength( ) + 1 ; i++ )
    {
        if ( ( *pChar != '.' ) && ( i !=aString.getLength( ) ) )
            tmpBuffer[nCharCounter++] = *pChar;
        else 
        {
            tmpBuffer[nCharCounter] = '\0';
            nCharCounter = 0;
            bsByteSequence[nByteSeqCounter++] = atoi( tmpBuffer );
        }
        pChar++;
    }
    return bsByteSequence;
}

/** print a socket result name.
*/
void printSocketResult( oslSocketResult eResult )
{
    printf( "#printSocketResult# " );
    if (!eResult)
    switch (eResult)
    {
        case osl_Socket_Ok:
            printf( "client connected\n");
            break;
        case osl_Socket_Error:
            printf("got an error ... exiting\r\n\r\n" );
            break;
        case osl_Socket_TimedOut:
            printf( "timeout\n");
            break;
    }
}

/** if 4 parts of an IP addr are equal to specified values
*/
sal_Bool ifIpv4is( const ::rtl::ByteSequence Ipaddr, sal_Int8 seq1, sal_Int8 seq2, sal_Int8 seq3, sal_Int8 seq4 )
{ 
    if ( ( Ipaddr[0]  == seq1 ) && ( Ipaddr[1] == seq2 ) && ( Ipaddr[2] == seq3 ) && ( Ipaddr[3] == seq4 ) )
        return sal_True;
    return sal_False;
}







