executable   = /bin/echo
arguments    = OK
universe     = scheduler
output       = job_dagman_splice-N-subdir1/$(job).out
error        = job_dagman_splice-N-subdir1/$(job).err
log          = job_dagman_splice-N-subdir1/job_dagman_splice-N-subdir1.log
Notification = NEVER
queue
