


//TBD

#if 0 
		const string ipaddr( dotted( r.ipaddr ) );
		const char* cInfoTestIp( getenv( "INFO_TEST_IP" ) );
		if ( cInfoTestIp )
		{
			const string InfoTestIp( cInfoTestIp );
			Log( "INFO_TEST_IP", ipaddr + string("?" ) +  InfoTestIp );
			if ( ipaddr!=InfoTestIp )
			{
				const string filename( r.options.path + string( "UnderConstruction.html" ) );
				if ( ! FileExists( filename ) ) return new InfoKruncher::RestResponse( 404 , "text/html" , ServiceName, false, "", "", "<html>Not Found</html>" );
				stringstream sfile;
				LoadFile( filename.c_str(), sfile );
				return new InfoKruncher::RestResponse( 401 , "text/html" , ServiceName, false, "", "", sfile.str() );
			}
		}
#endif
