FROM archlinux:latest

RUN pacman -Sy --noconfirm --needed base-devel git sudo

RUN useradd -m builder && \
    printf 'builder ALL=(ALL) NOPASSWD: ALL\n' > /etc/sudoers.d/builder && \
    chmod 440 /etc/sudoers.d/builder

USER builder
WORKDIR /home/builder

RUN git clone https://aur.archlinux.org/yay.git && \
    cd yay && \
    makepkg -si --noconfirm

COPY --chown=builder:builder smoke.sh /home/builder/smoke.sh

RUN chmod +x /home/builder/smoke.sh

CMD ["/home/builder/smoke.sh"]
