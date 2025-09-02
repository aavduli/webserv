FROM debian:bullseye

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
	build-essential \
	valgrind \
	zsh \
	curl \
	ca-certificates \
	procps \
	netcat-openbsd \
	libc6-dbg \
	gdb \
	&& rm -rf /var/lib/apt/lists/*

RUN useradd -ms /bin/zsh dev
USER dev
WORKDIR /home/dev/app

COPY --chown=dev:dev . .

RUN make re

EXPOSE 8080

ENTRYPOINT ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "--trace-origin=yes", "./webserv"]
