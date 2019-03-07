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

/*
	This code tests the sin_to_string() function implementation.
 */

#include "condor_common.h"
#include "condor_debug.h"
#include "condor_config.h"
#include "internet.h"
#include "function_test_driver.h"
#include "emit.h"
#include "unit_test_utils.h"

static bool test_normal_case(void);
static bool test_hostname(void);
static bool test_no_angle_brackets(void);

bool FTEST_is_valid_sinful(void) {
		// beginning junk for getPortFromAddr(() {
	emit_function("int is_valid_sinful(char* sinful)");
	emit_comment("Determines if a function is a valid sinful string.");
	emit_problem("None");
	
		// driver to run the tests and all required setup
	FunctionDriver driver;
	driver.register_function(test_normal_case);
	driver.register_function(test_hostname);
	driver.register_function(test_no_angle_brackets);
	
		// run the tests
	return driver.do_all_functions();
}

static bool test_normal_case() {
	emit_test("Is normal input identified correctly?");
	char* input = strdup( "<208.122.19.56:47>" );
	emit_input_header();
	emit_param("SINFUL", input);
	int result = is_valid_sinful( input );
	free( input );
	emit_output_expected_header();
	emit_retval("%s", tfstr(TRUE));
	emit_output_actual_header();
	emit_retval("%s", tfstr(result));
	if(result != TRUE) {
		FAIL;
	}
	PASS;
}

static bool test_hostname() {
	emit_test("Are hostnames instead of IP addresses rejected like they should be?");
	char* input = strdup( "<balthazar.cs.wisc.edu:47>" );
	emit_input_header();
	emit_param("SINFUL", input);
	int result = is_valid_sinful( input );
	free( input );
	emit_output_expected_header();
	emit_retval("%s", tfstr(FALSE));
	emit_output_actual_header();
	emit_retval("%s", tfstr(result));
	if(result != FALSE) {
		FAIL;
	}
	PASS;
}

static bool test_no_angle_brackets() {
	emit_test("Is the string correctly rejected if there are no angle brackets?");
	char* input = strdup( "209.172.63.167:8080" );
	emit_input_header();
	emit_param("SINFUL", input);
	int result = is_valid_sinful( input );
	free( input );
	emit_output_expected_header();
	emit_retval("%s", tfstr(FALSE));
	emit_output_actual_header();
	emit_retval("%s", tfstr(result));
	if(result != FALSE) {
		FAIL;
	}
	PASS;
}
