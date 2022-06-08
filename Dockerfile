FROM alpine:latest
RUN apk -U upgrade
RUN apk add --no-cache gcc musl-dev make
WORKDIR /home
