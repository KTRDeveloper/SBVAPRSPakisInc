#!/bin/sh

docker build --no-cache -t satcomp-sbva-prs-pakisinc:common ../ --file common/Dockerfile
docker build --no-cache -t satcomp-sbva-prs-pakisinc:leader ../ --file leader/Dockerfile
