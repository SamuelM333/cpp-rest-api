FROM gcc:8.2

RUN apt update && apt install -y libssl-dev libsasl2-dev

COPY ./install-cmake.sh /opt
RUN chmod +x /opt/install-cmake.sh
RUN sh /opt/install-cmake.sh

COPY ./install-dependencies.sh /opt
RUN chmod +x /opt/install-dependencies.sh
RUN sh /opt/install-dependencies.sh

COPY . /usr/src/metal-api
WORKDIR /usr/src/metal-api

RUN mkdir build
WORKDIR /usr/src/metal-api/build
RUN cmake -G 'Unix Makefiles' ..
RUN make

COPY ./start.sh /usr/src/metal-api
WORKDIR /usr/src/metal-api
