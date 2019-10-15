FROM archlinux/base:latest

RUN pacman -Sy base-devel vim valgrind git bc ccache cmake gmp --needed --noconfirm
WORKDIR /et
