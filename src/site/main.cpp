
#include <infokruncher.h>
#include <infosite.h>

	struct ServiceList : vector< InfoKruncher::ServiceOptions >
	{
		virtual operator bool ()
		{
			{
				InfoKruncher::ServiceOptions o;
				o.port=80;
				o.protocol=InfoKruncher::ServiceOptions::Protocol::http;
				push_back( o );
			}
			return true;
		}
	};


	struct TestSite : InfoKruncher::Site
	{
		virtual string LoadResponse( const string& uri, const stringvector& headers )
		{

			int status( 401 );

			stringstream ss;

			const string contenttype(Hyper::ContentType(uri));

			LoadFile(uri.c_str(), ss);
			if ( ss.str().size() ) status=200;

			const string ExistingCookie( Hyper::mimevalue( headers, "cookie" ) );
			{stringstream ssl; ssl<<"ExistingCookie:" << ExistingCookie; Log( ssl.str() );}
			const string CookieName("webkruncher.com.wip");

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
	};

	struct Sites : vector< InfoKruncher::Service<TestSite> > 
	{
		void Terminate();
	};

	template<> void InfoKruncher::Service< TestSite >::ForkAndServe( const ServiceOptions& svcoptions )
	{
		if ( svcoptions.protocol == ServiceOptions::Protocol::http )  RunService< PlainInformation::streamingsocket  >( svcoptions );
		if ( svcoptions.protocol == ServiceOptions::Protocol::https ) RunService< SecureInformation::streamingsocket >( svcoptions );
	}

	template<> void InfoKruncher::Service< TestSite >::Terminate() 
		{ subprocesses.Terminate(); }

	void Sites::Terminate()
	{
		for ( iterator it=begin(); it!=end(); it++ )
			it->Terminate();
	}

int main( int argc, char** argv )
{
	stringstream ssexcept;
	try
	{
		chdir( "/home/jmt/websites/text/webkruncher" );

		Initialize();
		InfoKruncher::Options< ServiceList > options( argc, argv );
		if ( ! options ) throw "Invalid options";
		KruncherTools::Daemonizer daemon( options.daemonize, "WebKruncher" );

		Sites sites;

		const ServiceList& servicelist( options.servicelist );
		for ( ServiceList::const_iterator it=servicelist.begin(); it!=servicelist.end(); it++ )
		{
			InfoKruncher::Service<TestSite> info;
			sites.push_back( info );
			InfoKruncher::Service<TestSite>& site( sites.back() );
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

