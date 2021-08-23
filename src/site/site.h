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




#ifndef WEBKRUNCHER_SITE_H
#define WEBKRUNCHER_SITE_H
#include <infotools.h>
using namespace KruncherTools;
#include <infossock.h>
#include <infosock.h>
#include <hyper.h>
#include <uuid.h>

namespace InfoKruncher
{
	using namespace std;


	struct ServiceList : vector< ServiceOptions >
	{
		virtual operator bool ()
		{
			cout << "Setting up options" << endl;
			{
				ServiceOptions o;
				o.port=81;
				o.protocol=ServiceOptions::Protocol::http;
				push_back( o );
			}
			{
				ServiceOptions o;
				o.port=82;
				o.protocol=ServiceOptions::Protocol::http;
				push_back( o );
			}
			return true;
		}
	};

	struct Options : KruncherTools::Args
	{
		Options() = delete;
		Options( int _argc, char** _argv ) : 
			Args( _argc,  _argv ),
			daemonize( true )
		{}
		virtual operator bool ()
		{
			if ( ! Args::operator bool () ) return false;
			if ( find( "-d" ) != end() ) daemonize=false;
			return !!servicelist;
		}
		ServiceList servicelist;
		bool daemonize;
	};


	struct Site : ServiceBase
	{
		void ForkAndServe( const ServiceOptions& );
		void Terminate();
		void Throttle( const ServiceOptions& );
		template< class SocketType >
			void ServePage( string& , KruncherTools::stringvector& , SocketType& );

		void ServeRequest( string& requestline, KruncherTools::stringvector& headers, SecureInformation::Socket& ss )
			{ ServePage(requestline, headers, ss ); }

		void ServeRequest( string& requestline, KruncherTools::stringvector& headers, PlainInformation::Socket& ss )
			{ ServePage(requestline, headers, ss ); }
	};

	struct ResponseBase
	{
		ResponseBase( const string _tbd, int& _status )
			: tbd( _tbd ), status( _status ) {}
		protected:
		template <class SocketType>
			void Serve( SocketType& sock, Hyper::Request< SocketType >& request );
		const string tbd;
		int& status;
	};

	template <class SocketType >
		struct ResponseHome : Hyper::Response< SocketType >, private ResponseBase
	{
		ResponseHome(Hyper::Request< SocketType >& _request, const string _tbd, int& _status) 
			: request(_request), ResponseBase(_tbd, _status) {}
		virtual void operator ()();
		protected:
		Hyper::Request< SocketType >& request;
	};

	template <class SocketType>
		struct RequestManager : Hyper::Request< SocketType >
	{
		RequestManager(const string& _request, const stringvector& _headers, SocketType& _sock ) :
			Hyper::HyperBase< SocketType >(_request, _headers, _sock ),
			Hyper::Request< SocketType >(_request, _headers, _sock )
		{}

		operator Hyper::Response< SocketType >& ();

		private:
		Hyper::Response< SocketType >* ifBinary(const string tbd)
		{
			if (Hyper::Request< SocketType >::request.find("GET /")==0) 
			{
				//if (request.find(".ico ")!=string::npos) if ( ! response.get() ) response=unique_ptr<Response< SocketType > >(new Response_Binary(*this, tbd, status));
				//if (request.find(".png ")!=string::npos) if ( ! response.get() ) response=unique_ptr<Response< SocketType > >(new Response_Binary(*this, tbd, status));
				//if (request.find(".jpg ")!=string::npos) if ( ! response.get() ) response=unique_ptr<Response< SocketType > >(new Response_Binary(*this, tbd, status));
				//if (request.find(".gif ")!=string::npos) if ( ! response.get() ) response=unique_ptr<Response< SocketType > >(new Response_Binary(*this, tbd, status));
			}
			return NULL;
		}
		int status;
	};

	struct Sites : vector< InfoKruncher::Service<InfoKruncher::Site> > 
	{
		void Terminate();
	};

} //namespace InfoKruncher

#endif // WEBKRUNCHER_SITE_H

