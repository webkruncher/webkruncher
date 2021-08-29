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


#include <infokruncher.h>
#include <infosite.h>
#include <webkruncher.h>


template<> void InfoKruncher::Service< WebKruncher >::ForkAndServe( const SocketProcessOptions& svcoptions )
{
	if ( svcoptions.protocol == SocketProcessOptions::Protocol::http )  RunService< streamingsocket  >( svcoptions );
	if ( svcoptions.protocol == SocketProcessOptions::Protocol::https ) RunService< streamingsocket >( svcoptions );
}

struct Sites : vector< InfoKruncher::Service<WebKruncher> > { void Terminate(); };
template<> void InfoKruncher::Service< WebKruncher >::Terminate() { subprocesses.Terminate(); }
void Sites::Terminate() { for ( iterator it=begin(); it!=end(); it++ ) it->Terminate(); }



int main( int argc, char** argv )
{
	stringstream ssexcept;
	try
	{
		cout << yellow << "webkruncher is starting up" << normal << endl;
		Initialize();
		InfoKruncher::Options< ServiceList > options( argc, argv );
		if ( ! options ) throw "Invalid options";
		KruncherTools::Daemonizer daemon( options.daemonize, "WebKruncher" );

		Sites sites;

		const ServiceList& servicelist( options.servicelist );
		for ( ServiceList::const_iterator it=servicelist.begin(); it!=servicelist.end(); it++ )
		{
			InfoKruncher::Service<WebKruncher> info;
			sites.push_back( info );
			InfoKruncher::Service<WebKruncher>& site( sites.back() );
			const InfoKruncher::SocketProcessOptions& svcoptions( *it );
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

