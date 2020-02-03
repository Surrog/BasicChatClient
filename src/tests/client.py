
import unittest
import os
import subprocess
import socket
import json
import sys


bin_path = os.path.join(os.getcwd(), "..", "..", "bin")
if os.name == "nt":
	client = os.path.join(bin_path, "Debug", "client.exe")
else:
	client = os.path.join(bin_path, "client")

address = "localhost"
port = 5750


def convert(val):
	if sys.version < "3":
		return str(val)
	else:
		return str(val).encode("utf-8")


class TrackerTest(unittest.TestCase):
	@classmethod
	def setUpClass(self):
		self.process = subprocess.Popen([client], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

	@classmethod
	def tearDownClass(self):
		self.process.kill()

	def test_connection(self):
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.assertEqual(s.connect_ex((address, port)), 0)
		s.close()

if __name__ == '__main__':
	print(client)

	unittest.main()