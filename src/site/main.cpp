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


namespace InfoKruncher
{
	template<> 
		void InfoKruncher::Service< WebKruncher >::ForkAndServe( const SocketProcessOptions& svcoptions )
	{
		RunService( svcoptions );
	}
	template<> void InfoKruncher::Service< WebKruncher >::Terminate() { subprocesses.Terminate(); }
} // InfoKruncher

//struct Sites : vector< InfoKruncher::Service<WebKruncher> > { void Terminate(); };
//void Sites::Terminate() { for ( iterator it=begin(); it!=end(); it++ ) it->Terminate(); }



int main( int argc, char** argv )
{
	stringstream ssexcept;
	try
	{
		Initialize();
		InfoKruncher::Options< ServiceList > options( argc, argv );
		if ( ! options ) throw string( "Invalid options" );

		const ServiceList& workerlist( options.workerlist );

		const size_t nSites( options.workerlist.size() );

		if ( options.find( "--check-config" ) != options.end() )
		{
			cerr << "Configuration:" << endl << workerlist << endl;
			return 0;
		}
		
		cerr << yellow << "webkruncher is starting up" << normal << endl;
		KruncherTools::Daemonizer daemon( options.daemonize, "WebKruncher" );

		InfoKruncher::Service<WebKruncher> sites[ nSites ];

		for ( size_t c=0;  c < nSites; c++ )
		{
			InfoKruncher::Service<WebKruncher>& site( sites[ c ] );
			const InfoKruncher::SocketProcessOptions& svcoptions( workerlist[ c ] );
			site.ForkAndServe( svcoptions);
		}
		while ( !TERMINATE ) usleep( (rand()%100000)+100000 );
		Log( "webkruncher is exiting" );
		for ( size_t t=0; t < nSites; t++ ) sites[ t ].Terminate();
	}
	catch( const exception& e ) { ssexcept<<e.what(); }
	catch( const string& s ) { ssexcept<<s;}
	catch( const char* s ) { ssexcept<<s;}
	catch( ... ) { ssexcept<<"unknown";}
	if ( ! ssexcept.str().empty() ) ExceptionLog( "main", ssexcept.str() );

	return 0;
}

