executable   = ./job_dagman_retry_recovery-nodeC.pl
arguments    = $(DAGManJobId)
universe     = scheduler
output       = job_dagman_retry_recovery-nodeC.out
error        = job_dagman_retry_recovery-nodeC.err
log          = job_dagman_retry_recovery.log
# Note: we need getenv = true for the node job to talk to the schedd of
# the personal condor that's running the test.
getenv       = true
Notification = NEVER
queue
