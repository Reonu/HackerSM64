FROM ubuntu:22.04 as build

RUN apt-get update && \
    apt-get install -y \
        binutils-mips-linux-gnu \
        bsdextrautils \
        build-essential \
        libcapstone-dev \
        pkgconf \
        python3

RUN mkdir /hackersm64
WORKDIR /hackersm64
ENV PATH="/hackersm64/tools:${PATH}"

CMD echo 'Usage: docker run --rm --mount type=bind,source="$(pwd)",destination=/hackersm64 hackersm64 make VERSION=us -j4\n' \
         'See https://github.com/HackerN64/HackerSM64/blob/master/README.md for more information'
