executable   = ./job_dagman_node_prio.pl
arguments    = $(nodename)
universe     = scheduler
output       = $(nodename).out
error        = $(nodename).err
log          = job_dagman_node_prio.log
Notification = NEVER
queue
