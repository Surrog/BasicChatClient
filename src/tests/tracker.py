
import unittest
import os
import subprocess
import socket
import json
import sys


bin_path = os.path.join(os.getcwd(), "..", "..", "bin")
if os.name == "nt":
	tracker = os.path.join(bin_path, "Debug", "tracker.exe")
else:
	tracker = os.path.join(bin_path, "tracker")

address = "localhost"
port = 8752

def convert(val):
	if sys.version < "3":
		return str(val)
	else:
		return str(val).encode("utf-8")


class TrackerTest(unittest.TestCase):
	@classmethod
	def setUpClass(self):
		self.process = subprocess.Popen([tracker], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

	@classmethod
	def tearDownClass(self):
		self.process.kill()

	def test_bad_connection(self):
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.assertNotEqual(s.connect_ex((address, 8543)), 0)
		s.close()

	def test_starting_server(self):
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.assertEqual(s.connect_ex((address, port)), 0)
		s.close()

	def test_bad_authent(self):
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.assertEqual(s.connect_ex((address, port)), 0)
		s.send(convert(json.dumps({"id": -1, "data":"toto"})))
		try:
			with self.assertRaises(ConnectionResetError) as cm:
				s.recv(1024)
		except:
			pass

		s.close()

	def test_authent(self):
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.assertEqual(s.connect_ex((address, port)), 0)
		s.send(convert(json.dumps({"id": 0, "data":"toto"})))
		value = s.recv(1024)
		data = json.loads(value)
		#self.assertEqual(data, bytes(json.dumps({"id": 0, "data":"toto"}), "utf-8"))
		s.close()

	def test_broadcast(self):
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.assertEqual(s.connect_ex((address, port)), 0)
		data_to_send = {"id": 0, "data":"toto"}
		s.send(convert(json.dumps(data_to_send)))
		value = s.recv(1024)
		data = json.loads(value)
		print(data)
		print(data_to_send)
#				self.assertEqual(data["id"], data_to_send["id"])
#				self.assertEqual(data["data"], data_to_send["data"])
		s.close()		

if __name__ == '__main__':
	print(tracker)

	unittest.main()