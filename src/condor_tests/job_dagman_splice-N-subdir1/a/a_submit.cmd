executable   = /bin/echo
arguments    = OK
universe     = scheduler
output       = job_dagman_splice-N-subdir1/a/$(job).out
error        = job_dagman_splice-N-subdir1/a/$(job).err
log          = job_dagman_splice-N-subdir1/a/a_submit.log
Notification = NEVER
queue
