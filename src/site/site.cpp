/*
 * Copyright (c) Jack M. Thompson WebKruncher.com, exexml.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebKruncher nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Jack M. Thompson ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Jack M. Thompson BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <iostream>
#include <sstream>
#include <infokruncher.h>
#include <site.h>

using namespace std;
using namespace KruncherTools;

#include <infossock.h>
#include <infosock.h>

#include <process.h>

volatile bool TERMINATE(false);
using namespace KruncherTools;
#include <uuid/uuid.h>
using namespace Hyper;

namespace InfoKruncher
{
#ifdef UNIX
	inline string GetUuid()
	{
		uuid_t  UuId;
		unsigned int uuidstatus( 0 );
		uuid_create(&UuId, &uuidstatus); 
		char* result( NULL );	
		uuid_to_string(&UuId, &result, &uuidstatus);
		const string uuid( (char*) result );
		free( result );
		return uuid;
	}
#else
	inline string GetUuid()
	{
		unsigned char result[ 512 ];
		uuid_generate(result);
		const string uuid( (char*) result );
		return uuid;
	}
#endif

	template < class SocketType >
		void Site::GetPage( const Hyper::Request<SocketType>& request, const string& uri, const KruncherTools::stringvector& headers, SocketType& sock )
	{
		sock.flush();

		stringstream ss;

		const string contenttype(Hyper::ContentType(uri));

		LoadFile(uri.c_str(), ss);
		const int status( 200 );

		const string ExistingCookie( request.sValue( "Cookie" ) );
		//{stringstream ssl; ssl<<"ExistingCookie:" << ExistingCookie; Log( ssl.str() );}
		const string CookieName("webkruncher.com.wip");

		string NewCookie;

		if ( ExistingCookie.empty() )
		{
			NewCookie=GetUuid();
			{stringstream ssl; ssl<<"Created uuid:" << NewCookie; Log( ssl.str() );}
		}


		stringstream response;
		response << "HTTP/1.1 ";
		response << status << " " << Hyper::statusText(status) << endl;
		response << "Content-Type: " << contenttype << endl;
		response << "Server: WebKruncher" << endl;
		response << "Connection: close" << endl;
		response << "Content-Length:" << ss.str().size() << endl;
		if ( ! NewCookie.empty() ) response << "Set-Cookie:" << CookieName << "=" << NewCookie << ";" << endl;
		response << endl;
		response << ss.str();
		sock.write(response.str().c_str(), response.str().size());
		sock.flush();
	}

	template < class SocketType >
		void Site::ServePage( string& requestline, KruncherTools::stringvector& headers, SocketType& sock )
	{
		const Hyper::Request< SocketType > request( requestline, headers, sock );
		stringvector requestparts;
		requestparts.split( requestline, " " );
		if ( requestparts.size() < 3 ) return;
		const string method( requestparts[ 0 ] );
		const string resource( requestparts[ 1 ] );

		const string uri( ( ( method == "GET" ) && ( resource == "/" ) ) ? "index.html" : string(".") + resource );
		//{stringstream ss; ss << fence << "Request" << fence << method << fence << resource << fence << uri << fence; Log( ss.str() ); }

		GetPage< SocketType >( request, uri, headers, sock );
	}

	template<> void Service< Site >::ForkAndServe( const ServiceOptions& svcoptions )
	{
		if ( svcoptions.protocol == ServiceOptions::Protocol::http )  RunService< PlainInformation::streamingsocket  >( svcoptions );
		if ( svcoptions.protocol == ServiceOptions::Protocol::https ) RunService< SecureInformation::streamingsocket >( svcoptions );
	}

	template<> void Service< Site >::Terminate() { subprocesses.Terminate(); }

	void Site::Throttle( const ServiceOptions& svcoptions )
		{ usleep( (rand()%10)+20 ); }



	void Sites::Terminate()
	{
		for ( iterator it=begin(); it!=end(); it++ )
			it->Terminate();
	}


} //namespace InfoKruncher


int main( int argc, char** argv )
{
	stringstream ssexcept;
	try
	{
		chdir( "/home/jmt/websites/text/webkruncher" );



		Initialize();
		InfoKruncher::Options options( argc, argv );
		if ( ! options ) throw "Invalid options";
		KruncherTools::Daemonizer daemon( options.daemonize, "WebKruncher" );

		InfoKruncher::Sites sites;

		const InfoKruncher::ServiceList& servicelist( options.servicelist );
		for ( InfoKruncher::ServiceList::const_iterator it=servicelist.begin(); it!=servicelist.end(); it++ )
		{
			InfoKruncher::Service<InfoKruncher::Site> info;
			sites.push_back( info );
			InfoKruncher::Service<InfoKruncher::Site>& site( sites.back() );
			const InfoKruncher::ServiceOptions& svcoptions( *it );
			site.ForkAndServe( svcoptions);
		}
		while ( !TERMINATE ) usleep( (rand()%100000)+100000 );
		sites.Terminate();
	}
	catch( const exception& e ) { ssexcept<<e.what(); }
	catch( const string& s ) { ssexcept<<s;}
	catch( const char* s ) { ssexcept<<s;}
	catch( ... ) { ssexcept<<"unknown";}
	if ( ! ssexcept.str().empty() ) ExceptionLog( "main", ssexcept.str() );
	return 0;
}

