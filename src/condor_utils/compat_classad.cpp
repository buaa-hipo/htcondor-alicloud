/***************************************************************
 *
 * Copyright (C) 1990-2007, Condor Team, Computer Sciences Department,
 * University of Wisconsin-Madison, WI.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License.  You may
 * obtain a copy of the License at
 * 
 *    http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************/
#include "condor_common.h"
#include "compat_classad.h"

#include "condor_classad.h"
#include "classad_oldnew.h"
#include "condor_attributes.h"
#include "classad/xmlSink.h"
#include "condor_config.h"
#include "Regex.h"
#include "classad/classadCache.h"
#include "env.h"
#include "condor_arglist.h"
#define CLASSAD_USER_MAP_RETURNS_STRINGLIST 1

class MapFile;
extern int reconfig_user_maps();
extern bool user_map_do_mapping(const char * mapname, const char * input, MyString & output);

#if defined(HAVE_DLOPEN)
#include <dlfcn.h>
#endif

using namespace std;

// gcc 4.3.4 doesn't seem to define FLT_MIN on OpenSolaris 2009.06
#if !defined(FLT_MIN) && defined(__FLT_MIN__)
  #define FLT_MIN  __FLT_MIN__
#endif
#if !defined(FLT_MAX) && defined(__FLT_MAX__)
  #define FLT_MAX  __FLT_MAX__
#endif

// Utility to clarify a couple of evaluations
static inline bool
IsStringEnd(const char *str, unsigned off)
{
	return (  (str[off] == '\0') || (str[off] == '\n') || (str[off] == '\r')  );
}

