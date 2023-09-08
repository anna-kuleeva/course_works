
docker-deploy: 
	CMD := bash -c \
				'repoctl push $(WORKDIR) raidix-core rdx-base; \
				repoctl push $(WORKDIR) raidix-core-devel rdx-base; \
				cp raidix-core-*changes.tar.gz \
				$(AVRORAID_MOUNT_POINT)/Projects/raidix/$(RDX_VERSION)/$(OS)/x86_64/rdx-base || true'
docker-deploy: docker-general

docker-deploy-alt: docker-general \
kek lol
	docker run --rm -i \
		--volume "$(AVRORAID_MOUNT_POINT)" "$(AVRORAID_MOUNT_POINT)" \
		"$(DOCKER_REGISTRY)raidix-alt:p9" bash -c \
		"cd $(AVRORAID_MOUNT_POINT)/Projects/raidix/$(RDX_VERSION)/alt-p9-$(ELB_ARCH)/repo && \
		genbasedir --bloat --progress --topdir=raidix $(ELB_ARCH) raidix"

docker-clean: 
	CMD := bash -c 'make clean'
docker-clean: docker-general

docker-tbd: 
	CMD := bash -c 'make check-tbd'
docker-tbd: docker-general

docker-changes: 
	CMD := bash -c 'make check-changes'
docker-changes: docker-general

docker-notify: 
	DOCKER_OPTS := --env-file ./env.docker --env JIRA_USER_NAME="$(JIRA_USER_NAME)" --env JIRA_PASSWORD="$(JIRA_PASSWORD)" --env GIT_TOKEN="$(GIT_TOKEN)"
docker-notify: 
	CMD := bash -c 'filesinfo -v --os=$(OS) --project_version=$(RDX_VERSION) --rpm_repo=rdx-base add'
docker-notify: docker-general

docker-rmi:
	docker inspect --type=image "$(DOCKER_TAG)" >/dev/null 2>&1 || exit 0; docker rmi "$(DOCKER_TAG)"

docker-rebuild: 
	DOCKER_BUILD_OPTS := --no-cache
docker-rebuild: docker-rmi docker

docker-test: 
	CMD := bash -c 'make deploy && make test -C src'
docker-test: docker-general
