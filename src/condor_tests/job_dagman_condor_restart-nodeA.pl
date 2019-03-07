#! /usr/bin/env perl

$file = "job_dagman_condor_restart-nodeA.do_restart";

if (-e $file) {
	unlink $file or die "Unlink failed: $!";

	print "About to restart Condor\n";
	open (OUTPUT, "condor_restart 2>&1 |") or die "Can't fork: $!";
	while (<OUTPUT>) {
		print "$_";
	}
	close (OUTPUT) or die "Condor_restart failed: $?";

	# If the condor_restart works we'll get killed some time during
	# this sleep; otherwise we want the test to timeout and fail.
	sleep 10000;

	# If we ever get to here, we want to exit with failure.
	print "Process was not killed; condor_restart must have failed\n";
	exit(1);

} else {
	print "Node job succeeds\n";
}
