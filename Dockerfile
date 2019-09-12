FROM archlinux/base:latest

RUN pacman -Sy base-devel vim valgrind git bc ccache --needed --noconfirm
WORKDIR /et