namespace compat_classad {

static StringList ClassAdUserLibs;

static void registerClassadFunctions();
static void classad_debug_dprintf(const char *s);

bool ClassAd::m_initConfig = false;
bool ClassAd::m_strictEvaluation = false;

void ClassAd::
Reconfig()
{
	m_strictEvaluation = param_boolean( "STRICT_CLASSAD_EVALUATION", false );
	classad::SetOldClassAdSemantics( !m_strictEvaluation );

	classad::ClassAdSetExpressionCaching( param_boolean( "ENABLE_CLASSAD_CACHING", false ) );

	char *new_libs = param( "CLASSAD_USER_LIBS" );
	if ( new_libs ) {
		StringList new_libs_list( new_libs );
		free( new_libs );
		new_libs_list.rewind();
		char *new_lib;
		while ( (new_lib = new_libs_list.next()) ) {
			if ( !ClassAdUserLibs.contains( new_lib ) ) {
				if ( classad::FunctionCall::RegisterSharedLibraryFunctions( new_lib ) ) {
					ClassAdUserLibs.append( new_lib );
				} else {
					dprintf( D_ALWAYS, "Failed to load ClassAd user library %s: %s\n",
							 new_lib, classad::CondorErrMsg.c_str() );
				}
			}
		}
	}

	reconfig_user_maps();

	char *user_python_char = param("CLASSAD_USER_PYTHON_MODULES");
	if (user_python_char)
	{
		std::string user_python(user_python_char);
		free(user_python_char); user_python_char = NULL;
		char *loc_char = param("CLASSAD_USER_PYTHON_LIB");
		if (loc_char && !ClassAdUserLibs.contains(loc_char))
		{
			std::string loc(loc_char);
			if (classad::FunctionCall::RegisterSharedLibraryFunctions(loc.c_str()))
			{
				ClassAdUserLibs.append(loc.c_str());
#if defined(HAVE_DLOPEN)
				void *dl_hdl = dlopen(loc.c_str(), RTLD_LAZY);
				if (dl_hdl) // Not warning on failure as the RegisterSharedLibraryFunctions should have done that.
				{
					void (*registerfn)(void) = (void (*)(void))dlsym(dl_hdl, "Register");
					if (registerfn) {registerfn();}
					dlclose(dl_hdl);
				}
#endif
			}
			else
			{
				dprintf(D_ALWAYS, "Failed to load ClassAd user python library %s: %s\n",
					loc.c_str(), classad::CondorErrMsg.c_str());
			}
		}
		if (loc_char) {free(loc_char);}
	}
	if (!m_initConfig)
	{
		registerClassadFunctions();
		classad::ExprTree::set_user_debug_function(classad_debug_dprintf);
		m_initConfig = true;
	}
}

static classad::MatchClassAd *the_match_ad = NULL;
static bool the_match_ad_in_use = false;
classad::MatchClassAd *getTheMatchAd( classad::ClassAd *source,
									  classad::ClassAd *target )
{
	ASSERT( !the_match_ad_in_use );
	the_match_ad_in_use = true;

	if( !the_match_ad ) {
		the_match_ad = new classad::MatchClassAd( );
	}
	the_match_ad->ReplaceLeftAd( source );
	the_match_ad->ReplaceRightAd( target );

	if ( !ClassAd::m_strictEvaluation ) {
		source->alternateScope = target;
		target->alternateScope = source;
	}

	return the_match_ad;
}

void releaseTheMatchAd()
{
	ASSERT( the_match_ad_in_use );

	classad::ClassAd *ad;
	ad = the_match_ad->RemoveLeftAd();
	ad->alternateScope = NULL;
	ad = the_match_ad->RemoveRightAd();
	ad->alternateScope = NULL;

	the_match_ad_in_use = false;
}


static
bool stringListSize_func( const char * /*name*/,
						  const classad::ArgumentList &arg_list,
						  classad::EvalState &state, classad::Value &result )
{
	classad::Value arg0, arg1;
	std::string list_str;
	std::string delim_str = ", ";

	// Must have one or two arguments
	if ( arg_list.size() < 1 || arg_list.size() > 2 ) {
		result.SetErrorValue();
		return( true );
	}

	// Evaluate both arguments
	if( !arg_list[0]->Evaluate( state, arg0 ) ||
		( arg_list.size() == 2 && !arg_list[1]->Evaluate( state, arg1 ) ) ) {
		result.SetErrorValue();
		return false;
	}

	// If either argument isn't a string, then the result is
	// an error.
	if( !arg0.IsStringValue( list_str) ||
		( arg_list.size() == 2 && !arg1.IsStringValue( delim_str ) ) ) {
		result.SetErrorValue();
		return true;
	}

	StringList sl( list_str.c_str(), delim_str.c_str() );
	result.SetIntegerValue( sl.number() );

	return true;
}

static
double sum_func( double item, double accumulator )
{
	return item + accumulator;
}

static
double min_func( double item, double accumulator )
{
	return item < accumulator ? item : accumulator;
}

static
double max_func( double item, double accumulator )
{
	return item > accumulator ? item : accumulator;
}

static
bool stringListSummarize_func( const char *name,
							   const classad::ArgumentList &arg_list,
							   classad::EvalState &state, classad::Value &result )
{
	classad::Value arg0, arg1;
	std::string list_str;
	std::string delim_str = ", ";
	bool is_avg = false;
	double (* func)( double, double ) = NULL;
	double accumulator;
	bool is_real = false;
	bool empty_allowed = false;

	// Must have one or two arguments
	if ( arg_list.size() < 1 || arg_list.size() > 2 ) {
		result.SetErrorValue();
		return( true );
	}

	// Evaluate both arguments
	if( !arg_list[0]->Evaluate( state, arg0 ) ||
		( arg_list.size() == 2 && !arg_list[1]->Evaluate( state, arg1 ) ) ) {
		result.SetErrorValue();
		return false;
	}

	// If either argument isn't a string, then the result is
	// an error.
	if( !arg0.IsStringValue( list_str) ||
		( arg_list.size() == 2 && !arg1.IsStringValue( delim_str ) ) ) {
		result.SetErrorValue();
		return true;
	}

	if ( strcasecmp( name, "stringlistsum" ) == 0 ) {
		func = sum_func;
		accumulator = 0.0;
		empty_allowed = true;
	} else if ( strcasecmp( name, "stringlistavg" ) == 0 ) {
		func = sum_func;
		accumulator = 0.0;
		empty_allowed = true;
		is_avg = true;
	} else if ( strcasecmp( name, "stringlistmin" ) == 0 ) {
		func = min_func;
		accumulator = FLT_MAX;
	} else if ( strcasecmp( name, "stringlistmax" ) == 0 ) {
		func = max_func;
		accumulator = FLT_MIN;
	} else {
		result.SetErrorValue();
		return false;
	}

	StringList sl( list_str.c_str(), delim_str.c_str() );
	if ( sl.number() == 0 ) {
		if ( empty_allowed ) {
			result.SetRealValue( 0.0 );
		} else {
			result.SetUndefinedValue();
		}
		return true;
	}

	sl.rewind();
	const char *entry;
	while ( (entry = sl.next()) ) {
		double temp;
		int r = sscanf(entry, "%lf", &temp);
		if (r != 1) {
			result.SetErrorValue();
			return true;
		}
		if (strspn(entry, "+-0123456789") != strlen(entry)) {
			is_real = true;
		}
		accumulator = func( temp, accumulator );
	}

	if ( is_avg ) {
		accumulator /= sl.number();
	}

	if ( is_real ) {
		result.SetRealValue( accumulator );
	} else {
		result.SetIntegerValue( (long long)accumulator );
	}

	return true;
}

static
bool stringListMember_func( const char *name,
							const classad::ArgumentList &arg_list,
							classad::EvalState &state, classad::Value &result )
{
	classad::Value arg0, arg1, arg2;
	std::string item_str;
	std::string list_str;
	std::string delim_str = ", ";

	// Must have two or three arguments
	if ( arg_list.size() < 2 || arg_list.size() > 3 ) {
		result.SetErrorValue();
		return( true );
	}

	// Evaluate both arguments
	if( !arg_list[0]->Evaluate( state, arg0 ) ||
		!arg_list[1]->Evaluate( state, arg1 ) ||
		( arg_list.size() == 3 && !arg_list[2]->Evaluate( state, arg2 ) ) ) {
		result.SetErrorValue();
		return false;
	}

	// If any argument isn't a string, then the result is
	// an error.
	if( !arg0.IsStringValue( item_str) ||
		!arg1.IsStringValue( list_str) ||
		( arg_list.size() == 3 && !arg2.IsStringValue( delim_str ) ) ) {
		result.SetErrorValue();
		return true;
	}

	StringList sl( list_str.c_str(), delim_str.c_str() );
	int rc;
	if ( strcasecmp( name, "stringlistmember" ) == 0 ) {
		rc = sl.contains( item_str.c_str() );
	} else {
		rc = sl.contains_anycase( item_str.c_str() );
	}
	result.SetBooleanValue( rc ? true : false );

	return true;
}

static int regexp_str_to_options( const char *option_str )
{
	int options = 0;
	while (*option_str) {
		switch (*option_str) {
			case 'i':
			case 'I':
				options |= Regex::caseless;
				break;
			case 'm':
			case 'M':
				options |= Regex::multiline;
				break;
			case 's':
			case 'S':
				options |= Regex::dotall;
				break;
			case 'x':
			case 'X':
				options |= Regex::extended;
				break;
			default:
				// Ignore for forward compatibility 
				break;
		}
		option_str++;
	}
	return options;
}

static
bool stringListRegexpMember_func( const char * /*name*/,
								  const classad::ArgumentList &arg_list,
								  classad::EvalState &state,
								  classad::Value &result )
{
	classad::Value arg0, arg1, arg2, arg3;
	std::string pattern_str;
	std::string list_str;
	std::string delim_str = ", ";
	std::string options_str;

	// Must have two or three arguments
	if ( arg_list.size() < 2 || arg_list.size() > 4 ) {
		result.SetErrorValue();
		return( true );
	}

	// Evaluate both arguments
	if( !arg_list[0]->Evaluate( state, arg0 ) ||
		!arg_list[1]->Evaluate( state, arg1 ) ||
		( arg_list.size() >= 3 && !arg_list[2]->Evaluate( state, arg2 ) ) ||
		( arg_list.size() == 4 && !arg_list[3]->Evaluate( state, arg3 ) ) ) {
		result.SetErrorValue();
		return false;
	}

	// If any argument isn't a string, then the result is
	// an error.
	if( !arg0.IsStringValue( pattern_str) ||
		!arg1.IsStringValue( list_str) ||
		( arg_list.size() >= 3 && !arg2.IsStringValue( delim_str ) ) ||
		( arg_list.size() == 4 && !arg3.IsStringValue( options_str ) ) ) {
		result.SetErrorValue();
		return true;
	}

	StringList sl( list_str.c_str(), delim_str.c_str() );
	if ( sl.number() == 0 ) {
		result.SetUndefinedValue();
		return true;
	}

	Regex r;
	const char *errstr = 0;
	int errpos = 0;
	bool valid;
	int options = regexp_str_to_options(options_str.c_str());

	/* can the pattern be compiled */
	valid = r.compile(pattern_str.c_str(), &errstr, &errpos, options);
	if (!valid) {
		result.SetErrorValue();
		return true;
	}

	result.SetBooleanValue( false );

	sl.rewind();
	char *entry;
	while( (entry = sl.next())) {
		if (r.match(entry)) {
			result.SetBooleanValue( true );
		}
	}

	return true;
}

// map an input string using the given mapping set, this works using the
// same mapping function that the security layer uses to map security principals
// usage: 
//           // the 2 argument form returns a list of groups
//        userMap("user_to_groups", "user") -> {"group1","group2"}
// 
//           // the 3 argument form returns a single string. it will return the preferred group
//           // if the user is in that group. otherwise it will return the first group
//        userMap("user_to_groups", "user", "group2") -> "group2"
//        userMap("user_to_groups", "user", "group3") -> "group1"

//           // the 4 argument form return a single group. it will return the preferred group
//           // if the user is in that group. otherwise it will return the first group.
//           // If the user is not in any groups, it will return the 4th argument
//        userMap("user_to_groups", "user", "group2", "defgroup") -> "group2"
//        userMap("user_to_groups", "user", "group3", "defgroup") -> "group1"

static
bool userMap_func( const char * /*name*/,
	const classad::ArgumentList & arg_list,
	classad::EvalState & state,
	classad::Value & result)
{
	classad::Value mapVal, userVal, prefVal;

	size_t cargs = arg_list.size();
	if (cargs < 2 || cargs > 4) {
		result.SetErrorValue();
		return true;
	}
	if ( ! arg_list[0]->Evaluate(state, mapVal) ||
		 ! arg_list[1]->Evaluate(state, userVal) ||
		 (cargs >= 3 && !arg_list[2]->Evaluate(state, prefVal)) ||
		 (cargs >= 4 && !arg_list[3]->Evaluate(state, result))
		) {
		result.SetErrorValue();
		return false; // propagate evaluation failure.
	}

	std::string mapName, userName;
	if ( ! mapVal.IsStringValue(mapName) || ! userVal.IsStringValue(userName)) {
		if (mapVal.IsErrorValue() || userVal.IsErrorValue()) {
			result.SetErrorValue();
		} else if (cargs < 4) {
			result.SetUndefinedValue();
		}
		return true;
	}

	MyString output;
	if (user_map_do_mapping(mapName.c_str(), userName.c_str(), output)) {
		StringList items(output.Value(), ",");

		if (cargs == 2) {
			// 2 arg form, return a list.
		#ifdef CLASSAD_USER_MAP_RETURNS_STRINGLIST
			result.SetStringValue(output.c_str());
		#else
			classad_shared_ptr<classad::ExprList> lst( new classad::ExprList() );
			ASSERT(lst);
			for (const char * str = items.first(); str != NULL; str = items.next()) {
				classad::Value val; val.SetStringValue(str);
				lst->push_back(classad::Literal::MakeLiteral(val));
			}
			result.SetListValue(lst);
		#endif
		} else {
			// 3 arg form, return a string either the preferred item, or the first item
			// preferred item match is case-insensitive
			std::string pref;
			const char * selected_item = NULL;
			const bool any_case = true;
			if (prefVal.IsStringValue(pref)) { selected_item = items.find(pref.c_str(), any_case); }
		#if 1
			else if (prefVal.IsUndefinedValue()) { selected_item = items.first(); }
		#else
			// if preferred item is not in the list, use the first item
			if ( ! selected_item) { selected_item = items.first(); }
		#endif
			if (selected_item) {
				result.SetStringValue(selected_item);
			} else if (cargs < 4) {
				// if result has not been primed with the default value (i.e. this is the 3 arg form)
				// set the result to undefined now.
				result.SetUndefinedValue();
			}
		}
	} else if (cargs < 4) {
		// 2 or 3 arg form, and no mapping, return undefined.
		result.SetUndefinedValue();
	}

	return true;
}


// split user@domain or slot@machine, result is always a list of 2 strings
// if there is no @ in the input, then for user@domain the domain is empty
// for slot@machine the slot is empty
static
bool splitAt_func( const char * name,
	const classad::ArgumentList &arg_list,
	classad::EvalState &state, 
	classad::Value &result )
{
	classad::Value arg0;

	// Must have one argument
	if ( arg_list.size() != 1) {
		result.SetErrorValue();
		return true;
	}

	// Evaluate the argument
	if( !arg_list[0]->Evaluate( state, arg0 )) {
		result.SetErrorValue();
		return false;
	}

	// If either argument isn't a string, then the result is
	// an error.
	std::string str;
	if( !arg0.IsStringValue(str) ) {
		result.SetErrorValue();
		return true;
	}

	classad::Value first;
	classad::Value second;

	size_t ix = str.find_first_of('@');
	if (ix >= str.size()) {
		if (0 == strcasecmp(name, "splitslotname")) {
			first.SetStringValue("");
			second.SetStringValue(str);
		} else {
			first.SetStringValue(str);
			second.SetStringValue("");
		}
	} else {
		first.SetStringValue(str.substr(0, ix));
		second.SetStringValue(str.substr(ix+1));
	}

	classad_shared_ptr<classad::ExprList> lst( new classad::ExprList() );
	ASSERT(lst);
	lst->push_back(classad::Literal::MakeLiteral(first));
	lst->push_back(classad::Literal::MakeLiteral(second));

	result.SetListValue(lst);

	return true;
}

// split using arbitrary separator characters
static
bool splitArb_func( const char * /*name*/,
	const classad::ArgumentList &arg_list,
	classad::EvalState &state, 
	classad::Value &result )
{
	classad::Value arg0;

	// Must have one or 2 arguments
	if ( arg_list.size() != 1 && arg_list.size() != 2) {
		result.SetErrorValue();
		return true;
	}

	// Evaluate the first argument
	if( !arg_list[0]->Evaluate( state, arg0 )) {
		result.SetErrorValue();
		return false;
	}

	// if we have 2 arguments, the second argument is a set of
	// separator characters, the default set of separators is , and whitespace
	std::string seps = ", \t";
	classad::Value arg1;
	if (arg_list.size() >= 2 && ! arg_list[1]->Evaluate( state, arg1 )) {
		result.SetErrorValue();
		return false;
	}

	// If either argument isn't a string, then the result is
	// an error.
	std::string str;
	if( !arg0.IsStringValue(str) ) {
		result.SetErrorValue();
		return true;
	}
	if (arg_list.size() >= 2 && ! arg1.IsStringValue(seps)) {
		result.SetErrorValue();
		return true;
	}

	classad_shared_ptr<classad::ExprList> lst( new classad::ExprList() );
	ASSERT(lst);

	// walk the input string, splitting at each instance of a separator
	// character and discarding empty strings.  Thus leading and trailing
	// separator characters are ignored, and runs of multiple separator
	// characters have special treatment, multiple whitespace seps are
	// treated as a single sep, as are single separators mixed with whitespace.
	// but runs of a single separator are handled individually, thus
	// "foo, bar" is the same as "foo ,bar" and "foo,bar".  But not the same as
	// "foo,,bar", which produces a list of 3 items rather than 2.
	size_t ixLast = 0;
	classad::Value val;
	if (seps.length() > 0) {
		size_t ix = str.find_first_of(seps, ixLast);
		int      ch = -1;
		while (ix < str.length()) {
			if (ix - ixLast > 0) {
				val.SetStringValue(str.substr(ixLast, ix - ixLast));
				lst->push_back(classad::Literal::MakeLiteral(val));
			} else if (!isspace(ch) && ch == str[ix]) {
				val.SetStringValue("");
				lst->push_back(classad::Literal::MakeLiteral(val));
			}
			if (!isspace(str[ix])) ch = str[ix];
			ixLast = ix+1;
			ix = str.find_first_of(seps, ixLast);
		}
	}
	if (str.length() > ixLast) {
		val.SetStringValue(str.substr(ixLast));
		lst->push_back(classad::Literal::MakeLiteral(val));
	}

	result.SetListValue(lst);

	return true;
}


static void
problemExpression(const std::string &msg, classad::ExprTree *problem, classad::Value &result)
{
	result.SetErrorValue();
	classad::ClassAdUnParser up;
	std::string problem_str;
	up.Unparse(problem_str, problem);
	std::stringstream ss;
	ss << msg << "  Problem expression: " << problem_str;
	classad::CondorErrMsg = ss.str();
}


static bool
ArgsToList( const char * name,
	const classad::ArgumentList &arguments,
	classad::EvalState &state,
	classad::Value &result )
{
	if ((arguments.size() != 1) && (arguments.size() != 2))
	{
		std::stringstream ss;
		result.SetErrorValue();
		ss << "Invalid number of arguments passed to " << name << "; one string argument expected.";
		classad::CondorErrMsg = ss.str();
		return true;
	}
	int vers = 2;
	if (arguments.size() == 2)
	{
		classad::Value val;
		if (!arguments[1]->Evaluate(state, val))
		{
			problemExpression("Unable to evaluate second argument.", arguments[1], result);
			return false;
		}
		if (!val.IsIntegerValue(vers))
		{
			problemExpression("Unable to evaluate second argument to integer.", arguments[1], result);
			return true;
		}
		if ((vers != 1) && (vers != 2))
		{
			std::stringstream ss;
			ss << "Valid values for version are 1 or 2.  Passed expression evaluates to " << vers << ".";
			problemExpression(ss.str(), arguments[1], result);
			return true;
		}
	}
	classad::Value val;
	if (!arguments[0]->Evaluate(state, val))
	{
		problemExpression("Unable to evaluate first argument.", arguments[0], result);
		return false;
	}
	std::string args;
	if (!val.IsStringValue(args))
	{
		problemExpression("Unable to evaluate first argument to string.", arguments[0], result);
		return true;
	}
	ArgList arg_list;
	MyString error_msg;
	if ((vers == 1) && !arg_list.AppendArgsV1Raw(args.c_str(), &error_msg))
	{
		std::stringstream ss;
		ss << "Error when parsing argument to arg V1: " << error_msg.Value();
		problemExpression(ss.str(), arguments[0], result);
		return true;
	}
	else if ((vers == 2) && !arg_list.AppendArgsV2Raw(args.c_str(), &error_msg))
	{
		std::stringstream ss;
		ss << "Error when parsing argument to arg V2: " << error_msg.Value();
		problemExpression(ss.str(), arguments[0], result);
		return true;
	}
	std::vector<classad::ExprTree*> list_exprs;

	for (int idx=0; idx<arg_list.Count(); idx++)
	{
		classad::Value string_val;
		string_val.SetStringValue(arg_list.GetArg(idx));
		classad::ExprTree *expr = classad::Literal::MakeLiteral(string_val);
		if (!expr)
		{
			for (std::vector<classad::ExprTree*>::iterator it = list_exprs.begin(); it != list_exprs.end(); it++)
			{
				if (*it) {delete *it; *it=NULL;}
			}
			classad::CondorErrMsg = "Unable to create string expression.";
			result.SetErrorValue();
			return false;
		}
		list_exprs.push_back(expr);
	}
	classad_shared_ptr<classad::ExprList> result_list(classad::ExprList::MakeExprList(list_exprs));
	if (!result_list.get())
	{
		for (std::vector<classad::ExprTree*>::iterator it = list_exprs.begin(); it != list_exprs.end(); it++)
		{
			if (*it) {delete *it; *it=NULL;}
		}
		classad::CondorErrMsg = "Unable to create expression list.";
		result.SetErrorValue();
		return false;
	}
	result.SetListValue(result_list);
	return true;
}


static bool
ListToArgs(const char * name,
	const classad::ArgumentList &arguments,
	classad::EvalState &state,
	classad::Value &result)
{
	if ((arguments.size() != 1) && (arguments.size() != 2))
	{
		std::stringstream ss;
		result.SetErrorValue();
		ss << "Invalid number of arguments passed to " << name << "; one list argument expected.";
		classad::CondorErrMsg = ss.str();
		return true;
	}
	int vers = 2;
	if (arguments.size() == 2)
	{
		classad::Value val;
		if (!arguments[1]->Evaluate(state, val))
		{
			problemExpression("Unable to evaluate second argument.", arguments[1], result);
			return false;
		}
		if (!val.IsIntegerValue(vers))
		{
			problemExpression("Unable to evaluate second argument to integer.", arguments[1], result);
			return true;
		}
		if ((vers != 1) && (vers != 2))
		{
			std::stringstream ss;
			ss << "Valid values for version are 1 or 2.  Passed expression evaluates to " << vers << ".";
			problemExpression(ss.str(), arguments[1], result);
			return true;
		}
	}
	classad::Value val;
	if (!arguments[0]->Evaluate(state, val))
	{
		problemExpression("Unable to evaluate first argument.", arguments[0], result);
		return false;
	}
	classad_shared_ptr<classad::ExprList> args;
	if (!val.IsSListValue(args))
	{
		problemExpression("Unable to evaluate first argument to list.", arguments[0], result);
		return true;
	}
	ArgList arg_list;
	size_t idx=0;
	for (classad::ExprList::const_iterator it=args->begin(); it!=args->end(); it++, idx++)
	{
		classad::Value value;
		if (!(*it)->Evaluate(state, value))
		{
			std::stringstream ss;
			ss << "Unable to evaluate list entry " << idx << ".";
			problemExpression(ss.str(), *it, result);
			return false;
		}
		std::string tmp_str;
		if (!value.IsStringValue(tmp_str))
		{
			std::stringstream ss;
			ss << "Entry " << idx << " did not evaluate to a string.";
			problemExpression(ss.str(), *it, result);
			return true;
		}
		arg_list.AppendArg(tmp_str.c_str());
	}
	MyString error_msg, result_mystr;
	if ((vers == 1) && !arg_list.GetArgsStringV1Raw(&result_mystr, &error_msg))
	{
		std::stringstream ss;
		ss << "Error when parsing argument to arg V1: " << error_msg.Value();
		problemExpression(ss.str(), arguments[0], result);
		return true;
	}
	else if ((vers == 2) && !arg_list.GetArgsStringV2Raw(&result_mystr, &error_msg))
	{
		std::stringstream ss;
		ss << "Error when parsing argument to arg V2: " << error_msg.Value();
		problemExpression(ss.str(), arguments[0], result);
		return true;
	}
	result.SetStringValue(result_mystr.Value());
	return true;
}


static bool
EnvironmentV1ToV2(const char * name,
	const classad::ArgumentList &arguments,
	classad::EvalState &state,
	classad::Value &result)
{
	if (arguments.size() != 1)
	{
		std::stringstream ss;
		result.SetErrorValue();
		ss << "Invalid number of arguments passed to " << name << "; one string argument expected.";
		classad::CondorErrMsg = ss.str();
		return true;
	}
	classad::Value val;
	if (!arguments[0]->Evaluate(state, val))
	{
		problemExpression("Unable to evaluate first argument.", arguments[0], result);
		return false;
	}
		// By returning undefined if the input is undefined, the caller doesn't need an
		// ifThenElse statement to see if the V1 environment is used in the first place.
		// For example, we can do this:
		//   mergeEnvironment(envV1toV2(Env), Environment, "FOO=BAR");
		// without worrying about whether Env or Environment is already defined.
	if (val.IsUndefinedValue())
	{
		result.SetUndefinedValue();
		return true;
	}
	std::string args;
	if (!val.IsStringValue(args))
	{
		problemExpression("Unable to evaluate first argument to string.", arguments[0], result);
		return true;
	}
	Env env;
	MyString error_msg;
	if (!env.MergeFromV1Raw(args.c_str(), &error_msg))
	{
		std::stringstream ss;
		ss << "Error when parsing argument to environment V1: " << error_msg.Value();
		problemExpression(ss.str(), arguments[0], result);
		return true;
	}
	MyString result_mystr;
	env.getDelimitedStringV2Raw(&result_mystr, NULL);
	result.SetStringValue(result_mystr.Value());
	return true;
}


static bool
MergeEnvironment(const char * /*name*/,
	const classad::ArgumentList &arguments,
	classad::EvalState &state,
	classad::Value &result)
{
	Env env;
	size_t idx = 0;
	for (classad::ArgumentList::const_iterator it=arguments.begin(); it!=arguments.end(); it++, idx++)
	{
		classad::Value val;
		if (!(*it)->Evaluate(state, val))
		{
			std::stringstream ss;
			ss << "Unable to evaluate argument " << idx << ".";
			problemExpression(ss.str(), *it, result);
			return false;
		}
			// Skip over undefined values; this makes it more natural
			// to merge environments for a condor classad where the
			// user may or may not have specified an environment string.
		if (val.IsUndefinedValue())
		{
			continue;
		}
		std::string env_str;
		if (!val.IsStringValue(env_str))
		{
			std::stringstream ss;
			ss << "Unable to evaluate argument " << idx << ".";
			problemExpression(ss.str(), *it, result);
			return true;
		}
		MyString error_msg;
		if (!env.MergeFromV2Raw(env_str.c_str(), &error_msg))
		{
			std::stringstream ss;
			ss << "Argument " << idx << " cannot be parsed as environment string.";
			problemExpression(ss.str(), *it, result);
			return true;
		}
	}
	MyString result_mystr;
	env.getDelimitedStringV2Raw(&result_mystr, NULL);
	result.SetStringValue(result_mystr.Value());
	return true;
}


static bool
return_home_result(const std::string &default_home,
	const std::string &error_msg,
	classad::Value    &result,
	bool isError)
{
	if (default_home.size())
	{
		result.SetStringValue(default_home);
	}
	else
	{
		if (isError) {result.SetErrorValue();} else {result.SetUndefinedValue();}
		classad::CondorErrMsg = error_msg;
	}
	return true;
}

/**
 * Implementation of the userHome function.
 *
 * This returns the home directory of a given username as configured on the system.
 * The home directory is determined using the getpwdnam call.
 *
 * Arguments:
 *   - name.  The username to lookup.
 *   - default (optional).  The default value to return if no user home exists.
 *
 * Returns:
 *   - ERROR on Windows platforms.
 *   - ERROR if invoked with the incorrect number of arguments.
 *   - otherwise, `default` if default is given AND the value of `default` evaluates to a non-empty string AND:
 *     - the name cannot be evaluated to a string/UNDEFINED, OR
 *     - the user does not exist on the system, OR
 *     - the user does not have a home directory.
 *   - otherwise, ERROR if the first argument cannot be evaluated or does not evaluate to a string or
 *     UNDEFINED value (and default does not evaluate to a non-empty string).
 *   - otherwise, UNDEFINED if the first argument evaluates to UNDEFINED
 *   - otherwise, a string specifying the user's home directory.
 */

static bool
userHome_func(const char *                 name,
	const classad::ArgumentList &arguments,
	classad::EvalState          &state,
	classad::Value              &result)
{
	if ((arguments.size() != 1) && (arguments.size() != 2))
	{
		std::stringstream ss;
		result.SetErrorValue();
		ss << "Invalid number of arguments passed to " << name << "; " << arguments.size() << "given, 1 required and 1 optional.";
		classad::CondorErrMsg = ss.str();
		return false;
	}

#ifdef WIN32
	result.SetErrorValue();
	classad::CondorErrMsg = "UserHome is not available on Windows.";
	return true;
#endif

	std::string default_home;
	classad::Value default_home_value;
	if (arguments.size() != 2 || !arguments[1]->Evaluate(state, default_home_value) || !default_home_value.IsStringValue(default_home))
	{
		default_home = "";
	}

	std::string owner_string;
	classad::Value owner_value;
	if (!arguments[0]->Evaluate(state, owner_value))
	{
		
	}
	if (owner_value.IsUndefinedValue() && !default_home.size())
	{
		result.SetUndefinedValue();
		return true;
	}
	if (!owner_value.IsStringValue(owner_string))
	{
		std::string unp_string;
		std::stringstream ss;
		classad::ClassAdUnParser unp; unp.Unparse(unp_string, arguments[0]);
		ss << "Could not evaluate the first argument of " << name << " to string.  Expression: " << unp_string << ".";
		return return_home_result(default_home, ss.str(), result, true);
	}

	errno = 0;
#ifndef WIN32
		// The UserHome function has potential side-effects (such as hitting a LDAP server)
		// that might have a noticable impact on the system.  Disable unless explicitly requested.
		// (really, this only is useful in the very limited HTCondor-CE context.)
	if (!param_boolean("CLASSAD_ENABLE_USER_HOME", false ))
	{
		return return_home_result(default_home, "UserHome is currently disabled; to enable set CLASSAD_ENABLE_USER_HOME=true in the HTCondor config.",
			result, false);
	}
	struct passwd *info = getpwnam(owner_string.c_str());
	if (!info)
	{
		std::stringstream ss;
		ss << "Unable to find home directory for user " << owner_string;
		if (errno) {
			ss << ": " << strerror(errno) << "(errno=" << errno << ")";
		} else {
			ss << ": No such user.";
		}
		return return_home_result(default_home, ss.str(), result, false);
	}

	if (!info->pw_dir)
	{
		std::stringstream ss;
		ss << "User " << owner_string << " has no home directory.";
		return return_home_result(default_home, ss.str(), result, false);
	}
	std::string home_string = info->pw_dir;
	result.SetStringValue(home_string);
#endif

	return true;
}


static
void registerClassadFunctions()
{
	std::string name;
	name = "envV1ToV2";
	classad::FunctionCall::RegisterFunction(name, EnvironmentV1ToV2);

	name = "mergeEnvironment";
	classad::FunctionCall::RegisterFunction(name, MergeEnvironment);

	name = "listToArgs";
	classad::FunctionCall::RegisterFunction(name, ListToArgs);

	name = "argsToList";
	classad::FunctionCall::RegisterFunction(name, ArgsToList);

	name = "stringListSize";
	classad::FunctionCall::RegisterFunction( name,
											 stringListSize_func );
	name = "stringListSum";
	classad::FunctionCall::RegisterFunction( name,
											 stringListSummarize_func );
	name = "stringListAvg";
	classad::FunctionCall::RegisterFunction( name,
											 stringListSummarize_func );
	name = "stringListMin";
	classad::FunctionCall::RegisterFunction( name,
											 stringListSummarize_func );
	name = "stringListMax";
	classad::FunctionCall::RegisterFunction( name,
											 stringListSummarize_func );
	name = "stringListMember";
	classad::FunctionCall::RegisterFunction( name,
											 stringListMember_func );
	name = "stringListIMember";
	classad::FunctionCall::RegisterFunction( name,
											 stringListMember_func );
	name = "stringList_regexpMember";
	classad::FunctionCall::RegisterFunction( name,
											 stringListRegexpMember_func );

	name = "userHome";
	classad::FunctionCall::RegisterFunction(name, userHome_func);

	name = "userMap";
	classad::FunctionCall::RegisterFunction(name, userMap_func);

	// user@domain, slot@machine & sinful string crackers.
	name = "splitusername";
	classad::FunctionCall::RegisterFunction( name, splitAt_func );
	name = "splitslotname";
	classad::FunctionCall::RegisterFunction( name, splitAt_func );
	name = "split";
	classad::FunctionCall::RegisterFunction( name, splitArb_func );
	//name = "splitsinful";
	//classad::FunctionCall::RegisterFunction( name, splitSinful_func );
}

void
classad_debug_dprintf(const char *s) {
	dprintf(D_FULLDEBUG, "%s", s);
}

ClassAd::ClassAd()
{
	if ( !m_initConfig ) {
		this->Reconfig();
		m_initConfig = true;
	}

	ResetName();
    ResetExpr();

	DisableDirtyTracking();
}

ClassAd::ClassAd( const ClassAd &ad ) : classad::ClassAd(ad)
{
	if ( !m_initConfig ) {
		this->Reconfig();
		m_initConfig = true;
	}

	CopyFrom( ad );

	ResetName();
    ResetExpr();

}

ClassAd::ClassAd( const classad::ClassAd &ad )
{
	if ( !m_initConfig ) {
		this->Reconfig();
		m_initConfig = true;
	}

	CopyFrom( ad );

	ResetName();
    ResetExpr();

}

ClassAd::~ClassAd()
{
}

ClassAd::
ClassAd( FILE *file, const char *delimitor, int &isEOF, int&error, int &empty )
{
	if ( !m_initConfig ) {
		this->Reconfig();
		m_initConfig = true;
	}

	DisableDirtyTracking();

	ResetName();
    ResetExpr();


	int index;
	MyString buffer;
	MyStringFpSource myfs(file, false);
	size_t delimLen = strlen( delimitor );

	empty = TRUE;

	while( 1 ) {

			// get a line from the file
		if ( buffer.readLine( myfs, false ) == false ) {
			error = ( isEOF = feof( file ) ) ? 0 : errno;
			return;
		}

			// did we hit the delimitor?
		if ( strncmp( buffer.Value(), delimitor, delimLen ) == 0 ) {
				// yes ... stop
			isEOF = feof( file );
			error = 0;
			return;
		}

			// Skip any leading white-space
		index = 0;
		while ( index < buffer.Length() &&
				( buffer[index] == ' ' || buffer[index] == '\t' ) ) {
			index++;
		}

			// if the rest of the string is empty, try reading again
			// if it starts with a pound character ("#"), treat as a comment
		if( index == buffer.Length() || buffer[index] == '\n' ||
			buffer[index] == '#' ) {
			continue;
		}

			// Insert the string into the classad
		if( Insert( buffer.Value() ) == FALSE ) { 	
				// print out where we barfed to the log file
			dprintf(D_ALWAYS,"failed to create classad; bad expr = '%s'\n",
					buffer.Value());
				// read until delimitor or EOF; whichever comes first
			buffer = "";
			while ( strncmp( buffer.Value(), delimitor, delimLen ) &&
					!feof( file ) ) {
				buffer.readLine( myfs, false );
			}
			isEOF = feof( file );
			error = -1;
			return;
		} else {
			empty = FALSE;
		}
	}
}

CondorClassAdFileParseHelper::~CondorClassAdFileParseHelper()
{
	switch (parse_type) {
		case Parse_xml: {
			classad::ClassAdXMLParser * parser = (classad::ClassAdXMLParser *)new_parser;
			delete parser;
			new_parser = NULL;
		} break;
		case Parse_json: {
			classad::ClassAdJsonParser * parser = (classad::ClassAdJsonParser *)new_parser;
			delete parser;
			new_parser = NULL;
		} break;
		case Parse_new: {
			classad::ClassAdParser * parser = (classad::ClassAdParser *)new_parser;
			delete parser;
			new_parser = NULL;
		} break;
		default: break;
	}
	ASSERT( ! new_parser);
}


bool CondorClassAdFileParseHelper::line_is_ad_delimitor(const std::string & line)
{
	if (blank_line_is_ad_delimitor) {
		const char * p = line.c_str();
		while (*p && isspace(*p)) ++p;
		return ( ! *p || *p == '\n');
	}
	return starts_with(line, ad_delimitor);
}

// this method is called before each line is parsed.
// return 0 to skip (is_comment), 1 to parse line, 2 for end-of-classad, -1 for abort
int CondorClassAdFileParseHelper::PreParse(std::string & line, ClassAd & /*ad*/, FILE* /*file*/)
{
	// if this line matches the ad delimitor, tell the parser to stop parsing
	if (line_is_ad_delimitor(line))
		return 2; //end-of-classad

	// check for blank lines or lines whose first character is #
	// tell the parse to skip those lines, otherwise tell the parser to
	// parse the line.
	for (size_t ix = 0; ix < line.size(); ++ix) {
		if (line[ix] == '#' || line[ix] == '\n')
			return 0; // skip this line, but don't stop parsing.
		if (line[ix] != ' ' && line[ix] != '\t')
			break;
	}
	return 1; // parse this line.
}

// this method is called when the parser encounters an error
// return 0 to skip and continue, 1 to re-parse line, 2 to quit parsing with success, -1 to abort parsing.
int CondorClassAdFileParseHelper::OnParseError(std::string & line, ClassAd & /*ad*/, FILE* file)
{
	if (parse_type >= Parse_xml && parse_type < Parse_auto) {
		// here line is actually errmsg.
		//PRAGMA_REMIND("report parse errors for new parsers?")
		return -1;
	}

		// print out where we barfed to the log file
	dprintf(D_ALWAYS,"failed to create classad; bad expr = '%s'\n",
			line.c_str());

	// read until delimitor or EOF; whichever comes first
	line = "NotADelim=1";
	while ( ! line_is_ad_delimitor(line)) {
		if (feof(file))
			break;
		if ( ! readLine(line, file, false))
			break;
	}
	return -1; // abort
}


int CondorClassAdFileParseHelper::NewParser(ClassAd & ad, FILE* file, bool & detected_long, std::string & errmsg)
{
	detected_long = false;
	if (parse_type < Parse_xml || parse_type > Parse_auto) {
		// return 0 to indicate this is a -long form (line oriented) parse type
		return 0;
	}

	int rval = 1;
	switch(parse_type) {
		case Parse_xml: {
			classad::ClassAdXMLParser * parser = (classad::ClassAdXMLParser *)new_parser;
			if ( ! parser) {
				parser = new classad::ClassAdXMLParser();
				new_parser = (void*)parser;
			}
			ASSERT(parser);
			bool fok = parser->ParseClassAd(file, ad);
			if (fok) {
				rval = ad.size();
			} else if (feof(file)) {
				rval = -99;
			} else {
				rval = -1;
			}
		} break;

		case Parse_json: {
			classad::ClassAdJsonParser * parser = (classad::ClassAdJsonParser *)new_parser;
			if ( ! parser) {
				parser = new classad::ClassAdJsonParser();
				new_parser = (void*)parser;
			}
			ASSERT(parser);
			bool fok = parser->ParseClassAd(file, ad, false);
			if ( ! fok) {
				bool keep_going = false;
				classad::Lexer::TokenType tt = parser->getLastTokenType();
				if (inside_list) {
					if (tt == classad::Lexer::LEX_COMMA) { keep_going = true; }
					else if (tt == classad::Lexer::LEX_CLOSE_BOX) { keep_going = true; inside_list = false; }
				} else {
					if (tt == classad::Lexer::LEX_OPEN_BOX) { keep_going = true; inside_list = true; }
				}
				if (keep_going) {
					fok = parser->ParseClassAd(file, ad, false); 
				}
			}
			if (fok) {
				rval = ad.size();
			} else if (feof(file)) {
				rval = -99;
			} else {
				rval = -1;
			}
		} break;

		case Parse_new: {
			classad::ClassAdParser * parser = (classad::ClassAdParser *)new_parser;
			if ( ! parser) {
				parser = new classad::ClassAdParser();
				new_parser = (void*)parser;
			}
			ASSERT(parser);
			bool fok = parser->ParseClassAd(file, ad);
			if ( ! fok) {
				bool keep_going = false;
				classad::Lexer::TokenType tt = parser->getLastTokenType();
				if (inside_list) {
					if (tt == classad::Lexer::LEX_COMMA) { keep_going = true; }
					else if (tt == classad::Lexer::LEX_CLOSE_BRACE) { keep_going = true; inside_list = false; }
				} else {
					if (tt == classad::Lexer::LEX_OPEN_BRACE) { keep_going = true; inside_list = true; }
				}
				if (keep_going) {
					fok = parser->ParseClassAd(file, ad, false); 
				}
			}
			if (fok) {
				rval = ad.size();
			} else if (feof(file)) {
				rval = -99;
			} else {
				rval = -1;
			}
		} break;

		case Parse_auto: { // parse line oriented until we figure out the parse type
			// get a line from the file
			std::string buffer;
			for (;;) {
				if ( ! readLine(buffer, file, false)) {
					return feof(file) ? -99 : -1;
				}

				int ee = PreParse(buffer, ad, file);
				if (ee == 1) {
					// pre-parser says parse it. can we use it to figure out what type we are?
					// if we are still scanning to decide what the parse type is, we should be able to figure it out now...
					if (buffer == "<?xml version=\"1.0\"?>\n") {
						parse_type = Parse_xml;
						return NewParser(ad, file, detected_long, errmsg); // skip this line, but don't stop parsing, from now on
					} else if (buffer == "[\n" || buffer == "{\n") {
						char ch = buffer[0];
						// could be json or new classads, read character to figure out which.
						int ch2 = fgetc(file);
						if (ch == '{' && ch2 == '[') {
							inside_list = true;
							ungetc('[', file);
							parse_type = Parse_new;
							return NewParser(ad, file, detected_long, errmsg);
						} else if (ch == '[' && ch2 == '{') {
							inside_list = true;
							ungetc('{', file);
							parse_type = Parse_json;
							return NewParser(ad, file, detected_long, errmsg);
						} else {
							buffer = ""; buffer[0] = ch;
							readLine(buffer, file, true);
						}
					}
					// this doesn't look like a new classad prolog, so just parse it
					// using the line oriented parser.
					parse_type = Parse_long;
					errmsg = buffer;
					detected_long = true;
					return 0;
				}
			}
			
		} break;

		default: rval = -1; break;
	}

	return rval;
}


// returns number of attributes added to the ad
int ClassAd::
InsertFromFile(FILE* file, /*out*/ bool& is_eof, /*out*/ int& error, ClassAdFileParseHelper* phelp /*=NULL*/)
{
	int ee = 1;
	int cAttrs = 0;
	std::string buffer;

	if (phelp) {
		// new classad style parsers do all of the work in the NewParser callback
		// they will return non-zero to indicate that they are new classad style parsers.
		bool detected_long = false;
		cAttrs = phelp->NewParser(*this, file, detected_long, buffer);
		if (cAttrs > 0) {
			error = 0;
			is_eof = false;
			return cAttrs;
		} else if (cAttrs < 0) {
			if (cAttrs == -99) {
				error = 0;
				is_eof = true;
				return 0;
			}
			is_eof = feof(file);
			error = cAttrs;
			return phelp->OnParseError(buffer, *this, file);
		}
		// got a 0 from NewParser, fall down into the old (-long) style parser
		if (detected_long && ! buffer.empty()) {
			// buffer has the first line that we want to parse, jump into the
			// middle of the line-oriented parse loop.
			goto parse_line;
		}
	}

	while( 1 ) {

			// get a line from the file
		if ( ! readLine(buffer, file, false)) {
			is_eof = feof(file);
			error = is_eof ? 0 : errno;
			return cAttrs;
		}

		// if there is a helper, give the helper first crack at the line
		// otherwise set ee to decide what to do with this line.
		ee = 1;
		if (phelp) {
			ee = phelp->PreParse(buffer, *this, file);
		} else {
			// default is to skip blank lines and comment lines
			for (size_t ix = 0; ix < buffer.size(); ++ix) {
				if (buffer[ix] == '#' || buffer[ix] == '\n') {
					ee = 0; // skip blank line or comment
					break;
				}
				// ignore leading whitespace.
				if (buffer[ix] == ' ' || buffer[ix] == '\t') {
					continue;
				}
				ee = 1; // 1 is parse
				break;
			}
		}
		if (ee == 0) // comment or blank line, skip it and read the next
			continue;
		if (ee != 1) { // 1 is parse, <0, is abort, >1 is end_of_ad
			error = (ee < 0) ? ee : 0;
			is_eof = feof(file);
			return cAttrs;
		}

parse_line:
		// Insert the string into the classad
		if (Insert(buffer.c_str()) !=  0) {
			++cAttrs;
		} else {
			ee = -1;
			if (phelp) {
				ee = phelp->OnParseError(buffer, *this, file);
				if (1 == ee) {
					// buffer has (presumably) been modified, re-try parsing.
					// but only retry once.
					if (Insert(buffer.c_str()) != 0) {
						++cAttrs;
					} else {
						ee = phelp->OnParseError(buffer, *this, file);
						if (1 == ee) ee = -1;  // treat another attempt to reparse as a failure.
					}
				}
			}

			// the value of ee tells us whether to quit or keep going or
			// < 0 is abort
			//   0 is skip line - keep going
			//   1 is line was parsed, keep going
			// > 1 is end-of-ad, quit the loop
			if (ee < 0 || ee > 1) {
				error = ee > 1 ? 0 : ee;
				is_eof = feof(file);
				return cAttrs;
			}
		}
	}
}


bool CondorClassAdFileIterator::begin(
	FILE* fh,
	bool close_when_done,
	CondorClassAdFileParseHelper::ParseType type)
{
	parse_help = new CondorClassAdFileParseHelper("\n", type);
	free_parse_help = true;
	file = fh;
	close_file_at_eof = close_when_done;
	error = 0;
	at_eof = false;
	return true;
}

bool CondorClassAdFileIterator::begin(
	FILE* fh,
	bool close_when_done,
	CondorClassAdFileParseHelper & helper)
{
	parse_help = &helper;
	free_parse_help = false;
	file = fh;
	close_file_at_eof = close_when_done;
	error = 0;
	at_eof = false;
	return true;
}

int CondorClassAdFileIterator::next(ClassAd & classad, bool merge /*=false*/)
{
	if ( ! merge) classad.Clear();
	if (at_eof) return 0;
	if ( ! file) { error = -1; return -1; }

	int cAttrs = classad.InsertFromFile(file, at_eof, error, parse_help);
	if (cAttrs > 0) return cAttrs;
	if (at_eof) {
		if (file && close_file_at_eof) { fclose(file); file = NULL; }
		return 0;
	}
	if (error < 0)
		return error;
	return 0;
}

ClassAd * CondorClassAdFileIterator::next(classad::ExprTree * constraint)
{
	if (at_eof) return NULL;

	for (;;) {
		ClassAd * ad = new ClassAd();
		int cAttrs = this->next(*ad, true);
		bool include_classad = cAttrs > 0 && error >= 0;
		if (include_classad && constraint) {
			classad::Value val;
			if (ad->EvaluateExpr(constraint,val)) {
				if ( ! val.IsBooleanValueEquiv(include_classad)) {
					include_classad = false;
				}
			}
		}
		if (include_classad) {
			return ad;
		}
		delete ad;
		ad = NULL;

		if (at_eof || error < 0) break;
	}
	return NULL;
}

CondorClassAdFileParseHelper::ParseType CondorClassAdFileIterator::getParseType()
{
	if (parse_help) return parse_help->getParseType();
	return (CondorClassAdFileParseHelper::ParseType)-1;
}

CondorClassAdFileParseHelper::ParseType CondorClassAdListWriter::setFormat(CondorClassAdFileParseHelper::ParseType typ)
{
	if ( ! wrote_header && ! cNonEmptyOutputAds) out_format = typ;
	return out_format;
}

CondorClassAdFileParseHelper::ParseType CondorClassAdListWriter::autoSetFormat(CondorClassAdFileParseHelper & parse_help)
{
	if (out_format != CondorClassAdFileParseHelper::Parse_auto) return out_format;
	return setFormat(parse_help.getParseType());
}

// append a classad into the given output buffer, writing list header or separator as needed.
// return:
//    < 0 failure,
//    0   nothing written
//    1   non-empty ad appended
int CondorClassAdListWriter::appendAd(const ClassAd & ad, std::string & output, StringList * whitelist, bool hash_order)
{
	if (ad.size() == 0) return 0;
	size_t cchBegin = output.size();

	classad::References attrs;
	classad::References *print_order = NULL;
	if ( ! hash_order || whitelist) {
		sGetAdAttrs(attrs, ad, false, whitelist);
		print_order = &attrs;
	}

	// if we havn't picked a format yet, pick long.
	if (out_format < ClassAdFileParseType::Parse_long || out_format > ClassAdFileParseType::Parse_new) {
		out_format = ClassAdFileParseType::Parse_long;
	}

	switch (out_format) {
	default:
	case ClassAdFileParseType::Parse_long: {
			if (print_order) {
				sPrintAdAttrs(output, ad, *print_order);
			} else {
				sPrintAd(output, ad);
			}
			if (output.size() > cchBegin) { output += "\n"; }
		} break;

	case ClassAdFileParseType::Parse_json: {
			classad::ClassAdJsonUnParser  unparser;
			output += cNonEmptyOutputAds ? ",\n" : "[\n";
			if (print_order) {
				unparser.Unparse(output, &ad, *print_order);
			} else {
				unparser.Unparse(output, &ad);
			}
			if (output.size() > cchBegin+2) {
				needs_footer = wrote_header = true;
				output += "\n";
			} else {
				output.erase(cchBegin);
			}
		} break;

	case ClassAdFileParseType::Parse_new: {
			classad::ClassAdUnParser  unparser;
			output += cNonEmptyOutputAds ? ",\n" : "{\n";
			if (print_order) {
				unparser.Unparse(output, &ad, *print_order);
			} else {
				unparser.Unparse(output, &ad);
			}
			if (output.size() > cchBegin+2) {
				needs_footer = wrote_header = true;
				output += "\n";
			} else {
				output.erase(cchBegin);
			}
		} break;

	case ClassAdFileParseType::Parse_xml: {
			classad::ClassAdXMLUnParser  unparser;
			unparser.SetCompactSpacing(false);
			size_t cchTmp = cchBegin;
			if (0 == cNonEmptyOutputAds) {
				AddClassAdXMLFileHeader(output);
				cchTmp = output.size();
			}
			if (print_order) {
				unparser.Unparse(output, &ad, *print_order);
			} else {
				unparser.Unparse(output, &ad);
			}
			if (output.size() > cchTmp) {
				needs_footer = wrote_header = true;
				//no extra newlines for xml
				// output += "\n";
			} else {
				output.erase(cchBegin);
			}
		} break;
	}

	if (output.size() > cchBegin) {
		++cNonEmptyOutputAds;
		return 1;
	}
	return 0;
}

// append a classad list footer into the given output buffer if needed
// return:
//    < 0 failure,
//    0   nothing written
//    1   non-empty ad appended
int CondorClassAdListWriter::appendFooter(std::string & buf, bool xml_always_write_header_footer)
{
	int rval = 0;
	switch (out_format) {
	case ClassAdFileParseType::Parse_xml:
		if ( ! wrote_header) {
			if (xml_always_write_header_footer) {
				// for XML output, we ALWAYS write the header and footer, even if there were no ads.
				AddClassAdXMLFileHeader(buf);
			} else {
				break;
			}
		}
		AddClassAdXMLFileFooter(buf);
		rval = 1;
		break;
	case ClassAdFileParseType::Parse_new:
		if (cNonEmptyOutputAds) { buf += "}\n"; rval = 1; }
		break;
	case ClassAdFileParseType::Parse_json:
		if (cNonEmptyOutputAds) { buf += "]\n"; rval = 1; }
	default:
		// nothing to do.
		break;
	}
	needs_footer = false;
	return rval;
}

// write a classad into the given output stream attributes are sorted unless hash_order is true
// return:
//    < 0 failure,
//    0   nothing written
//    1   non-empty ad written
static const int cchReserveForPrintingAds = 16384;
int CondorClassAdListWriter::writeAd(const ClassAd & ad, FILE * out, StringList * whitelist, bool hash_order)
{
	buffer.clear();
	if ( ! cNonEmptyOutputAds) buffer.reserve(cchReserveForPrintingAds);

	int rval = appendAd(ad, buffer, whitelist, hash_order);
	if (rval < 0) return rval;

	if ( ! buffer.empty()) {
		fputs(buffer.c_str(), out);
	}
	return rval;
}

// write a classad list footer into the given output stream if needed
// return:
//    < 0 failure,
//    0   nothing written
//    1   non-empty ad written
int CondorClassAdListWriter::writeFooter(FILE* out, bool xml_always_write_header_footer)
{
	buffer.clear();
	appendFooter(buffer, xml_always_write_header_footer);
	if ( ! buffer.empty()) {
		int rval = fputs(buffer.c_str(), out);
		return (rval < 0) ? rval : 1;
	}
	return 0;
}

bool
ClassAdAttributeIsPrivate( char const *name )
{
	if( strcasecmp(name,ATTR_CLAIM_ID) == 0 ) {
			// This attribute contains the secret capability cookie
		return true;
	}
	if( strcasecmp(name,ATTR_PAIRED_CLAIM_ID) == 0 ) {
			// This attribute contains the secret capability cookie
		return true;
	}
	if( strcasecmp(name,ATTR_CAPABILITY) == 0 ) {
			// This attribute contains the secret capability cookie
		return true;
	}
	if( strcasecmp(name,ATTR_CLAIM_IDS) == 0 ) {
			// This attribute contains secret capability cookies
		return true;
	}
	if( strcasecmp(name,ATTR_TRANSFER_KEY) == 0 ) {
			// This attribute contains the secret file transfer cookie
		return true;
	}
	if (strcasecmp(name,ATTR_CHILD_CLAIM_IDS) == 0) {
			// In a partitionable slot, contains all the claim ids
		return true;
	}
	if (strcasecmp(name,ATTR_CLAIM_ID_LIST) == 0) {
			// In a partitionable slot with consumption policies,
			// contains extra claim ids
		return true;
	}
	return false;
}

bool ClassAd::Insert( const std::string &attrName, classad::ExprTree *& expr)
{
	return classad::ClassAd::Insert( attrName, expr );
}

int ClassAd::Insert( const char *name, classad::ExprTree *& expr )
{
	string str(name);
	return Insert( str, expr ) ? TRUE : FALSE;
}

int
ClassAd::Insert(const std::string &str)
{
	// this is not the optimial path, it would be better to
	// use either the 2 argument insert, or the const char* form below
	return this->Insert(str.c_str());
}

int
ClassAd::Insert( const char *str )
{
	if ( ! InsertLongFormAttrValue(*this, str, true)) {
		return FALSE;
	}
	return TRUE;
}

int ClassAd::
AssignExpr(char const *name,char const *value)
{
	classad::ClassAdParser par;
	classad::ExprTree *expr = NULL;
	par.SetOldClassAd( true );

	if ( value == NULL ) {
		value = "Undefined";
	}
	if ( !par.ParseExpression( value, expr, true ) ) {
		return FALSE;
	}
	if ( !Insert( name, expr ) ) {
		delete expr;
		return FALSE;
	}
	return TRUE;
}

int ClassAd::
Assign(char const *name,char const *value)
{
	if ( value == NULL ) {
		return AssignExpr( name, NULL );
	} else {
		return InsertAttr( name, value ) ? TRUE : FALSE;
	}
}

//  void ClassAd::
//  ResetExpr() { this->ptrExpr = exprList; }

//  ExprTree* ClassAd::
//  NextExpr(){}

//  void ClassAd::
//  ResetName() { this->ptrName = exprList; }

//  const char* ClassAd::
//  NextNameOriginal(){}


//  ExprTree* ClassAd::
//  Lookup(char *) const{}

//  ExprTree* ClassAd::
//  Lookup(const char*) const{}

//int ClassAd::
//LookupString( const char *name, char *value ) const 
//{
//	string strVal;
//	if( !EvaluateAttrString( string( name ), strVal ) ) {
//		return 0;
//	}
//	strcpy( value, strVal.c_str( ) );
//	return 1;
//} 

int ClassAd::
LookupString(const char *name, char *value, int max_len) const
{
	string strVal;
	if( !EvaluateAttrString( string( name ), strVal ) ) {
		return 0;
	}
	strncpy( value, strVal.c_str( ), max_len );
	if ( value && max_len && value[max_len - 1] ) value[max_len - 1] = '\0';
	return 1;
}

int ClassAd::
LookupString (const char *name, char **value) const 
{
	string strVal;
	if( !EvaluateAttrString( string( name ), strVal ) ) {
		return 0;
	}
	const char *strValCStr = strVal.c_str( );
	*value = (char *) malloc(strlen(strValCStr) + 1);
	if (*value != NULL) {
		strcpy(*value, strValCStr);
		return 1;
	}

	return 0;
}

int ClassAd::
LookupString( const char *name, MyString &value ) const 
{
	string strVal;
	if( !EvaluateAttrString( string( name ), strVal ) ) {
		return 0;
	}
	value = strVal.c_str();
	return 1;
} 

int ClassAd::
LookupString( const char *name, std::string &value ) const 
{
	if( !EvaluateAttrString( string( name ), value ) ) {
		return 0;
	}
	return 1;
} 

int ClassAd::
LookupInteger( const char *name, int &value ) const 
{
	bool    boolVal;
	int     haveInteger;
	string  sName(name);
	int		tmp_val;

	if( EvaluateAttrInt(sName, tmp_val ) ) {
		value = tmp_val;
		haveInteger = TRUE;
	} else if( EvaluateAttrBool(sName, boolVal ) ) {
		value = boolVal ? 1 : 0;
		haveInteger = TRUE;
	} else {
		haveInteger = FALSE;
	}
	return haveInteger;
}

int ClassAd::
LookupInteger( const char *name, long &value ) const 
{
	bool    boolVal;
	int     haveInteger;
	string  sName(name);
	long	tmp_val;

	if( EvaluateAttrInt(sName, tmp_val ) ) {
		value = tmp_val;
		haveInteger = TRUE;
	} else if( EvaluateAttrBool(sName, boolVal ) ) {
		value = boolVal ? 1 : 0;
		haveInteger = TRUE;
	} else {
		haveInteger = FALSE;
	}
	return haveInteger;
}

int ClassAd::
LookupInteger( const char *name, long long &value ) const 
{
	bool    boolVal;
	int     haveInteger;
	string  sName(name);
	long long	tmp_val;

	if( EvaluateAttrInt(sName, tmp_val ) ) {
		value = tmp_val;
		haveInteger = TRUE;
	} else if( EvaluateAttrBool(sName, boolVal ) ) {
		value = boolVal ? 1 : 0;
		haveInteger = TRUE;
	} else {
		haveInteger = FALSE;
	}
	return haveInteger;
}

int ClassAd::
LookupFloat( const char *name, float &value ) const
{
	double  doubleVal;
	long long intVal;
	int     haveFloat;

	if(EvaluateAttrReal( string( name ), doubleVal ) ) {
		haveFloat = TRUE;
		value = (float) doubleVal;
	} else if(EvaluateAttrInt( string( name ), intVal ) ) {
		haveFloat = TRUE;
		value = (float)intVal;
	} else {
		haveFloat = FALSE;
	}
	return haveFloat;
}

int ClassAd::
LookupFloat( const char *name, double &value ) const
{
	double  doubleVal;
	long long intVal;
	int     haveFloat;

	if(EvaluateAttrReal( string( name ), doubleVal ) ) {
		haveFloat = TRUE;
		value = doubleVal;
	} else if(EvaluateAttrInt( string( name ), intVal ) ) {
		haveFloat = TRUE;
		value = (double)intVal;
	} else {
		haveFloat = FALSE;
	}
	return haveFloat;
}

int ClassAd::
LookupBool( const char *name, int &value ) const
{
	long long intVal;
	bool  boolVal;
	int haveBool;
	string sName;

	sName = string(name);

	if (EvaluateAttrBool(name, boolVal)) {
		haveBool = true;
		value = boolVal ? 1 : 0;
	} else if (EvaluateAttrInt(name, intVal)) {
		haveBool = true;
		value = (intVal != 0) ? 1 : 0;
	} else {
		haveBool = false;
	}
	return haveBool;
}

int ClassAd::
LookupBool( const char *name, bool &value ) const
{
	long long intVal;
	bool  boolVal;
	int haveBool;
	string sName;

	sName = string(name);

	if (EvaluateAttrBool(name, boolVal)) {
		haveBool = true;
		value = boolVal ? true : false;
	} else if (EvaluateAttrInt(name, intVal)) {
		haveBool = true;
		value = (intVal != 0) ? true : false;
	} else {
		haveBool = false;
	}
	return haveBool;
}

int ClassAd::
EvalAttr( const char *name, classad::ClassAd *target, classad::Value & value)
{
	int rc = 0;

	if( target == this || target == NULL ) {
		if( EvaluateAttr( name, value ) ) {
			rc = 1;
		}
		return rc;
	}

	getTheMatchAd( this, target );
	if( this->Lookup( name ) ) {
		if( this->EvaluateAttr( name, value ) ) {
			rc = 1;
		}
	} else if( target->Lookup( name ) ) {
		if( target->EvaluateAttr( name, value ) ) {
			rc = 1;
		}
	}
	releaseTheMatchAd();
	return rc;
}

int ClassAd::
EvalString( const char *name, classad::ClassAd *target, char *value )
{
	int rc = 0;
	string strVal;

	if( target == this || target == NULL ) {
		if( EvaluateAttrString( name, strVal ) ) {
			strcpy( value, strVal.c_str( ) );
			rc = 1;
		}
		return rc;
	}

	getTheMatchAd( this, target );
	if( this->Lookup( name ) ) {
		if( this->EvaluateAttrString( name, strVal ) ) {
			strcpy( value, strVal.c_str( ) );
			rc = 1;
		}
	} else if( target->Lookup( name ) ) {
		if( target->EvaluateAttrString( name, strVal ) ) {
			strcpy( value, strVal.c_str( ) );
			rc = 1;
		}
	}
	releaseTheMatchAd();
	return rc;
}

/*
 * Ensure that we allocate the value, so we have sufficient space
 */
int ClassAd::
EvalString (const char *name, classad::ClassAd *target, char **value)
{
    
	string strVal;
    bool foundAttr = false;
	int rc = 0;

	if( target == this || target == NULL ) {
		if( EvaluateAttrString( name, strVal ) ) {

            *value = (char *)malloc(strlen(strVal.c_str()) + 1);
            if(*value != NULL) {
                strcpy( *value, strVal.c_str( ) );
                rc = 1;
            } else {
                rc = 0;
            }
		}
		return rc;
	}

	getTheMatchAd( this, target );

    if( this->Lookup(name) ) {

        if( this->EvaluateAttrString( name, strVal ) ) {
            foundAttr = true;
        }		
    } else if( target->Lookup(name) ) {
        if( this->EvaluateAttrString( name, strVal ) ) {
            foundAttr = true;
        }		
    }

    if(foundAttr)
    {
        *value = (char *)malloc(strlen(strVal.c_str()) + 1);
        if(*value != NULL) {
            strcpy( *value, strVal.c_str( ) );
            rc = 1;
        }
    }

	releaseTheMatchAd();
	return rc;
}

int ClassAd::
EvalString(const char *name, classad::ClassAd *target, MyString & value)
{
    char * pvalue = NULL;
    //this one makes sure length is good
    int ret = EvalString(name, target, &pvalue); 
    if(ret == 0) { return ret; }
    value = pvalue;
    free(pvalue);
    return ret;
}

int ClassAd::
EvalString(const char *name, classad::ClassAd *target, std::string & value)
{
    char * pvalue = NULL;
    //this one makes sure length is good
    int ret = EvalString(name, target, &pvalue); 
    if(ret == 0) { return ret; }
    value = pvalue;
    free(pvalue);
    return ret;
}

int ClassAd::
EvalInteger (const char *name, classad::ClassAd *target, long long &value)
{
	int rc = 0;
	classad::Value val;

	if( target == this || target == NULL ) {
		if( EvaluateAttr( name, val ) ) { 
			rc = 1;
		}
	}
	else 
	{
	  getTheMatchAd( this, target );
	  if( this->Lookup( name ) ) {
		  if( this->EvaluateAttr( name, val ) ) {
			  rc = 1;
		  }
	  } else if( target->Lookup( name ) ) {
		  if( target->EvaluateAttr( name, val ) ) {
			  rc = 1;
		  }
	  }
	  releaseTheMatchAd();
	}
	
	
	// we have a "val" now cast if needed.
	if ( 1 == rc ) 
	{
	  double doubleVal;
	  long long intVal;
	  bool boolVal;

	  if( val.IsRealValue( doubleVal ) ) {
	    value = ( long long )doubleVal;
	  }
	  else if( val.IsIntegerValue( intVal ) ) {
	    value = intVal;
	  }
	  else if( val.IsBooleanValue( boolVal ) ) {
	    value = ( long long )boolVal;
	  }
	  else 
	  { 
	    // if we got here there is an issue with evaluation.
	    rc = 0;
	  }
			
	}
	
	return rc;
}

int ClassAd::
EvalFloat (const char *name, classad::ClassAd *target, double &value)
{
	int rc = 0;
	classad::Value val;
	double doubleVal;
	long long intVal;
	bool boolVal;

	if( target == this || target == NULL ) {
		if( EvaluateAttr( name, val ) ) {
			if( val.IsRealValue( doubleVal ) ) {
				value = doubleVal;
				rc = 1;
			}
			if( val.IsIntegerValue( intVal ) ) {
				value = intVal;
				rc = 1;
			}
			if( val.IsBooleanValue( boolVal ) ) {
				value = boolVal;
				rc = 1;
			}
		}
		return rc;
	}

	getTheMatchAd( this, target );
	if( this->Lookup( name ) ) {
		if( this->EvaluateAttr( name, val ) ) {
			if( val.IsRealValue( doubleVal ) ) {
				value = doubleVal;
				rc = 1;
			}
			if( val.IsIntegerValue( intVal ) ) {
				value = intVal;
				rc = 1;
			}
			if( val.IsBooleanValue( boolVal ) ) {
				value = boolVal;
				rc = 1;
			}
		}
	} else if( target->Lookup( name ) ) {
		if( target->EvaluateAttr( name, val ) ) {
			if( val.IsRealValue( doubleVal ) ) {
				value = doubleVal;
				rc = 1;
			}
			if( val.IsIntegerValue( intVal ) ) {
				value = intVal;
				rc = 1;
			}
			if( val.IsBooleanValue( boolVal ) ) {
				value = boolVal;
				rc = 1;
			}
		}
	}
	releaseTheMatchAd();
	return rc;
}

#define IS_DOUBLE_TRUE(val) (bool)(int)((val)*100000)

int ClassAd::
EvalBool  (const char *name, classad::ClassAd *target, int &value)
{
	int rc = 0;
	classad::Value val;
	double doubleVal;
	long long intVal;
	bool boolVal;

	if( target == this || target == NULL ) {
		if( EvaluateAttr( name, val ) ) {
			if( val.IsBooleanValue( boolVal ) ) {
				value = boolVal ? 1 : 0;
				rc = 1;
			} else if( val.IsIntegerValue( intVal ) ) {
				value = intVal ? 1 : 0;
				rc = 1;
			} else if( val.IsRealValue( doubleVal ) ) {
				value = IS_DOUBLE_TRUE(doubleVal) ? 1 : 0;
				rc = 1;
			}
		}
		return rc;
	}

	getTheMatchAd( this, target );
	if( this->Lookup( name ) ) {
		if( this->EvaluateAttr( name, val ) ) {
			if( val.IsBooleanValue( boolVal ) ) {
				value = boolVal ? 1 : 0;
				rc = 1;
			}
			if( val.IsIntegerValue( intVal ) ) {
				value = intVal ? 1 : 0;
				rc = 1;
			}
			if( val.IsRealValue( doubleVal ) ) {
				value = IS_DOUBLE_TRUE(doubleVal) ? 1 : 0;
				rc = 1;
			}
		}
	} else if( target->Lookup( name ) ) {
		if( target->EvaluateAttr( name, val ) ) {
			if( val.IsBooleanValue( boolVal ) ) {
				value = boolVal ? 1 : 0;
				rc = 1;
			}
			if( val.IsIntegerValue( intVal ) ) {
				value = intVal ? 1 : 0;
				rc = 1;
			}
			if( val.IsRealValue( doubleVal ) ) {
				value = IS_DOUBLE_TRUE(doubleVal) ? 1 : 0;
				rc = 1;
			}
		}
	}

	releaseTheMatchAd();
	return rc;
}

bool ClassAd::
initFromString( char const *str,MyString *err_msg )
{
	bool succeeded = true;

	// First, clear our ad so we start with a fresh ClassAd
	Clear();

	char *exprbuf = new char[strlen(str)+1];
	ASSERT( exprbuf );

	while( *str ) {
		while( isspace(*str) ) {
			str++;
		}

		size_t len = strcspn(str,"\n");
		strncpy(exprbuf,str,len);
		exprbuf[len] = '\0';

		if( str[len] == '\n' ) {
			len++;
		}
		str += len;

		if (!Insert(exprbuf)) {
			if( err_msg ) {
				err_msg->formatstr("Failed to parse ClassAd expression: '%s'",
					exprbuf);
			} else {
				dprintf(D_ALWAYS,"Failed to parse ClassAd expression: '%s'\n",
					exprbuf);
			}
			succeeded = false;
			break;
		}
	}

	delete [] exprbuf;
	return succeeded;
}


