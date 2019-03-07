#! /usr/bin/python
#
# Starts a Skype File Transfer.
# The open file dialog is set to the current directory.
# The users to send to are searched through their Skype name and their
# full name (display name actually).
#
# Requires at least Skype 2.0.0.27.
#
# (c) Copyright 2007, Vincent Oberle, vincent@oberle.org
#
# This software may be used and distributed according to the terms
# of the GNU Public License, incorporated herein by reference.


import os
import sys
import re
from optparse import OptionParser

import Skype4Py


parser = OptionParser('%prog user [user*]', version="%prog 0.1")

options, args = parser.parse_args()
if len(args) < 1:
	parser.print_help()
	sys.exit(0)

dest = args[1:]
d= args[0]
msg = ''
for m in dest:
  msg +=m+' '
path = os.getcwd() # use current working directory

skype = Skype4Py.Skype()
skype.FriendlyName = 'file_sender'
skype.Attach() # attach to Skype client


ids = []
print dest 
# quick-filter type of search
	# first look for exact Skype ID match
for user in skype.Friends:
    #print 'Found exact match'
    if user.Handle.lower().find(d.lower()) != -1:     ids.append(user)
    elif user.DisplayName.lower().find(d.lower()) != -1: ids.append(user)

#ids.append(user)
    found = True
 #   break
#	if found: continue
#	for user in skype.Friends:


# print who will send to, no need to ask for confirmation as user can still cancel the open file window
#msg = "Hi Condor Messaging Integration Test..good morning :)"
#msg = dest[0]
print 'Sending to:'
for i in ids:
	if i.FullName: print ' ' + i.FullName + ' (' + i.Handle + ')'
	else: print ' ' + i.Handle
        skype.SendMessage(i.Handle, msg)


if ids: # ids is a list of Skype IDs
	in_str = ''
	if path: in_str = ' IN ' + path
	ids_str = ids[0].Handle
	for i in ids[1:]: ids_str = ids_str + ', ' + i.Handle
	# OPEN FILETRANSFER echo123 IN C:\temp
	cmd = 'OPEN FILETRANSFER ' + ids_str + in_str
#	print cmd
#	skype.SendCommand(skype.Command(cmd))
