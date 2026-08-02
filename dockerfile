FROM alpine:3.18

# Install build tools and MariaDB client (MySQL-compatible)
RUN apk add --no-cache \
    gcc \
    musl-dev \
    mariadb-dev \
    make

WORKDIR /app

COPY . .

# Build with correct include path for Alpine/MariaDB
RUN make CFLAGS="-Wall -Wextra -g -I/usr/include/mysql"

CMD ["./campus_sports"]