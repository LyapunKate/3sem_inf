#хотим получать информацию от нескольких клиенов

#создаём хеш-таблицу, ключом будет файловый дескриптор clients { }
#Чтение данных обрабатывать в обработку исключений, если клиент шлёт что-то не то, то закрываем соединение с этим клиентом

import select
import socket

def main():
	srv_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
	srv_sock.bind(('', 7500))
	srv_sock.listen(10)
	#create poll object for handing multiple sockets (srv_sock + client connections)
	poll_obj = select.poll()
	poll_obj.registr(srv_sock, select.POLLIN)
	#Create dict for clients: fd -> (addr, socket, fobj)
	clients = {}
	try:
		while True:
			for fd, event in poll_obj.poll(): #итерируемся по событиям
				if fd == srv_sock.fileno():
					#Get one more connection from the queue		
					client_sock, client_addr = srv_sock.accept()
					print ("Connected from: ", client_addr)
					#Wrap socket with a file-like objects
					client_fobj = client_sock.makefile('rw', encoding = 'utf-8')
					#Add this client to dict and poll_obj for further handling
					clients[client_sock.fileno()] = (client_addr, client_sock, client_fobj)
					poll_obj.register(client_sock, select.POLLIN)	

#плохой клиент может подвесить сервер, если будет слишком долго отвечать, поэтому надо настроить timeout
#если попали внутрь этого if следовательно пришло новое соединение, после обработки идём на начало цикла
					continue
#если не попали в if следовательно сообщение пришло куда-то в другой дескриптор
				#New data from some client:
				print('Client event ', fd, event)
				(client_addr, client_sock, client_fobj) = clients[fd]
#проверяем есть ли новые данные или это ошибка
				if event == select.POLLIN:
					#New data from the client, read it and sent to others
#может посылать в час по чайной ложке, тогда сервер будет висеть и не сможет обрабатывать других
#или наоборот прислать огромный объём данных который просто не обработается
#нужно добавить обработку этих ошибок
 					line = client_fobj.readline()
					if len(line) > 0
						print('Client data.', line.strip())
#так как это широковещательный чат, то нужно разослать это всем остальным
						for other_client_fd in clients:
							if other_client_fd == fd:
								continue
							(other_client_addr, _, other_client_fobj) = clients[other_client_fd]
							print('\tForwarding to', other_client_addr)
							other_client_fobj.write(line)
							other_client_fobj.flush()
						#Successfully forwarded to all other clients, handle next event:
						continue
#если ни один из if не выполнился, следовательно клиент отвалился и надо его отсоединить
				print('Disconnecting' , client_addr)
				poll_obj.unregister(fd)
				client_fobj.close()
				client_sock.close()
				del client[fd]


