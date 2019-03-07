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
#include "condor_debug.h"
#include "sysapi.h"
#include "sysapi_externs.h"
#include "test.h"
#include "stdio.h"
#include "math.h"


static int delta_check( int			blockno,
						double		means[],
						double		max_sd_ratio,
						const char	*label )
{
	if ( blockno > 0 ) {
		double	delta = means[blockno] - means[blockno-1];
		double	ratio = fabs( delta / means[blockno-1] );
		if ( ratio > max_sd_ratio ) {
			dprintf(D_ALWAYS,
					"SysAPI: WARNING: %s MIPS Blk %d: "
					"Delta vs Blk %d = %2.2f%% > %2.2f%%\n",
					label, blockno, blockno-1,
					ratio * 100.0, max_sd_ratio * 100.0 );
			return 1;
		}
	}
	return 0;
}

static int sd_check( int		blockno,
					 double		mean,
					 double		sds[],
					 double		max_sd_ratio,
					 const char	*label )
{
	double	max_sd = mean * max_sd_ratio;
	if ( sds[blockno] > max_sd ) {
		dprintf(D_ALWAYS,
				"SysAPI: WARNING: %s MIPS blk %d: "
				"SD = %.0f (%2.2f%%) > %.0f (%2.2f%%) of the mean.\n",
				label, blockno, sds[blockno], 100.0 * sds[blockno] / mean,
				max_sd, max_sd_ratio * 100.0
			);
		return 1;
	}
	return 0;
}

int mips_test(int test_blocksize,
			  double max_sd_variation_ratio, 
			  double ratio_failed_test_blocks_ok)
{
	int foo,  bar;
	int	return_val = 0;
	int i, j;
	int	num_tests = 0;
	int	num_warnings = 0;
	double mean, raw_mean;
	double variance, raw_variance;
	int test[test_blocksize], raw_test[test_blocksize];
	double testblocks_sd[test_blocksize], raw_testblocks_sd[test_blocksize];
	double testblocks_mean[test_blocksize], raw_testblocks_mean[test_blocksize];

	dprintf(D_ALWAYS, 
		"SysAPI: Running mips_test.\n");
	dprintf(D_ALWAYS, 
		"\tI will test sysapi_mips (and sysapi_mips_raw) in\n");
	dprintf(D_ALWAYS, 
		"\tblocks of %d tests, and take the standard\n", test_blocksize);
	dprintf(D_ALWAYS, 
		"\tdeviation of those test blocks. If the standard\n");
	dprintf(D_ALWAYS, 
		"\tdeviation is above %2f%% of the average,\n", max_sd_variation_ratio);
	dprintf(D_ALWAYS, 
		"\tthe mips reported are erratic and the test block\n");
	dprintf(D_ALWAYS, 
		"\tis considered a failure.\n");
	dprintf(D_ALWAYS, 
		"\tI will run %d test blocks, and if more than %2f%% of those blocks\n",
		test_blocksize, ratio_failed_test_blocks_ok*100);
	dprintf(D_ALWAYS, 
		"\tfail, this entire test fails.\n");

	foo = sysapi_mips_raw();
	dprintf(D_ALWAYS, "SysAPI: Initial sysapi_mips_raw -> %d\n", foo);
	bar = sysapi_mips();
	dprintf(D_ALWAYS, "SysAPI: Initial sysapi_mips -> %d\n", bar);

	for (i=0; i<test_blocksize; i++) {
		raw_mean = 0;
		raw_variance = 0;

		for (j=0; j<test_blocksize; j++) {
			raw_test[j] = sysapi_mips_raw();

			raw_mean += raw_test[j];

			dprintf(D_ALWAYS, "SysAPI: Testblock %d, test %d: Raw mips: %d\n",
					i, j, raw_test[j]);

			if (raw_test[j] <= 0) {
				dprintf(D_ALWAYS, "SysAPI: ERROR! Raw mips (%d) is negative!\n", foo);
				return_val = 1;
			}
		}

		raw_mean /= test_blocksize;
		raw_testblocks_mean[i] = raw_mean;
		dprintf(D_ALWAYS, "SysAPI: Testblock %d, mean:   Raw mips: %.1f\n",
				i, raw_mean );

		/* Test if there were any unusually large jumps in the means of the testblocks */
		num_tests++;
		if ( delta_check( i, raw_testblocks_mean, max_sd_variation_ratio, "Raw" ) ) {
			num_warnings++;
		}

		for (j=0; j<test_blocksize; j++) {
			raw_variance += (raw_test[j] - raw_mean)*(raw_test[j] - raw_mean);
		}
		raw_variance /= test_blocksize;

		raw_testblocks_sd[i] = sqrt(raw_variance);

		dprintf(D_ALWAYS,
				"SysAPI: Standard deviation of raw testblock %d is: %d MIPS\n", i, 
				(int)raw_testblocks_sd[i]);

		num_tests++;
		if ( sd_check( i, raw_mean, raw_testblocks_sd, max_sd_variation_ratio, "Raw" ) ) {
			num_warnings++;
		}
	}

	if (((double)num_warnings/(double)num_tests) > max_sd_variation_ratio) {
		dprintf(D_ALWAYS,
				"SysAPI: ERROR! Failing because %d raw MIPS tests failed > %d (%2.2f%%).\n",
				num_warnings, (int)(max_sd_variation_ratio*test_blocksize), max_sd_variation_ratio*100);
		return_val = return_val | 1;
	}
	
	dprintf(D_ALWAYS, "\n\n");
	num_warnings = 0;

	for (i=0; i<test_blocksize; i++) {
		mean = 0;
		variance = 0;

		for (j=0; j<test_blocksize; j++) {
			test[j] = sysapi_mips_raw();

			mean += test[j];

			dprintf(D_ALWAYS, "SysAPI: Testblock %d, test %d: Cooked mips: %d\n",
					i, j, test[j]);

			if (test[j] <= 0) {
				dprintf(D_ALWAYS, "SysAPI: ERROR! Cooked mips (%d) is negative!\n", foo);
				return_val = 1;
			}
		}

		mean /= test_blocksize;
		testblocks_mean[i] = mean;
		dprintf(D_ALWAYS, "SysAPI: Testblock %d, mean  : Cooked mips: %.1f\n",
				i, mean);

		/* Test if there were any unusually large jumps in the means of the testblocks */
		num_tests++;
		if ( delta_check( i, testblocks_mean, max_sd_variation_ratio, "Cooked" ) ) {
			num_warnings++;
		}

		for (j=0; j<test_blocksize; j++) {
			variance += (test[j] - mean)*(test[j] - mean);
		}
		variance /= test_blocksize;

		testblocks_sd[i] = sqrt(variance);

		dprintf(D_ALWAYS, "SysAPI: Standard deviation of testblock %d is: %d MIPS\n", i, 
				(int)testblocks_sd[i]);

		num_tests++;
		if ( sd_check( i, mean, testblocks_sd, max_sd_variation_ratio, "Cooked" ) ) {
			num_warnings++;
		}
	}

	if (((double)num_warnings/(double)num_tests) > max_sd_variation_ratio) {
			dprintf(D_ALWAYS,
					"SysAPI: ERROR! Failing because %d cooked MIPS tests failed > %d (%2.2f%%).\n",
					num_warnings, (int)(max_sd_variation_ratio*test_blocksize), max_sd_variation_ratio*100);
			return_val = return_val | 1;
	}
	return return_val;
}
