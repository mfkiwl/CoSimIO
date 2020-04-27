#     ______     _____ _           ________
#    / ____/___ / ___/(_)___ ___  /  _/ __ |
#   / /   / __ \\__ \/ / __ `__ \ / // / / /
#  / /___/ /_/ /__/ / / / / / / // // /_/ /
#  \____/\____/____/_/_/ /_/ /_/___/\____/
#  Kratos CoSimulationApplication
#
#  License:         BSD License, see license.txt
#
#  Main authors:    Philipp Bucher (https://github.com/philbucher)
#

# python imports
import unittest, sys, os

if __name__ == '__main__':
    sys.path.append(os.pardir) # necessary to import from parent dir

from testing_utilities import RunTestRunner

# This test is not strictly necessary, since the test-runner can also be executed directly
# Hence it is only for better integration with the remaining tests, such that they can be run alltogether
class CoSimIOTests(unittest.TestCase):
    def test_with_testrunner(self):
        runner = RunTestRunner("test_runner_co_sim_io_tests")

        self.assertEqual(runner.GetReturnCode(), 0)


if __name__ == '__main__':
    unittest.main()
