build:
	g++ server.cpp -o server 
	g++ server2.cpp -o server2
	g++ client.cpp -o client

server1:
	./server 8080

server2:
	./server2 8081

client:
	./client 8080

docker:
	docker build -t client .
	sudo docker run --name client_container --network="host" -v "$(pwd):/app" -it --rm client
