FROM satcomp-sbva-prs-pakisinc:common AS builder
USER root

FROM satcomp-infrastructure:leader

WORKDIR /

COPY --chown=ecs-user --from=builder /BuildDir/bin/ /

COPY --chown=ecs-user /docker/leader/init_solver.sh /competition
COPY --chown=ecs-user /docker/leader/solver /competition

USER ecs-user

RUN chmod +x /competition/init_solver.sh
RUN chmod +x /competition/solver
