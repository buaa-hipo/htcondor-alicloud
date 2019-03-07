#ifndef _CONDOR_GET_FUNCTION_H
#define _CONDOR_GET_FUNCTION_H

// #include "condor_common.h"
// #include "compat_classad.h"
// #include "classad_collection.h"
// #include "gahp-client.h"
// #include "Functor.h"
// #include "GetFunction.h"

class GetFunction : public Functor {
	public:
		GetFunction( const std::string & f, ClassAd * r, EC2GahpClient * g, ClassAd * s,
			const std::string & su, const std::string & pkf, const std::string & skf,
			ClassAdCollection * c, const std::string & cid ) :
			functionARN( f ), reply( r ), gahp( g ), scratchpad( s ),
			service_url( su ), public_key_file( pkf ), secret_key_file( skf ),
			commandID( cid ), commandState( c )
		{ ASSERT(! functionARN.empty()); }

		virtual ~GetFunction() { }

		virtual int operator() ();
		virtual int rollback();

	private:
		std::string functionARN;

		ClassAd * reply;
		EC2GahpClient * gahp;
		ClassAd * scratchpad;

		std::string service_url, public_key_file, secret_key_file;

		std::string commandID;
		ClassAdCollection * commandState;
};

#endif /* _CONDOR_GET_FUNCTION_H */
