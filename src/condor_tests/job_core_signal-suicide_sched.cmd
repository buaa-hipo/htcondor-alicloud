universe   = scheduler
executable = x_job_signal-suicide.pl
log = job_core_signal-suicide_sched.log
error = job_core_signal-suicide_sched.err
output = job_core_signal-suicide_sched.out
should_transfer_files = YES
Notification = NEVER
arguments  = $(Process)
wantcorefile = false
queue 15

