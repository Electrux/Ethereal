FROM archlinux/base:latest

RUN pacman -Sy base-devel vim valgrind git bc ccache cmake gmp --needed --noconfirm
WORKDIR /et
RUN echo 'export PREFIX_DIR="/usr"' >> ~/.bashrc
RUN echo 'alias bs="rm -rf dockerbuild && mkdir -p dockerbuild && cd dockerbuild && cmake .. && make -j$(nproc) && make install && cd .."' >> ~/.bashrc
