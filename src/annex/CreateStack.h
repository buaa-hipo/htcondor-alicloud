#ifndef _CONDOR_CREATE_STACK_H
#define _CONDOR_CREATE_STACK_H

// #include "condor_common.h"
// #include "compat_classad.h"
// #include "classad_collection.h"
// #include "gahp-client.h"
// #include "Functor.h"
// #include "CreateStack.h"

class CreateStack : public Functor {
	public:
		CreateStack( ClassAd * r, EC2GahpClient * g, ClassAd * s,
			const std::string & su, const std::string & pkf, const std::string & skf,
			const std::string & sn, const std::string & stu,
			const std::map< std::string, std::string > & p,
			ClassAdCollection * c, const std::string & cid ) :
			reply( r ), cfGahp( g ), scratchpad( s ),
			service_url( su ), public_key_file( pkf ), secret_key_file( skf ),
			stackName( sn ), stackURL( stu ), stackParameters( p ),
			commandState( c ), commandID( cid )
		{ }
		virtual ~CreateStack() { }

		virtual int operator() ();
		virtual int rollback();

	private:
		ClassAd * reply;
		EC2GahpClient * cfGahp;
		ClassAd * scratchpad;

		std::string service_url, public_key_file, secret_key_file;

		std::string stackName, stackURL;
		std::map< std::string, std::string > stackParameters;

		ClassAdCollection * commandState;
		std::string commandID;
};

#endif /* _CONDOR_CREATE_CREATE_STACK_H */
