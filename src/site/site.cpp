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
#include <uuid.h>
using namespace Hyper;

namespace InfoKruncher
{
	template <class SocketType>
		void ResponseBase::Serve( SocketType& sock, Hyper::Request< SocketType >& request )
	{
		sock.flush();

		stringstream ss;
		string srequest(request.c_str());

		string file( tbd );
		const string contenttype(Hyper::ContentType(srequest));

		cout << "Loading " << file << endl;
		LoadFile(file.c_str(), ss);
		status=200;

		string ExistingCookie( request.sValue( "Cookie" ) );
		{stringstream ssl; ssl<<"ExistingCookie:" << ExistingCookie; Log( ssl.str() );}
		const string CookieName("webkruncher.com.wip");

		string NewCookie;

		if ( ExistingCookie.empty() )
		{
			uuid_t  UuId;
			unsigned int uuidstatus( 0 );
			uuid_create(&UuId, &uuidstatus);

			char* result( NULL );	
			uuid_to_string(&UuId, &result, &uuidstatus);
			if ( result ) 
			{
				NewCookie=result;
				free( result );
				{stringstream ssl; ssl<<"Created uuid:" << NewCookie; Log( ssl.str() );}
			} else {
				Log("Cannot create uuid");
			}
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
		void ResponseHome< SocketType >::operator ()()
	{
		SocketType& sock(request);
		Serve( sock, request );
	}


	template < class SocketType >
		RequestManager< SocketType >::operator Hyper::Response< SocketType >& ()
	{
		const string& Req( HyperBase< SocketType >::request );
		unique_ptr<Response<SocketType> >& Respond( Hyper::Request< SocketType >:: response );
		if (  Respond.get() ) return *Respond.get(); 
		const string tbd("index.html");

		ifBinary(tbd);
		if (  Respond.get() ) return *Respond.get(); 
		if ( Req.find("GET / ")==0) if ( ! Respond.get() ) Respond=unique_ptr<Response< SocketType > >( new ResponseHome< SocketType >(*this, tbd, status) );
		if ( Respond.get() ) { return *Respond.get(); }
		if ( ! Respond.get() ) throw string( "Can't get response" );
		return *Respond.get(); 
	}


	template < class SocketType >
		void Site::ServePage( string& requestline, KruncherTools::stringvector& headers, SocketType& ss )
	{
		RequestManager< SocketType > request(requestline, headers, ss );
		if (!request.resolve()) 
		{
			ss.close();
			return ;
		}
		Response< SocketType >& rq(request);
		rq();
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

