executable   = ./job_dagman_jobstate_log-upper-nodeA.pl
arguments    = $(nodename)
universe     = scheduler
output       = $(nodename).out
error        = $(nodename).err
log          = job_dagman_jobstate_log-upper-nodeA.log
Notification = NEVER
+pegasus_site = "local"
queue 2
