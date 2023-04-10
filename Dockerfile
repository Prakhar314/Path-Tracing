FROM gcc:bullseye
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get -y update && apt-get install -y
RUN apt-get install -y libsdl2-image-dev libglm-dev libglew-dev libopengl-dev cmake
# COPY ~/.bashrc /root/.bashrc
