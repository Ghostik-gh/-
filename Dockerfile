FROM gcc

WORKDIR /app

COPY . /app

RUN make build

EXPOSE 8080

CMD ["./client 8080"]