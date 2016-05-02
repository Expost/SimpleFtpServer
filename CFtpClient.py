import socket
import sys
import getpass
import re

class CTransfer:
	def __init__(self,ip,port):
		self.ip = ip
		self.port = port
		self.s = None
		self.mode = None    # passive is 1,port is 0
		self.command = {"delete":self.do_delete,
		"cd":self.do_cd,"get":self.do_get,"help":self.do_help,"ls":self.do_ls,"mkdir":self.do_mkdir,
		"passive":self.do_passive,"pwd":self.do_pwd,"quit":self.do_quit,"rename":self.do_rename,
		"rmdir":self.do_rmdir,"send":self.do_send,"size":self.do_size,"recv":self.do_get,
		"put":self.do_send}
	def send(self,buf):
		try:
			n = self.s.send(buf)
		except Exception,e:
			print "send msg error",e
			n = -1
		return n

	def recv(self):
		buf = ""
		while 1:
			b = self.s.recv(1)
			buf += b
			if b == "\n":
				break
		return buf

	def connect(self):
		self.s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		try:
			self.s.connect((self.ip,self.port))
		except Exception,e:
			print "Connect error ",e
			exit()
		self.printf(self.recv())
	def printf(self,buf):
		print buf,
	def login(self):
		while 1:
			username = raw_input("Username: ")
			password = getpass.getpass("Password: ")
			self.send("USER %s\r\n" %username)
			self.recv()
			self.send("PASS %s\r\n" %password)
			buf = self.recv()
			self.printf(buf)
			if buf.split()[0] == "230":
				break

	def do_delete(self,command):
		l = command.split()
		if len(l) == 2:
			self.send("DELE %s\r\n" %l[1])
			self.printf(self.recv())
		else:
			print "please specify the file name to delete."

	def do_cd(self,command):
		l = command.split()
		if len(l) == 2:
			self.send("CWD %s\r\n" %l[1])
			self.printf(self.recv())
		else:
			print "please specify the path to change."

	def do_get(self,command):
		l = command.split()
		if len(l) != 2:
			print "please specify the file name to download."
		else:
			if self.mode == 1:
				self.send("PASV\r\n");
				buf = self.recv()
				self.printf(buf)
				if buf.split()[0] == "227":
					ll = re.findall("\d+",buf)
					ip = "%s.%s.%s.%s" %(ll[1],ll[2],ll[3],ll[4])
					port = int(ll[5]) * 256 + int(ll[6])
					self.send("TYPE I\r\n")
					self.printf(self.recv())

					self.send("RETR %s\r\n" %l[1])
					sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
					try:
						sock.connect((ip,port))
					except:
						print "data connect error"
						sock.close()
						return
					with open(l[1],"wb") as f:
						while 1:
							buf = sock.recv(4096)
							if buf:
								f.write(buf)
							else:
								break
					
					sock.close()
					self.printf(self.recv())
					self.printf(self.recv())
			elif self.mode == 0:
				print "port mode is not supported."

	def do_help(self,command):
		print "Now supported command is:"
		for i in self.command:
			print i,
		print 
	def do_ls(self,command):
		if self.mode == 1:
			self.send("PASV\r\n");
			buf = self.recv()
			self.printf(buf)
			if buf.split()[0] == "227":
				l = re.findall("\d+",buf)
				ip = "%s.%s.%s.%s" %(l[1],l[2],l[3],l[4])
				port = int(l[5]) * 256 + int(l[6])
				self.send("TYPE A\r\n")
				self.printf(self.recv())
				self.send("LIST\r\n")
				
				sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
				try:
					sock.connect((ip,port))
				except:
					print "data connect error"
					sock.close()
					return
				dirinfo = sock.recv(1000000)
				sock.close()

				self.printf(self.recv())
				self.printf(dirinfo)
				self.printf(self.recv())
			else:
				print "ftp server can not support pasv mode,please use port mode."

		elif self.mode == 0:
			print "port mode is not supported."

	def do_mkdir(self,command):
		l = command.split()
		if len(l) == 2:
			self.send("MKD %s\r\n" %l[1])
			self.printf(self.recv())
		else:
			print "please specify the dir name to create."

	def do_passive(self,command):
		if self.mode == 1:
			self.mode = 0
			print "passive mode off."
		elif self.mode == 0:
			self.mode = 1
			print "passive mode on."
		else:
			self.mode = 1
			print "passive mode on."

	def do_pwd(self,command):
		self.send("PWD\r\n")
		self.printf(self.recv())

	def do_quit(self,command):
		self.send("QUIT\r\n")
		self.printf(self.recv())
		exit()

	def do_rename(self,command):
		l = command.split()
		if len(l)  == 3:
			self.send("RNFR %s\r\n" %l[1])
			buf = self.recv()
			if buf.split()[0] == "550":
				print "file not exist."
			elif buf.split()[0] == "350":
				self.send("RNTO %s\r\n" %l[2])
				self.printf(self.recv())
		else:
			print "please specify the old name and new name."

	def do_rmdir(self,command):
		l = command.split()
		if len(l)  == 2:
			self.send("RMD %s\r\n" %l[1])
			self.printf(self.recv())
		else:
			print "please specify the dir name to delete."

	def do_send(self,command):
		l = command.split()
		if len(l) != 2:
			print "please specify the file name to upload."
		else:
			if self.mode == 1:
				self.send("PASV\r\n");
				buf = self.recv()
				self.printf(buf)
				if buf.split()[0] == "227":
					ll = re.findall("\d+",buf)
					ip = "%s.%s.%s.%s" %(ll[1],ll[2],ll[3],ll[4])
					port = int(ll[5]) * 256 + int(ll[6])
					self.send("TYPE I\r\n")
					self.printf(self.recv())

					self.send("STOR %s\r\n" %l[1])
					sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
					try:
						sock.connect((ip,port))
					except:
						print "data connect error"
						sock.close()
						return
					with open(l[1],"rb") as f:
						for data in f:
							sock.send(data)
					
					sock.close()
					self.printf(self.recv())
					self.printf(self.recv())
			elif self.mode == 0:
				print "port mode is not supported."

	def do_size(self,command):
		l = command.split()
		if len(l)  == 2:
			self.send("SIZE %s\r\n" %l[1])
			self.printf(self.recv())
		else:
			print "please specify the file name to get size."

	def exc_command(self):
		while 1:
			command = raw_input("ftp> ")
			if command.split()[0].lower() in self.command:
				self.command[command.split()[0].lower()](command)
			else:
				print "?Invalid command,please use help to see suported command."

	def run(self):
		try:
			self.connect()
			
			self.login()
			self.exc_command()
		except KeyboardInterrupt,e:
			print "bye."
		

def main():
	if len(sys.argv) < 3:
		print "please use SimpleFtpClient [ip] [port] execute this program."
	else:
		t = CTransfer(sys.argv[1],int(sys.argv[2]))
		t.run()


if __name__ == "__main__":
	main()