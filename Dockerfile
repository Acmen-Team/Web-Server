FROM ubuntu

RUN mkdir /usr/src/server

COPY dist /usr/src/server

WORKDIR /usr/src/server

CMD ["./webserver"]
