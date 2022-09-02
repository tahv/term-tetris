FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc \
    make \
    libncursesw5-dev

WORKDIR /app
COPY . .
RUN make all
CMD ["./tetris"]
