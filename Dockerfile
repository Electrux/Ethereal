FROM archlinux/base:latest

RUN pacman -Sy base-devel vim valgrind git bc ccache cmake gmp --needed --noconfirm
WORKDIR /et
RUN echo 'export PREFIX_DIR="/usr"' >> ~/.bashrc
RUN echo 'alias bs="rm -rf build && mkdir -p build && cd build && cmake .. && make -j8 && make install && cd .."' >> ~/.bashrc
