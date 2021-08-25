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


	string WebKruncher::LoadResponse( const string& uri, const stringvector& headers )
	{
		int status( 401 );

		stringstream ss;

		const string contenttype(Hyper::ContentType(uri));

		LoadFile(uri.c_str(), ss);
		if ( ss.str().size() ) status=200;

		const string ExistingCookie( Hyper::mimevalue( headers, "cookie" ) );
		{stringstream ssl; ssl<<"ExistingCookie:" << ExistingCookie; Log( ssl.str() );}
		const string CookieName("webkruncher.com.testsite");

		string NewCookie;

		if ( ExistingCookie.empty() )
		{
			NewCookie=KruncherTools::GetUuid();
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

		string s( response.str() );
		return s;
	}

