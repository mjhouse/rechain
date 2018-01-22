#!/usr/bin/env python3

import subprocess
import sys
import os
import re

# directory of script
HOME = os.path.dirname(os.path.realpath(__file__)).rstrip('/')

# working directory of tests
WORK = HOME + '/../..'

# colors
RED   = "\033[1;31m"
GREEN = "\033[0;32m"
BLUE  = "\033[1;34m"
RESET = "\033[0;0m"

class Test:

	def __init__( self, command ):
		if isinstance(command,str):
			command = re.sub('(;|\s|\t)+$','',command)
			test = [ x.strip() for x in command.split(';') ]

			if len(test) != 5:
				raise ValueError('Malformed test string')

			self.name    = test[0]
			self.runner  = test[1]
			self.script  = test[2]
			self.args    = test[3]
			self.timeout = float(test[4])

		else:
			raise ValueError('Non-string argument given to Test');

	@property
	def divider( self ):
		return '----------' + ('-' * len(self.name))

	def before( self ):
		print(self.divider)
		print('Test:     {}'.format(self.name))
		print('Status:   Running\n')

	def after( self, status, color ):
		print('\nTest:   {}'.format(self.name))
		print('\tStatus: {}{}{}'.format(color,status,RESET))
		print(self.divider)

	def run( self ):
		self.before()

		status = ''
		color  = ''

		try:
			r = subprocess.run( [self.runner,HOME + '/' + self.script,self.args],
				timeout = self.timeout,
				cwd     = WORK
				)

			if r.returncode == 0:
				status = 'SUCCESS'
				color  = GREEN 
			else:
				status = 'FAILURE'
				color  = RED

		except subprocess.TimeoutExpired as e:
			status = 'TIMED OUT'
			color  = BLUE

		self.after(status,color)

	def __str__( self ):
		return '<Test: "{}", Command: "{} {} {}", Timeout: {}s>'.format(
			self.name,
			self.runner,
			self.script,
			self.args,
			self.timeout
		)

if __name__=='__main__':

	os.environ['RECHAIN_HOME'] = HOME + '/data'

	tests = []
	with open(HOME + '/test.conf') as f:
		for line in f:
			if not line.startswith('#'):
				tests.append( Test(line) )
	
	for test in tests:
		test.run()
