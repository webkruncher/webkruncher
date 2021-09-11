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
#include <exexml.h>

	string WebKruncher::LoadResponse( Responder& r  )
	{
		int status( 400 );

		stringstream ss;

		const string contenttype(Hyper::ContentType( r.uri ));

		const string filename( r.options.path + r.uri );

		LoadFile(filename.c_str(), ss);
		if ( ss.str().size() ) status=200;

		stringstream ssmsg;
		if ( r.options.protocol == InfoKruncher::https ) ssmsg << "https";
		if ( r.options.protocol == InfoKruncher::http )  ssmsg << "http";

		ssmsg << fence << filename; 
		Log( "WebKruncher::LoadResponse", ssmsg.str() );


		const string host( Hyper::mimevalue( r.headers, "host" ) );
		const string ExistingCookie( Hyper::mimevalue( r.headers, "cookie" ) );
		const string CookieName( host );

		string NewCookie;

		if ( ExistingCookie.empty() )
		{
			NewCookie=KruncherTools::GetUuid();
			{stringstream ssl; ssl<<"Created uuid:" << NewCookie; Log( ssl.str() );}
			cerr << green << fence << r.uri << fence << yellow << host  << red << fence << NewCookie << fence << normal << endl;
		} else 
			cerr << green << fence << r.uri << fence << yellow << host  << teal << fence << ExistingCookie << fence << normal << endl;

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

		string s( response.str() );
		return s;
	}

	void WebKruncher::Throttle( const InfoKruncher::SocketProcessOptions& svcoptions )
	{
		usleep( (rand()%10)+20 );
	}


