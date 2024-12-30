# Emscripten Dockerfile for Nomad
FROM emscripten/emsdk

RUN apt update
RUN apt upgrade -y

RUN apt update
RUN apt install -y software-properties-common lsb-release
RUN apt clean all
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
RUN apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main"
RUN apt install kitware-archive-keyring
RUN rm /etc/apt/trusted.gpg.d/kitware.gpg
RUN apt update
RUN apt install cmake -y

WORKDIR /app/src

COPY . .

RUN mkdir ../out

RUN emcmake cmake -S . -B ../out

RUN emmake make

RUN emcc [-Ox] nomad.o -o nomad.js