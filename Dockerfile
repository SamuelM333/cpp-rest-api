# GCC support can be specified at major, minor, or micro version
# (e.g. 8, 8.2 or 8.2.0).
# See https://hub.docker.com/r/library/gcc/ for all supported GCC
# tags from Docker Hub.
# See https://docs.docker.com/samples/library/gcc/ for more on how to use this image
FROM gcc:8.2

RUN apt update && apt install -y cmake libssl-dev libsasl2-dev

COPY install-deps.sh /opt
RUN chmod +x /opt/install-deps.sh
RUN sh /opt/install-deps.sh

# These commands copy your files into the specified directory in the image
# and set that as the working location
COPY . /usr/src/metal-api
WORKDIR /usr/src/metal-api

# This command compiles your app using GCC, adjust for your source code
RUN mkdir cmake-build-debug
RUN cmake --build cmake-build-debug --target metal_api -- -j 6

# This command runs your application, comment out this line to compile only
CMD ["./metal_api"]

LABEL Name=metal-api Version=0.0.1
