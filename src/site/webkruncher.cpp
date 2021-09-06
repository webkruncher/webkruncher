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


string Tabify( const string line, const int ntabs )
{
	stringstream ss;
	stringvector lines;
	lines.split( line, "\n" );
	for ( stringvector::iterator it=lines.begin();it!=lines.end();it++)
	{
		string L( *it );
		for ( int j=0; j<ntabs; j++ ) ss << tab;
		ss << L << endl;
		
	}
	return ss.str();
}


namespace ServiceXml
{
	using namespace XmlFamily;

	struct Item : XmlNode
	{
		friend struct Configuration;
		virtual XmlNodeBase* NewNode(Xml& _doc,XmlNodeBase* parent,stringtype name) 
		{ 
			XmlNodeBase* ret(NULL);
			ret=new Item(_doc,parent,name,servicelist); 
			ret->SetTabLevel( __tablevel+1 );
			return ret;
		}
		virtual ostream& operator<<(ostream& o) const ;
		virtual bool operator()(ostream& o) { return XmlNode::operator()(o); }
		Item(Xml& _doc,const XmlNodeBase* _parent,stringtype _name, ServiceList& _servicelist ) : XmlNode(_doc,_parent,_name ), servicelist( _servicelist ) {}
		operator bool () const
		{
			if ( name == "site" ) 
			{
				InfoKruncher::SocketProcessOptions o;
				Load( o );
				servicelist.push_back( o );
			}
			Load( NodeOptions );

			for (XmlFamily::XmlNodeSet::const_iterator it=children.begin();it!=children.end();it++) 
			{
				const Item& n=static_cast<const Item&>(*(*it));
				if (!n) return false;
			}
			return true;
		}
	
		private:
		void Load( InfoKruncher::SocketProcessOptions& options ) const
		{
				for(XmlFamily::XmlAttributes::const_iterator it=attributes.begin();
					it!=attributes.end();it++)
						options( it->first, it->second );
		}
		ServiceList& servicelist;
		mutable InfoKruncher::SocketProcessOptions NodeOptions;
	};
	inline ostream& operator<<(ostream& o,const Item& xmlnode){return xmlnode.operator<<(o);}


		ostream& Item::operator<<(ostream& o)  const
		{
			if ( name != "root" )
			{
				for ( int j=0; j<__tablevel; j++ ) o << tab;
				o << name << endl ;
				stringstream ss;
				ss << NodeOptions ;
				const string st( Tabify( ss.str(), __tablevel ) );
				o << st ;
			}

			for (XmlFamily::XmlNodeSet::const_iterator it=children.begin();it!=children.end();it++) 
			{
				const Item& n=static_cast<const Item&>(*(*it));
				o  << n;
			}
			return o;
		}


	struct Configuration : Xml
	{
		Configuration( ServiceList& _servicelist ) : servicelist( _servicelist ) {}
		virtual XmlNode* NewNode(Xml& _doc,stringtype name) { return new Item(_doc,NULL,name, servicelist); }
		ostream& operator<<(ostream& o) const //
		{
			if ( ! Root ) return o;
			const Item& nodes( static_cast< const Item& >( *Root ) );
			o << nodes;
			return o;
		}
		operator Item& () { if (!Root) throw string("No root node"); return static_cast<Item&>(*Root); }
		operator bool() const
		{
			if ( ! Root ) return false;
			const Item& item( static_cast< Item& >( *Root ) );
			return !!item;
		}
		private:
		ServiceList& servicelist;
	};
	inline ostream& operator<<(ostream& o,Configuration& xml){return xml.operator<<(o);}
} // ServiceXml

	bool ServiceList::operator()( const KruncherTools::Args& options)
	{

		KruncherTools::Args::const_iterator xmlname( options.find( "--xml" ) );
		if ( xmlname != options.end() )
		{
			ServiceXml::Configuration xml( *this );
			const string xmltxt( LoadFile( xmlname->second ) );
			if ( xmltxt.empty() ) return false;
			xml.Load( (char*)xmltxt.c_str() );
			if (  ! xml ) return false;;
			if ( options.find( "--check-config" ) != options.end() )
				cerr << xml << endl;
		}
			
		
		
		if ( options.find( "--http" ) != options.end() )
		{
			InfoKruncher::SocketProcessOptions o;
			o.port=80;
			o.protocol=InfoKruncher::SocketProcessOptions::Protocol::http;
			o.path="/home/jmt/websites/text/webkruncher/";
			push_back( o );
		}

		if ( options.find( "--https" ) != options.end() )
		{
			SecureInformation::init_openssl();
			InfoKruncher::SocketProcessOptions o;
			o.port=443;
			o.protocol=InfoKruncher::SocketProcessOptions::Protocol::https;
			o.path="/home/jmt/websites/text/webkruncher/";

			const string passwordfile( "/etc/webkruncher.pwd" );
			if ( KruncherTools::FileExists( passwordfile ) )
			{
				o.keypasswd=KruncherTools::LoadFile( passwordfile );
			} else {
				cout << "Ssl Password: ";
				o.keypasswd=KruncherTools::getpass();
			}

			const string certs( "/etc/certs/webkruncher/" );
			o.cadir=certs;
			o.certfile=certs+string("WEBKRUNCHER.COM.crt");
			o.cafile=certs+string("dv_chain.txt");
			o.keyfile=certs+string("server.key");
			push_back( o );
		}
		return true;
	}

	string WebKruncher::LoadResponse( Responder& r  )
	{
		int status( 401 );

		stringstream ss;

		const string contenttype(Hyper::ContentType( r.uri ));

		const string filename( r.options.path + r.uri );

		LoadFile(filename.c_str(), ss);
		if ( ss.str().size() ) status=200;

		stringstream ssmsg;
		if ( r.options.protocol == InfoKruncher::SocketProcessOptions::Protocol::https ) ssmsg << "https";
		if ( r.options.protocol == InfoKruncher::SocketProcessOptions::Protocol::http )  ssmsg << "http";

		ssmsg << fence << filename; 
		Log( "WebKruncher::LoadResponse", ssmsg.str() );

		const string ExistingCookie( Hyper::mimevalue( r.headers, "cookie" ) );
		const string CookieName("webkruncher.com");

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

	void WebKruncher::Throttle( const InfoKruncher::SocketProcessOptions& svcoptions )
	{
		usleep( (rand()%10)+20 );
	}


