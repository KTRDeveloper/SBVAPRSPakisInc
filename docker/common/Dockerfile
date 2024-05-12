FROM satcomp-infrastructure:common
USER root
#  Install required softwares

# RUN  sed -i s@/archive.ubuntu.com/@/mirrors.bfsu.edu.cn/@g /etc/apt/sources.list
RUN apt update
RUN DEBIAN_FRONTEND=noninteractive apt install -y vim cmake build-essential zlib1g-dev libopenmpi-dev wget unzip python3 gfortran curl
RUN apt install -y libboost-all-dev

WORKDIR /
COPY / /BuildDir

WORKDIR /BuildDir
RUN chmod +x clean.sh
RUN chmod +x build.sh
RUN ./build.sh
RUN ./clean.sh