		// output functions
int
fPrintAd( FILE *file, const classad::ClassAd &ad, bool exclude_private, StringList *attr_white_list )
{
	MyString buffer;

	sPrintAd( buffer, ad, exclude_private, attr_white_list );
	if ( fprintf(file, "%s", buffer.Value()) < 0 ) {
		return FALSE;
	} else {
		return TRUE;
	}
}

void
dPrintAd( int level, const classad::ClassAd &ad, bool exclude_private )
{
	if ( IsDebugCatAndVerbosity( level ) ) {
		MyString buffer;

		sPrintAd( buffer, ad, exclude_private );

		dprintf( level|D_NOHEADER, "%s", buffer.Value() );
	}
}

int
sPrintAd( MyString &output, const classad::ClassAd &ad, bool exclude_private, StringList *attr_white_list )
{
	classad::ClassAd::const_iterator itr;

	classad::ClassAdUnParser unp;
	unp.SetOldClassAd( true, true );
	string value;

	const classad::ClassAd *parent = ad.GetChainedParentAd();

	if ( parent ) {
		for ( itr = parent->begin(); itr != parent->end(); itr++ ) {
			if ( attr_white_list && !attr_white_list->contains_anycase(itr->first.c_str()) ) {
				continue; // not in white-list
			}
			if ( ad.LookupIgnoreChain(itr->first) ) {
				continue; // attribute exists in child ad; we will print it below
			}
			if ( !exclude_private ||
				 !ClassAdAttributeIsPrivate( itr->first.c_str() ) ) {
				value = "";
				unp.Unparse( value, itr->second );
				output.formatstr_cat( "%s = %s\n", itr->first.c_str(),
									value.c_str() );
			}
		}
	}

	for ( itr = ad.begin(); itr != ad.end(); itr++ ) {
		if ( attr_white_list && !attr_white_list->contains_anycase(itr->first.c_str()) ) {
			continue; // not in white-list
		}
		if ( !exclude_private ||
			 !ClassAdAttributeIsPrivate( itr->first.c_str() ) ) {
			value = "";
			unp.Unparse( value, itr->second );
			output.formatstr_cat( "%s = %s\n", itr->first.c_str(),
								value.c_str() );
		}
	}

	return TRUE;
}

int
sPrintAd( std::string &output, const classad::ClassAd &ad, bool exclude_private, StringList *attr_white_list )
{
	MyString myout;
	int rc = sPrintAd( myout, ad, exclude_private, attr_white_list );
	output += myout;
	return rc;
}

/** Get a sorted list of attributes that are in the given ad, and also match the given whitelist (if any)
	and privacy criteria.
	@param attrs the set of attrs to insert into. This is set is NOT cleared first.
	@return TRUE
*/
int
sGetAdAttrs( classad::References &attrs, const classad::ClassAd &ad, bool exclude_private, StringList *attr_white_list, bool ignore_parent )
{
	classad::ClassAd::const_iterator itr;

	for ( itr = ad.begin(); itr != ad.end(); itr++ ) {
		if ( attr_white_list && !attr_white_list->contains_anycase(itr->first.c_str()) ) {
			continue; // not in white-list
		}
		if ( !exclude_private ||
			 !ClassAdAttributeIsPrivate( itr->first.c_str() ) ) {
			attrs.insert(itr->first);
		}
	}

	const classad::ClassAd *parent = ad.GetChainedParentAd();
	if ( parent && ! ignore_parent) {
		for ( itr = parent->begin(); itr != parent->end(); itr++ ) {
			if ( attrs.find(itr->first) != attrs.end() ) {
				continue; // we already inserted this into the attrs list.
			}
			if ( attr_white_list && !attr_white_list->contains_anycase(itr->first.c_str()) ) {
				continue; // not in white-list
			}
			if ( !exclude_private ||
				 !ClassAdAttributeIsPrivate( itr->first.c_str() ) ) {
				attrs.insert(itr->first);
			}
		}
	}

	return TRUE;
}

/** Format the given attributes from the ClassAd as an old ClassAd into the given string
	@param output The std::string to write into
	@return TRUE
	Note: this function and its sister that outputs to std::string do not call each other for reasons of efficiency...
*/
int
sPrintAdAttrs( MyString &output, const classad::ClassAd &ad, const classad::References &attrs )
{
	classad::ClassAdUnParser unp;
	unp.SetOldClassAd( true, true );
	std::string line;

	classad::References::const_iterator it;
	for (it = attrs.begin(); it != attrs.end(); ++it) {
		const ExprTree * tree = ad.Lookup(*it); // use Lookup rather than find in case we have a parent ad.
		if (tree) {
			line = *it;
			line += " = ";
			unp.Unparse( line, tree );
			line += "\n";
			output += line;
		}
	}

	return TRUE;
}

/** Format the given attributes from the ClassAd as an old ClassAd into the given string
	@param output The std::string to write into
	@return TRUE
	Note: this function and its sister the outputs to MyString do not call each other for reasons of efficiency...
*/
int
sPrintAdAttrs( std::string &output, const classad::ClassAd &ad, const classad::References &attrs )
{
	classad::ClassAdUnParser unp;
	unp.SetOldClassAd( true, true );

	classad::References::const_iterator it;
	for (it = attrs.begin(); it != attrs.end(); ++it) {
		const ExprTree * tree = ad.Lookup(*it); // use Lookup rather than find in case we have a parent ad.
		if (tree) {
			output += *it;
			output += " = ";
			unp.Unparse( output, tree );
			output += "\n";
		}
	}

	return TRUE;
}

// Taken from the old classad's function. Got rid of incorrect documentation. 
////////////////////////////////////////////////////////////////////////////////// Print an expression with a certain name into a buffer. 
// The returned buffer will be allocated with malloc(), and it needs
// to be free-ed with free() by the user.
////////////////////////////////////////////////////////////////////////////////
char*
sPrintExpr(const classad::ClassAd &ad, const char* name)
{
	char *buffer = NULL;
	size_t buffersize = 0;
	classad::ClassAdUnParser unp;
    string parsedString;
	classad::ExprTree* expr;

	unp.SetOldClassAd( true, true );

    expr = ad.Lookup(name);

    if(!expr)
    {
        return NULL;
    }

    unp.Unparse(parsedString, expr);

    buffersize = strlen(name) + parsedString.length() +
                    3 +     // " = "
                    1;      // null termination
    buffer = (char*) malloc(buffersize);
    ASSERT( buffer != NULL );

    snprintf(buffer, buffersize, "%s = %s", name, parsedString.c_str() );
    buffer[buffersize - 1] = '\0';

    return buffer;
}

// ClassAd methods

