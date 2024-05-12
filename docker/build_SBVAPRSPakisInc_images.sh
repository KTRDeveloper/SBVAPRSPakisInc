#!/bin/sh

docker build -t satcomp-sbva-prs-pakisinc:common ../ --file common/Dockerfile
docker build -t satcomp-sbva-prs-pakisinc:leader ../ --file leader/Dockerfile
