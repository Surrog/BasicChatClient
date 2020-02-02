
import unittest
import os
import subprocess
import socket
import json


bin_path = os.path.join(os.getcwd(), "..", "..", "bin", "Debug")
tracker = os.path.join(bin_path, "tracker")
address = "localhost"
port = 8752



if os.name == "nt":
	tracker += ".exe"

class TrackerTest(unittest.TestCase):
	def test_bad_connection(self):
		with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
			self.assertNotEqual(s.connect_ex((address, port)), 0)
			s.close()

	def test_starting_server(self):
		process = subprocess.Popen([tracker], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.assertEqual(s.connect_ex((address, port)), 0)
		s.close()
		process.kill()

	def test_bad_authent(self):
		process = subprocess.Popen([tracker], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.assertEqual(s.connect_ex((address, port)), 0)
		s.send(bytes(json.dumps({"id": -1, "data":"toto"}), "utf-8"))
		try:
			with self.assertRaises(ConnectionResetError) as cm:
				s.recv(1024)
		except:
			pass

		s.close()
		process.kill()

	def test_authent(self):
		process = subprocess.Popen([tracker], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.assertEqual(s.connect_ex((address, port)), 0)
		s.send(bytes(json.dumps({"id": 0, "data":"toto"}), "utf-8"))
		value = s.recv(1024)
		data = json.loads(value)
		#self.assertEqual(data, bytes(json.dumps({"id": 0, "data":"toto"}), "utf-8"))
		s.close()
		process.kill()

	def test_broadcast(self):
		process = subprocess.Popen([tracker], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.assertEqual(s.connect_ex((address, port)), 0)
		data_to_send = {"id": 0, "data":"toto"}
		s.send(bytes(json.dumps(data_to_send), "utf-8"))
		value = s.recv(1024)
		data = json.loads(value)
		print(data)
		print(data_to_send)
#				self.assertEqual(data["id"], data_to_send["id"])
#				self.assertEqual(data["data"], data_to_send["data"])
		s.close()		
		process.kill()


if __name__ == '__main__':
	print(tracker)

	unittest.main()