		// Type operations
void
SetMyTypeName( classad::ClassAd &ad, const char *myType )
{
	if( myType ) {
		ad.InsertAttr( ATTR_MY_TYPE, string( myType ) );
	}

	return;
}

const char*
GetMyTypeName( const classad::ClassAd &ad )
{
	static string myTypeStr;
	if( !ad.EvaluateAttrString( ATTR_MY_TYPE, myTypeStr ) ) {
		return "";
	}
	return myTypeStr.c_str( );
}

void
SetTargetTypeName( classad::ClassAd &ad, const char *targetType )
{
	if( targetType ) {
		ad.InsertAttr( ATTR_TARGET_TYPE, string( targetType ) );
	}

	return;
}

const char*
GetTargetTypeName( const classad::ClassAd &ad )
{
	static string targetTypeStr;
	if( !ad.EvaluateAttrString( ATTR_TARGET_TYPE, targetTypeStr ) ) {
		return "";
	}
	return targetTypeStr.c_str( );
}

void ClassAd::
ResetExpr()
{
	m_exprItrState = ItrUninitialized;
    m_dirtyItrInit = false;
}

void ClassAd::
ResetName()
{
	m_nameItrState = ItrUninitialized;
}

const char *ClassAd::
NextNameOriginal()
{
	const char *name = NULL;
	classad::ClassAd *chained_ad = GetChainedParentAd();

	if( m_nameItrState == ItrUninitialized ) {
		m_nameItr = begin();
		m_nameItrState = ItrInThisAd;
	}

	// After iterating through all the names in this ad,
	// get all the names in our chained ad as well.
	if ( chained_ad && m_nameItrState != ItrInChain && m_nameItr == end() ) {
		m_nameItr = chained_ad->begin();
		m_nameItrState = ItrInChain;
	}
	if ( ( m_nameItrState!=ItrInChain && m_nameItr == end() ) ||
		 ( m_nameItrState==ItrInChain && chained_ad == NULL ) ||
		 ( m_nameItrState==ItrInChain && m_nameItr == chained_ad->end() ) ) {
		return NULL;
	}
	name = m_nameItr->first.c_str();
	m_nameItr++;
	return name;
}

// Back compatibility helper methods

void AddExplicitTargetRefs( classad::ClassAd &ad ) 
{
	set< string, classad::CaseIgnLTStr > definedAttrs;
	for( classad::AttrList::iterator a = ad.begin( ); a != ad.end( ); a++ ) {
		definedAttrs.insert(a->first);
	}
	
	for( classad::AttrList::iterator a = ad.begin( ); a != ad.end( ); a++ ) {
		if ( a->second->GetKind() != classad::ExprTree::LITERAL_NODE ) 
		{
		  classad::ExprTree * pTree = compat_classad::AddExplicitTargetRefs( a->second, definedAttrs );
			ad.Insert( a->first, pTree ) ;
		}
	}
}

void RemoveExplicitTargetRefs( classad::ClassAd &ad )
{
	for( classad::AttrList::iterator a = ad.begin( ); a != ad.end( ); a++ ) {
		if ( a->second->GetKind() != classad::ExprTree::LITERAL_NODE ) 
		{
		  classad::ExprTree * pTree = compat_classad::RemoveExplicitTargetRefs( a->second );
			ad.Insert( a->first, pTree );
		}
	}
}  


void ClassAd:: 
AddTargetRefs( TargetAdType /*target_type*/, bool /*do_version_check*/ )
{
	// Disable AddTargetRefs for now
	return;

	// Remove use of CondorVersionInfo, which would require several
	// additional files in libcondorapi.
#if 0
	MyString ver_str;
	if ( do_version_check && this->LookupString( ATTR_VERSION, ver_str ) ) {
		CondorVersionInfo ver( ver_str.Value() );
		if ( ver.built_since_version( 7, 5, 1 ) ) {
			return;
		}
	}

	for( classad::AttrList::iterator a = begin(); a != end(); a++ ) {
		if ( a->second->GetKind() != classad::ExprTree::LITERAL_NODE ) {
			this->Insert( a->first, 
						  compat_classad::AddTargetRefs( a->second, target_type ) );
		}
	}
#endif
}

// Determine if a value is valid to be written to the log. The value
// is a RHS of an expression. According to LogSetAttribute::WriteBody,
// the only invalid character is a '\n'.
bool
IsValidAttrValue(const char *value)
{
    //NULL value is not invalid, may translate to UNDEFINED
    if(!value)
    {
        return true;
    }

    //According to the old classad's docs, LogSetAttribute::WriteBody
    // says that the only invalid character for a value is '\n'.
    // But then it also includes '\r', so whatever.
    while (*value) {
        if(((*value) == '\n') ||
           ((*value) == '\r')) {
            return false;
        }
        value++;
    }

    return true;
}

//	Decides if a string is a valid attribute name, the LHS
//  of an expression.  As per the manual, valid names:
//
//  Attribute names are sequences of alphabetic characters, digits and 
//  underscores, and may not begin with a digit

bool
IsValidAttrName(const char *name) {
		// NULL pointer certainly false
	if (!name) {
		return false;
	}

		// Must start with alpha or _
	if (!isalpha(*name) && *name != '_') {
		return false;
	}

	name++;

		// subsequent letters must be alphanum or _
	while (*name) {
		if (!isalnum(*name) && *name != '_') {
			return false;
		}
		name++;
	}

	return true;
}

bool ClassAd::NextExpr( const char *&name, ExprTree *&value )
{
	classad::ClassAd *chained_ad = GetChainedParentAd();

	if( m_exprItrState == ItrUninitialized ) {
		m_exprItr = begin();
		m_exprItrState = ItrInThisAd;
	}

	// After iterating through all the attributes in this ad,
	// get all the attributes in our chained ad as well.
	if ( chained_ad && m_exprItrState != ItrInChain && m_exprItr == end() ) {
		m_exprItr = chained_ad->begin();
		m_exprItrState = ItrInChain;
	}
	if ( ( m_exprItrState!=ItrInChain && m_exprItr == end() ) ||
		 ( m_exprItrState==ItrInChain && chained_ad == NULL ) ||
		 ( m_exprItrState==ItrInChain && m_exprItr == chained_ad->end() ) ) {
		return false;
	}
	name = m_exprItr->first.c_str();
	value = m_exprItr->second;
	m_exprItr++;
	return true;
}

//provides a way to get the next dirty expression in the set of 
//  dirty attributes.
bool ClassAd::
NextDirtyExpr(const char *&name, classad::ExprTree *&expr)
{
    //this'll reset whenever ResetDirtyItr is called
    if(!m_dirtyItrInit)
    {
        m_dirtyItr = dirtyBegin();
        m_dirtyItrInit = true;
    }

	name = NULL;
    expr = NULL;

	// get the next dirty attribute if we aren't past the end.
	// Removed attributes appear in the list, but we don't want
	// to return them in the old ClassAd API, so skip them.
	while ( m_dirtyItr != dirtyEnd() ) {
		name = m_dirtyItr->c_str();
		expr = classad::ClassAd::Lookup(*m_dirtyItr);
		m_dirtyItr++;
		if ( expr ) {
			break;
		} else {
			name = NULL;
		}
	}

    return expr != NULL;
}

void ClassAd::
SetDirtyFlag(const char *name, bool dirty)
{
	if ( dirty ) {
		MarkAttributeDirty( name );
	} else {
		MarkAttributeClean( name );
	}
}

void ClassAd::
GetDirtyFlag(const char *name, bool *exists, bool *dirty) const
{
	if ( Lookup( name ) == NULL ) {
		if ( exists ) {
			*exists = false;
		}
		return;
	}
	if ( exists ) {
		*exists = true;
	}
	if ( dirty ) {
		*dirty = IsAttributeDirty( name );
	}
}

void ClassAd::
CopyAttribute( char const *target_attr, classad::ClassAd *source_ad )
{
	CopyAttribute( target_attr, target_attr, source_ad );
}


void ClassAd::
CopyAttribute( char const *target_attr, char const *source_attr,
			   classad::ClassAd *source_ad )
{
	ASSERT( target_attr );
	ASSERT( source_attr );
        if (!source_ad) {source_ad = this;}
        
	CopyAttribute(target_attr, *this, source_attr, *source_ad);
}

void ClassAd::
CopyAttribute(const char *target_attr, classad::ClassAd &target_ad, const char *source_attr, const classad::ClassAd &source_ad)
{
	classad::ExprTree *e = source_ad.Lookup( source_attr );
	if ( e ) {
		e = e->Copy();
		target_ad.Insert( target_attr, e );
	} else {
		target_ad.Delete( target_attr );
	}
}

//////////////XML functions///////////

int
fPrintAdAsXML(FILE *fp, const classad::ClassAd &ad, StringList *attr_white_list)
{
    if(!fp)
    {
        return FALSE;
    }

    std::string out;
    sPrintAdAsXML(out,ad,attr_white_list);
    fprintf(fp, "%s", out.c_str());
    return TRUE;
}

int
sPrintAdAsXML(MyString &output, const classad::ClassAd &ad, StringList *attr_white_list)
{
	std::string std_output;
	int rc = sPrintAdAsXML(std_output, ad, attr_white_list);
	output += std_output;
	return rc;
}

int
sPrintAdAsXML(std::string &output, const classad::ClassAd &ad, StringList *attr_white_list)
{
	classad::ClassAdXMLUnParser unparser;
	std::string xml;

	unparser.SetCompactSpacing(false);
	if ( attr_white_list ) {
		classad::ClassAd tmp_ad;
		classad::ExprTree *expr;
		const char *attr;
		attr_white_list->rewind();
		while( (attr = attr_white_list->next()) ) {
			if ( (expr = ad.Lookup( attr )) ) {
				classad::ExprTree *new_expr = expr->Copy();
				tmp_ad.Insert( attr, new_expr );
			}
		}
		unparser.Unparse( xml, &tmp_ad );
	} else {
		unparser.Unparse( xml, &ad );
	}
	output += xml;
	return TRUE;
}
///////////// end XML functions /////////

int
fPrintAdAsJson(FILE *fp, const classad::ClassAd &ad, StringList *attr_white_list)
{
    if(!fp)
    {
        return FALSE;
    }

    std::string out;
    sPrintAdAsJson(out,ad,attr_white_list);
    fprintf(fp, "%s", out.c_str());
    return TRUE;
}

int
sPrintAdAsJson(MyString &output, const classad::ClassAd &ad, StringList *attr_white_list)
{
	std::string std_output;
	int rc = sPrintAdAsJson(std_output, ad, attr_white_list);
	output += std_output;
	return rc;
}

int
sPrintAdAsJson(std::string &output, const classad::ClassAd &ad, StringList *attr_white_list)
{
	classad::ClassAdJsonUnParser unparser;

	if ( attr_white_list ) {
		classad::ClassAd tmp_ad;
		classad::ExprTree *expr;
		const char *attr;
		attr_white_list->rewind();
		while( (attr = attr_white_list->next()) ) {
			if ( (expr = ad.Lookup( attr )) ) {
				classad::ExprTree *new_expr = expr->Copy();
				tmp_ad.Insert( attr, new_expr );
			}
		}
		unparser.Unparse( output, &tmp_ad );
	} else {
		unparser.Unparse( output, &ad );
	}
	return TRUE;
}

char const *
QuoteAdStringValue(char const *val, std::string &buf)
{
    if(val == NULL) {
        return NULL;
    }

    buf.clear();

    classad::Value tmpValue;
    classad::ClassAdUnParser unparse;

	unparse.SetOldClassAd( true, true );

    tmpValue.SetStringValue(val);
    unparse.Unparse(buf, tmpValue);

    return buf.c_str();
}

void ClassAd::ChainCollapse()
{
    classad::ExprTree *tmpExprTree;

	classad::ClassAd *parent = GetChainedParentAd();

    if(!parent)
    {   
        //nothing chained, time to leave
        return;
    }

    Unchain();

    classad::AttrList::iterator itr; 

    for(itr = parent->begin(); itr != parent->end(); itr++)
    {
        // Only move the value from our chained ad into our ad when it 
        // does not already exist. Hence the Lookup(). 
        // This means that the attributes in our classad takes precedence
        // over the ones in the chained class ad.

        if( !Lookup((*itr).first) )
        {
            tmpExprTree = (*itr).second;     

            //deep copy it!
            tmpExprTree = tmpExprTree->Copy(); 
            ASSERT(tmpExprTree); 

            //K, it's clear. Insert it, but don't try to 
            Insert((*itr).first, tmpExprTree);
        }
    }
}

void ClassAd::
GetReferences(const char* attr,
                StringList *internal_refs,
                StringList *external_refs) const
{
    ExprTree *tree;

    tree = Lookup(attr);
    if(tree != NULL)
    {
		_GetReferences( tree, internal_refs, external_refs );
    }
}

bool ClassAd::
GetExprReferences(const char* expr,
				  StringList *internal_refs,
				  StringList *external_refs) const
{
	classad::ClassAdParser par;
	classad::ExprTree *tree = NULL;
	par.SetOldClassAd( true );

    if ( !par.ParseExpression( expr, tree, true ) ) {
        return false;
    }

	_GetReferences( tree, internal_refs, external_refs );

	delete tree;

	return true;
}

bool ClassAd::
GetExprReferences(classad::ExprTree *tree,
				  StringList *internal_refs,
				  StringList *external_refs) const
{
	_GetReferences( tree, internal_refs, external_refs );
	return true;
}

static void AppendReference( StringList &reflist, char const *name )
{
	char const *end = strchr(name,'.');
	std::string buf;
	if( end ) {
			// if attribute reference is of form 'one.two.three...'
			// only insert 'one' in the list of references

		if( end == name ) {
				// If reference is of form '.one.two.three...'
				// only insert 'one'.  This is unlikely to be correct,
				// because the root scope is likely to be the MatchClassAd,
				// but inserting an empty attribute name would make it
				// harder to understand what is going on, so it seems
				// better to insert 'one'.
			end = strchr(end,'.');
		}

		buf.append(name,end-name);
		name = buf.c_str();
	}

	if( !reflist.contains_anycase(name) ) {
		reflist.append(name);
	}
}

void ClassAd::
_GetReferences(classad::ExprTree *tree,
			   StringList *internal_refs,
			   StringList *external_refs) const
{
	if ( tree == NULL ) {
		return;
	}

	classad::References ext_refs_set;
	classad::References int_refs_set;
	classad::References::iterator set_itr;

	bool ok = true;
	if( external_refs && !GetExternalReferences(tree, ext_refs_set, true) ) {
		ok = false;
	}
	if( internal_refs && !GetInternalReferences(tree, int_refs_set, true) ) {
		ok = false;
	}
	if( !ok ) {
		dprintf(D_FULLDEBUG,"warning: failed to get all attribute references in ClassAd (perhaps caused by circular reference).\n");
		dPrintAd(D_FULLDEBUG, *this);
		dprintf(D_FULLDEBUG,"End of offending ad.\n");
	}

		// We first process the references and save results in
		// final_*_refs_set.  The processing may hit duplicates that
		// are referred to xand then copy from there to the caller's
		// StringLists.  This scales better than trying to remove
		// duplicates while inserting into the StringList.

	if ( external_refs ) {
		for ( set_itr = ext_refs_set.begin(); set_itr != ext_refs_set.end();
			  set_itr++ ) {
			const char *name = set_itr->c_str();
			// Check for references to things in the MatchClassAd
			// and do the right thing.  This does not cover all
			// possible ways of referencing things in the match ad,
			// but it covers the ones users are expected to use
			// and the ones expected from OptimizeAdForMatchmaking.
			if ( strncasecmp( name, "target.", 7 ) == 0 ) {
				AppendReference( *external_refs, &set_itr->c_str()[7] );
			} else if ( strncasecmp( name, "other.", 6 ) == 0 ) {
				AppendReference( *external_refs, &set_itr->c_str()[6] );
			} else if ( strncasecmp( name, ".left.", 6 ) == 0 ) {
				AppendReference( *external_refs, &set_itr->c_str()[6] );
			} else if ( strncasecmp( name, ".right.", 7 ) == 0 ) {
				AppendReference( *external_refs, &set_itr->c_str()[7] );
			} else {
				AppendReference( *external_refs, set_itr->c_str() );
			}
		}
	}

	if ( internal_refs ) {
		for ( set_itr = int_refs_set.begin(); set_itr != int_refs_set.end();
			  set_itr++ ) {
			AppendReference( *internal_refs, set_itr->c_str() );
		}
	}
}



// the freestanding functions 

classad::ExprTree *
AddExplicitTargetRefs(classad::ExprTree *tree,
						std::set < std::string, classad::CaseIgnLTStr > & definedAttrs) 
{
	if( tree == NULL ) {
		return NULL;
	}
	classad::ExprTree::NodeKind nKind = tree->GetKind( );
	switch( nKind ) {
	case classad::ExprTree::ATTRREF_NODE: {
		classad::ExprTree *expr = NULL;
		string attr = "";
		bool abs = false;
		( ( classad::AttributeReference * )tree )->GetComponents(expr,attr,abs);
		if( abs || expr != NULL ) {
			return tree->Copy( );
		}
		else {
			if( definedAttrs.find( attr ) == definedAttrs.end( ) ) {
					// attribute is not defined, so insert "target"
				classad::AttributeReference *target = NULL;
				target = classad::AttributeReference::MakeAttributeReference(NULL,
																	"target");
				return classad::AttributeReference::MakeAttributeReference(target,attr);
			}
			else {
				return tree->Copy( );
			}
		}
	}
	case classad::ExprTree::OP_NODE: {
		classad::Operation::OpKind oKind;
		classad::ExprTree * expr1 = NULL;
		classad::ExprTree * expr2 = NULL;
		classad::ExprTree * expr3 = NULL;
		classad::ExprTree * newExpr1 = NULL;
		classad::ExprTree * newExpr2 = NULL;
		classad::ExprTree * newExpr3 = NULL;
		( ( classad::Operation * )tree )->GetComponents( oKind, expr1, expr2, expr3 );
		if( expr1 != NULL ) {
			newExpr1 = AddExplicitTargetRefs( expr1, definedAttrs );
		}
		if( expr2 != NULL ) {
			newExpr2 = AddExplicitTargetRefs( expr2, definedAttrs );
		}
		if( expr3 != NULL ) {
			newExpr3 = AddExplicitTargetRefs( expr3, definedAttrs );
		}
		return classad::Operation::MakeOperation( oKind, newExpr1, newExpr2, newExpr3 );
	}
	case classad::ExprTree::FN_CALL_NODE: {
		std::string fn_name;
		classad::ArgumentList old_fn_args;
		classad::ArgumentList new_fn_args;
		( ( classad::FunctionCall * )tree )->GetComponents( fn_name, old_fn_args );
		for ( classad::ArgumentList::iterator i = old_fn_args.begin(); i != old_fn_args.end(); i++ ) {
			new_fn_args.push_back( AddExplicitTargetRefs( *i, definedAttrs ) );
		}
		return classad::FunctionCall::MakeFunctionCall( fn_name, new_fn_args );
	}
	default: {
 			// old ClassAds have no function calls, nested ClassAds or lists
			// literals have no attrrefs in them
		return tree->Copy( );
	}
	}
} 

classad::ExprTree *RemoveExplicitTargetRefs( classad::ExprTree *tree )
{
	if( tree == NULL ) {
		return NULL;
	}
	classad::ExprTree::NodeKind nKind = tree->GetKind( );
	switch( nKind ) {
	case classad::ExprTree::ATTRREF_NODE: {
		classad::ExprTree *expr = NULL;
		string attr = "";
		bool abs = false;
		( ( classad::AttributeReference * )tree )->GetComponents(expr,attr,abs);
		if(!abs && (expr != NULL)) {
			string newAttr = "";
			classad::ExprTree *exp = NULL;
			abs = false;
			( ( classad::AttributeReference * )expr )->GetComponents(exp,newAttr,abs);
			if (strcasecmp(newAttr.c_str(), "target") == 0){
				return classad::AttributeReference::MakeAttributeReference(NULL,attr);
			}	 
		} 
		return tree->Copy();
	}
	case classad::ExprTree::OP_NODE: {
		classad::Operation::OpKind oKind;
		classad::ExprTree * expr1 = NULL;
		classad::ExprTree * expr2 = NULL;
		classad::ExprTree * expr3 = NULL;
		classad::ExprTree * newExpr1 = NULL;
		classad::ExprTree * newExpr2 = NULL;
		classad::ExprTree * newExpr3 = NULL;
		( ( classad::Operation * )tree )->GetComponents( oKind, expr1, expr2, expr3 );
		if( expr1 != NULL ) {
			newExpr1 = RemoveExplicitTargetRefs( expr1  );
		}
		if( expr2 != NULL ) {
			newExpr2 = RemoveExplicitTargetRefs( expr2 );
		}
		if( expr3 != NULL ) {
			newExpr3 = RemoveExplicitTargetRefs( expr3 );
		}
		return classad::Operation::MakeOperation( oKind, newExpr1, newExpr2, newExpr3 );
	}
	case classad::ExprTree::FN_CALL_NODE: {
		std::string fn_name;
		classad::ArgumentList old_fn_args;
		classad::ArgumentList new_fn_args;
		( ( classad::FunctionCall * )tree )->GetComponents( fn_name, old_fn_args );
		for ( classad::ArgumentList::iterator i = old_fn_args.begin(); i != old_fn_args.end(); i++ ) {
			new_fn_args.push_back( RemoveExplicitTargetRefs( *i ) );
		}
		return classad::FunctionCall::MakeFunctionCall( fn_name, new_fn_args );
	}
	default: {
 			// old ClassAds have no function calls, nested ClassAds or lists
			// literals have no attrrefs in them
		return tree->Copy( );
	}
	}
}	


classad::ExprTree *AddTargetRefs( classad::ExprTree *tree, TargetAdType /*target_type*/ )
{
	// Disable AddTargetRefs for now
	return tree->Copy();

}

const char *ConvertEscapingOldToNew( const char *str )
{
	static std::string new_str;
	new_str = "";
	ConvertEscapingOldToNew( str, new_str );
	return new_str.c_str();
}

void ConvertEscapingOldToNew( const char *str, std::string &buffer )
{
		// String escaping is different between new and old ClassAds.
		// We need to convert the escaping from old to new style before
		// handing the expression to the new ClassAds parser.
	while( *str ) {
		size_t n = strcspn(str,"\\");
		buffer.append(str,n);
		str += n;
		if ( *str == '\\' ) {
			buffer.append( 1, '\\' );
			str++;
			if(  (str[0] != '"') ||
				 ( /*(str[0] == '"') && */ IsStringEnd(str, 1) )   )
			{
				buffer.append( 1, '\\' );
			}
		}
	}
		// remove trailing whitespace
	int ix = (int)buffer.size();
	while (ix > 1) {
		char ch = buffer[ix-1];
		if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n')
			break;
		--ix;
	}
	buffer.resize(ix);
}

// split a single line of -long-form classad into attr and value part
// removes leading whitespace and whitespace around the =
// set rhs to point to the first non whitespace character after the =
// you can pass this to ConvertEscapingOldToNew
// returns true if there was an = and the attr was non-empty
//
bool SplitLongFormAttrValue(const char * line, std::string &attr, const char* &rhs)
{
	while (isspace(*line)) ++line;

	const char * peq = strchr(line, '=');
	if ( ! peq) return false;

	const char * p = peq;
	while (p > line && ' ' == p[-1]) --p;
	attr.clear();
	attr.append(line, p-line);

	// set rhs to the first non-space character after the =
	p = peq+1;
	while (' ' == *p) ++p;
	rhs = p;

	return !attr.empty();
}

// split a single line of -long form classad into addr and value, then
// parse and insert into the given classad, using the classadCache or not as requested.
//
bool InsertLongFormAttrValue(classad::ClassAd & ad, const char * line, bool use_cache)
{
	std::string attr;
	const char * rhs;
	if ( ! SplitLongFormAttrValue(line, attr, rhs)) {
		return false;
	}

	if (use_cache) {
		return ad.InsertViaCache(attr, rhs);
	}

	classad::ClassAdParser parser;
	parser.SetOldClassAd(true);
	ExprTree *tree = parser.ParseExpression(rhs);
	if ( ! tree) {
		return false;

	}
	return ad.Insert(attr, tree);
}


// end functions

} // namespace compat_classad
