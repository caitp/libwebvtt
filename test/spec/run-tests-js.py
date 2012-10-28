#!/usr/bin/env python

"""
run-tests-js.py <good_test_dir> <bad_test_dir>

Runs all spec tests for WebVTT using the JavaScript WebVTT Parser

Requires node.js and the webvtt npm module,
see https://github.com/humphd/node-webvtt
"""

import os
import sys
import subprocess

"""Helper exception to inform caller that a program is not found"""
class ProgramNotFound(Exception):
  def __init__(self, program):
    self.value = "program '%s' not found." % program
    self.program = program

  def __str__(self):
    return repr(self.value)

"""Object representing the webvtt test-harness"""
class TestHarness:
  module = None
  test_root = "./"

  def __init__(self, test_root):
    self.test_root = test_root
  
  """helper, used to find the location of a program (from http://stackoverflow.com/questions/377017/test-if-executable-exists-in-python)"""
  def which(self,pgm):
    def is_exe(fpath):
      return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(pgm)
    if fpath:
      if is_exe(pgm):
        return pgm
    else:
      for path in os.environ["PATH"].split(os.pathsep):
        exe_file = os.path.join(path, pgm)
        if is_exe(exe_file):
          return exe_file
    return None

  """method which finds the full-path of the webvtt script we want to call. the windows shell behaves better when calling webvtt.cmd, as far as I can tell, so when os.name == "nt" or "ce", we will use that version, as opposed to the shell script"""
  def find_webvtt(self):
    if os.name == "nt" or os.name == "ce":
      # use the windows-shell friendly version
      self.module = self.which("webvtt.cmd")
    else:
      # use the unix-shell friendly version
      self.module = self.which("webvtt")
    if self.module == None:
      raise ProgramNotFound("webvtt")

  def set_module(self, module):
    self.module = module
	  
  """run a set of tests. we need to know the root of the tests, the test filename, and the expected return code"""
  def run_test_set(self, root, files, expected):
    failed = 0
    passed = 0

    # Run good tests
    for f in files:
      # Get file's absolute path
      file_path = os.path.join(root, f)
      with open(os.devnull, 'wb') as silent:
        retcode = subprocess.call([self.module, "-s", file_path],stdout=silent,stderr=silent)
      # If we did NOT get expected, add file to fail list & increase fail count.
      if retcode != expected:
        failed = failed + 1
        print "Failed %s" % file_path
      else:
        passed = passed + 1

    return failed, passed

  """Run all tests. Find the location of the webvtt module we're using, and rethrow if not found. Then call run_test_set for each test set defined in test_suite"""
  def run_tests(self):
    if self.module == None:
      try:
        self.find_webvtt()
      except:
        raise
    failed_total = 0
    passed_total = 0
    known_failed_total = 0
    known_passed_total = 0

    for test_suite in [{'name': 'good', 'expected': 0},
                       {'name': 'bad', 'expected': 1}]:
      root = os.path.realpath(os.path.join(self.test_root, test_suite['name']))
	  if os.path.isdir(root):
        failed, passed = self.run_test_set(root, os.listdir(root), test_suite['expected'])
        failed_total = failed_total + failed
        passed_total = passed_total + passed

    for test_suite in [{'name': 'known-good', 'expected': 1},
                       {'name': 'known-bad', 'expected': 0}]:
      root = os.path.realpath(os.path.join(self.test_root, test_suite['name']))
	  if os.path.isdir(root):
        known_failed, known_passed = self.run_test_set(root, os.listdir(root), test_suite['expected'])
        known_failed_total = known_failed_total + known_failed
        known_passed_total = known_passed_total + known_passed

    print "\n%s Passed, %s Failed, %s Known Failed, %s Known Passed, %s Total" % (passed_total, failed_total, known_passed_total, known_failed_total, passed_total + failed_total + known_passed_total + known_failed_total)

def main():
  harness = TestHarness(sys.argv[2])
  harness.set_module(sys.argv[1])
  try:
    harness.run_tests()
  except ProgramNotFound:
    print "Error: missing npm webvtt module, see https://github.com/humphd/node-webvtt"
    sys.exit(1)

if __name__ == '__main__':
  status = main()
  sys.exit(status)
