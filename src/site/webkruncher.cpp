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
#include <db/auth/infoxmlauth.h>
#include <db/site/infodataservice.h>

	const string ServiceName( "WebKruncher" );
	struct DataResource : InfoKruncher::Resource
	{
		DataResource( const InfoKruncher::Responder& _responder, InfoDataService::VisitorBase& _visitor  ) 
			: 
				InfoKruncher::Resource( _responder ), 
				Status( 0 ) ,
				visitor( _visitor )
		{}
		virtual operator bool ();
		int Status;
		protected:
		InfoDataService::VisitorBase& visitor;
	};
	
	DataResource::operator bool ()
	{
		const bool IsDefault( responder.IsDefault() );
		uri= ( IsDefault  ? "index.html" : string(".") + responder.resource );
		contenttype=( Hyper::ContentType( uri ) );

		if ( 
			( ! IsDefault ) && 
			( visitor.IsNewCookie() )  &&
			( contenttype != "text/javascript" )
		)
		{
			payload << "Cookies are required to enter this site";
			cerr << "No Cookies:" << endl;
			contenttype="text/html";
			Status=422;
			return true;
		}

		const string filename( responder.options.path + uri );
		LoadFile( filename.c_str(), payload );
		return true;
	}
	string WebKruncher::LoadResponse( InfoKruncher::Responder& r  )
	{
		DbRecords::RecordSet<InfoDataService::Visitor> records;
		records+=r;

		DataResource Payload( r, records );
		if ( ! Payload ) return "";

		if ( Payload.Status )
		{
			InfoKruncher::RestResponse respond( Payload.Status, Payload.contenttype, ServiceName, false, "", "", Payload.payload.str() );
			return respond;
		}

		const string& uri( Payload.uri );
		int status( Payload.Status );

		InfoDb::Site::Roles roles( uri, r.headers, r.ipaddr, r.options.text );	

		const string& contenttype( Payload.contenttype );
		const stringstream& ss( Payload.payload );
		if ( ss.str().size() ) status=200;
		
		InfoAuth::Authorization auth( ss.str(), contenttype, roles );

		const string& Text( auth );
		status=auth;

		InfoKruncher::RestResponse respond( status, contenttype, ServiceName, records.IsNewCookie(), records.CookieName(), records.Cookie(), Text );
		return respond;

#if 0
		DbRecords::RecordSet<InfoDataService::Visitor> records;
		records+=r;

		DataResource Payload( r, records );
		if ( ! Payload ) return "";

		const string& uri( Payload.uri );

		InfoDb::Site::Roles roles( uri, r.headers, r.ipaddr, r.options.text );	
		int status( 400 );

		const string& contenttype( Payload.contenttype );
		const stringstream& ss( Payload.payload );
		if ( ss.str().size() ) status=200;
		
		InfoAuth::Authorization auth( ss.str(), contenttype, roles );

		const string& Text( auth );
		status=auth;

		stringstream ssmsg;
		if ( r.options.protocol == InfoKruncher::https ) ssmsg << "https";
		if ( r.options.protocol == InfoKruncher::http )  ssmsg << "http";


		stringstream response;
		response << "HTTP/1.1 ";
		response << status << " " << Hyper::statusText(status) << endl;
		response << "Content-Type: " << contenttype << endl;
		response << "Server: InfoSite" << endl;
		response << "Connection: close" << endl;
		response << "Content-Length:" << Text.size() << endl;
		if ( records.IsNewCookie() ) response << "Set-Cookie:" << records.CookieName() << "=" << records.Cookie() << ";" << endl;
		response << endl;

		response << Text;

		string s( response.str() );
		return s;
#endif
	}

	void WebKruncher::Throttle( const InfoKruncher::SocketProcessOptions& svcoptions )
	{
		usleep( (rand()%10)+20 );
	}


