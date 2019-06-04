FROM archlinux/base:latest

RUN pacman -Sy base-devel vim valgrind git bc --needed --noconfirm
RUN git clone https://github.com/Electrux/Alacrity-Lang.git
WORKDIR /Alacrity-Lang
RUN ./bootstrap.sh
WORKDIR /et